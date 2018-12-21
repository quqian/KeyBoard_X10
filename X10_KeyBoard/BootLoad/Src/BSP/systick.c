#include "includes.h"
#include "gd32f3x0.h"
#include "systick.h"
//#include "FreeRTOSConfig.h"
//#include "portmacro.h"
//#include "task.h"

#define TICK_RATE_HZ				(1000U)

static uint32_t DelayCount;


void SysTick_Handler(void)
{	
    //if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
    //{
    //    xPortSysTickHandler();	
    //}
    //gpio_bit_toggle(GPIOA, GPIO_PIN_8);
}

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / TICK_RATE_HZ))
	{
        /* capture error */
        while (1);
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_1ms(uint32_t count)
{
    DelayCount = count;

    while(0U != DelayCount);
}

void delay_decrement(void)
{
    if (0U != DelayCount)
	{
        DelayCount--;
    }
}







static uint8_t  fac_us=0;							//us��ʱ������			   
//static uint16_t fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

void SystickInit(void)
{
	uint32_t reload;
	
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);//ѡ���ⲿʱ��  HCLK
	fac_us = SystemCoreClock / 1000000;				//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	reload = SystemCoreClock / 1000000;				//ÿ���ӵļ������� ��λΪM  
	reload *= 1000000 / TICK_RATE_HZ;			//����configTICK_RATE_HZ�趨���ʱ��
												//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��0.233s����	
//	fac_ms = 1000 / configTICK_RATE_HZ;				//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD = reload; 						//ÿ1/configTICK_RATE_HZ���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
}


void DelayUs(uint32_t us)
{
	uint32_t ticks;
	uint32_t told;
	uint32_t tnow;
	uint32_t tcnt = 0;
	uint32_t reload = SysTick->LOAD;				//LOAD��ֵ	    
	
	ticks = us * fac_us; 						//��Ҫ�Ľ����� 
	told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{
			if(tnow < told)
			{
				tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			}
			else
			{
				tcnt += reload - tnow + told;
			}
			told = tnow;
			if(tcnt >= ticks)
			{
				break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
			}
		}  
	}									    
}  

void DelayMsWithNoneOs(uint32_t nms)
{
	uint32_t i;
	
	for(i = 0; i < nms; i++) 
	{
		DelayUs(1000);
	}

	return;
}



