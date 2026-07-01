#ifndef __CONFIG_PROTOCOL_H
#define __CONFIG_PROTOCOL_H

#include "stm32f10x.h"
#include <stdint.h>

int16_t Pressure_GetValue(void);
uint8_t Pressure_CommOK(void);
#endif
