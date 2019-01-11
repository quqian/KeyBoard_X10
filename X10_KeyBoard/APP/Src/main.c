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
	uint32_t NFCardTicks = GetTimeTicks();
	uint32_t GreenLedTicks = NFCardTicks;
    uint32_t GetKeyTicks = NFCardTicks;
	uint32_t TimeFlagTicks = GetTimeTicks();
	uint32_t SystemTimerTicks = NFCardTicks;
	uint32_t SystemStatusTicks = NFCardTicks;
	
	ReadCardTicks = NFCardTicks;
 //    int64_t timeaaa;
    
	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//设置Flash地址偏移
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//设置系统中断优先级分组4	
	
    BspInit();
	BuleReconnect();
    
	while(1)
    {
        FeedWatchDog();
		
 //       BlueSendCmd("AT\r\n", "OK", 3000);
        #if EN_BLUETOOTH
            //蓝牙
            HandleBlueMsg();
        #endif
		TimeFlagTicks = GetTimeTicks();
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
			
            if(((GetKeyTicks + 350) <= GetTimeTicks()) || (GetKeyTicks > GetTimeTicks()))
            {
                GetKeyTicks = GetTimeTicks();
                GetKey();
            }
            
			//处理通信数据
            ComRecvMainBoardData();
 			 
			if((((NFCardTicks + 100) <= TimeFlagTicks) || (NFCardTicks > TimeFlagTicks)) && (0 == SystemStatus.card_state))
            {
                NFCardTicks = TimeFlagTicks;
				if(GlobalInfo.UpgradeFlag != 0xa5)
				{
				//	if(CL_OK == BswDrv_FM175XX_SetPowerDown(0)) 	//退出睡眠
					{
						NFCardTask();
					}
				}
             //   BswDrv_FM175XX_SetPowerDown(1);			//进入睡眠
                FeedWatchDog();
            }
			
            if(((ReadCardTicks + 5000) <= GetTimeTicks()) || (ReadCardTicks > GetTimeTicks()))
            {
                ReadCardTicks = GetTimeTicks();
				GlobalInfo.cardFlag = 1;
            }
         //   DebugRecvProc();
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

			
		#if EN_BLUETOOTH
            //如果没有蓝牙链接，定期检测CK模块是否正常工作
            if(SystemStatus.blue_state == 0                            //认为当前蓝牙状态是OK的
                && GlobalInfo.isBuleConnect == 0                                   //没有APP链接到蓝牙
                && (GetTimeTicks() - GlobalInfo.blueTestTime >= 60000))  //1分钟监测一次
            {
                GlobalInfo.blueTestTime = GetTimeTicks();
                if(BlueTest(1) != OK)
				{
                    CL_LOG("检测蓝牙错误.\n");
                    SystemStatus.blue_state = 1;
                    GlobalInfo.rebootBlueCnt++;
                }
				else
				{
                    SystemStatus.blue_state = 0;
                    GlobalInfo.rebootBlueCnt = 0;
                    CL_LOG("蓝牙检测成功.\n");
                }
            }

			#if 0
             //如果蓝牙异常，重启蓝牙设备，最多重启3次
            if(SystemInfo.blue_state ==0               		//有蓝牙设备
                && GlobalInfo.rebootBlueCnt <= 5                   //重启次数
                && SystemStatus.blue_state == 1)			//当前获取不到蓝牙信息
           	{      
				if(BuleReset_Async() == OK)
			   	{
                    CL_LOG("复位蓝牙.\n");
                    GlobalInfo.blueTestTime = GetTimeTicks() - (55000);
               }
            }
			#endif
            //判断蓝牙是否链接  2分钟没有收到数据就认为空闲
            if(GlobalInfo.isBuleConnect == 1 && (GetTimeTicks() - GlobalInfo.lastConnectTime > 120000))
			{
                GlobalInfo.lastConnectTime = GetTimeTicks();
                GlobalInfo.isBuleConnect = 0;
                CL_LOG("打开检测蓝牙标志.\n");
            }
		#endif
        }
    }
}





