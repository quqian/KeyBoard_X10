#ifndef __PROTO_WITH_MAINBOARD_H__
#define __PROTO_WITH_MAINBOARD_H__


#include "includes.h"
#include "flash_usr.h"
#include "flash.h"
#include "m1_card.h"

#define WRITE_SUCCESS           (0x00) //
#define WRITE_ERROR             (0x01)


enum{
    //ENUM_MODUL_BASE = 0x01,
	//ENUM_MODUL_CARD = 0x02,
	//ENUM_MODUL_UPGRADE = 0x03,
	MsgType_ALL 			= 0,
    MsgType_KEY 			= 1,
    MsgType_CARD 			= 2,
    MsgType_BLUE 			= 3,
    ENUM_MODUL_UPGRADE		= 4,
    //
	CMD_UP_SYSTEM_INFO 		= 0x01,  //�ϱ�������Ϣ
	CMD_SYSTEM_OPERATE  	= 0x02,  //ϵͳ����
	CMD_UP_SYSTEM_STATE 	= 0x03,  //״̬�ϱ�
	CMD_WRITE_PCB       	= 0x04,  //дPCB����
	CMD_READ_PCB        	= 0x05,  //��PCB
	//
	CMD_CARD_UP      		= 0x01, //�ϱ�������
	CMD_CARD_READ    		= 0x02, //����
	CMD_CARD_WRITE   		= 0x03, //д��

	//�����ϱ�
	CMD_KEY          		= 0x01,
	//
	CMD_SET_BLUE_NAME   	= 0x01,  //������������
	CMD_SET_BLUE_MAC    	= 0x02,  //�������ص�ַ
	CMD_BLUE_SEND       	= 0x03,  //������Ϣ
	CMD_BLUE_RECV       	= 0x04,  //������Ϣ

	//
	ENUM_UPGRADE_REQUEST 	= 0x01,
	ENUM_SEND_UPGRADE_PKT 	= 0x02,
};

typedef enum {
	CB_FIND_AA=0,
	CB_FIND_FIVE=1,
	CB_FIND_LEN=2,
	CB_FIND_VER=3,
	CB_FIND_SN=4,
	CB_FIND_MODULE=5,
	CB_FIND_CMD=6,
	CB_FIND_DATA=7,
	CB_FIND_CHECKSUM=8,
}CB_FIND;

#pragma pack(1)

typedef struct {
    uint8_t  aa;                                    //0.
    uint8_t  five;                                  //1.
	uint16_t len;								    //2. ����
    uint8_t  ver;                                   //3. �汾��
    uint8_t  sn;                                    //4. ������ˮ��
	uint8_t  module;							    //5. 01-��������   02-������  03-Զ������
    uint8_t  cmd;                                   //6. �������
}CB_HEAD_STR;

typedef struct {
    CB_HEAD_STR head;
    uint8_t  data[256 - sizeof(CB_HEAD_STR)];
}CB_STR_t;


typedef struct{
    uint8_t fw_verson;
    uint8_t BlueState;					// 0������ 1������
    uint8_t BlueVerb[20];					//�����汾
    uint8_t BlueName[12];
	uint8_t BlueAddr[5];				//�������ص�ַ
}REPORT_INFO_STR;			//����8051   -->  HT6X2X��

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}REPORT_INFO_ACK_STR;		//Ӧ��HT6X2X  -->  8051��

typedef struct{
	uint8_t Pcb[8];				//
}PCB_INFO_STR;			//

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}PCB_INFO_ACK_STR;		//

typedef struct{
    uint8_t OperationType;				// 1:��λϵͳ��2:��λ������3:���������㲥����
    uint8_t Para1;						//3:���������㲥���� =0:�رչ㲥��=1:�򿪹㲥
	uint8_t Para2;
	uint8_t Para3;
}OPERATION_STR;							// ���� HT6X2X  -->  8051��

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}OPRATION_ACK_STR;						//Ӧ�� 8051  -->  HT6X2X ��

typedef struct{
    uint8_t KeyVanual;					//��ֵ   ����0~9��10���أ� 11ȷ��
}KEY_VANUAL_STR;						//����8051   -->  HT6X2X��

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}KEY_VANUAL_ACK_STR;

typedef struct{
    uint8_t CardType;					//1���뿨��2�����뿨
    uint8_t CardSn[4];					//�����к�
}CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}CARDINFO_ACK_STR;

typedef struct{
    uint8_t KeyA[6];					//
    uint8_t Sector;					//
    uint8_t BlockNumb;
}READ_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
    uint8_t Data[16];	
}READ_CARDINFO_ACK_STR;

typedef struct{
    uint8_t KeyA[6];					//
    uint8_t Sector;					//
    uint8_t blockNun;
	uint8_t blockData[16];	
}WRITE_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}WRITE_CARDINFO_ACK_STR;

typedef struct{
	uint8_t BlueName[12];	
}BLUE_NAME_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}BLUE_NAME_ACK_STR;

typedef struct{
	uint8_t BlueGateWay[5];	
}BLUE_GATEWAY_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}BLUE_GATEWAY_ACK_STR;

typedef struct{
	uint8_t MSG_Type;					//0��������1��2.4G
	uint16_t MSG_Len;
	uint8_t Data[64];
}MSG_STR;

typedef struct{
    uint8_t result;						// 0�����ճɹ���1������ʧ��
}MSG_ACK_STR;

typedef struct{
    uint32_t filesize;
    uint32_t package;
    uint16_t checkSum;
    uint8_t  fw_verson;
}START_UPGRADE_REQUEST_STR, *pSTART_UPGRADE_REQUEST_STR;

typedef struct{
    uint8_t result;						// 0: ����������1���ܾ�����
}START_UPGRADE_REQUEST_ACK_STR;

typedef struct{
    uint8_t index;
    uint8_t  data[64];
}DOWN_LOAD_FW_STR,*pDOWN_LOAD_FW_STR;

typedef struct{
	uint8_t result;						// 0: ���ճɹ���1��ʧ��
	uint8_t index;
}DOWN_LOAD_FW_STR_ACK;

#pragma pack()



extern uint8_t isSyncSystemInfo;

extern void ComRecvMainBoardData(void);
extern int SendMsg_ACK(uint8_t type, uint8_t cmd, uint8_t result);
extern int CardTypeUpLoad(uint8_t cardType,uint8_t *serialNum);
extern int SyncSystemInfo(void);
//״̬ͬ��
extern int SyncSystemState(void);
extern void App_CB_SendData(CB_STR_t *ptk,uint16_t len, uint8_t  module, uint8_t  cmd);


#endif






