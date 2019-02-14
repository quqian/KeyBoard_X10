/**
  ******************************************************************************
  * @file    tsl_acq_stm32f0xx.c
  * @author  MCD Application Team
  * @version V1.4.4
  * @date    31-March-2014
  * @brief   This file contains all functions to manage the TSC acquisition
  *          on STM32F0xx products.
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
#include "tsl_acq_stm32f0xx.h"
#include "tsl_globals.h"
//#include "stm32f0xx_it.h"

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define NU      (0) // Not Used IO
#define CHANNEL (1) // Channel IO
#define SHIELD  (2) // Shield IO (= Channel IO but not acquired)
#define SAMPCAP (3) // Sampling Capacitor IO

/* Private macros ------------------------------------------------------------*/

// Used by assert
#define IS_BANK_INDEX_OK(INDEX)    (((INDEX) == 0) || (((INDEX) > 0) && ((INDEX) < TSLPRM_TOTAL_BANKS)))
#define IS_SRC_INDEX_0_5_OK(INDEX) (((INDEX) == 0) || (((INDEX) > 0) && ((INDEX) < 6)))
#define IS_SRC_INDEX_0_7_OK(INDEX) (((INDEX) == 0) || (((INDEX) > 0) && ((INDEX) < 8)))

/* Private variables ---------------------------------------------------------*/
uint32_t DelayDischarge;

/* Private functions prototype -----------------------------------------------*/
void SoftDelay(uint32_t val);

/**
  * @brief  Initializes the TouchSensing GPIOs.
  * @param  None
  * @retval None
  */
