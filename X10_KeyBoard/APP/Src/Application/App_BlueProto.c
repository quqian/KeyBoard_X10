#include "includes.h"
#include "App_BlueProto.h"


uint8_t gBlueInitStep = 0;
int isDevt = 0;
uint8_t gwMac[5];



int SendCKB24Pkt(uint8_t nodeType, BLUE_PKT_STR *pkt, uint16_t len)
{
	int i = 0;
	
    while (GET_BLUE_IRQ()) 
	{
        DelayMsWithNoneOs(50);
        if (100 <= ++i) 
		{
            CL_LOG("CKB24 wait idle timeout,error.\n");
            return CL_FAIL;
        }
    }
	if(GET_CD_STATE() == RESET)
	{
	    CKB24_CD_HIGH();
		DelayMsWithNoneOs(2);
	}

	pkt->head.ab = 0xab;
	pkt->head.cd = 0xcd;
    pkt->head.target = nodeType;
	if(nodeType == 1)
	{
        memcpy(pkt->head.addr, SystemInfo.gw_mac, 5);
    }
    pkt->head.len = len;
    pkt->data[len] = GetPktSum((void*)pkt, sizeof(BLUE_HEAD_STR)+len);
    pkt->data[len+1] = '\r';
    pkt->data[len+2] = '\n';
	//PrintfData("SendBluePkt", (void*)pkt, sizeof(BLUE_HEAD_STR)+len+1+2);
	UsartSend(DEBUG_INDEX, (void*)pkt, sizeof(BLUE_HEAD_STR)+len+1+2);
	
    return CL_OK;
}

const BlUE_INITTAB gBlueInitTab[] =
{
    {NULL, "OK", 3000, 0, BuleCheckReset},
	{"AT+ECHO=0\r\n", "OK", 3000, 10, BuleModeEcho},
	{"AT+VERS?\r\n", "OK", 3000, 10, BuleModeVers},
	{"AT+NAME?\r\n", "OK", 3000, 10, BuleModeGetName},
	{NULL, "OK", 3000, 10, BuleSetName},
	{"AT+PAIR=1\r\n", "OK", 3000, 10, BuleModePair},
};

void CKB24_UM_HardReset(void)
{
    CKB24_POWER_DIS();
	FeedWatchDog();
    DelayMsWithNoneOs(2000);
    CKB24_POWER_EN();
	FeedWatchDog();
    DelayMsWithNoneOs(1000);
}

int BlueCheckRes(char *cmd, char *res, uint16_t tmo, uint8_t *pbuff, uint16_t size)
{
    char *ret=NULL;
    uint8_t c;
    int cnt = 0;
    int retv = CL_FAIL;
    uint16_t time = 0;
    char  temp[20];
	uint32_t BlueTimeTicksFlag = GetTimeTicks();
	
    memset(pbuff, 0, size);

    for(time = 0; time < tmo; time+= 50) 
	{
        DelayMsWithNoneOs(50);
		BlueTimeTicksFlag = GetTimeTicks();
		while(((BlueTimeTicksFlag + 10) >= GetTimeTicks()) && (BlueTimeTicksFlag <= GetTimeTicks()))
		{
			while(CL_OK == FIFO_S_Get(&(UartPortHandleInfo->rxBuffCtrl), &c)) 
			{
				BlueTimeTicksFlag = GetTimeTicks();
				CL_LOG("c:[%x, %c].\n", c, c);
	            if (c) 
				{
	                if (size > cnt) 
					{
	                    pbuff[cnt++] = c;
	                }
					else
					{
	                    //CL_LOG("cnt=%d,error.\n",cnt);
	                }
	            }
	        }
		}
        
        ret = strstr((const char*)pbuff, (const char*)res);
        if (ret) 
		{
			//获取版本号
			if(strstr(cmd,"AT+VERS?"))
			{
				char  temp[21];
				memset(temp,0,21);
				sscanf((char*)pbuff,"+VERS: %s",temp);
				if(strlen(temp) >= 20)
				{
					memcpy(SystemInfo.blue_version, temp,20);
				}
				else
				{
					strcpy((char*)SystemInfo.blue_version,temp);
				}				
				CL_LOG("blue ver:%s.\n", SystemInfo.blue_version);
			}
			//获取蓝牙名称
			if(strstr(cmd,"AT+NAME?"))
			{
				memset(temp,0,20);
				sscanf((char*)pbuff,"+NAME: %s",temp);
				if(strlen(temp) >= 12)
				{
					memcpy(SystemInfo.blue_name, temp,12);
				}
				else
				{
					strcpy(SystemInfo.blue_name, temp);
				}	
				CL_LOG("blue name:%s.\n", SystemInfo.blue_name);
			}
			
			//获取网关mac
			if(strstr(cmd,"AT+GWID?"))
			{
				char  temp[20];
				memset(temp,0,20);
				sscanf((char*)pbuff,"+GWID: %s",temp);
				StringToBCD(gwMac,temp);
				CL_LOG("gwmac:%s.\n",temp);
			}
			//查询是否终端设备
			if(strstr(cmd,"AT+DEVT?"))
			{
				sscanf((char*)pbuff,"+DEVT: %d",&isDevt);
			}
            retv = CL_OK;
            break;
        }
    }

    return retv;
}

