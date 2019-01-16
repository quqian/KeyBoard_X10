#ifndef __APP_BLUE_PROTO_H__
#define __APP_BLUE_PROTO_H__

#include "usart.h"



#define CKB24_POWER_EN()    gpio_bit_set(GPIOA, GPIO_PIN_8) 
#define CKB24_POWER_DIS()   gpio_bit_reset(GPIOA, GPIO_PIN_8)

#define CKB24_RST_HIGH()    gpio_bit_set(GPIOB, GPIO_PIN_8) 
#define CKB24_RST_LOW()     gpio_bit_reset(GPIOB, GPIO_PIN_8) 

#define CKB24_CD_HIGH()     gpio_bit_set(GPIOB, GPIO_PIN_9)
#define CKB24_CD_LOW()      gpio_bit_reset(GPIOB, GPIO_PIN_9)
#define GET_CD_STATE()		gpio_input_bit_get(GPIOB, GPIO_PIN_9)

#define GET_BLUE_IRQ()      gpio_input_bit_get(GPIOB, GPIO_PIN_15)


enum{
    BLUE_RESET = 0,
	BLUE_MODE_ECHO,
	BLUE_MODE_VER,
	BLUE_MODE_GETNAME,
#if SET_BLUE_NAME
	BLUE_MODE_SETNAME,
#endif
	BLUE_MODE_PAIR,
    BLUE_STATE_NUM
};

enum{
    NODE_BLUE = 0,
    NODE_24G
};

enum {
    BLUE_AB,
    BLUE_CD,
    BLUE_TARGET,
    BLUE_ADDR,
    BLUE_LEN,
    BLUE_RX_DATA,
    BLUE_CHK,
    BLUE_END,
};

typedef struct{
    char *cmd;
    char *res;
    int wait;
    int nwait;
    int (*process)(char ok, uint8_t retry);
}BlUE_INITTAB;

#pragma pack(1)

typedef struct {
	uint8_t  ab;                                //0.
    uint8_t  cd;                                //1.
    uint8_t  target;                            //2.
    uint16_t len;            
}BLUE_RX_HEAD_STR;

typedef struct {
    uint8_t  ab;                                //0.
    uint8_t  cd;                                //1.
    uint8_t  target;                            //2.
    uint8_t  addr[6];                           //3.
    uint16_t len;                               //4.
}BLUE_HEAD_STR;

typedef struct {
    BLUE_HEAD_STR head;
    uint8_t  data[255];
}BLUE_PKT_STR;

typedef struct {
    BLUE_RX_HEAD_STR head;
    uint8_t  data[255];
}BLUE_RECEIVE_PKT_STR;


#pragma pack()


extern void Bluetooth_Init(void);
extern int BuleCheckReset(char ok, uint8_t retry);
extern int BuleModeEcho(char ok, uint8_t retry);
extern int BuleModeVers(char ok, uint8_t retry);
extern int BuleModeGetName(char ok, uint8_t retry);
extern int BuleSetName(char ok, uint8_t retry);
extern int BuleModePair(char ok, uint8_t retry);
extern int BuleReconnect(void);
extern int BlueSendCmd(char *cmd, char *ack, uint16_t waittime);
extern int SendCKB24Pkt(uint8_t nodeType, uint8_t *pkt, uint16_t len);
extern int SetBlueName(char *name);
extern int SetGW_MacAddr(char *mac);
extern int TransBlueData(unsigned char *gBuleRxData,unsigned int datalen);
extern int BlueTest(int retry);
extern void HandleBlueMsg(void);
extern void BlueToothHandle(void);
extern int BuleModePairProcess(void);


#endif





