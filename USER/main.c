#include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
#include "config_opt.h"
#include "config_mos.h"
#include "config_TJC.h"
#include <stdlib.h>
#include "config_protocol.h"
/******** 压力模拟参数 ********/
#define PRESSURE_MIN   0
#define PRESSURE_MAX   630
#define PRESSURE_INIT  300


/************************************************
函数: TJC_SendPressure
功能: 发送压力值和曲线
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
        /******** 1. 每1秒更新一次压力值 ********/
        pressure_send_time += 50;

        if(pressure_send_time >= 1000)
        {
            pressure_send_time = 0;

            pressure_value = Pressure_GetValue();

            TJC_SendPressure(pressure_value);
           //TJCPrintf("add s0.id,0,62");
        }

        /******** 2. 压力判断：压力>0持续1秒 ********/
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

        /******** 3. 系统状态发送给屏 ********/
        if(system_ok != last_system_ok)
        {
            TJC_SetSystemState(system_ok);
            last_system_ok = system_ok;
        }

        /******** 4. 系统正常后，判断产品合格/不合格 ********/
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

        /******** 5. 产品结果发送给屏 ********/
        if(product_result != last_product_result)
        {
            TJC_SetProductResult(product_result);
            last_product_result = product_result;
        }

        Delay_ms(50);
    }
}