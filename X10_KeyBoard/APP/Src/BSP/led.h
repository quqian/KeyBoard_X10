#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "includes.h"



#define GREEN_LIGHT_ON()       			gpio_bit_set(GPIOC, GPIO_PIN_15)
#define GREEN_LIGHT_OFF()      			gpio_bit_reset(GPIOC, GPIO_PIN_15)

extern void LedInit(void);
//extern void RedLed(void);
extern void GreenLed(void);


#endif




