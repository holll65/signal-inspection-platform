#ifndef __CONFIG_TJC_H
#define __CONFIG_TJC_H

#include "stm32f10x.h"
#include <stdint.h>

void TJC_Init(uint32_t baud);

void TJC_SendEnd(void);
void TJC_SendCmd(const char *cmd);
void TJCPrintf(const char *fmt, ...);

void TJC_SetSystemState(uint8_t system_ok);
void TJC_SetProductResult(uint8_t product_ok);
uint32_t TJC_ReadNumber(const char *cmd);

#endif  