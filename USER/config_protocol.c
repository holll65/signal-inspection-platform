#include "config_protocol.h"
#include "config_rs485.h"
#include <stdlib.h>

static uint16_t pressure_value =0;   // ³õÊ¼Ñ¹Á¦300KPa
static uint8_t pressure_comm_ok = 0;
int16_t pressure_kpa; 
// uint16_t Pressure_GetValue(void)
// {
//     int8_t delta;

//     delta = (rand() % 5) - 2;   // -2~+2 KPa

//     pressure_value += delta;

//     if(pressure_value > 630)
//     {
//         pressure_value = 630;
//     }

//     if(pressure_value < 10)
//     {
//         pressure_value = 10;
//     }

//     return pressure_value;
// }
int16_t Pressure_GetValue(void)
{
     
    if(RS485_ReadPressure(&pressure_kpa))
    {
        pressure_comm_ok = 1;
       
        pressure_value = pressure_kpa;
    }
    else
    {
        pressure_comm_ok = 0;
    }

    return pressure_value;   // ?????????
}
uint8_t Pressure_CommOK(void)
{
    return pressure_comm_ok;
}
//  if(RS485_ReadPressure(&pressure))
//     {
//         sprintf(str, "%d", pressure);
//         TJCPrintf("main.t3.txt=\"%s\"", str);
//     }

//     Delay_ms(200);