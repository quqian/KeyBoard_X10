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
	
	//ͬ��ϵͳ��Ϣ��X10p
	if((0 == isSyncSystemInfo) && (((SystemTimerTicks + 5000) <= GetTimeTicks()) || (SystemTimerTicks > GetTimeTicks())))
    {
        SystemTimerTicks = GetTimeTicks();
		
		SyncSystemInfo();
    }
    //ͬ��ϵͳ״̬��X10p
    if(((SystemStatusTicks + 30000) <= GetTimeTicks()) || (SystemStatusTicks > GetTimeTicks()))
	{
        SystemStatusTicks = GetTimeTicks();

        SyncSystemState();
    }
}

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
	uint32_t TimeFlagTicks = GetTimeTicks();
    
	nvic_vector_table_set(FLASH_BASE, BOOT_SIZE);        	//����Flash��ַƫ��
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);		//����ϵͳ�ж����ȼ�����4	
	
    BspInit();
	BuleReconnect();
    
	while(1)
    {
        FeedWatchDog();

        #if EN_BLUETOOTH
			//͸������ģ������
            HandleBlueMsg();
        #endif
		TimeFlagTicks = GetTimeTicks();
        {
        	//led��
			GreenLedHandle();
			//��������
            GetKeyHandle();
			//����ͨ������
            ComRecvMainBoardData();
			//�������
 			//   DebugRecvProc(); 
			//ˢ������
			NFCardTaskHandle();
			//����������ֺ�����
			MainBoardDataHandle();
			//������������
			BlueToothHandle();
        }
    }
}





