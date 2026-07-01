#include "stm32f10x.h"
#include "config_rs485.h"
#include "config_delay.h"
#include "config_TJC.h"
#include <stdio.h>

#define PRESSURE_SLAVE_ADDR   0x01
#define RS485_RX_TIMEOUT      50000

static uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i;
    uint8_t j;

    for(i = 0; i < len; i++)
    {
        crc ^= buf[i];

        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

void RS485_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1 |
        RCC_APB2Periph_AFIO,
        ENABLE);

    /* PA9 TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA10 RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA8 485方向控制 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RS485_DE_RX();

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_ReceiveData(USART1);
}

void RS485_ClearRx(void)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
}

void RS485_SendByte(uint8_t data)
{
    USART_SendData(USART1, data);

    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void RS485_SendBuf(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    USART_ClearFlag(USART1, USART_FLAG_TC);

    RS485_DE_TX();

    Delay_ms(2);

    for(i = 0; i < len; i++)
    {
        RS485_SendByte(buf[i]);
    }

    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

    Delay_ms(2);

    RS485_DE_RX();
}

uint8_t RS485_ReadByte(uint8_t *data)
{
    uint32_t timeout = RS485_RX_TIMEOUT;

    while(timeout--)
    {
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            *data = USART_ReceiveData(USART1);
            return 1;
        }
    }

    return 0;
}

uint8_t RS485_ReadPressure(int16_t *pressure_raw)
{
    uint8_t tx[8];
    uint8_t rx[7];
    uint16_t crc;
    uint8_t i;

    RS485_ClearRx();

    tx[0] = PRESSURE_SLAVE_ADDR;
    tx[1] = 0x03;
    tx[2] = 0x00;
    tx[3] = 0x02;        /* 读0002H，跟随显示 */
    tx[4] = 0x00;
    tx[5] = 0x01;        /* 只读1个寄存器 */

    crc = Modbus_CRC16(tx, 6);
    tx[6] = crc & 0xFF;
    tx[7] = crc >> 8;

    RS485_SendBuf(tx, 8);

    for(i = 0; i < 7; i++)
    {
        if(RS485_ReadByte(&rx[i]) == 0)
        {
            return 0;
        }
    }

    crc = Modbus_CRC16(rx, 5);

    if(rx[5] != (crc & 0xFF) || rx[6] != (crc >> 8))
    {
        return 0;
    }

    if(rx[0] != PRESSURE_SLAVE_ADDR)
    {
        return 0;
    }

    if(rx[1] != 0x03)
    {
        return 0;
    }

    if(rx[2] != 0x02)
    {
        return 0;
    }

    /* 有符号16位压力值 */
    *pressure_raw = (int16_t)(((uint16_t)rx[3] << 8) | rx[4]);

    return 1;
}