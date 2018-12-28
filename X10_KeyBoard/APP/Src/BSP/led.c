
#include "led.h"



/*****************************************************************************
** Function name:   	LedInit
** Descriptions:        LedInit≥ı ºªØ
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void LedInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
    
//    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
//	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
}


void GreenLed(void)
{
    static uint8_t GreenledFlag = 0;
    
    if(0 == GreenledFlag++)
    {
        GREEN_LIGHT_ON();
    }
    else
    {
        GreenledFlag = 0;
        GREEN_LIGHT_OFF();
    }                   
}

