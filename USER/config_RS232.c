#include "config_RS232.h"

/* 模板版：寄存器级简化初始化，实际项目可替换为标准外设库初始化 */

void RS232_Init(uint32_t baud)
{
    (void)baud;
    /* TODO:
     * 1. 使能 GPIOA / USART1 时钟
     * 2. PA9 配置复用推挽输出
     * 3. PA10 配置浮空输入
     * 4. USART1 设置 9600 8N1
     * 5. 开启 RXNE 中断
     */
}

void RS232_SendByte(uint8_t ch)
{
    (void)ch;
    /* TODO: 等待 TXE，然后写 USART1->DR */
}

void RS232_SendBuf(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        RS232_SendByte(buf[i]);
    }
}

uint8_t RS232_ReadByte(uint8_t *ch)
{
    (void)ch;
    /* TODO: 从接收环形缓冲区读取 1 字节 */
    return 0;
}
