#include "config_RS232.h"
#include "stm32f10x.h"

#define RX_BUFFER_SIZE 256

volatile uint8_t  rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_head = 0;
volatile uint16_t rx_tail = 0;


/************************************************
函数: RS232_Init
功能: USART3 初始化
接口:
PC10 -> TX
PC11 -> RX
波特率: 115200/9600
*************************************************/
void RS232_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 开启时钟
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOC |
        RCC_APB2Periph_AFIO,
        ENABLE);

    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_USART3,
        ENABLE);

    /*************** TX PC10 ***************/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*************** RX PC11 ***************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*************** USART3 ***************/
    USART_InitStructure.USART_BaudRate            = baud;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                =
        USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);

    // 开启接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // USART3使能
    USART_Cmd(USART3, ENABLE);

    /*************** NVIC ***************/
    NVIC_InitStructure.NVIC_IRQChannel =
        USART3_IRQn;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}


/************************************************
函数: RS232_SendByte
功能: 发送1字节
*************************************************/
void RS232_SendByte(uint8_t ch)
{
    USART_SendData(USART3, ch);

    while (USART_GetFlagStatus(
               USART3,
               USART_FLAG_TXE) == RESET);
}


/************************************************
函数: RS232_SendBuf
功能: 发送数组
*************************************************/
void RS232_SendBuf(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        RS232_SendByte(buf[i]);
    }
}


/************************************************
函数: RS232_ReadByte
功能: 从缓冲区读取1字节
返回:
1 = 成功
0 = 无数据
*************************************************/
uint8_t RS232_ReadByte(uint8_t *ch)
{
    if (rx_head == rx_tail)
    {
        return 0;
    }

    *ch = rx_buffer[rx_tail];

    rx_tail++;

    if (rx_tail >= RX_BUFFER_SIZE)
    {
        rx_tail = 0;
    }

    return 1;
}


/************************************************
函数: USART3_IRQHandler
功能: USART3中断
*************************************************/
void USART3_IRQHandler(void)
{
    uint8_t data;

    if (USART_GetITStatus(
            USART3,
            USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART3);

        rx_buffer[rx_head] = data;

        rx_head++;

        if (rx_head >= RX_BUFFER_SIZE)
        {
            rx_head = 0;
        }

        USART_ClearITPendingBit(
            USART3,
            USART_IT_RXNE);
    }
}