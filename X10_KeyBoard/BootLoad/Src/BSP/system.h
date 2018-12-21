#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "includes.h"

typedef struct
{
    uint32_t WifiSerialNum;
	uint8_t printfSwitch;
}SYSTEM_INFO_T;

typedef struct
{
	uint8_t  current_usr_card_id[16];       	//ˢ����ȡ�Ŀ���
	uint8_t  card_id[5];       			//ˢ����ȡ�Ŀ���
	uint8_t card_state;                         //������״̬  0 :��ʾ��ʼ��ok 1: ��ʾ��ʼ������
	uint8_t ReadCardFlag;
	uint8_t WifiInitOK;
	uint8_t gWifiSendMux;
	uint8_t gProtoSendMux;
	uint8_t pCardId[8];
	uint8_t packageSn;
	uint8_t ShakeHandState;
	uint32_t Systick;
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
extern void TimerConfig(uint8_t Index, uint32_t Period);
extern uint32_t GetTimeTicks(void);



extern SYSTEM_INFO_T	SystemInfo;
extern GLOBAL_INFO_T	GlobalInfo;



#endif


