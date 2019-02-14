/**
  ******************************************************************************
  * @file    tsl_time_stm32f0xx.c
  * @author  MCD Application Team
  * @version V1.4.4
  * @date    31-March-2014
  * @brief   This file contains all functions to manage the timing with STM32F0xx products.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tsl_time_stm32f0xx.h"

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype -----------------------------------------------*/

/**
  * @brief  Initialization of the timing module.
  * @param  None
  * @retval Status Return TSL_STATUS_ERROR if the Systick configuration has failed.
  */
TSL_Status_enum_T TSL_tim_Init(void)
{
	
	  timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER13);
    nvic_irq_enable(TIMER13_IRQn, 0, 0);
    timer_deinit(TIMER13);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = SystemCoreClock/1000000;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = TSLPRM_TICK_FREQ-1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER13,&timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER13);
    timer_interrupt_enable(TIMER13,TIMER_INT_UP);
    /* TIMER0 counter enable */
    timer_enable(TIMER13);
	  return TSL_STATUS_OK;
//  // Program one systick interrupt every (1 / TSLPRM_TICK_FREQ) ms
//  if (SysTick_Config(SystemCoreClock / TSLPRM_TICK_FREQ))
//  {
//    return TSL_STATUS_ERROR;
//  }
//  else
//  {
//    return TSL_STATUS_OK;
//  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