void TSL_acq_InitGPIOs(void)
{

 // GPIO_InitPara GPIO_InitStructure;
  uint32_t tmp_value_0;
  uint32_t tmp_value_1;
  uint32_t mode;
	uint32_t pull_up_down;
	uint32_t pin;
	uint8_t otype;
	uint32_t speed;

  //====================
  // GPIOs configuration
  //====================

  // Enable GPIOs clocks
  //RCC->AHBCCR |= (RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOC
	 rcu_periph_clock_enable(RCU_GPIOA);
	 rcu_periph_clock_enable(RCU_GPIOB);
	 rcu_periph_clock_enable(RCU_GPIOC);
	 rcu_periph_clock_enable(RCU_GPIOF);
	
	
//	  gpio_mode_set(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_14);
//    gpio_output_options_set(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_2MHZ,GPIO_PIN_14);

//    /* connect pin to peripheral */
//    gpio_af_set(GPIOB,GPIO_AF_3,GPIO_PIN_11);

  // Alternate function Output Open-Drain for Sampling Capacitor IOs
  //----------------------------------------------------------------

  mode = GPIO_MODE_AF;
  otype = GPIO_OTYPE_OD;
  speed = GPIO_OSPEED_2MHZ;
  pull_up_down = GPIO_PUPD_NONE;

  // GPIOA
  pin= 0;
#if TSLPRM_TSC_GROUP1_IO1 == SAMPCAP
  pin |= GPIO_PIN_0;
#endif
#if TSLPRM_TSC_GROUP1_IO2 == SAMPCAP
  pin|= GPIO_PIN_1;
#endif
#if TSLPRM_TSC_GROUP1_IO3 == SAMPCAP
  pin|= GPIO_PIN_2;
#endif
#if TSLPRM_TSC_GROUP1_IO4 == SAMPCAP
  pin |= GPIO_PIN_3;
#endif
#if TSLPRM_TSC_GROUP2_IO1 == SAMPCAP
  pin |= GPIO_PIN_4;
#endif
#if TSLPRM_TSC_GROUP2_IO2 == SAMPCAP
  pin |= GPIO_PIN_5;
#endif
#if TSLPRM_TSC_GROUP2_IO3 == SAMPCAP
  pin |= GPIO_PIN_6;
#endif
#if TSLPRM_TSC_GROUP2_IO4 == SAMPCAP
  pin |= GPIO_PIN_7;
#endif
#if TSLPRM_TSC_GROUP4_IO1 == SAMPCAP
  pin |= GPIO_PIN_9;
#endif
#if TSLPRM_TSC_GROUP4_IO2 == SAMPCAP
  pin |= GPIO_PIN_10;
#endif
#if TSLPRM_TSC_GROUP4_IO3 == SAMPCAP
  pin |= GPIO_PIN_11;
#endif
#if TSLPRM_TSC_GROUP4_IO4 == SAMPCAP
  pin |= GPIO_PIN_12;
#endif
  //GPIO_Init(GPIOA, &GPIO_InitStructure);

  gpio_mode_set( GPIOA,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOA,  otype,  speed,  pin);

  // GPIOB
  pin = 0;
#if TSLPRM_TSC_GROUP3_IO2 == SAMPCAP
  pin |= GPIO_PIN_0;
#endif
#if TSLPRM_TSC_GROUP3_IO3 == SAMPCAP
  pin |= GPIO_PIN_1;
#endif
#if TSLPRM_TSC_GROUP3_IO4 == SAMPCAP
  pin|= GPIO_PIN_2;
#endif
#if TSLPRM_TSC_GROUP5_IO1 == SAMPCAP
  pin |= GPIO_PIN_3;
#endif
#if TSLPRM_TSC_GROUP5_IO2 == SAMPCAP
  pin |= GPIO_PIN_4;
#endif
#if TSLPRM_TSC_GROUP5_IO3 == SAMPCAP
  pin|= GPIO_PIN_6;
#endif
#if TSLPRM_TSC_GROUP5_IO4 == SAMPCAP
  pin |= GPIO_PIN_7;
#endif
#if TSLPRM_TSC_GROUP6_IO1 == SAMPCAP
  pin |= GPIO_PIN_11;
#endif
#if TSLPRM_TSC_GROUP6_IO2 == SAMPCAP
  pin |= GPIO_PIN_12;
#endif
#if TSLPRM_TSC_GROUP6_IO3 == SAMPCAP
  pin |= GPIO_PIN_13;
#endif
#if TSLPRM_TSC_GROUP6_IO4 == SAMPCAP
  pin |= GPIO_PIN_14;
#endif
  gpio_mode_set( GPIOB,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOB,  otype,  speed,  pin);

  // GPIOC
#if TSLPRM_TSC_GROUP3_IO1 == SAMPCAP
  pin = GPIO_PIN_5;
  gpio_mode_set( GPIOC,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOC,  otype,  speed,  pin);
#endif

#if (TSC_GROUP8_ENABLED > 0)
  // GPIOD
  pin = 0;
#if TSLPRM_TSC_GROUP8_IO1 == SAMPCAP
  pin |= GPIO_PIN_12;
#endif
#if TSLPRM_TSC_GROUP8_IO2 == SAMPCAP
  pin |= GPIO_PIN_13;
#endif
#if TSLPRM_TSC_GROUP8_IO3 == SAMPCAP
  pin |= GPIO_PIN_14;
#endif
#if TSLPRM_TSC_GROUP8_IO4 == SAMPCAP
  pin |= GPIO_PIN_15;
#endif
  gpio_mode_set( GPIOD,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOD,  otype,  speed,  pin);
#endif // TSC_GROUP8_ENABLED

#if (TSC_GROUP7_ENABLED > 0)
// GPIOE
  pin = 0;
#if TSLPRM_TSC_GROUP7_IO1 == SAMPCAP
  pin |= GPIO_PIN_2;
#endif
#if TSLPRM_TSC_GROUP7_IO2 == SAMPCAP
  pin |= GPIO_PIN_3;
#endif
#if TSLPRM_TSC_GROUP7_IO3 == SAMPCAP
  pin |= GPIO_PIN_4;
#endif
#if TSLPRM_TSC_GROUP7_IO4 == SAMPCAP
  pin |= GPIO_PIN_5;
#endif
  gpio_mode_set( GPIOE,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOE,  otype,  speed,  pin);
#endif // TSC_GROUP7_ENABLED

  // Alternate function Output Push-Pull for Channel and Shield IOs
  //---------------------------------------------------------------

   otype = GPIO_OTYPE_PP;

  // GPIOA
  pin = 0;
#if (TSLPRM_TSC_GROUP1_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP1_IO1 == SHIELD)
  pin |= GPIO_PIN_0;
#endif
#if (TSLPRM_TSC_GROUP1_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP1_IO2 == SHIELD)
  pin |= GPIO_PIN_1;
#endif
#if (TSLPRM_TSC_GROUP1_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP1_IO3 == SHIELD)
  pin |= GPIO_PIN_2;
#endif
#if (TSLPRM_TSC_GROUP1_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP1_IO4 == SHIELD)
  pin |= GPIO_PIN_3;
#endif
#if (TSLPRM_TSC_GROUP2_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP2_IO1 == SHIELD)
  pin |= GPIO_PIN_4;
#endif
#if (TSLPRM_TSC_GROUP2_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP2_IO2 == SHIELD)
  pin |= GPIO_PIN_5;
#endif
#if (TSLPRM_TSC_GROUP2_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP2_IO3 == SHIELD)
 pin |= GPIO_PIN_6;
#endif
#if (TSLPRM_TSC_GROUP2_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP2_IO4 == SHIELD)
  pin |= GPIO_PIN_7;
#endif
#if (TSLPRM_TSC_GROUP4_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP4_IO1 == SHIELD)
  pin |= GPIO_PIN_9;
#endif
#if (TSLPRM_TSC_GROUP4_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP4_IO2 == SHIELD)
  pin |= GPIO_PIN_10;
#endif
#if (TSLPRM_TSC_GROUP4_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP4_IO3 == SHIELD)
  pin |= GPIO_PIN_11;
#endif
#if (TSLPRM_TSC_GROUP4_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP4_IO4 == SHIELD)
 pin |= GPIO_PIN_12;
#endif
  gpio_mode_set( GPIOA,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOA,  otype,  speed,  pin);

  // GPIOB
  pin = 0;
#if (TSLPRM_TSC_GROUP3_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP3_IO2 == SHIELD)
  pin |= GPIO_PIN_0;
#endif
#if (TSLPRM_TSC_GROUP3_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP3_IO3 == SHIELD)
  pin |= GPIO_PIN_1;
#endif
#if (TSLPRM_TSC_GROUP3_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP3_IO4 == SHIELD)
 pin |= GPIO_PIN_2;
#endif
#if (TSLPRM_TSC_GROUP5_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP5_IO1 == SHIELD)
  pin |= GPIO_PIN_3;
#endif
#if (TSLPRM_TSC_GROUP5_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP5_IO2 == SHIELD)
 pin |= GPIO_PIN_4;
#endif
#if (TSLPRM_TSC_GROUP5_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP5_IO3 == SHIELD)
  pin |= GPIO_PIN_6;
#endif
#if (TSLPRM_TSC_GROUP5_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP5_IO4 == SHIELD)
  pin |= GPIO_PIN_7;
#endif
#if (TSLPRM_TSC_GROUP6_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP6_IO1 == SHIELD)
  pin |= GPIO_PIN_11;
#endif
#if (TSLPRM_TSC_GROUP6_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP6_IO2 == SHIELD)
  pin |= GPIO_PIN_12;
#endif
#if (TSLPRM_TSC_GROUP6_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP6_IO3 == SHIELD)
  pin |= GPIO_PIN_13;
#endif
#if (TSLPRM_TSC_GROUP6_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP6_IO4 == SHIELD)
  pin|= GPIO_PIN_14;
#endif
  gpio_mode_set( GPIOB,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOB,  otype,  speed,  pin);

  // GPIOC
#if (TSLPRM_TSC_GROUP3_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP3_IO1 == SHIELD)
  pin = GPIO_PIN_5;
  gpio_mode_set( GPIOC,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOC,  otype,  speed,  pin);
#endif

#if (TSC_GROUP8_ENABLED > 0)
  // GPIOD
  pin = 0;
#if (TSLPRM_TSC_GROUP8_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP8_IO1 == SHIELD)
  pin |= GPIO_PIN_12;
#endif
#if (TSLPRM_TSC_GROUP8_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP8_IO2 == SHIELD)
 pin |= GPIO_PIN_13;
#endif
#if (TSLPRM_TSC_GROUP8_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP8_IO3 == SHIELD)
  pin |= GPIO_PIN_14;
#endif
#if (TSLPRM_TSC_GROUP8_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP8_IO4 == SHIELD)
 pin |= GPIO_PIN_15;
#endif
   gpio_mode_set( GPIOD,  mode,  pull_up_down,  pin);
   gpio_output_options_set( GPIOD,  otype,  speed,  pin);
#endif // TSC_GROUP8_ENABLED

#if (TSC_GROUP7_ENABLED > 0)
  // GPIOE
  pin = 0;
#if (TSLPRM_TSC_GROUP7_IO1 == CHANNEL) || (TSLPRM_TSC_GROUP7_IO1 == SHIELD)
  pin |= GPIO_PIN_2;
#endif
#if (TSLPRM_TSC_GROUP7_IO2 == CHANNEL) || (TSLPRM_TSC_GROUP7_IO2 == SHIELD)
  pin |= GPIO_PIN_3;
#endif
#if (TSLPRM_TSC_GROUP7_IO3 == CHANNEL) || (TSLPRM_TSC_GROUP7_IO3 == SHIELD)
  pin|= GPIO_PIN_4;
#endif
#if (TSLPRM_TSC_GROUP7_IO4 == CHANNEL) || (TSLPRM_TSC_GROUP7_IO4 == SHIELD)
  pin|= GPIO_PIN_5;
#endif
    gpio_mode_set( GPIOE,  mode,  pull_up_down,  pin);
  gpio_output_options_set( GPIOE,  otype,  speed,  pin);
#endif // TSC_GROUP7_ENABLED

  // Set Alternate-Function AF3 for GPIOA and GPIOB
  //-----------------------------------------------

  // GPIOA
  tmp_value_0 = 0;
  tmp_value_1 = 0;
#if TSLPRM_TSC_GROUP1_IO1 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (0 * 4));
#endif
#if TSLPRM_TSC_GROUP1_IO2 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (1 * 4));
#endif
#if TSLPRM_TSC_GROUP1_IO3 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (2 * 4));
#endif
#if TSLPRM_TSC_GROUP1_IO4 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (3 * 4));
#endif
#if TSLPRM_TSC_GROUP2_IO1 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (4 * 4));
#endif
#if TSLPRM_TSC_GROUP2_IO2 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (5 * 4));
#endif
#if TSLPRM_TSC_GROUP2_IO3 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (6 * 4));
#endif
#if TSLPRM_TSC_GROUP2_IO4 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (7 * 4));
#endif
#if TSLPRM_TSC_GROUP4_IO1 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (1 * 4));
#endif
#if TSLPRM_TSC_GROUP4_IO2 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (2 * 4));
#endif
#if TSLPRM_TSC_GROUP4_IO3 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (3 * 4));
#endif
#if TSLPRM_TSC_GROUP4_IO4 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (4 * 4));
#endif
  GPIO_AFSEL0(GPIOA) |= tmp_value_0;
  GPIO_AFSEL1(GPIOA) |= tmp_value_1;

  // GPIOB
  tmp_value_0 = 0;
  tmp_value_1 = 0;
#if TSLPRM_TSC_GROUP3_IO2 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (0 * 4));
#endif
#if TSLPRM_TSC_GROUP3_IO3 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (1 * 4));
#endif
#if TSLPRM_TSC_GROUP3_IO4 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (2 * 4));
#endif
#if TSLPRM_TSC_GROUP5_IO1 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (3 * 4));
#endif
#if TSLPRM_TSC_GROUP5_IO2 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (4 * 4));
#endif
#if TSLPRM_TSC_GROUP5_IO3 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (6 * 4));
#endif
#if TSLPRM_TSC_GROUP5_IO4 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)3 << (7 * 4));
#endif
#if TSLPRM_TSC_GROUP6_IO1 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (3 * 4));
#endif
#if TSLPRM_TSC_GROUP6_IO2 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (4 * 4));
#endif
#if TSLPRM_TSC_GROUP6_IO3 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (5 * 4));
#endif
#if TSLPRM_TSC_GROUP6_IO4 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)3 << (6 * 4));
#endif
  GPIO_AFSEL0(GPIOB) |= tmp_value_0;
  GPIO_AFSEL1(GPIOB) |= tmp_value_1;

  // Set Alternate-Function AF1 for GPIOD and GPIOE
  //-----------------------------------------------

#if (TSC_GROUP8_ENABLED > 0)
  // GPIOD
  tmp_value_1 = 0;
#if TSLPRM_TSC_GROUP8_IO1 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)1 << (4 * 4));
#endif
#if TSLPRM_TSC_GROUP8_IO2 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)1 << (5 * 4));
#endif
#if TSLPRM_TSC_GROUP8_IO3 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)1 << (6 * 4));
#endif
#if TSLPRM_TSC_GROUP8_IO4 != NU
  tmp_value_1 |= (uint32_t)((uint32_t)1 << (7 * 4));
#endif
  GPIO_AFSEL1(GPIOD) |= tmp_value_1;
#endif // TSC_GROUP8_ENABLED

#if (TSC_GROUP7_ENABLED > 0)
  // GPIOE
  tmp_value_0 = 0;
#if TSLPRM_TSC_GROUP7_IO1 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)1 << (2 * 4));
#endif
#if TSLPRM_TSC_GROUP7_IO2 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)1 << (3 * 4));
#endif
#if TSLPRM_TSC_GROUP7_IO3 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)1 << (4 * 4));
#endif
#if TSLPRM_TSC_GROUP7_IO4 != NU
  tmp_value_0 |= (uint32_t)((uint32_t)1 << (5 * 4));
#endif
   GPIO_AFSEL0(GPIOE) |= tmp_value_0;
#endif // TSC_GROUP7_ENABLED
  
  //==================
  // TSC configuration
  //==================

  // Enable TSC clock
 // RCC->AHBENR |= RCC_AHBENR_TSEN;
 //RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_TSI,ENABLE);
 	 rcu_periph_clock_enable(RCU_TSI);
  // Disable Schmitt trigger hysteresis on all used TS IOs (Channel, Shield and Sampling IOs)
  //-----------------------------------------------------------------------------------------

  tmp_value_0 = 0xFFFFFFFF;
#if TSLPRM_TSC_GROUP1_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 0);
#endif
#if TSLPRM_TSC_GROUP1_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 1);
#endif
#if TSLPRM_TSC_GROUP1_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 2);
#endif
#if TSLPRM_TSC_GROUP1_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 3);
#endif
#if TSLPRM_TSC_GROUP2_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 4);
#endif
#if TSLPRM_TSC_GROUP2_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 5);
#endif
#if TSLPRM_TSC_GROUP2_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 6);
#endif
#if TSLPRM_TSC_GROUP2_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 7);
#endif
#if TSLPRM_TSC_GROUP3_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 8);
#endif
#if TSLPRM_TSC_GROUP3_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 9);
#endif
#if TSLPRM_TSC_GROUP3_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 10);
#endif
#if TSLPRM_TSC_GROUP3_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 11);
#endif
#if TSLPRM_TSC_GROUP4_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 12);
#endif
#if TSLPRM_TSC_GROUP4_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 13);
#endif
#if TSLPRM_TSC_GROUP4_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 14);
#endif
#if TSLPRM_TSC_GROUP4_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 15);
#endif
#if TSLPRM_TSC_GROUP5_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 16);
#endif
#if TSLPRM_TSC_GROUP5_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 17);
#endif
#if TSLPRM_TSC_GROUP5_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 18);
#endif
#if TSLPRM_TSC_GROUP5_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 19);
#endif
#if TSLPRM_TSC_GROUP6_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 20);
#endif
#if TSLPRM_TSC_GROUP6_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 21);
#endif
#if TSLPRM_TSC_GROUP6_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 22);
#endif
#if TSLPRM_TSC_GROUP6_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 23);
#endif

#if (TSC_GROUP7_ENABLED > 0)
#if TSLPRM_TSC_GROUP7_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 24);
#endif
#if TSLPRM_TSC_GROUP7_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 25);
#endif
#if TSLPRM_TSC_GROUP7_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 26);
#endif
#if TSLPRM_TSC_GROUP7_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 27);
#endif
#endif // TSC_GROUP7_ENABLED

#if (TSC_GROUP8_ENABLED > 0)
#if TSLPRM_TSC_GROUP8_IO1 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 28);
#endif
#if TSLPRM_TSC_GROUP8_IO2 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 29);
#endif
#if TSLPRM_TSC_GROUP8_IO3 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 30);
#endif
#if TSLPRM_TSC_GROUP8_IO4 != NU
  tmp_value_0 &= (uint32_t)~((uint32_t)1 << 31);
#endif
#endif // TSC_GROUP8_ENABLED

  TSI_PHM &= tmp_value_0;

  // Set Sampling Capacitor IOs
  //---------------------------

  tmp_value_0 = 0;
#if TSLPRM_TSC_GROUP1_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 0);
#endif
#if TSLPRM_TSC_GROUP1_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 1);
#endif
#if TSLPRM_TSC_GROUP1_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 2);
#endif
#if TSLPRM_TSC_GROUP1_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 3);
#endif
#if TSLPRM_TSC_GROUP2_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 4);
#endif
#if TSLPRM_TSC_GROUP2_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 5);
#endif
#if TSLPRM_TSC_GROUP2_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 6);
#endif
#if TSLPRM_TSC_GROUP2_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 7);
#endif
#if TSLPRM_TSC_GROUP3_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 8);
#endif
#if TSLPRM_TSC_GROUP3_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 9);
#endif
#if TSLPRM_TSC_GROUP3_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 10);
#endif
#if TSLPRM_TSC_GROUP3_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 11);
#endif
#if TSLPRM_TSC_GROUP4_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 12);
#endif
#if TSLPRM_TSC_GROUP4_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 13);
#endif
#if TSLPRM_TSC_GROUP4_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 14);
#endif
#if TSLPRM_TSC_GROUP4_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 15);
#endif
#if TSLPRM_TSC_GROUP5_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 16);
#endif
#if TSLPRM_TSC_GROUP5_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 17);
#endif
#if TSLPRM_TSC_GROUP5_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 18);
#endif
#if TSLPRM_TSC_GROUP5_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 19);
#endif
#if TSLPRM_TSC_GROUP6_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 20);
#endif
#if TSLPRM_TSC_GROUP6_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 21);
#endif
#if TSLPRM_TSC_GROUP6_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 22);
#endif
#if TSLPRM_TSC_GROUP6_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 23);
#endif

#if (TSC_GROUP7_ENABLED > 0)
#if TSLPRM_TSC_GROUP7_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 24);
#endif
#if TSLPRM_TSC_GROUP7_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 25);
#endif
#if TSLPRM_TSC_GROUP7_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 26);
#endif
#if TSLPRM_TSC_GROUP7_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 27);
#endif
#endif // TSC_GROUP7_ENABLED

#if (TSC_GROUP8_ENABLED > 0)
#if TSLPRM_TSC_GROUP8_IO1 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 28);
#endif
#if TSLPRM_TSC_GROUP8_IO2 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 29);
#endif
#if TSLPRM_TSC_GROUP8_IO3 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 30);
#endif
#if TSLPRM_TSC_GROUP8_IO4 == SAMPCAP
  tmp_value_0 |= (uint32_t)((uint32_t)1 << 31);
#endif
#endif // TSC_GROUP8_ENABLED

  TSI_SAMPCFG |= tmp_value_0;

}


