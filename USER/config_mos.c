#include "config_mos.h"

void GPIO_MOS_Config(void)	
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOB   //GPIOB端口时钟                  	                  
                       	, ENABLE);	             //使能        
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; //LED1->PD9 , LED2->PD11 , LED3->PD13 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                      //IO模式推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                     //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);                                //GPIOD配置初始化
 // 你的MOS电路：高电平 = 关闭，低电平 = 导通
    // 所以上电初始化后先全部关闭
    GPIO_SetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9);           //GPIO-PD9 , GPIO-PD11 , GPIO-PD13端口配置为低电平
                   				  
}
 
 