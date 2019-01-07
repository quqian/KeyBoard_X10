#include "gd32f3x0.h"
#include "tsiKey.h"
#include "includes.h"
#include "flash.h"
#include "flash_usr.h"
#include "usart.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_fwdgt.h"
#include "gd32f3x0_timer.h"
#include "gd32f3x0_misc.h"
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_tsi.h"
#include <stdlib.h>
#include <stdio.h>



#if 1
#define TSI_G0P0_GPIO_PIN_X		GPIO_PIN_0
#define TSI_G0P0_GPIO_X			GPIOA
#define TSI_G0P1_GPIO_PIN_X		GPIO_PIN_1
#define TSI_G0P1_GPIO_X			GPIOA


#define TSI_G1P0_GPIO_PIN_X		GPIO_PIN_4
#define TSI_G1P0_GPIO_X			GPIOA
#define TSI_G1P1_GPIO_PIN_X		GPIO_PIN_5
#define TSI_G1P1_GPIO_X			GPIOA
#define TSI_G1P2_GPIO_PIN_X		GPIO_PIN_6
#define TSI_G1P2_GPIO_X			GPIOA
#define TSI_G1P3_GPIO_PIN_X		GPIO_PIN_7
#define TSI_G1P3_GPIO_X			GPIOA


#define TSI_G2P1_GPIO_PIN_X		GPIO_PIN_0
#define TSI_G2P1_GPIO_X			GPIOB
#define TSI_G2P2_GPIO_PIN_X		GPIO_PIN_1
#define TSI_G2P2_GPIO_X			GPIOB
#define TSI_G2P3_GPIO_PIN_X		GPIO_PIN_2
#define TSI_G2P3_GPIO_X			GPIOB


#define TSI_G3P0_GPIO_PIN_X		GPIO_PIN_9
#define TSI_G3P0_GPIO_X			GPIOA
#define TSI_G3P1_GPIO_PIN_X		GPIO_PIN_10
#define TSI_G3P1_GPIO_X			GPIOA
#define TSI_G3P2_GPIO_PIN_X		GPIO_PIN_11
#define TSI_G3P2_GPIO_X			GPIOA
#define TSI_G3P3_GPIO_PIN_X		GPIO_PIN_12
#define TSI_G3P3_GPIO_X			GPIOA


#define TSI_G5P0_GPIO_PIN_X		GPIO_PIN_11
#define TSI_G5P0_GPIO_X			GPIOB
#define TSI_G5P1_GPIO_PIN_X		GPIO_PIN_12
#define TSI_G5P1_GPIO_X			GPIOB
#define TSI_G5P2_GPIO_PIN_X		GPIO_PIN_13
#define TSI_G5P2_GPIO_X			GPIOB
#define TSI_G5P3_GPIO_PIN_X		GPIO_PIN_14
#define TSI_G5P3_GPIO_X			GPIOB
#endif


uint8_t KeyValue = 0xff;



/* the current cycle number array of the channel pin */
uint16_t SampleNumG0[4] = {0, 0, 0, 0};
/* reference value sample array of TSI group5 */
uint16_t SampleRefnumArrayG0P1[20] = {0};
uint16_t SampleRefnumArrayG0P2[20] = {0};
uint16_t SampleRefnumArrayG0P3[20] = {0};
/* average value of cycles */
uint16_t SampleRefnumG0[4] = {0,};


/* the current cycle number array of the channel pin */
uint16_t SampleNumG1[4] = {0,0,0,0};
/* reference value sample array of TSI group5 */
uint16_t SampleRefnumArrayG1P1[20] = {0};
uint16_t SampleRefnumArrayG1P2[20] = {0};
uint16_t SampleRefnumArrayG1P3[20] = {0};
/* average value of cycles */
uint16_t SampleRefnumG1[4] = {0,};

/* the current cycle number array of the channel pin */
uint16_t SampleNumG2[4] = {0,0,0,0};
/* reference value sample array of TSI group5 */
uint16_t SampleRefnumArrayG2P1[20] = {0};
uint16_t SampleRefnumArrayG2P2[20] = {0};
uint16_t SampleRefnumArrayG2P3[20] = {0};
/* average value of cycles */
uint16_t SampleRefnumG2[4] = {0,};