/**
  * @brief  Initializes the acquisition module.
  * @param  None
  * @retval Status
  */
TSL_Status_enum_T TSL_acq_Init(void)
{

#if TSLPRM_TSC_GPIO_CONFIG > 0
  TSL_acq_InitGPIOs();
#endif

  // Enable TSC clock
	 rcu_periph_clock_enable(RCU_TSI);
 // RCC->AHBCCR |= RCC_AHBPERIPH_TSI;
  // TSC enabled
  TSI_CTL0  = 0x01;

  // Set CTPH
#if TSLPRM_TSC_CTPH > 0
  TSI_CTL0|= (uint32_t)((uint32_t)TSLPRM_TSC_CTPH << 28) & 0xF0000000;
#endif

  // Set CTPL
#if TSLPRM_TSC_CTPL > 0
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_CTPL << 24) & 0x0F000000;
#endif

  // Set SpreadSpectrum
#if TSLPRM_TSC_USE_SS > 0
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_USE_SS << 16) & 0x00010000;
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_SSD << 17) & 0x00FE0000;
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_SSPSC << 15) & 0x00008000;
#endif

  // Set Prescaler
#if TSLPRM_TSC_PGPSC > 0
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_PGPSC << 12) & 0x00007000;
#endif

  // Set Max Count
