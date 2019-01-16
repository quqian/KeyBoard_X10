#ifndef __NFCARD_H__
#define __NFCARD_H__



#include "includes.h"

#define ENTRANCE_GUARD_CARD_SECTOR_OFFSET       (10)  //门禁卡扇区偏移

//读卡处理
extern void Check_M1_Card(void);

extern void NFCardTask(void);
extern int BswDrv_FM175XX_SetPowerDown(uint8_t mode);
extern void NFCardTaskHandle(void);

#endif





