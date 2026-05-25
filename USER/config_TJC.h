#ifndef __CONFIG_TJC_H
#define __CONFIG_TJC_H

#include "stm32f10x.h"
#include <stdint.h>

void TJC_Init(uint32_t baud);
void TJC_SendCmd(const char *cmd);
void TJC_SetText(const char *obj, const char *txt);
void TJC_SetNumber(const char *obj, int value);
void TJC_UpdatePressure(float pressure_kpa);
void TJC_AddDataRecord(float pressure_kpa, uint8_t pass);
void TJC_AddAlarm(const char *reason, const char *state);

#endif
