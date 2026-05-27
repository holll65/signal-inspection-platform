  #include "stm32f10x.h"
#include "config_relay.h"
#include "config_delay.h"
#include "config_opt.h"
#include "config_mos.h"
//˵��

 int main(void)
	 { 
	
		GPIO_relay_Config();
		GPIO_MOS_Config();
	    GPIO_OPT_Config();
      
		while(1)
		 	{ 
			 
				
				if(X1==0)
					{
    					 relay1 = 1;
					}
				else
					{
    					relay1 = 0;
					}
				if(X2==0)
					{
						 relay2 = 1;
					}
				else
					{
    					relay2 = 0;
					}

		 	}
	 }

	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
	 
