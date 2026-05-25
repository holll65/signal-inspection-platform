#ifndef __CONFIG_RS485_H
#define __CONFIG_RS485_H

#include "stm32f10x.h"
#include <stdint.h>

/*
 * 推荐用于压力采集：
 * USART1：PA9/PA10
 * PA8：SP3485 DE/RE 方向控制
 */

void RS485_Init(uint32_t baud);
void RS485_SendBuf(uint8_t *buf, uint16_t len);
uint8_t RS485_ReadPressure(float *pressure_kpa);

#endif
