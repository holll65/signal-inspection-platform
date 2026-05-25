#include "stm32f10x.h"
#include "config_delay.h"
#include "config_RS232.h"
#include "config_rs485.h"
#include "config_TJC.h"
#include "config_protocol.h"

#define PRESSURE_LOW_LIMIT      0.0f
#define PRESSURE_HIGH_LIMIT     630.0f

#define GPIO_CRL_MASK(pos)      (~(0xFu << ((pos) * 4)))
#define GPIO_CRL_SET(pos, val)  (((val) & 0xFu) << ((pos) * 4))
#define GPIO_CRH_MASK(pos)      (~(0xFu << (((pos) - 8) * 4)))
#define GPIO_CRH_SET(pos, val)  (((val) & 0xFu) << (((pos) - 8) * 4))

#define MODE_INPUT_PD           0x8u
#define MODE_OUT_PP_50MHZ       0x3u

static float g_pressure_kpa = 0.0f;

static void Relay_AllOff(void)
{
    GPIOA->BSRR = (1u << 15) << 16;
    GPIOC->BSRR = ((1u << 12) | (1u << 13)) << 16;
    GPIOD->BSRR = (1u << 2) << 16;
}

static void Relay_GPIO_Init(void)
{
    RCC->APB2ENR |= (1u << 0) | (1u << 2) | (1u << 3) | (1u << 4) | (1u << 5);

    /* 释放 PA15，保留 SWD 调试。 */
    AFIO->MAPR = (AFIO->MAPR & ~(0x7u << 24)) | (0x2u << 24);

    Relay_AllOff();

    GPIOA->CRH &= GPIO_CRH_MASK(15);
    GPIOA->CRH |= GPIO_CRH_SET(15, MODE_OUT_PP_50MHZ);

    GPIOC->CRH &= GPIO_CRH_MASK(12);
    GPIOC->CRH |= GPIO_CRH_SET(12, MODE_OUT_PP_50MHZ);
    GPIOC->CRH &= GPIO_CRH_MASK(13);
    GPIOC->CRH |= GPIO_CRH_SET(13, MODE_OUT_PP_50MHZ);

    GPIOD->CRL &= GPIO_CRL_MASK(2);
    GPIOD->CRL |= GPIO_CRL_SET(2, MODE_OUT_PP_50MHZ);
    GPIOB->ODR &= ~((1u << 0) | (1u << 1));

    GPIOB->CRL &= GPIO_CRL_MASK(0);
    GPIOB->CRL |= GPIO_CRL_SET(0, MODE_INPUT_PD);
    GPIOB->CRL &= GPIO_CRL_MASK(1);
    GPIOB->CRL |= GPIO_CRL_SET(1, MODE_INPUT_PD);
}

static void Relay_Task(void)
{
    static uint8_t startup_lock = 0;

    if (startup_lock < 3)
    {
        startup_lock++;
        Relay_AllOff();
        return;
    }

    GPIOA->BSRR = (1u << 15) << 16;
    GPIOC->BSRR = (1u << 12) << 16;

    if (GPIOB->IDR & (1u << 0))
        GPIOC->BSRR = (1u << 13);
    else
        GPIOC->BSRR = (1u << 13) << 16;

    if (GPIOB->IDR & (1u << 1))
        GPIOD->BSRR = (1u << 2);
    else
        GPIOD->BSRR = (1u << 2) << 16;
}

static void Board_Init(void)
{
    Delay_Init();

    /*
     * 原例程结构：
     * config_RS232.c/h：USART1 PA9/PA10 9600
     * config_TJC.c/h：陶晶驰 X5 驱动
     * 新增：
     * config_rs485.c/h：485采集压力，可根据实际硬件选择是否复用USART1
     */

    Relay_GPIO_Init();

    RS232_Init(9600);
    TJC_Init(9600);

    /* 如果实际压力传感器走485，则启用 */
    RS485_Init(9600);

    TJC_SendCmd("page main");
    TJC_SetText("t_err", "系统初始化完成");
}

static void App_PressureTask(void)
{
    uint8_t ok;

    ok = RS485_ReadPressure(&g_pressure_kpa);

    if (ok)
    {
        TJC_UpdatePressure(g_pressure_kpa);

        if (g_pressure_kpa >= PRESSURE_LOW_LIMIT &&
            g_pressure_kpa <= PRESSURE_HIGH_LIMIT)
        {
            TJC_SetText("t_err", "压力正常");
            TJC_AddDataRecord(g_pressure_kpa, 1);
        }
        else
        {
            TJC_SetText("t_err", "压力超限");
            TJC_AddDataRecord(g_pressure_kpa, 0);
            TJC_AddAlarm("压力超限", "未恢复");
        }
    }
    else
    {
        TJC_SetText("t_err", "压力传感器通讯故障");
        TJC_AddAlarm("压力传感器通讯故障", "未恢复");
    }
}

int main(void)
{
    Board_Init();

    while (1)
    {
        Relay_Task();
        App_PressureTask();
        Delay_ms(1000);
    }
}
