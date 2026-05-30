#include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
#include "config_opt.h"
#include "config_mos.h"
#include "config_TJC.h"
#include <stdlib.h>
#include "config_protocol.h"
/******** Pressure simulation range ********/
#define PRESSURE_MIN   0
#define PRESSURE_MAX   630
#define PRESSURE_INIT  300


/************************************************
Function: TJC_SendPressure
Purpose : Send pressure value to the screen
*************************************************/
void TJC_SendPressure(uint16_t pressure_value)
{
    uint16_t curve_value;

    TJCPrintf("main.n0.val=%d", pressure_value);

    curve_value = pressure_value * 125 / 630;

    TJCPrintf("add s0.id,0,%d", curve_value);
}
int main(void)
{
    uint8_t system_ok = 0;
    uint8_t last_system_ok = 255;

    uint8_t product_result = 0;
    uint8_t last_product_result = 255;

    uint16_t pressure_value = 0;
    uint16_t pressure_time = 0;
    uint16_t pressure_send_time = 0;
    
    
    uint8_t log_written = 0;

    Delay_Init();
    GPIO_relay_Config();
    GPIO_MOS_Config();
    GPIO_OPT_Config();

    TJC_Init(9600);

    srand(1234);

    TJC_SetSystemState(0);
    TJC_SetProductResult(0);
    TJC_SendCmd("page main");

    while(1)
    {
        /******** 1. Update pressure once per second ********/
        pressure_send_time += 50;

        if(pressure_send_time >= 1000)
        {
            pressure_send_time = 0;

            pressure_value = Pressure_GetValue();

            TJC_SendPressure(pressure_value);
           //TJCPrintf("add s0.id,0,62");
        }

        /******** 2. If pressure stays above 0 for 1 second, system is ready ********/
        if(pressure_value > 0)
        {
            if(pressure_time < 1000)
            {
                pressure_time += 50;
            }

            if(pressure_time >= 1000)
            {
                system_ok = 1;
            }
        }
        else
        {
            pressure_time = 0;
            system_ok = 0;
            product_result = 0;

            relay1 = 0;
            relay2 = 0;
        }

        /******** 3. Sync system state to screen ********/
        if(system_ok != last_system_ok)
        {
            TJC_SetSystemState(system_ok);
            last_system_ok = system_ok;
        }

        /******** 4. Judge product result when system is ready ********/
        if(system_ok == 1)
        {
            if(X1 == 0)
            {
                relay1 = 1;
                relay2 = 0;
                product_result = 1;
            }
            else if(X2 == 0)
            {
                relay1 = 0;
                relay2 = 1;
                product_result = 2;
            }
            else
            {
                relay1 = 0;
                relay2 = 0;
                product_result = 0;
            }
        }
        else
        {
            product_result = 0;
        }

        /******** 5. Sync product result to screen ********/
        if(product_result != last_product_result)
        {
            TJC_SetProductResult(product_result);
            last_product_result = product_result;
        }

        /******** 6. Write pass/fail log once only ********/
        if(product_result == 1 && log_written == 0)
        {
            TJC_AddDataLog(pressure_value, "�ϸ�");
            log_written = 1;
        }
        else if(product_result == 2 && log_written == 0)
        {
            TJC_AddDataLog(pressure_value, "���ϸ�");
            log_written = 1;
        }

        /******** 7. Reset log flag when result returns to idle ********/
        if(product_result == 0)
        {
            log_written = 0;
        }

        Delay_ms(50);
    }
}
