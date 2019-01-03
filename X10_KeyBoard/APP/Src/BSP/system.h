#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "includes.h"
#include "gd32f3x0_rtc.h"
#include "gd32f3x0_timer.h"


#define USE_TIMER1				1
#define TIMER_MAX				2




typedef struct
{
    uint32_t magic_number;
	uint8_t printfSwitch;
}SAVE_INFO_T;

typedef struct
{
	unsigned char soft_version;
    unsigned char blue_state;
    unsigned char blue_version[20];
    unsigned char blue_name[12];
    unsigned char gw_mac[5];
}SYSTEM_INFO_T;

typedef struct
{
    unsigned char system_state;					//û��ʹ��
    unsigned char blue_state;					//
    unsigned char card_state;					//������״̬  0 :��ʾ��ʼ��ok 1: ��ʾ��ʼ������
    unsigned char key_state;					//û��ʹ��
}SYSTEM_STATUS_T;

typedef struct{
	uint32_t CurrentPackage; 	//��ǰ���չ̼����ݰ�
	uint32_t lastIndex;
	uint32_t ReceiveSize;
}UPGRADE_FW_INFO_STR;

typedef struct
{
	uint8_t  current_usr_card_id[16];       	//ˢ����ȡ�Ŀ���
	uint8_t  card_id[5];       			//ˢ����ȡ�Ŀ���
//	uint8_t card_state;                         //������״̬  0 :��ʾ��ʼ��ok 1: ��ʾ��ʼ������
	uint8_t ReadCardFlag;
	uint8_t WifiInitOK;
	uint8_t gWifiSendMux;
	uint8_t gProtoSendMux;
	uint8_t pCardId[8];
	uint8_t packageSn;
	uint8_t ShakeHandState;
	uint8_t UpgradeFlag;
	UPGRADE_FW_INFO_STR UpgradeFw;
	uint8_t TestCardFlag;
	uint8_t UpperMonitorTestFlag;
	uint32_t UpperMonitorTicks;
	uint32_t ReportTicks;
	rtc_parameter_struct RtcData;
	rtc_alarm_struct  rtc_alarm;
}GLOBAL_INFO_T;

extern void WatchDogInit(void) ;
extern void FeedWatchDog(void);
extern void LoadSystemInfo(void);
extern void SystemResetRecord(void);
extern void StartDelay(void);
extern int MuxSempTake(uint8_t *pSemp);
extern void MuxSempGive(uint8_t *pSemp);
extern uint8_t Asc2Int(char ch);
extern int StringToBCD(unsigned char *BCD, const char *str);
extern char *BCDToString(char *dest, unsigned char *BCD, int bytes);
extern int GetPktSum(uint8_t *pData, uint16_t len);
extern void TimerConfig(uint8_t Index, uint32_t Period, uint8_t UserIrq);
extern uint32_t GetTimeTicks(void);



extern SAVE_INFO_T		SaveInfo;
extern SYSTEM_INFO_T	SystemInfo;
extern SYSTEM_STATUS_T	SystemStatus;
extern GLOBAL_INFO_T	GlobalInfo;

extern rcu_periph_enum RCU_TIMERX[TIMER_MAX];
extern uint32_t TIMERX[TIMER_MAX];
extern uint8_t TIMERX_IRQ[TIMER_MAX];


#endif


