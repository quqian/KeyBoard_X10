
#include "led.h"
#include "system.h"


/*****************************************************************************
** Function name:   	LedInit
** Descriptions:        LedInit初始化
** input parameters:    
** Returned value:	    None
** Author:              quqian
*****************************************************************************/
void LedInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
	gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	
    GREEN_LIGHT_ON();
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

void GreenLedHandle(void)
{
	static uint32_t GreenLedTicks = 0;

 	if(0xa5 == GlobalInfo.BlueLedFlag)
	{
		if(((GreenLedTicks + 3000) <= GetTimeTicks()) || (GreenLedTicks > GetTimeTicks()))
	    {
	        GreenLedTicks = GetTimeTicks();
			GreenLed();
	    }
	}
	else
	{
		if(((GreenLedTicks + 500) <= GetTimeTicks()) || (GreenLedTicks > GetTimeTicks()))
	    {
	        GreenLedTicks = GetTimeTicks();
			GreenLed();
	    //    CL_LOG("SystemCoreClock[%d]\n", SystemCoreClock);
	        #if 0
	        timeaaa = GetRtcTimeStamp();+
	        CL_LOG("时间戳[%d]\n", (uint32_t)timeaaa);
	        #endif
	    }
	}
}


