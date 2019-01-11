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
    WatchDogInit();             //���Ź���ʼ��
    FeedWatchDog();
#if USE_TIMER1
    TimerConfig(1, 1000, 1);
#endif
	SystickInit();
	SimUartInit();
    DelayMsWithNoneOs(1000);
	UsartInit();                //���ڳ�ʼ��
	RtcInit();
    DelayMsWithNoneOs(1000);
	LoadSystemInfo();
    SystemResetRecord();
    printf("UsartInit OK!\r\n");
	TsiKeyInit();
#if EN_BLUETOOTH
    //�����ӿڳ�ʼ��
    Bluetooth_Init();
#else
    SystemInfo.blue_state = 1;
    SystemStatus.blue_state = 1;
#endif
//    SC8042B_Init();
	FM175XX_Config();

////������ʾ    
//    PlayVoice(VOIC_WELCOME);            //��ӭʹ��
//    DelayMsWithNoneOs(900);
////    PlayVoice(VOIC_SHARE_CHARGE);       //������
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
    
	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//����Flash��ַƫ��
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//����ϵͳ�ж����ȼ�����4	
	
    BspInit();
	BuleReconnect();
    
	while(1)
    {
        FeedWatchDog();
		
 //       BlueSendCmd("AT\r\n", "OK", 3000);
        #if EN_BLUETOOTH
            //����
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
                CL_LOG("ʱ���[%d]\n", (uint32_t)timeaaa);
                #endif
            }
			
            if(((GetKeyTicks + 350) <= GetTimeTicks()) || (GetKeyTicks > GetTimeTicks()))
            {
                GetKeyTicks = GetTimeTicks();
                GetKey();
            }
            
			//����ͨ������
            ComRecvMainBoardData();
 			 
			if((((NFCardTicks + 100) <= TimeFlagTicks) || (NFCardTicks > TimeFlagTicks)) && (0 == SystemStatus.card_state))
            {
                NFCardTicks = TimeFlagTicks;
				if(GlobalInfo.UpgradeFlag != 0xa5)
				{
				//	if(CL_OK == BswDrv_FM175XX_SetPowerDown(0)) 	//�˳�˯��
					{
						NFCardTask();
					}
				}
             //   BswDrv_FM175XX_SetPowerDown(1);			//����˯��
                FeedWatchDog();
            }
			
            if(((ReadCardTicks + 5000) <= GetTimeTicks()) || (ReadCardTicks > GetTimeTicks()))
            {
                ReadCardTicks = GetTimeTicks();
				GlobalInfo.cardFlag = 1;
            }
         //   DebugRecvProc();
			//ͬ��ϵͳ��Ϣ��X10p
			if((0 == isSyncSystemInfo) && (((SystemTimerTicks + 5000) <= TimeFlagTicks) || (SystemTimerTicks > TimeFlagTicks)))
            {
                SystemTimerTicks = TimeFlagTicks;
				
				SyncSystemInfo();
            }
            //ͬ��ϵͳ״̬��X10p
	        if(((SystemStatusTicks + 30000) <= TimeFlagTicks) || (SystemStatusTicks > TimeFlagTicks))
			{
	            SystemStatusTicks = TimeFlagTicks;

	            SyncSystemState();
	        }

			
		#if EN_BLUETOOTH
            //���û���������ӣ����ڼ��CKģ���Ƿ���������
            if(SystemStatus.blue_state == 0                            //��Ϊ��ǰ����״̬��OK��
                && GlobalInfo.isBuleConnect == 0                                   //û��APP���ӵ�����
                && (GetTimeTicks() - GlobalInfo.blueTestTime >= 60000))  //1���Ӽ��һ��
            {
                GlobalInfo.blueTestTime = GetTimeTicks();
                if(BlueTest(1) != OK)
				{
                    CL_LOG("�����������.\n");
                    SystemStatus.blue_state = 1;
                    GlobalInfo.rebootBlueCnt++;
                }
				else
				{
                    SystemStatus.blue_state = 0;
                    GlobalInfo.rebootBlueCnt = 0;
                    CL_LOG("�������ɹ�.\n");
                }
            }

			#if 0
             //��������쳣�����������豸���������3��
            if(SystemInfo.blue_state ==0               		//�������豸
                && GlobalInfo.rebootBlueCnt <= 5                   //��������
                && SystemStatus.blue_state == 1)			//��ǰ��ȡ����������Ϣ
           	{      
				if(BuleReset_Async() == OK)
			   	{
                    CL_LOG("��λ����.\n");
                    GlobalInfo.blueTestTime = GetTimeTicks() - (55000);
               }
            }
			#endif
            //�ж������Ƿ�����  2����û���յ����ݾ���Ϊ����
            if(GlobalInfo.isBuleConnect == 1 && (GetTimeTicks() - GlobalInfo.lastConnectTime > 120000))
			{
                GlobalInfo.lastConnectTime = GetTimeTicks();
                GlobalInfo.isBuleConnect = 0;
                CL_LOG("�򿪼��������־.\n");
            }
		#endif
        }
    }
}