int BlueSendCmd(char *cmd, char *ack, uint16_t waittime)
{
	uint8_t gBlueRecvData[128];
	uint8_t res = 1;

    CKB24_CD_LOW();
	FeedWatchDog();
    DelayMsWithNoneOs(2);
    //UsartFlush(BLUE_TOOTH);
    CL_LOG("发送命令[%s].\n", cmd);
	UsartSend(DEBUG_INDEX,(void*)cmd, strlen(cmd));
	
    if ((ack==NULL) || (waittime==0)) 
	{
        CKB24_CD_HIGH();
        return CL_OK;
    }

    if (CL_OK == BlueCheckRes(cmd, ack, waittime, gBlueRecvData, sizeof(gBlueRecvData))) 
	{
        res = CL_OK; /*check success, retrun 0*/
    }
    CKB24_CD_HIGH();
	return res;
}

int BuleCheckReset(char ok, uint8_t retry)
{
    uint8_t i = 0;
    
    CKB24_UM_HardReset();
	
	SystemInfo.blue_state = 1;
	SystemInfo.blue_state = 1;
	
    for (i=0; i<2; i++) 
	{
        if (BlueSendCmd("AT\r\n", "OK", 3000)==0) 
		{
            CL_LOG("blue test ok.\n");
            gBlueInitStep++;
            return CL_OK;
        }
        CL_LOG("blue reset fail,retry.\n");
		FeedWatchDog();
        DelayMsWithNoneOs(2000);
    }
    CL_LOG("blue reset fail.\n");
    return CL_FAIL;
}

int BuleCheckResetProcess(void)
{
    uint8_t i = 0;
    
    CKB24_UM_HardReset();
	
	SystemInfo.blue_state = 1;
    SystemStatus.blue_state = 1;
	
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd("AT\r\n", "OK", 3000) == 0) 
		{
            CL_LOG("blue test ok.\n");
            gBlueInitStep++;
            return CL_OK;
        }
        CL_LOG("blue reset fail,retry.\n");
		FeedWatchDog();
        DelayMsWithNoneOs(2000);
    }
    CL_LOG("blue reset fail.\n");
    return CL_FAIL;
}

int BlueTest(int retry)
{
    uint8_t i = 0;
    
    for (i = 0; i < retry; i++) 
	{
        if (BlueSendCmd("AT\r\n", "OK", 3000) == 0) 
		{
        //    CL_LOG("AT命令检测成功.\n");
			
            return CL_OK;
        }
        if(retry > 1)
		{
            FeedWatchDog();
            DelayMsWithNoneOs(1000);
        }
    }
	
    return ERROR;
}