/* the current cycle number array of the channel pin */
uint16_t SampleNumG3[4] = {0,0,0,0};
/* reference value sample array of TSI group5 */
uint16_t SampleRefnumArrayG3P1[20] = {0};
uint16_t SampleRefnumArrayG3P2[20] = {0};
uint16_t SampleRefnumArrayG3P0[20] = {0};
/* average value of cycles */
uint16_t SampleRefnumG3[4] = {0,};


/* the current cycle number array of the channel pin */
uint16_t SampleNumG5[4] = {0,0,0,0};
/* reference value sample array of TSI group5 */
uint16_t SampleRefnumArrayG5P1[20] = {0};
uint16_t SampleRefnumArrayG5P2[20] = {0};
uint16_t SampleRefnumArrayG5P3[20] = {0};
/* average value of cycles */
uint16_t SampleRefnumG5[4] = {0,};


void delay(uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

//键值上报
int KeyValuesUpLoad(uint8_t values)
{    
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	KEY_VANUAL_STR* SystemStatusAck = (KEY_VANUAL_STR*)pBuff->data;

	SystemStatusAck->KeyVanual = values;
    App_CB_SendData(pBuff, sizeof(KEY_VANUAL_STR), MsgType_KEY, CMD_KEY);
//	PrintfData("KeyValuesUpLoad", (uint8_t*)pBuff, sizeof(KEY_VANUAL_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

//按键返回事件
void KeyCallBack(uint8_t values)
{
	if((0xff != values) && (11 >= values))
	{
		KeyValuesUpLoad(values);
		CL_LOG("[key: %d]\r\n", values);
	}
}

void gpio_config(void)
{
#if 1
	gpio_mode_set(TSI_G0P0_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G0P0_GPIO_PIN_X);
    gpio_output_options_set(TSI_G0P0_GPIO_X, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, TSI_G0P0_GPIO_PIN_X);

    /* GPIOB12  GPIOB13 GPIOB14 */
    /* alternate function output push-pull for channel and shield IOs */
    gpio_mode_set(TSI_G0P1_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G0P1_GPIO_PIN_X);
	gpio_output_options_set(TSI_G0P1_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G0P1_GPIO_PIN_X);

    /* connect pin to peripheral */
    gpio_af_set(TSI_G0P0_GPIO_X, GPIO_AF_3, TSI_G0P0_GPIO_PIN_X);
    gpio_af_set(TSI_G0P1_GPIO_X, GPIO_AF_3, TSI_G0P1_GPIO_PIN_X);


	
    gpio_mode_set(TSI_G1P0_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G1P0_GPIO_PIN_X);
    gpio_output_options_set(TSI_G1P0_GPIO_X, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, TSI_G1P0_GPIO_PIN_X);

    /* GPIOB12  GPIOB13 GPIOB14 */
    /* alternate function output push-pull for channel and shield IOs */
    gpio_mode_set(TSI_G1P1_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G1P1_GPIO_PIN_X);
	gpio_output_options_set(TSI_G1P1_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G1P1_GPIO_PIN_X);
	gpio_mode_set(TSI_G1P2_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G1P2_GPIO_PIN_X);
	gpio_output_options_set(TSI_G1P2_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G1P2_GPIO_PIN_X);
	gpio_mode_set(TSI_G1P3_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G1P3_GPIO_PIN_X);
    gpio_output_options_set(TSI_G1P3_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G1P3_GPIO_PIN_X);

    /* connect pin to peripheral */
    gpio_af_set(TSI_G1P0_GPIO_X, GPIO_AF_3, TSI_G1P0_GPIO_PIN_X);
    gpio_af_set(TSI_G1P1_GPIO_X, GPIO_AF_3, TSI_G1P1_GPIO_PIN_X);
    gpio_af_set(TSI_G1P2_GPIO_X, GPIO_AF_3, TSI_G1P2_GPIO_PIN_X);
    gpio_af_set(TSI_G1P3_GPIO_X, GPIO_AF_3, TSI_G1P3_GPIO_PIN_X);



	gpio_mode_set(TSI_G2P1_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G2P1_GPIO_PIN_X);
    gpio_output_options_set(TSI_G2P1_GPIO_X, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, TSI_G2P1_GPIO_PIN_X);

	gpio_mode_set(TSI_G2P2_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G2P2_GPIO_PIN_X);
	gpio_output_options_set(TSI_G2P2_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G2P2_GPIO_PIN_X);
	gpio_mode_set(TSI_G2P3_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G2P3_GPIO_PIN_X);
    gpio_output_options_set(TSI_G2P3_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G2P3_GPIO_PIN_X);

    /* connect pin to peripheral */
    gpio_af_set(TSI_G2P1_GPIO_X, GPIO_AF_3, TSI_G2P1_GPIO_PIN_X);
    gpio_af_set(TSI_G2P2_GPIO_X, GPIO_AF_3, TSI_G2P2_GPIO_PIN_X);
    gpio_af_set(TSI_G2P3_GPIO_X, GPIO_AF_3, TSI_G2P3_GPIO_PIN_X);


	
    /* alternate function output open-drain for sampling capacitor IO */
    gpio_mode_set(TSI_G3P3_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G3P3_GPIO_PIN_X);
    gpio_output_options_set(TSI_G3P3_GPIO_X, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, TSI_G3P3_GPIO_PIN_X);

    /* GPIOB12  GPIOB13 GPIOB14 */
    /* alternate function output push-pull for channel and shield IOs */
    gpio_mode_set(TSI_G3P1_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G3P1_GPIO_PIN_X);
	gpio_output_options_set(TSI_G3P1_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G3P1_GPIO_PIN_X);
	gpio_mode_set(TSI_G3P2_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G3P2_GPIO_PIN_X);
	gpio_output_options_set(TSI_G3P2_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G3P2_GPIO_PIN_X);
	gpio_mode_set(TSI_G3P0_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G3P0_GPIO_PIN_X);
    gpio_output_options_set(TSI_G3P0_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G3P0_GPIO_PIN_X);

    /* connect pin to peripheral */
    gpio_af_set(TSI_G3P0_GPIO_X, GPIO_AF_3, TSI_G3P0_GPIO_PIN_X);
    gpio_af_set(TSI_G3P1_GPIO_X, GPIO_AF_3, TSI_G3P1_GPIO_PIN_X);
    gpio_af_set(TSI_G3P2_GPIO_X, GPIO_AF_3, TSI_G3P2_GPIO_PIN_X);
    gpio_af_set(TSI_G3P3_GPIO_X, GPIO_AF_3, TSI_G3P3_GPIO_PIN_X);


	
	/* GPIOB11 */
    /* alternate function output open-drain for sampling capacitor IO */
    gpio_mode_set(TSI_G5P0_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G5P0_GPIO_PIN_X);
    gpio_output_options_set(TSI_G5P0_GPIO_X, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, TSI_G5P0_GPIO_PIN_X);

    /* GPIOB12  GPIOB13 GPIOB14 */
    /* alternate function output push-pull for channel and shield IOs */
    gpio_mode_set(TSI_G5P1_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G5P1_GPIO_PIN_X);
	gpio_output_options_set(TSI_G5P1_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G5P1_GPIO_PIN_X);
	gpio_mode_set(TSI_G5P2_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G5P2_GPIO_PIN_X);
	gpio_output_options_set(TSI_G5P2_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G5P2_GPIO_PIN_X);
	gpio_mode_set(TSI_G5P3_GPIO_X, GPIO_MODE_AF, GPIO_PUPD_NONE, TSI_G5P3_GPIO_PIN_X);
    gpio_output_options_set(TSI_G5P3_GPIO_X, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, TSI_G5P3_GPIO_PIN_X);

    /* connect pin to peripheral */
    gpio_af_set(TSI_G5P0_GPIO_X, GPIO_AF_3, TSI_G5P0_GPIO_PIN_X);
    gpio_af_set(TSI_G5P1_GPIO_X, GPIO_AF_3, TSI_G5P1_GPIO_PIN_X);
    gpio_af_set(TSI_G5P2_GPIO_X, GPIO_AF_3, TSI_G5P2_GPIO_PIN_X);
    gpio_af_set(TSI_G5P3_GPIO_X, GPIO_AF_3, TSI_G5P3_GPIO_PIN_X);
#endif
}

void tsi_config(void)
{
    /* TSI configure */
    tsi_init(TSI_CTCDIV_DIV32, TSI_CHARGE_2CTCLK, TSI_TRANSFER_2CTCLK, TSI_MAXNUM2047);
    tsi_sofeware_mode_config();
#if 1
	tsi_sample_pin_enable(TSI_SAMPCFG_G0P0);
    tsi_group_enable(TSI_GCTL_GE0);
    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G0P0 | TSI_PHM_G0P1);


	
	tsi_sample_pin_enable(TSI_SAMPCFG_G1P0);
    tsi_group_enable(TSI_GCTL_GE1);
    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G1P0 | TSI_PHM_G1P1 | TSI_PHM_G1P2 | TSI_PHM_G1P3);


	tsi_sample_pin_enable(TSI_SAMPCFG_G2P1);
    tsi_group_enable(TSI_GCTL_GE2);
    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G2P1 | TSI_PHM_G2P2 | TSI_PHM_G2P3);

	
	tsi_sample_pin_enable(TSI_SAMPCFG_G3P3);
    tsi_group_enable(TSI_GCTL_GE3);
    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G3P0 | TSI_PHM_G3P1 | TSI_PHM_G3P2 | TSI_PHM_G3P3);


	tsi_sample_pin_enable(TSI_SAMPCFG_G5P0);
    tsi_group_enable(TSI_GCTL_GE5);
    /* disable hysteresis mode */
    tsi_hysteresis_off(TSI_PHM_G5P0 | TSI_PHM_G5P1 | TSI_PHM_G5P2 | TSI_PHM_G5P3);