#if TSLPRM_TSC_MCV > 0
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_MCV << 5) & 0x000000E0;
#endif

  // Set IO default in Output PP Low to discharge all capacitors
  TSI_CTL0 &= (uint32_t)(~(1 << 4));

  // Set Synchronization Mode
#if TSLPRM_TSC_AM > 0

  // Set Synchronization Pin in Alternate-Function mode
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Set GPIOB clock

#if TSLPRM_TSC_SYNC_PIN == 0 // PB08
  GPIOB->MODER  &= 0xFFFCFFFF;
  GPIOB->MODER  |= 0x00020000;
  GPIOB->AFR[1] |= 0x00000003;
#else // PB10
  GPIOB->MODER  &= 0xFFCFFFFF;
  GPIOB->MODER  |= 0x00200000;
  GPIOB->AFR[1] |= 0x00000300;
#endif

  // Set Synchronization Polarity
 TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_SYNC_POL << 3) & 0x00000008;

  // Set acquisition mode
  TSI_CTL0 |= (uint32_t)((uint32_t)TSLPRM_TSC_AM << 2) & 0x00000004;

#endif

#if TSLPRM_USE_ACQ_INTERRUPT > 0

  // Set both EOA and MCE interrupts
  TSI_INTEN |= 0x03;

  // Configure NVIC
  NVIC_SetPriority(TSI_IRQn, 0);
  NVIC_EnableIRQ(TSI_IRQn);

