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
	CMD_UP_SYSTEM_INFO 		= 0x01,  //上报基本信息
	CMD_SYSTEM_OPERATE  	= 0x02,  //系统操作
	CMD_UP_SYSTEM_STATE 	= 0x03,  //状态上报
	CMD_WRITE_PCB       	= 0x04,  //写PCB编码
	CMD_READ_PCB        	= 0x05,  //读PCB
	//
	CMD_CARD_UP      		= 0x01, //上报卡类型
	CMD_CARD_READ    		= 0x02, //读卡
	CMD_CARD_WRITE   		= 0x03, //写卡

	//按键上报
	CMD_KEY          		= 0x01,
	//
	CMD_SET_BLUE_NAME   	= 0x01,  //设置蓝牙名称
	CMD_SET_BLUE_MAC    	= 0x02,  //设置网关地址
	CMD_BLUE_SEND       	= 0x03,  //发送消息
	CMD_BLUE_RECV       	= 0x04,  //接收消息

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
	uint16_t len;								    //2. 长度
    uint8_t  ver;                                   //3. 版本号
    uint8_t  sn;                                    //4. 报文流水号
	uint8_t  module;							    //5. 01-基本操作   02-卡操作  03-远程升级
    uint8_t  cmd;                                   //6. 命令代码
}CB_HEAD_STR;

typedef struct {
    CB_HEAD_STR head;
    uint8_t  data[256 - sizeof(CB_HEAD_STR)];
}CB_STR_t;


typedef struct{
    uint8_t fw_verson;
    uint8_t BlueState;					// 0—正常 1—故障
    uint8_t BlueVerb[20];					//蓝牙版本
    uint8_t BlueName[12];
	uint8_t BlueAddr[5];				//蓝牙网关地址
}REPORT_INFO_STR;			//请求（8051   -->  HT6X2X）

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}REPORT_INFO_ACK_STR;		//应答（HT6X2X  -->  8051）

typedef struct{
	uint8_t Pcb[8];				//
}PCB_INFO_STR;			//

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}PCB_INFO_ACK_STR;		//

typedef struct{
    uint8_t OperationType;				// 1:复位系统；2:复位蓝牙；3:设置蓝牙广播开关
    uint8_t Para1;						//3:设置蓝牙广播开关 =0:关闭广播；=1:打开广播
	uint8_t Para2;
	uint8_t Para3;
}OPERATION_STR;							// 请求 HT6X2X  -->  8051）

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}OPRATION_ACK_STR;						//应答（ 8051  -->  HT6X2X ）

typedef struct{
    uint8_t KeyVanual;					//键值   数字0~9；10返回； 11确认
}KEY_VANUAL_STR;						//请求（8051   -->  HT6X2X）

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}KEY_VANUAL_ACK_STR;

typedef struct{
    uint8_t CardType;					//1密码卡；2非密码卡
    uint8_t CardSn[4];					//卡序列号
}CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}CARDINFO_ACK_STR;

typedef struct{
    uint8_t KeyA[6];					//
    uint8_t Sector;					//
    uint8_t BlockNumb;
}READ_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
    uint8_t Data[16];	
}READ_CARDINFO_ACK_STR;

typedef struct{
    uint8_t KeyA[6];					//
    uint8_t Sector;					//
    uint8_t blockNun;
	uint8_t blockData[16];	
}WRITE_CARDINFO_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}WRITE_CARDINFO_ACK_STR;

typedef struct{
	uint8_t BlueName[12];	
}BLUE_NAME_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}BLUE_NAME_ACK_STR;

typedef struct{
	uint8_t BlueGateWay[5];	
}BLUE_GATEWAY_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}BLUE_GATEWAY_ACK_STR;

typedef struct{
	uint8_t MSG_Type;					//0：蓝牙；1：2.4G
	uint16_t MSG_Len;
	uint8_t Data[64];
}MSG_STR;

typedef struct{
    uint8_t result;						// 0：接收成功；1：接收失败
}MSG_ACK_STR;

typedef struct{
    uint32_t filesize;
    uint32_t package;
    uint16_t checkSum;
    uint8_t  fw_verson;
}START_UPGRADE_REQUEST_STR, *pSTART_UPGRADE_REQUEST_STR;

typedef struct{
    uint8_t result;						// 0: 可以升级；1：拒绝升级
}START_UPGRADE_REQUEST_ACK_STR;

typedef struct{
    uint8_t index;
    uint8_t  data[64];
}DOWN_LOAD_FW_STR,*pDOWN_LOAD_FW_STR;

typedef struct{
	uint8_t result;						// 0: 接收成功；1：失败
	uint8_t index;
}DOWN_LOAD_FW_STR_ACK;

#pragma pack()



extern uint8_t isSyncSystemInfo;

extern void ComRecvMainBoardData(void);
extern int SendMsg_ACK(uint8_t type, uint8_t cmd, uint8_t result);
extern int CardTypeUpLoad(uint8_t cardType,uint8_t *serialNum);
extern int SyncSystemInfo(void);
//状态同步
extern int SyncSystemState(void);
extern void App_CB_SendData(CB_STR_t *ptk,uint16_t len, uint8_t  module, uint8_t  cmd);


#endif