#endif
    /* enable TSI */
    tsi_enable();
}


/*!
    \brief      acquisition pin y of group x,x=0..5,y=0..3 
    \param[in]  tsi_groupx_piny: TSI_CHCFG_GxPy,pin y of group x
    \param[out] none
    \retval     none
*/
void tsi_transfer_pin(uint32_t tsi_groupx_piny)
{
    /* configure the TSI pin channel mode */
    tsi_channel_pin_enable(tsi_groupx_piny);

    /* wait capacitors discharge */
    delay(0xD00);

    /* clear both MNERR and CTCF flags */
    tsi_flag_clear(TSI_FLAG_CTCF_CLR|TSI_FLAG_MNERR_CLR);

    /* start a new acquisition */
    tsi_software_start();

    /* wait the specified TSI flag state: MNERR or CTCF */
    while(RESET == tsi_flag_get(TSI_FLAG_CTCF|TSI_FLAG_MNERR));
}

void InitTsiG3(void)
{
	uint32_t m = 0;
	
	/* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++)
    {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G3P1);

        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
            /* get charge transfer complete cycle number */
          //  SampleRefnumArrayG3P1[m] = tsi_group5_cycle_get();
			SampleRefnumArrayG3P1[m] = tsi_group3_cycle_get();
        }

        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G3P1);

        /* get charge transfer complete cycle number of group5 pin2 */
        tsi_transfer_pin(TSI_CHCFG_G3P2);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
          //  SampleRefnumArrayG3P2[m] = tsi_group5_cycle_get();
			SampleRefnumArrayG3P2[m] = tsi_group3_cycle_get();
        }

        tsi_channel_pin_disable(TSI_CHCFG_G3P2);

        /* get charge transfer complete cycle number of group5 pin3 */
        tsi_transfer_pin(TSI_CHCFG_G3P0);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
          //  SampleRefnumArrayG3P0[m] = tsi_group5_cycle_get();
			SampleRefnumArrayG3P0[m] = tsi_group3_cycle_get();
        }
              
        tsi_channel_pin_disable(TSI_CHCFG_G3P0);

        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++)
    {
        SampleRefnumG3[1] += SampleRefnumArrayG3P1[m];
        SampleRefnumG3[2] += SampleRefnumArrayG3P2[m];
        SampleRefnumG3[0] += SampleRefnumArrayG3P0[m];
    }

    /* average channel cycle value are obtained */
    SampleRefnumG3[1] = SampleRefnumG3[1]/19;
    SampleRefnumG3[2] = SampleRefnumG3[2]/19;
    SampleRefnumG3[0] = SampleRefnumG3[0]/19;
}