#endif

  // Initialize the delay that will be used to discharge the capacitors
  DelayDischarge = (uint32_t)((TSLPRM_DELAY_DISCHARGE_ALL * (uint32_t)(SystemCoreClock/1000000)) / 48);

  return TSL_STATUS_OK;

}


/**
  * @brief Configures a Bank.
  * @param[in] idx_bk  Index of the Bank to configure
  * @retval Status
  */
TSL_Status_enum_T TSL_acq_BankConfig(TSL_tIndex_T idx_bk)
{
  uint32_t idx_ch;
  uint32_t objs; /* bit field of TSL_ObjStatus_enum_T type */
  uint32_t gx;
  uint32_t ioy;
  CONST TSL_Bank_T *bank = &(TSL_Globals.Bank_Array[idx_bk]);
  CONST TSL_ChannelSrc_T *pchSrc = bank->p_chSrc;
  CONST TSL_ChannelDest_T *pchDest = bank->p_chDest;

  // Check parameters (if USE_FULL_ASSERT is defined)
 // assert_param(IS_BANK_INDEX_OK(idx_bk));

  // Mark the current bank processed
  TSL_Globals.This_Bank = idx_bk;

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Enable the Gx_IOy used as channels (channels + shield)
  TSI_CHCFG = bank->msk_IOCCR_channels;
  // Enable acquisition on selected Groups
  TSI_GCTL = bank->msk_IOGCSR_groups;
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // For all channels of the bank check if they are OFF or BURST_ONLY
  // and set acquisition status flag
  for (idx_ch = 0; idx_ch < bank->NbChannels; idx_ch++)
  {

    // Check Object status flag
    objs = bank->p_chData[pchDest->IdxDest].Flags.ObjStatus;

    if (objs != TSL_OBJ_STATUS_ON)
    {
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // Get the Channel Group mask
      gx = pchSrc->msk_IOGCSR_group;
      // Stop acquisition of the Group
      TSI_GCTL &= (uint32_t)~gx;
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if (objs == TSL_OBJ_STATUS_OFF)
      {
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Get the Channel IO mask
        ioy = pchSrc->msk_IOCCR_channel;
        // Stop Burst of the Channel
        TSI_CHCFG &= (uint32_t)~ioy;
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      }
    }

    // Next channel
    pchSrc++;
    pchDest++;
  }

  return TSL_STATUS_OK;
}


