#ifndef __CONFIG_RS232_H
#define __CONFIG_RS232_H

#include "stm32f10x.h"
#include <stdint.h>

/*
 * USART1：PA9/PA10
 * 在你的说明中用于共享 RS232 总线。
 * 若改成 485，可将 PA9/PA10 接 SP3485，同时增加 DE 控制脚。
 */

void RS232_Init(uint32_t baud);
void RS232_SendByte(uint8_t ch);
void RS232_SendBuf(uint8_t *buf, uint16_t len);
uint8_t RS232_ReadByte(uint8_t *ch);

#endif
