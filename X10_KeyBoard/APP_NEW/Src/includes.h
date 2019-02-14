#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include "gd32f3x0.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_misc.h"
#include "gd32f3x0_fmc.h"
#include "gd32f3x0_fmc.h"
#include "system.h"
#include "systick.h"
#include "ProtoWithMainBoard.h"
#include "sc8042b.h"
#include "gd32f3x0_rtc.h"
#include "rtc.h"
#include "main.h"

#define TEST_PRINTF_LOG                      1		//正式版本时为 0

#define FW_VERSION                      1		//正式版本时只改主版本
#if TEST_PRINTF_LOG
#define TEST_VERSION                    1		//测试版本才有
#endif

#define DEBUG_INDEX            			0
#define RS232_INDEX            			1




#define Feed_WDT						FeedWatchDog
#define EN_BLUETOOTH					1
#define SET_BLUE_NAME					0
#define BLUE_CONNECT_TICK				(60000)


#define CL_LOG(fmt,args...) do {    \
    printf("[X10KB %s] %s:(%d) " fmt, GetCurrentTime(), __func__, __LINE__, ##args); \
}while(0)

#define PRINTF_LOG(fmt,args...) do {    \
    printf("[X10KB %s]" fmt, GetCurrentTime(), ##args); \
}while(0)


//#define CL_LOG(fmt,args...) do {    \
//    printf("[X10KB %d] %s:(%d) " fmt, GetTimeTicks()/1000, __func__, __LINE__, ##args); \
//}while(0)



#endif