/**
  * @brief Start acquisition on a previously configured bank
  * @param None
  * @retval None
  */
void TSL_acq_BankStartAcq(void)
{
  // Clear both EOAIC and MCEIC flags
 // TSI->CEFR |= 0x03;
  TSI_INTC|=0x03;
  // Wait capacitors discharge
  SoftDelay(DelayDischarge);

#if TSLPRM_TSC_IODEF > 0 // Default = Input Floating
  // Set IO default in Input Floating
  TSI_CTL0 |= (1 << 4);
#endif

  // Start acquisition
  TSI_CTL0 |= 0x02;
}


/**
  * @brief Wait end of acquisition
  * @param None
  * @retval Status
  */
TSL_Status_enum_T TSL_acq_BankWaitEOC(void)
{
  TSL_Status_enum_T retval = TSL_STATUS_BUSY;

  // Check EOAF flag
  if (TSI_INTF & 0x01)
  {

#if TSLPRM_TSC_IODEF > 0 // Default = Input Floating
    // Set IO default in Output PP Low to discharge all capacitors
    TSC->CR &= (uint32_t)(~(1 << 4));
#endif

    // Check MCEF flag
    if (TSI_INTF & 0x02)
    {
      retval = TSL_STATUS_ERROR;
    }
    else
    {
      retval = TSL_STATUS_OK;
    }
  }

  return retval;
}


