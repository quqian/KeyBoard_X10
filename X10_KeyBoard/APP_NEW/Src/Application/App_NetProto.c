#include "includes.h"
#include "App_NetProto.h"

static void App_NetFrameHandle(uint8_t type,uint8_t *data,uint16_t len);

/**
 *	WIFI�������ݻص�����
 */
void BswSrv_WIFI_RxData_Callback(uint8_t *data,uint16_t len)
{
	App_NetFrameHandle(1,data,len);
}


/**
 *	GPRS�������ݻص�����
 */
void BswSrv_GPRS_RxData_Callback(uint8_t *data,uint16_t len)
{
	App_NetFrameHandle(2,data,len);
}



/**
 *	��������Э�鴦��
 */
void App_NetFrameHandle(uint8_t type,uint8_t *data,uint16_t len)
{

}


