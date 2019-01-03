#ifndef __TSI_KEY_H__
#define __TSI_KEY_H__

#include "includes.h"
#include "gd32f3x0_rtc.h"
#include "gd32f3x0_timer.h"




//按键返回事件
extern void key_onclick_callback(uint8_t values);
extern void GetKey(void);
extern void TsiKeyInit(void);



#endif