/**
  * @brief Return the current measure
  * @param[in] index Index of the measure source
  * @retval Measure
  */
TSL_tMeas_T TSL_acq_GetMeas(TSL_tIndex_T index)
{
  // Check parameters (if USE_FULL_ASSERT is defined)
#if (TSC_GROUP7_ENABLED > 0) || (TSC_GROUP8_ENABLED > 0)
//  assert_param(IS_SRC_INDEX_0_7_OK(idx_bk));
#else
//  assert_param(IS_SRC_INDEX_0_5_OK(idx_bk));
#endif
  //return((TSL_tMeas_T)(TSI->GXCYCNR[index]));
	uint32_t TSI_CYCNx;
	TSI_CYCNx=TSI+0x34+4*index;
//	REG32(TSI + 0x34U) 
	 return((TSL_tMeas_T)(REG32(TSI_CYCNx)));
	
}


/**
  * @brief Compute the Delta value
  * @param[in] ref Reference value
  * @param[in] meas Last Measurement value
  * @retval Delta value
  */
TSL_tDelta_T TSL_acq_ComputeDelta(TSL_tRef_T ref, TSL_tMeas_T meas)
{
  return((TSL_tDelta_T)(ref - meas));
}


/**
  * @brief Compute the Measurement value
  * @param[in] ref Reference value
  * @param[in] delta Delta value
  * @retval Measurement value
  */