int BuleModeEchoProcess(void)
{
    uint8_t i = 0;
    
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd("AT+ECHO=1\r\n", "OK", 3000) == 0) 
		{
            CL_LOG("回显 ok.\n");
            gBlueInitStep++;
            return CL_OK;
        }
        CL_LOG("蓝牙回显失败,retry.\n");
        DelayMsWithNoneOs(2000);
    }
    CL_LOG("回显失败.\n");
    return CL_FAIL;
}

int BuleModeVerProcess(void)
{
    uint8_t i = 0;
    
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd("AT+VERS?\r\n", "OK", 3000) == 0) 
		{
            CL_LOG("获取版本 ok.\n");
            gBlueInitStep++;
            return CL_OK;
        }
        CL_LOG("获取版本失败,retry.\n");
        DelayMsWithNoneOs(2000);
    }
    CL_LOG("获取版本失败.\n");
    return CL_FAIL;
}

int BuleGetNameProcess(void)
{
    uint8_t i = 0;
    
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd("AT+NAME?\r\n", "OK", 3000) == 0) 
		{
            CL_LOG("获取名字 ok.\n");
            gBlueInitStep++;
            return CL_OK;
        }
        CL_LOG("获取名字失败,retry.\n");
        DelayMsWithNoneOs(2000);
    }
    CL_LOG("获取名字失败.\n");
	
    return CL_FAIL;
}

int CheckBlueModeCmd(char ok, uint8_t retry, uint16_t delay)
{
    if (ok==0) 
	{
        gBlueInitStep++;
        return CL_OK;
    }
	else if (retry > 10) 
	{
        gBlueInitStep = BLUE_RESET;
	}
#if 0
	if(gTestInfo.testMode == 1)
	{
		BlueAgeTestResult = CL_FAIL;
		if(gTestInfo.testingItem == 0xFE)
		{
			gTestInfo.testingItem = 0xFF;
		}
	}
#endif

	return CL_FAIL;
}

int BuleModeEcho(char ok, uint8_t retry)
{
    CL_LOG("in.\n");
    return CheckBlueModeCmd(ok, retry, 0);
}

int BuleModeVers(char ok, uint8_t retry)
{
    CL_LOG("in.\n");
    return CheckBlueModeCmd(ok, retry, 0);
}

int BuleModeGetName(char ok, uint8_t retry)
{
    CL_LOG("in.\n");
    return CheckBlueModeCmd(ok, retry, 0);
}

int isArraryEmpty(uint8_t *array,int len)
{
    uint32_t i = 0;
    
	for(i = 0; i < len ; i++)
	{
		if(array[i] != 0x00)
		{
			return CL_FAIL;
		}
	}
	
	return CL_OK;
}

//设置2.4G gwmac地址
int SetGW_MacAddr(char *mac)
{
	uint32_t i = 0;
    char  buf[32];
	
    memset(buf,0,32);
	
    sprintf(buf,"AT+GWID=%s\r\n", mac);
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd(buf, "OK", 2000) == CL_OK) 
		{
            return CL_OK;
        }
		DelayMsWithNoneOs(1000);
    }
    return CL_FAIL;
}

//设置蓝牙名称
int SetBlueName(char *name)
{
    uint32_t i = 0;
    char  buf[32];
	
    memset(buf,0,32);
	
    sprintf(buf,"AT+NAME=%s\r\n",name);
    for (i = 0; i < 2; i++) 
	{
        if (BlueSendCmd(buf, "OK", 2000)==CL_OK) 
		{
            return CL_OK;
        }
        DelayMsWithNoneOs(1000);
    }
    return CL_FAIL;
}

int BuleSetName(char ok, uint8_t retry)
{
	char name[12] = {0,};
    
    CL_LOG("in.\n");

	if(isArraryEmpty(&GlobalInfo.station_id[3],5) == CL_OK)
	{
		CL_LOG("charger sn is all zero, no need to set blue name.\n");
		gBlueInitStep++;
        return CL_OK;
	}
	
	BCDToString(name,(void*)&GlobalInfo.station_id[3], 5);
	if (0 == memcmp(SystemInfo.blue_name, name, 10)) 
	{
		CL_LOG("name is ok, no need to set blue name.\n");
        gBlueInitStep++;
        return CL_OK;
	}
	
	
	if(SetBlueName(name) == CL_OK)
	{
		CL_LOG("set name=%s ok.\n",name);
		gBlueInitStep++;
		return CL_OK;
	}
	
    CL_LOG("set name fail.\n");
    return CL_FAIL;
}