void G3GetKey(void)
{
	/* acquisition pin1 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G3P1);
 
    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
        /* get charge transfer complete cycle number */
     //   SampleNumG3[1] = tsi_group5_cycle_get();
		SampleNumG3[1] = tsi_group3_cycle_get();
    }
    /* channel 1 touch */
    if((SampleRefnumG3[1]-SampleNumG3[1]) > 0x20)
    {
    	KeyValue = 2;
		printf("[Key: %d]\n", KeyValue);
    }
    else
    {
    }
    tsi_channel_pin_disable(TSI_CHCFG_G3P1);

    /* acquisition pin2 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G3P2);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
      //  SampleNumG3[2] = tsi_group5_cycle_get();
		SampleNumG3[2] = tsi_group3_cycle_get();
    }

    /* light LED2 */
    if((SampleRefnumG3[2]-SampleNumG3[2]) > 0x20)
    {
        KeyValue = 1;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G3P2);

    /* acquisition pin3 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G3P0);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
      //  SampleNumG3[0] =  tsi_group5_cycle_get();
		SampleNumG3[0] =  tsi_group3_cycle_get();
    }
    /* light LED3 */
    if((SampleRefnumG3[0]-SampleNumG3[0]) > 0x20)
    {
        KeyValue = 3;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G3P0);
}

