  #include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
//说明

 int main(void)
	 { 
		 u16 IO_IDR;	
	
    GPIO_relay_Config();
     
		 while(1)
		 { 
			 
				
			 IO_IDR=(GPIOB->IDR&0xfc3b);//过滤其他IO
				switch(IO_IDR)//模式显示
				{
				
				case 60475://PB12-X5状态获取
											   relay1 =!relay1 ;
												 Delay_ms(200);
				break;	
				case 56379://PB13-X6状态获取
											   relay2 =!relay2 ;
												 Delay_ms(200);
				break;	
				case 48187://PB14-X7状态获取
											   relay3 =!relay3 ;
												 Delay_ms(200);
				break;	
				case 31803://PB15-X8状态获取
											   relay4 =!relay4 ;
												 Delay_ms(200);
				break;					
				default:	break;
				}
				

		 }
	 }

	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
