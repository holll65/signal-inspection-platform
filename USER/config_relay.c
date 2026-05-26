#include "config_relay.h"

void GPIO_relay_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_AFIO |
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_GPIOB |
        RCC_APB2Periph_GPIOC |
        RCC_APB2Periph_GPIOD,
        ENABLE
    );

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // ������
    GPIO_SetBits(GPIOC, GPIO_Pin_12 | GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ������
    GPIO_SetBits(GPIOA, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // ������
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
}
