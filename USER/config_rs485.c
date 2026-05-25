#include "config_rs485.h"
#include "config_delay.h"

#define PRESSURE_SLAVE_ADDR      0x01
#define RS485_DE_TX()            /* TODO: PA8=1 */
#define RS485_DE_RX()            /* TODO: PA8=0 */

static uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i;
    uint8_t j;

    for (i = 0; i < len; i++)
    {
        crc ^= buf[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

void RS485_Init(uint32_t baud)
{
    (void)baud;
    /* TODO:
     * 1. 初始化 USART1 PA9/PA10
     * 2. 初始化 PA8 为推挽输出
     * 3. 默认 PA8=0 接收模式
     */
    RS485_DE_RX();
}

void RS485_SendBuf(uint8_t *buf, uint16_t len)
{
    RS485_DE_TX();
    Delay_ms(2);

    /* TODO: 使用 USART1 发送 */
    (void)buf;
    (void)len;

    Delay_ms(2);
    RS485_DE_RX();
}

uint8_t RS485_ReadPressure(float *pressure_kpa)
{
    uint8_t tx[8];
    uint16_t crc;

    tx[0] = PRESSURE_SLAVE_ADDR;
    tx[1] = 0x03;
    tx[2] = 0x00;
    tx[3] = 0x00;
    tx[4] = 0x00;
    tx[5] = 0x01;

    crc = Modbus_CRC16(tx, 6);
    tx[6] = crc & 0xFF;
    tx[7] = (crc >> 8) & 0xFF;

    RS485_SendBuf(tx, 8);

    /*
     * TODO: 接收 7 字节：
     * 地址 功能码 字节数 数据H 数据L CRCL CRCH
     * 示例中先给模拟值，实际项目替换为真实接收解析。
     */
    *pressure_kpa = 253.6f;

    return 1;
}
