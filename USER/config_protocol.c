#include "config_protocol.h"

#include <stdlib.h>

static uint16_t pressure_value = 300;   // ³õÊ¼Ñ¹Á¦300KPa

uint16_t Pressure_GetValue(void)
{
    int8_t delta;

    delta = (rand() % 5) - 2;   // -2~+2 KPa

    pressure_value += delta;

    if(pressure_value > 630)
    {
        pressure_value = 630;
    }

    if(pressure_value < 10)
    {
        pressure_value = 10;
    }

    return pressure_value;
}