TSL_tMeas_T TSL_acq_ComputeMeas(TSL_tRef_T ref, TSL_tDelta_T delta)
{
  return((TSL_tMeas_T)(ref - delta));
}


/**
  * @brief  Check noise (not used)
  * @param  None
  * @retval Status
  */
TSL_AcqStatus_enum_T TSL_acq_CheckNoise(void)
{
  return TSL_ACQ_STATUS_OK;
}


/**
  * @brief Check if a filter must be used on the current channel (not used)
  * @param[in] pCh Pointer on the channel data information
  * @retval Result TRUE if a filter can be applied
  */
TSL_Bool_enum_T TSL_acq_UseFilter(TSL_ChannelData_T *pCh)
{
  return TSL_TRUE;
}


/**
  * @brief Test if the Reference is incorrect (not used)
  * @param[in] pCh Pointer on the channel data information
  * @retval Result TRUE if the Reference is out of range
  */
TSL_Bool_enum_T TSL_acq_TestReferenceOutOfRange(TSL_ChannelData_T *pCh)
{
  return TSL_FALSE;
}


/**
  * @brief Test if the measure has crossed the reference target (not used)
  * @param[in] pCh Pointer on the channel data information
  * @param[in] new_meas Measure of the last acquisition on this channel
  * @retval Result TRUE if the Reference is valid
  */
TSL_Bool_enum_T TSL_acq_TestFirstReferenceIsValid(TSL_ChannelData_T *pCh, TSL_tMeas_T new_meas)
{
  return TSL_TRUE;
}


#if defined(__IAR_SYSTEMS_ICC__) // IAR/EWARM
#pragma optimize=low
#elif defined(__CC_ARM) // Keil/MDK-ARM
#pragma O1
#pragma Ospace
#elif defined(__TASKING__) // Altium/Tasking
#pragma optimize O0
#elif defined(__GNUC__) // Atollic/True Studio + Raisonance/RKit
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
/**
  * @brief  Software delay (private routine)
  * @param  val Wait delay
  * @retval None
  * @note Measurements done with HCLK=48MHz and Keil/MDK-ARM compiler
  * val =  500: ~ 53�s
  * val = 1000: ~106�s
  * val = 2000: ~210�s
  */
void SoftDelay(uint32_t val)
{
  uint32_t idx;
  for (idx = val; idx > 0; idx--)
  {}
}
#if defined(__TASKING__)
#pragma endoptimize
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