void InitTsiG5(void)
{
	uint32_t m = 0;
	
	/* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++)
    {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G5P1);

        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
            /* get charge transfer complete cycle number */
			SampleRefnumArrayG5P1[m] = tsi_group5_cycle_get();
        }

        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G5P1);

        /* get charge transfer complete cycle number of group5 pin2 */
        tsi_transfer_pin(TSI_CHCFG_G5P2);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
			SampleRefnumArrayG5P2[m] = tsi_group5_cycle_get();
        }

        tsi_channel_pin_disable(TSI_CHCFG_G5P2);

        /* get charge transfer complete cycle number of group5 pin3 */
        tsi_transfer_pin(TSI_CHCFG_G5P3);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
			SampleRefnumArrayG5P3[m] = tsi_group5_cycle_get();
        }
              
        tsi_channel_pin_disable(TSI_CHCFG_G5P3);

        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++)
    {
        SampleRefnumG5[1] += SampleRefnumArrayG5P1[m];
        SampleRefnumG5[2] += SampleRefnumArrayG5P2[m];
        SampleRefnumG5[3] += SampleRefnumArrayG5P3[m];
    }

    /* average channel cycle value are obtained */
    SampleRefnumG5[1] = SampleRefnumG5[1]/19;
    SampleRefnumG5[2] = SampleRefnumG5[2]/19;
    SampleRefnumG5[3] = SampleRefnumG5[3]/19;
}

void G5GetKey(void)
{
	/* acquisition pin1 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G5P1);
 
    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
        /* get charge transfer complete cycle number */
		SampleNumG5[1] = tsi_group5_cycle_get();
    }
    /* channel 1 touch */
    if((SampleRefnumG5[1]-SampleNumG5[1]) > 0x20)
    {
    	KeyValue = 6;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G5P1);

    /* acquisition pin2 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G5P2);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG5[2] = tsi_group5_cycle_get();
    }
	
    if((SampleRefnumG5[2]-SampleNumG5[2]) > 0x20)
    {
        KeyValue = 5;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G5P2);

    /* acquisition pin3 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G5P3);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG5[3] =  tsi_group5_cycle_get();
    }
    if((SampleRefnumG5[3] - SampleNumG5[3]) > 0x20)
    {
		KeyValue = 4;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G5P3);
}

void InitTsiG0(void)
{
	uint32_t m = 0;
	
	/* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++)
    {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G0P1);

        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
            /* get charge transfer complete cycle number */
			SampleRefnumArrayG0P1[m] = tsi_group0_cycle_get();
        }

        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G0P1);
        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++)
    {
        SampleRefnumG0[1] += SampleRefnumArrayG0P1[m];
    }

    /* average channel cycle value are obtained */
    SampleRefnumG0[1] = SampleRefnumG0[1]/19;
}

void G0GetKey(void)
{
    tsi_transfer_pin(TSI_CHCFG_G0P1);
 
    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
        /* get charge transfer complete cycle number */
		SampleNumG0[1] = tsi_group0_cycle_get();
    }
    /* channel 1 touch */
    if((SampleRefnumG0[1]-SampleNumG0[1]) > 0x20)
    {
		KeyValue = 11;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G0P1);
}

void InitTsiG1(void)
{
	uint32_t m = 0;
	
	/* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++)
    {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G1P1);

        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
            /* get charge transfer complete cycle number */
			SampleRefnumArrayG1P1[m] = tsi_group1_cycle_get();
        }

        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G1P1);

        /* get charge transfer complete cycle number of group5 pin2 */
        tsi_transfer_pin(TSI_CHCFG_G1P2);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
			SampleRefnumArrayG1P2[m] = tsi_group1_cycle_get();
        }

        tsi_channel_pin_disable(TSI_CHCFG_G1P2);

        /* get charge transfer complete cycle number of group5 pin3 */
        tsi_transfer_pin(TSI_CHCFG_G1P3);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
			SampleRefnumArrayG1P3[m] = tsi_group1_cycle_get();
        }
              
        tsi_channel_pin_disable(TSI_CHCFG_G1P3);

        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++)
    {
        SampleRefnumG1[1] += SampleRefnumArrayG1P1[m];
        SampleRefnumG1[2] += SampleRefnumArrayG1P2[m];
        SampleRefnumG1[3] += SampleRefnumArrayG1P3[m];
    }

    /* average channel cycle value are obtained */
    SampleRefnumG1[1] = SampleRefnumG1[1]/19;
    SampleRefnumG1[2] = SampleRefnumG1[2]/19;
    SampleRefnumG1[3] = SampleRefnumG1[3]/19;
}