int BuleModePair(char ok, uint8_t retry)
{
    CL_LOG("in.\n");
    return CheckBlueModeCmd(ok, retry, 0);
}

#if 1
int BuleReconnect(void)
{
	uint8_t step = BLUE_RESET;
	uint8_t retry = 0;
	
	while(1)
	{
		switch (step)
		{
			case BLUE_RESET:
				if(CL_OK == BuleCheckResetProcess())
				{
					step = BLUE_MODE_ECHO;
					retry = 0;
				}
				else
				{
					if(retry++ > 10)
					{
						return CL_FAIL;
					}
				}
			break;
			case BLUE_MODE_ECHO:
				if(CL_OK == BuleModeEchoProcess())
				{
					step = BLUE_MODE_VER;
					retry = 0;
				}
				else
				{
					if(retry++ > 10)
					{
						return CL_FAIL;
					}
				}
			break;
			case BLUE_MODE_VER:
				if(CL_OK == BuleModeVerProcess())
				{
					step = BLUE_MODE_GETNAME;
					retry = 0;
				}
				else
				{
					if(retry++ > 10)
					{
						return CL_FAIL;
					}
				}
			break;
			case BLUE_MODE_GETNAME:
				if(CL_OK == BuleGetNameProcess())
				{
					step = BLUE_MODE_SETNAME;
					retry = 0;
				}
				else
				{
					if(retry++ > 10)
					{
						return CL_FAIL;
					}
				}
			break;
			default:
				CL_LOG("错误.\n");
			//	step = BLUE_RESET;
			break;
		}
		DelayMsWithNoneOs(200);
	}
}
#else
int BuleReconnect(void)
{
	char ok;
    uint8_t retry = 0;

	gBlueInitStep = BLUE_RESET;
	while(1) 
	{
        DelayMsWithNoneOs(gBlueInitTab[gBlueInitStep].nwait*10);
        if (gBlueInitTab[gBlueInitStep].cmd) 
		{
            ok = BlueSendCmd(gBlueInitTab[gBlueInitStep].cmd,gBlueInitTab[gBlueInitStep].res,gBlueInitTab[gBlueInitStep].wait);
        }
        if (gBlueInitTab[gBlueInitStep].process) 
		{
            if (CL_OK == gBlueInitTab[gBlueInitStep].process(ok, retry)) 
			{
                retry = 0;
            }
			else
			{
                retry++;
            }
        }

		if (gBlueInitStep == BLUE_STATE_NUM) 
		{
			CL_LOG("blue init ok.\n");
			SystemInfo.blue_state = 0;
			SystemInfo.blue_state = 0;
			
			return CL_OK;
		}
	}
}
#endif

void BlueUsartInit(void)
{
    UARTx_Init(DEBUG_INDEX, ENABLE, 115200);	//蓝牙
}

void BlueGPIO_Conf(void)
{
#if 1
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	
	//BT_EN 
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
	gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

	//BT_RST
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_8);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
	
	//BT_IRQ
	gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_15);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

	//BT_C/D
	gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_9);
	gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    CKB24_POWER_EN();
    
    CKB24_RST_HIGH();
#endif
}

void Bluetooth_Init(void)
{
	//GPIO初始化
    BlueGPIO_Conf();
	
	BlueUsartInit();
}


void BswSrv_Blue_RxData_Callback(uint8_t *data,uint16_t len)
{

}

