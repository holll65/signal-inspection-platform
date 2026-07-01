#include "config_delay.h"

static volatile uint32_t g_ms_tick = 0;

void SysTick_Handler(void)
{
    if (g_ms_tick > 0)
    {
        g_ms_tick--;
    }
}

void Delay_Init(void)
{
    /* 72MHz / 1000 = 1ms */
    /* 模板中简化处理，实际可调用 SysTick_Config(SystemCoreClock / 1000); */
}

void Delay_ms(uint32_t ms)
{
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 7200; j++)
        {
            __asm volatile ("nop");
        }
    }
}