void G1GetKey(void)
{
    tsi_transfer_pin(TSI_CHCFG_G1P1);
 
    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
        /* get charge transfer complete cycle number */
		SampleNumG1[1] = tsi_group1_cycle_get();
    }
    /* channel 1 touch */
    if((SampleRefnumG1[1]-SampleNumG1[1]) > 0x20)
    {
        KeyValue = 0;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G1P1);

    /* acquisition pin2 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G1P2);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG1[2] = tsi_group1_cycle_get();
    }
	
    if((SampleRefnumG1[2]-SampleNumG1[2]) > 0x20)
    {
        KeyValue = 10;
		printf("[返回Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G1P2);

    /* acquisition pin3 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G1P3);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG1[3] =  tsi_group1_cycle_get();
    }
    if((SampleRefnumG1[3] - SampleNumG1[3]) > 0x20)
    {
		KeyValue = 9;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G1P3);
}

void InitTsiG2(void)
{
	uint32_t m = 0;
	
	/* reference cycle value acquisition and processing */
    for(m = 0; m < 20; m++)
    {
        /* get charge transfer complete cycle number of group5 pin1 */
        tsi_transfer_pin(TSI_CHCFG_G2P2);
        /* check the TSI flag:end of acquisition interrupt */
        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
            /* get charge transfer complete cycle number */
			SampleRefnumArrayG2P2[m] = tsi_group2_cycle_get();
        }
        /* disable the selected pin as channel pin */
        tsi_channel_pin_disable(TSI_CHCFG_G2P2);

        /* get charge transfer complete cycle number of group5 pin3 */
        tsi_transfer_pin(TSI_CHCFG_G2P3);

        if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
        {
			SampleRefnumArrayG2P3[m] = tsi_group2_cycle_get();
        }
              
        tsi_channel_pin_disable(TSI_CHCFG_G2P3);

        /* delay for a period of time while all banks have been acquired */
        delay(0x1000);
    }

    for(m = 1; m < 20; m++)
    {
        SampleRefnumG2[2] += SampleRefnumArrayG2P2[m];
        SampleRefnumG2[3] += SampleRefnumArrayG2P3[m];
    }

    /* average channel cycle value are obtained */
    SampleRefnumG2[2] = SampleRefnumG2[2]/19;
    SampleRefnumG2[3] = SampleRefnumG2[3]/19;
}

void G2GetKey(void)
{
    /* acquisition pin2 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G2P2);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG2[2] = tsi_group2_cycle_get();
    }
	
    if((SampleRefnumG2[2]-SampleNumG2[2]) > 0x20)
    {
        KeyValue = 8;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G2P2);

    /* acquisition pin3 of group5 */
    tsi_transfer_pin(TSI_CHCFG_G2P3);

    /* check the TSI flag--end of acquisition interrupt */
    if((uint8_t)SET == tsi_flag_get(TSI_FLAG_CTCF))
    {
		SampleNumG2[3] =  tsi_group2_cycle_get();
    }
    if((SampleRefnumG2[3] - SampleNumG2[3]) > 0x20)
    {
		KeyValue = 7;
		printf("[Key: %d]\n", KeyValue);
    }
    tsi_channel_pin_disable(TSI_CHCFG_G2P3);
}

void TsiKeyInit(void)
{
	/* TSI peripheral and GPIOB Periph clock enable */
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_TSI);
	
    /* configure the GPIO ports */
    gpio_config();

    /* configure the TSI peripheral */
    tsi_config();

	InitTsiG0();
	InitTsiG1();
	InitTsiG2();
    InitTsiG3();
    InitTsiG5();
	CL_LOG("TsiKeyInit OK\n");
}

void GetKey(void)
{
    FeedWatchDog();
	KeyValue = 0xff;
	G0GetKey();
	G1GetKey();
	G2GetKey();
	G3GetKey();
	G5GetKey();
	KeyCallBack(KeyValue);
}