//转发蓝牙、2.4G数据
int TransBlueData(unsigned char *gBuleRxData,unsigned int datalen)
{
	BLUE_PKT_STR* pBuleRxData = (BLUE_PKT_STR*)gBuleRxData;
//	uint8_t nodeType = pBuleRxData->head.target;
	uint8_t FrameBuff[256] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
    FeedWatchDog();

	memcpy(pBuff->data, pBuleRxData->data, datalen);
    App_CB_SendData(pBuff, datalen, MsgType_BLUE, CMD_BLUE_SEND);
//	PrintfData("PCBBumUpLoad", (uint8_t*)pBuff, sizeof(PCB_INFO_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

void HandleBlueMsg(void)
{
	static uint8_t  pktBuff[256] = {0,};
    static uint8_t  step = BLUE_AB;
    uint8_t  data;
    static uint8_t  sum;
    static uint16_t pktLen;
    static uint16_t len;
	static uint8_t  target;
	static uint16_t msgLen;

	while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo[0].rxBuffCtrl), &data)) 
	{
		switch (step) 
		{
			case BLUE_AB:
				if (data == 0xAB) 
				{
					step = BLUE_CD;
					pktBuff[0] = 0xAB;
					pktLen = 1;
					sum = 0xAB;
				}
				break;

			case BLUE_CD:
				if (data == 0xcd) 
				{
					step = BLUE_TARGET;
					pktBuff[1] = 0xcd;
					pktLen++;
					sum += data;
				}
				else if (data == 0xAB) 
				{
					step = BLUE_CD;
					pktBuff[0] = 0xAB;
					pktLen = 1;
					sum = 0xAB;
					CL_LOG("can not find cd.\n");
				}
				else
				{
					step = BLUE_AB;
					CL_LOG("can not find cd.\n");
				}
				break;

			case BLUE_TARGET:
				pktBuff[pktLen++] = data;
				len = 0;
				if(data == NODE_BLUE)
				{
					step = BLUE_LEN;
					GlobalInfo.isBuleConnect = 1;
					GlobalInfo.lastConnectTime = GetTimeTicks();
				}
				else
				{
					step = BLUE_ADDR;
				}
				sum += data;
				target = data;
				break;

			case BLUE_ADDR:
				pktBuff[pktLen++] = data;
				sum += data;
				if (6 == ++len) 
				{
					len = 0;
					step = BLUE_LEN;
				}
				break;

			case BLUE_LEN:
				pktBuff[pktLen++] = data;
				sum += data;
				if (2 == ++len) 
				{
					len = (pktBuff[pktLen-1]<<8) | pktBuff[pktLen-2];
					msgLen = len;
					if ((255) < len) 
					{
						step = BLUE_AB;
						CL_LOG("len=%d,error.\n", len);
					}
					else if(len == 0)
					{
						step = BLUE_CHK;
					}
					else
					{
						step = BLUE_RX_DATA;
					}
				}
				break;
			case BLUE_RX_DATA:
				pktBuff[pktLen++] = data;
				sum += data;
				if (0 == --len) 
				{
					step = BLUE_CHK;
				}
				break;

			case BLUE_CHK:
				pktBuff[pktLen++] = data;
				if (data == sum) 
				{
					step = BLUE_END;
					len = 0;
				}
				else
				{
					CL_LOG("recv data checksum error,sum=%#x,pkt sum=%#x.\n",sum,data);
					step = BLUE_AB;
				}
				break;

			case BLUE_END:
				if (2 == ++len) 
				{
					if (NODE_BLUE == target) 
					{
					#if 1
						//datalen 表示有效数据长度
                    	TransBlueData(pktBuff, msgLen);//数据透传
					#else
                        uint8_t *p = NULL;
						p = pktBuff+sizeof(BLUE_RX_HEAD_STR);
						for (len=0; len<msgLen; ) 
						{
							if (CL_OK == FIFO_S_Put(&gCKBCtrl.rxBtBuff, p[len])) 
							{
								len++;
							}
							else
							{
								CL_LOG("bt buff over flow error.\n");
								OS_DELAY_MS(2);
							}
						}
					#endif
					}
					step = BLUE_AB;
				}
				break;

			default:
				step = BLUE_AB;
				break;
		}
	}
}



