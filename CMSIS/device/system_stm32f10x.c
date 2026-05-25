#include "stm32f10x.h"

uint32_t SystemCoreClock = 72000000;

#define GPIO_CRL_MASK(pos)      (~(0xFu << ((pos) * 4)))
#define GPIO_CRL_SET(pos, val)  (((val) & 0xFu) << ((pos) * 4))
#define GPIO_CRH_MASK(pos)      (~(0xFu << (((pos) - 8) * 4)))
#define GPIO_CRH_SET(pos, val)  (((val) & 0xFu) << (((pos) - 8) * 4))

#define APB2ENR_AFIOEN          (1u << 0)
#define APB2ENR_IOPAEN          (1u << 2)
#define APB2ENR_IOPCEN          (1u << 4)
#define APB2ENR_IOPDEN          (1u << 5)

#define MODE_OUT_PP_2MHZ        0x2u

static void Relay_SafeState_Init(void)
{
    RCC->APB2ENR |= APB2ENR_AFIOEN |
                    APB2ENR_IOPAEN |
                    APB2ENR_IOPCEN |
                    APB2ENR_IOPDEN;

    /* 释放 PA15，保留 SWD 调试。 */
    AFIO->MAPR = (AFIO->MAPR & ~(0x7u << 24)) | (0x2u << 24);

    GPIOA->BSRR = (1u << 15) << 16;
    GPIOC->BSRR = ((1u << 12) | (1u << 13)) << 16;
    GPIOD->BSRR = (1u << 2) << 16;

    GPIOA->CRH &= GPIO_CRH_MASK(15);
    GPIOA->CRH |= GPIO_CRH_SET(15, MODE_OUT_PP_2MHZ);

    GPIOC->CRH &= GPIO_CRH_MASK(12);
    GPIOC->CRH |= GPIO_CRH_SET(12, MODE_OUT_PP_2MHZ);
    GPIOC->CRH &= GPIO_CRH_MASK(13);
    GPIOC->CRH |= GPIO_CRH_SET(13, MODE_OUT_PP_2MHZ);

    GPIOD->CRL &= GPIO_CRL_MASK(2);
    GPIOD->CRL |= GPIO_CRL_SET(2, MODE_OUT_PP_2MHZ);
}

void SystemInit(void)
{
    Relay_SafeState_Init();
}
