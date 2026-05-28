#include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
#include "config_opt.h"
#include "config_mos.h"
#include "config_TJC.h"
#include "config_delay.h"
#define PRESSURE_VALUE  1   // 这里先模拟压力>0，后面换成你的真实压力变量

int main(void)
{
    uint8_t system_ok = 0;
    uint8_t last_system_ok = 255;

    uint8_t product_result = 0;
    uint8_t last_product_result = 255;

    uint16_t pressure_time = 0;
	Delay_Init();
    GPIO_relay_Config();
    GPIO_MOS_Config();
    GPIO_OPT_Config();

    TJC_Init(115200);

    TJC_SetSystemState(0);       // 上电默认系统异常/未就绪
    TJC_SetProductResult(0);     // 上电默认未测试

    while(1)
    {
        /******** 1. 压力判断：压力>0持续1秒 ********/
        if(PRESSURE_VALUE > 0)
        {
            if(pressure_time < 1000)
            {
                pressure_time += 50;
            }

            if(pressure_time >= 1000)
            {
                system_ok = 1;   // 系统正常/压力合格
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

        /******** 2. 系统状态发送给屏 ********/
        if(system_ok != last_system_ok)
        {
            TJC_SetSystemState(system_ok);
            last_system_ok = system_ok;
        }

        /******** 3. 系统正常后，才判断产品合格/不合格 ********/
        if(system_ok == 1)
        {
            if(X1 == 0)
            {
                relay1 = 1;
                relay2 = 0;
                product_result = 1;   // 产品测试合格
            }
            else if(X2 == 0)
            {
                relay1 = 0;
                relay2 = 1;
                product_result = 2;   // 产品测试不合格
            }
            else
            {
                relay1 = 0;
                relay2 = 0;
                product_result = 0;   // 未测试
            }
        }
        else
        {
            product_result = 0;
        }

        /******** 4. 产品结果发送给屏 ********/
        if(product_result != last_product_result)
        {
            TJC_SetProductResult(product_result);
            last_product_result = product_result;
        }
		
        Delay_ms(50);
    }
}