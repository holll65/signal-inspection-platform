#include "config_opt.h"

void GPIO_OPT_Config(void)	
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOB   //GPIOB端口时钟                  	                  
                       	, ENABLE);	             //使能        
	
  // PB0 PB1 PB10 PB11 输入：检测 X1 X2 X3 X4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 
 