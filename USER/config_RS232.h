#ifndef __CONFIG_RS232_H
#define __CONFIG_RS232_H

#include "stm32f10x.h"
#include <stdint.h>


/*
 * USART3 RS232接口
 *
 * PC10 -> TX
 * PC11 -> RX
 *
 * 外接 SP3232 电平转换芯片
 * 再连接陶晶驰串口屏 RS232接口
 *
 * 波特率：
 * 9600 / 115200
 */


// 初始化串口
void RS232_Init(uint32_t baud);


// 发送1字节
void RS232_SendByte(uint8_t ch);


// 发送数组
void RS232_SendBuf(uint8_t *buf, uint16_t len);


// 读取1字节
// 返回值:
// 1 = 成功
// 0 = 无数据
uint8_t RS232_ReadByte(uint8_t *ch);


// 清空接收缓冲区
void RS232_ClearRxBuffer(void);


#endif
