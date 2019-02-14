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
#include "tsl_user.h"


uint32_t ReadCardTicks = 0;
uint8_t isSyncSystemInfo = 0;
/************************************************************************************************************/
/* Private typedefs ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

//#define TEST_TKEY(NB) ((MyTKeys[(NB)].p_Data->StateId == TSL_STATEID_DETECT) ||\
 //                      (MyTKeys[(NB)].p_Data->StateId == TSL_STATEID_DEB_RELEASE_DETECT))

#define TEST_TKEY(NB) ((MyTKeys[(NB)].p_Data->StateId == TSL_STATEID_DETECT))
/* Private variables ---------------------------------------------------------*/

// For debug purpose with STMStudio
uint8_t DS[TSLPRM_TOTAL_TKEYS + TSLPRM_TOTAL_LNRTS]; // To store the States (one per object)
int16_t DD[TSLPRM_TOTAL_TKEYS + (TSLPRM_TOTAL_LNRTS * 3)]; // To store the Deltas (one per channel)
/* Private functions prototype -----------------------------------------------*/

void Init_Std_GPIOs(void);
void ProcessSensors(void);
void SystickDelay(__IO uint32_t nTime);

/* Global variables ----------------------------------------------------------*/

__IO uint32_t Gv_SystickCounter;

/**
  * @brief  Initializes GPIOs not related to Touch-Sensing.
  * @param  None
  * @retval None
  */
void Init_Std_GPIOs(void)
{
 // GPIO_InitPara GPIO_InitStructure;
  
	 rcu_periph_clock_enable(RCU_GPIOA);
	 rcu_periph_clock_enable(RCU_GPIOB);
	 rcu_periph_clock_enable(RCU_GPIOC);
	 rcu_periph_clock_enable(RCU_GPIOD);
	 rcu_periph_clock_enable(RCU_GPIOF);

  // Configure all unused IOs in Analog to reduce current consumption.
  // Excepted PA13 (SWDAT) and PA14 (SWCLK) used for debugging the application.
  GPIO_CTL(GPIOA) |= 0xC3FFFFFF;
  GPIO_PUD(GPIOA) = 0;
  GPIO_CTL(GPIOB)  = 0xFFFFFFFF;
  GPIO_PUD(GPIOB) = 0;
  GPIO_CTL(GPIOC) = 0xFFFFFFFF;
  GPIO_PUD(GPIOC) = 0;
  GPIO_CTL(GPIOD) = 0xFFFFFFFF;
  GPIO_PUD(GPIOD) = 0;
  GPIO_CTL(GPIOF)  = 0xFFFFFFFF;
  GPIO_PUD(GPIOF) = 0;
 
}


/**
  * @brief  Manage the activity on sensors when touched/released (example)
  * @param  None
  * @retval None
  */
uint8_t key_value=0xff;
void ProcessSensors(void)
{
	uint32_t idx;
	uint32_t idx_ds = 0;
	uint32_t idx_dd = 0;
	__IO uint32_t touched = 0;
	GlobalInfo.TSIKeyValue = 0xff;
	if (DS[key_value]  != TSL_STATEID_DETECT)
	{
		key_value = 0xff;
	}

#if TSLPRM_TOTAL_TKEYS > 0  
	// Read all TKeys
	for (idx = 0; idx < TSLPRM_TOTAL_TKEYS; idx++)
	{
		// STMStudio debug
		DS[idx_ds++] = MyTKeys[idx].p_Data->StateId;    
		DD[idx_dd++] = MyTKeys[idx].p_ChD->Delta;
		if (TEST_TKEY(idx))
		{
			if(key_value == idx)
			{	
				return;//OLD Key Value return;
			}
			key_value = idx;
		//	printf("\r\nkey ID: %2d,", idx);
			switch(idx)
			{
				case 0 :
				//	printf("key value=Enter\r\n"); 
					GlobalInfo.TSIKeyValue = 11;
				break;
				case 1 :
				//	printf("key value=8\r\n");     
					GlobalInfo.TSIKeyValue = 8;
				break;
				case 2 :
				//	printf("key value=7\r\n");     
					GlobalInfo.TSIKeyValue = 7;
				break;
				case 3 :
				//	printf("key value=0\r\n");     
					GlobalInfo.TSIKeyValue = 0;
				break;//
				case 4 :
				//	printf("key value=ESC\r\n");   
					GlobalInfo.TSIKeyValue = 10;
				break;
				case 5 :
				//	printf("key value=9\r\n");    
					GlobalInfo.TSIKeyValue = 9;
				break;
				case 6 :
				//	printf("key value=3\r\n");     
					GlobalInfo.TSIKeyValue = 3;
				break;
				case 7 :
				//	printf("key value=2\r\n");  
					GlobalInfo.TSIKeyValue = 2;
				break;//
				case 8 :
				//	printf("key value=1\r\n");   
					GlobalInfo.TSIKeyValue = 1;
				break;//
				case 9 :
				//	printf("key value=6\r\n");   
					GlobalInfo.TSIKeyValue = 6;
				break;
				case 10:
				//	printf("key value=5\r\n");     
					GlobalInfo.TSIKeyValue = 5;
				break;
				case 11:
				//	printf("key value=4\r\n");  
					GlobalInfo.TSIKeyValue = 4;
				break;
				default: 
				//	GlobalInfo.TSIKeyValue = 0xff;
					printf("Error!!\r\n");  
				break;
			}
			if(((GlobalInfo.GetKeyTicks + 200) <= GetTimeTicks()) || (GlobalInfo.GetKeyTicks > GetTimeTicks()))
		    {
		        GlobalInfo.GetKeyTicks = GetTimeTicks();
		       // GetKey();
				KeyCallBack(GlobalInfo.TSIKeyValue);
		    }
		}
	}

	
#endif

}


/**
  * @brief  Executed when a sensor is in Error state
  * @param  None
  * @retval None
  */
void MyTKeys_ErrorStateProcess(void)
{
  // Add here your own processing when a sensor is in Error state

	for (;;)
	{
		SystickDelay(100);
	}
}


/**
  * @brief  Executed when a sensor is in Off state
  * @param  None
  * @retval None
  */
void MyTKeys_OffStateProcess(void)
{
  // Add here your own processing when a sensor is in Off state
}


//-------------------
// CallBack functions
//-------------------

/**
  * @brief  Executed at each timer interruption (option must be enabled)
  * @param  None
  * @retval None
  */
void TSL_CallBack_TimerTick(void)
{
  // Add here your own processing
}



/**
  * @brief  Add a delay using the Systick
  * @param  nTime Delay in milliseconds.
  * @retval None
  */
void SystickDelay(__IO uint32_t nTime)
{
  Gv_SystickCounter = nTime;
  while (Gv_SystickCounter != 0)
  {
    // The Gv_SystickCounter variable is decremented every ms by the Systick interrupt routine  
  }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/


/************************************************************************************************************/
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
   // WatchDogInit();             //看门狗初始化
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
	//TsiKeyInit();//
	 TSL_user_Init();
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
	Init_Std_GPIOs();//
    BspInit();
	BuleReconnect();
 //   SystickDelay(800);
	while(1)
    {
		if (TSL_user_Action() == TSL_STATUS_OK)
		{
			ProcessSensors(); // Execute sensors related tasks
		}
		else
		{
			// Execute other tasks...
		}
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
			// GetKeyHandle();
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





