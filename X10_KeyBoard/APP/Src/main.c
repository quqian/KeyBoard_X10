/*****************************************************************************
** @Author: quqian
** @Date: 2018-12-21 14:20:14 
** @File: main.c
** @MCU: GD32F350CBT6   
** @MCU max Speed: 108M
** @MCU Flash: 128K
** @MCU RAM: 16K
** @MCU Package: LQFP48
** @Last Modified by: quqian
** @Last Modified time: 2018-12-21 14:20:14 
*****************************************************************************/
#include "includes.h"
#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
//#include <task.h>
//#include "wifi.h"
#include "rtc.h"
#include "system.h"
#include "usart.h"
#include "flash.h"
#include "flash_usr.h"
#include "usart.h"
#include "spi.h"
#include "NFCard.h"
#include "GPRS_4G.h"
#include "gd32f3x0_libopt.h"
#include "system.h"
#include "sc8042b.h"
#include "fm175xx.h"
#include "NFCard.h"
#include "sim_uart.h"
#include "ComProto.h"
#include "led.h"
#include "tsiKey.h"



uint32_t ReadCardTicks = 0;
uint8_t cardFlag = 1;
uint8_t isSyncSystemInfo = 0;



void BspInit(void)
{
    LedInit();
    WatchDogInit();             //看门狗初始化
    FeedWatchDog();
#if USE_TIMER1
    TimerConfig(1, 1000, 1);
#endif
	SystickInit();
	SimUartInit();
    DelayMsWithNoneOs(1000);
	UsartInit();                //串口初始化
	RtcInit();
    DelayMsWithNoneOs(1000);
	LoadSystemInfo();
    SystemResetRecord();
    printf("UsartInit OK!\r\n");
	TsiKeyInit();
//    SC8042B_Init();
	FM175XX_Config();

////语音提示    
//    PlayVoice(VOIC_WELCOME);            //欢迎使用
//    DelayMsWithNoneOs(900);
////    PlayVoice(VOIC_SHARE_CHARGE);       //共享充电
//    DelayMsWithNoneOs(900);
    printf("\r\n BspInit ok\n");
    printf("\n===========================================================\n\n\n");
}


int main(void)
{
	uint32_t NFCardTicks = GetTimeTicks();
	uint32_t GreenLedTicks = NFCardTicks;
	uint32_t TimeFlagTicks = GetTimeTicks();
	uint32_t SystemTimerTicks = NFCardTicks;
	uint32_t SystemStatusTicks = NFCardTicks;
	
	ReadCardTicks = NFCardTicks;
//    int64_t timeaaa;
    
	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//设置Flash地址偏移
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//设置系统中断优先级分组4	
	
    BspInit();
	
	while(1)
    {
        FeedWatchDog();
		
		TimeFlagTicks = GetTimeTicks();
        {
        #if 1
			if(((GreenLedTicks + 500) <= GetTimeTicks()) || (GreenLedTicks > GetTimeTicks()))
            {
                GreenLedTicks = GetTimeTicks();
                GreenLed();
            //    CL_LOG("SystemCoreClock[%d]\n", SystemCoreClock);
                #if 0
                timeaaa = GetRtcTimeStamp();
                timeaaa = ((long long)(RTC_TIMER_STAMEP) + timeaaa);
                CL_LOG("时间戳[%d]\n", (uint32_t)timeaaa);
                #endif
                GetKey();
            }
			
			//处理通信数据
            ComRecvMainBoardData();

			if(((NFCardTicks + 500) <= TimeFlagTicks) || (NFCardTicks > TimeFlagTicks))
            {
                NFCardTicks = TimeFlagTicks;
				if(GlobalInfo.UpgradeFlag != 0xa5)
				{
					if(CL_OK == BswDrv_FM175XX_SetPowerDown(0)) 	//退出睡眠
					{
						NFCardTask();
					}
				}
                BswDrv_FM175XX_SetPowerDown(1);			//进入睡眠
                FeedWatchDog();
            }
            if(((ReadCardTicks + 5000) <= TimeFlagTicks) || (ReadCardTicks > TimeFlagTicks))
            {
                ReadCardTicks = TimeFlagTicks;
				cardFlag = 1;
            }
            DebugRecvProc();
			//同步系统信息到X10p
			if((0 == isSyncSystemInfo) && (((SystemTimerTicks + 5000) <= TimeFlagTicks) || (SystemTimerTicks > TimeFlagTicks)))
            {
                SystemTimerTicks = TimeFlagTicks;
				
				SyncSystemInfo();
            }
            //同步系统状态到X10p
	        if(((SystemStatusTicks + 30000) <= TimeFlagTicks) || (SystemStatusTicks > TimeFlagTicks))
			{
	            SystemStatusTicks = TimeFlagTicks;

	            SyncSystemState();
	        }
			
		#endif
        }
    }
}





