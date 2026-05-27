  #include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
//˵��

 int main(void)
	 { 
		 u16 key_state;	
	
    GPIO_relay_Config();
      
		 while(1)
		 { 
			 
				
			//  key_state = GPIO_ReadInputData(GPIOB) & (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			// 	switch(key_state)
			// 	{
				
			// 	case (GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15):
			// 								   relay1 =!relay1 ;
			// 									 Delay_ms(200);
			// 	break;	
			// 	case (GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15):
			// 								   relay2 =!relay2 ;
			// 									 Delay_ms(200);
			// 	break;	
			// 	case (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15):
			// 								   relay3 =!relay3 ;
			// 									 Delay_ms(200);
			// 	break;	
			// 	case (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14):
			// 								   relay4 =!relay4 ;
			// 									 Delay_ms(200);
			// 	break;					
			// 	default:	break;
			// 	}
				

		 }
	 }

	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
