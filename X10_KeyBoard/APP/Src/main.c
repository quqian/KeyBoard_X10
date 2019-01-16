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
#include "App_BlueProto.h"



uint32_t ReadCardTicks = 0;
uint8_t isSyncSystemInfo = 0;


void MainBoardDataHandle(void)
{
	static uint32_t SystemTimerTicks = 0;
	static uint32_t SystemStatusTicks = 0;
	
	//同步系统信息到X10p
	if((0 == isSyncSystemInfo) && (((SystemTimerTicks + 5000) <= GetTimeTicks()) || (SystemTimerTicks > GetTimeTicks())))
    {
        SystemTimerTicks = GetTimeTicks();
		
		SyncSystemInfo();
    }
    //同步系统状态到X10p
    if(((SystemStatusTicks + 30000) <= GetTimeTicks()) || (SystemStatusTicks > GetTimeTicks()))
	{
        SystemStatusTicks = GetTimeTicks();

        SyncSystemState();
    }
}

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
#if EN_BLUETOOTH
    //蓝牙接口初始化
    Bluetooth_Init();
#else
    SystemInfo.blue_state = 1;
    SystemStatus.blue_state = 1;
#endif
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
	uint32_t TimeFlagTicks = GetTimeTicks();
    
	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//设置Flash地址偏移
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//设置系统中断优先级分组4	
	
    BspInit();
	BuleReconnect();
    
	while(1)
    {
        FeedWatchDog();

        #if EN_BLUETOOTH
			//透传蓝牙模块数据
            HandleBlueMsg();
        #endif
		TimeFlagTicks = GetTimeTicks();
        {
        	//led灯
			GreenLedHandle();
			//触摸按键
            GetKeyHandle();
			//处理通信数据
            ComRecvMainBoardData();
			//产测软件
 			//   DebugRecvProc(); 
			//刷卡操作
			NFCardTaskHandle();
			//与主板的握手和心跳
			MainBoardDataHandle();
			//处理蓝牙数据
			BlueToothHandle();
        }
    }
}





