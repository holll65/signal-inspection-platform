#include "config_led.h"

void GPIO_LED_Config(void)	
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOC   //GPIOB端口时钟                  
                       	, ENABLE);	             //使能        
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8| GPIO_Pin_9; //LED1->PD9 , LED2->PD11 , LED3->PD13 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                      //IO模式推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                     //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);                                //GPIOD配置初始化
// GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8| GPIO_Pin_9);           //GPIO-PD9 , GPIO-PD11 , GPIO-PD13端口配置为低电平
 GPIO_SetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8| GPIO_Pin_9);             //GPIO-PD9 , GPIO-PD11 , GPIO-PD13端口配置为高电平	
 				  
}
 
