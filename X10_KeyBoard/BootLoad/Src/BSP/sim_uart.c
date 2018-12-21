#include "sim_uart.h"
#include "includes.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_usart.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_misc.h"
#include "systick.h"
#include "gd32f3x0_timer.h"



SIM_UART_STR gSimUartCtrl[SIM_UART_CNT];


int SimuUartSendData(SIM_UART_STR *pSimUart, const uint8_t *pData, uint16_t len)
{
//    uint32_t waitCnt = 4 * len;
//    uint32_t cnt = 0;
    pSimUart->allCnt = len;
    pSimUart->sendCnt = 0;
    pSimUart->pSendBuf = (void*)pData;
    pSimUart->gSendData = pData[0];
    pSimUart->gSendStat = 0;
    pSimUart->VirtualSendFlag = 0;
	timer_enable(TIMER0);
	#if 0
    while (pSimUart->sendCnt < pSimUart->allCnt)	//û�в���ϵͳҪע�͵����while
	{
		osDelay(30);	//
        Feed_WDT();
        if ((waitCnt < ++cnt) || (0xa5 == pSimUart->VirtualSendFlag))
		{
            //timer_disable(TIMER0);
            return CL_FAIL;
        }
    }
	#endif
	
    return CL_OK;
}


void SimUartGPIOInit (void)
{
	rcu_periph_clock_enable(RCU_GPIOB);
    
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_1);
}


void SimUartInit(void)
{
    uint32_t i = 0;
    
	memset(gSimUartCtrl, 0, sizeof(gSimUartCtrl));
	for (i = 0; i < SIM_UART_CNT; i++) 
	{
        gSimUartCtrl[i].recvStat = COM_STOP_BIT;
    }
    gSimUartCtrl[0].timerIndex = 1;
    gSimUartCtrl[0].baudRate = 52; //������ 19200
	SimUartGPIOInit();
	TimerConfig(0, 52);
}


void IO_USART_TX_PutVal(uint8_t val, uint8_t uartIndex)
{
    if (0 == uartIndex) 
	{
    	if (val)
		{
            gpio_bit_set(GPIOB, GPIO_PIN_1);
    	}
		else
		{
            gpio_bit_reset(GPIOB, GPIO_PIN_1);
    	}
    }
}

void SimUartSendByte(uint8_t sendStat, uint8_t sendData, uint8_t uartIndex)
{
	if (sendStat == COM_START_BIT) 
	{//������ʼλ
		IO_USART_TX_PutVal(0, uartIndex);
	}
    else if(sendStat == COM_STOP_BIT) 
	{//stop  high
		IO_USART_TX_PutVal(1, uartIndex);
	}
	else if(sendStat == 10) 
	{//stop  delay
	}
	else
	{
		IO_USART_TX_PutVal(sendData & (1<<(sendStat-1)), uartIndex);
	}
}


void TIMER0_BRK_UP_TRG_COM_IRQHandler(void)
{
#if 1
	SIM_UART_STR *pSimUart = &gSimUartCtrl[0];

	if(RESET != timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_UP))
    {
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);	
		SimUartSendByte(pSimUart->gSendStat, pSimUart->gSendData, 0);
        pSimUart->gSendStat++;
        
        if (11 == pSimUart->gSendStat) 
		{
            if (++pSimUart->sendCnt < pSimUart->allCnt) 
			{
                pSimUart->gSendData = pSimUart->pSendBuf[pSimUart->sendCnt];
                pSimUart->gSendStat = 0;
            }
			else
			{
				pSimUart->VirtualSendFlag = 0xa5;
                timer_disable(TIMER0);
            }
        }
		//gpio_bit_toggle(GPIOA, GPIO_PIN_8);
    }
#endif
}



