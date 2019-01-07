#include <stdint.h>
#include "ProtoWithMainBoard.h"
#include "system.h"
#include "usart.h"
#include "rtc.h"



START_UPGRADE_REQUEST_STR UpgradeRequestArray = {0,};

int Upgrade_FlashCheck(void)
{
	uint16_t checksum = 0;
	uint8_t  rbyte = 0;
	uint32_t i = 0;

	FeedWatchDog();
	for(i = 0; i < UpgradeRequestArray.filesize; i++)
	{
		//FeedWatchDog();
		FlashReadByte(AppUpBkpAddr + i, (void *)&rbyte, 1);
		checksum += rbyte;
        //printf("boot checksum[%#x], [%#x]\r\n", checksum, rbyte);
	}
	
	if(checksum != UpgradeRequestArray.checkSum)
	{
        printf("У�����!!![%#x], [%#x]\r\n", checksum, UpgradeRequestArray.checkSum);
		return 1;
	}
	
	return 0;
}

/*****************************************************************************
** Function name:       Upgrade_Write
** Descriptions:        
** input parameters:    pMsg: �ڲ�Э������ͷָ��
** output parameters:   None
** Returned value:	  None
** Author:              quqian
*****************************************************************************/
uint8_t Upgrade_Write(uint8_t *pData,uint16_t len)
{
    SYS_UPDATE_INFO_STR info; 
	SYS_UPDATE_INFO_STR UpgradeInfo; 
    uint32_t i = 0;
	
    //PrintfData("������������",pData,len);
    
	if(1 == FlashWrite(((uint32_t)AppUpBkpAddr + GlobalInfo.UpgradeFw.ReceiveSize), pData, len))
	{
		printf("11111111111111111, error!");
		return 1;
	}
    //CL_LOG("upgrade current package=%d.\n",fw_info.current_package);
    CL_LOG("total %d,get %d, [%d%%].\n", UpgradeRequestArray.filesize, (GlobalInfo.UpgradeFw.ReceiveSize + len), (GlobalInfo.UpgradeFw.ReceiveSize + len) * 100 / UpgradeRequestArray.filesize);
    if((GlobalInfo.UpgradeFw.CurrentPackage == UpgradeRequestArray.package) && (0 < UpgradeRequestArray.package) && 
		(UpgradeRequestArray.filesize == (GlobalInfo.UpgradeFw.ReceiveSize + len)))
    {
        CL_LOG("���һ�����Ѿ�д��, ��ʼУ��.\n");
		printf("CurrentPackage = [%d], [%d]\n", GlobalInfo.UpgradeFw.CurrentPackage, UpgradeRequestArray.package);
		printf("ReceiveSize = [%d], [%d]\n", GlobalInfo.UpgradeFw.ReceiveSize + len, UpgradeRequestArray.filesize);
		CL_LOG("GlobalInfo.UpgradeFw.AddrOffset = [%#x].\n", GlobalInfo.UpgradeFw.ReceiveSize);
        //���ݰ�������� У�����ݺ�
        if(Upgrade_FlashCheck() != 0)
		{
            CL_LOG("upgrade checksum error.\n");
            return 1;
        }
        //д����Ϣͷ
        
		for(i = 0; i < 3; i++)
		{
			memset(&info, 0, sizeof(info));
			memset(&UpgradeInfo, 0, sizeof(UpgradeInfo));
			info.checkSum = UpgradeRequestArray.checkSum;
	        info.size = UpgradeRequestArray.filesize;
	        info.updateFlag = 0xAA55BCDE;
		    {
		    	fmc_unlock();
				fmc_page_erase(UPGRADE_INFO);//�����������
				fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
				fmc_lock();
		    }
			FlashWrite((uint32_t)(UPGRADE_INFO), (void*)&info, sizeof(info));
			
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&UpgradeInfo, sizeof(UpgradeInfo));
			DelayMsWithNoneOs(20);
			FlashReadByte((uint32_t)UPGRADE_INFO, (void *)&info, sizeof(info));
			if((info.checkSum == UpgradeRequestArray.checkSum) && (info.size == UpgradeRequestArray.filesize) && (info.updateFlag == 0xAA55BCDE) &&
				(info.checkSum == UpgradeInfo.checkSum) && (info.size == UpgradeInfo.size) && (info.updateFlag == UpgradeInfo.updateFlag))
			{
				CL_LOG("upgrade success.\n");
				PlayVoice(VOIC_SUCCESS);
				//CL_LOG("upgrade success. size=%d checksum=%X\n", fw_info.size,fw_info.checkSum);
				return 2;
			}
			else
			{
	            CL_LOG("upgrade error, [%#x], [%#x], [%#x], error.\n", info.checkSum, info.size, info.updateFlag);
	            CL_LOG("UpgradeRequestArray upgrade error, [%#x], [%#x], error.\n", UpgradeRequestArray.checkSum, UpgradeRequestArray.filesize);
				if(3 == i)
				{
					return 1;
				}
			}
		}
    }
    else
	{
        GlobalInfo.UpgradeFw.CurrentPackage++;
		GlobalInfo.UpgradeFw.ReceiveSize += len;
    }
    
    return 0;
}

/*****************************************************************************
** Function name:       COM_Upgrade_Write
** Descriptions:        
** input parameters:    pMsg: �ڲ�Э������ͷָ��
** output parameters:   None
** Returned value:	  	None
** Author:              quqian
*****************************************************************************/
uint8_t COM_Upgrade_Write(uint8_t *data, uint16_t datalen)
{
	uint8_t index = data[0];
	uint8_t *pData = &data[1];
	uint16_t len = datalen - 10;
//	static uint32_t checkSum = 0;
//	uint32_t i = 0;
//	
//	for(i = 0; i < len; i++)
//	{
//		checkSum += pData[i];
//	}
//	CL_LOG("checkSum=[%#X], [%#X], [%d]\n", checkSum, len, index);
	//CL_LOG("index %d. \n", index);
	//CL_LOG("upgradeInfo.current_package = %d, len = %d. \n", fw_info.current_package, len);
//	CL_LOG("aaaaaaa[%#X], [%d]\n", len, index);
//	PrintfData("qqqqqqqq", pData, len);
	if((len > 64))
	{
		CL_LOG("len [%d], [%d],error!\n", len, index);
		return 1;
	}
	if((GlobalInfo.UpgradeFw.CurrentPackage != 1))
	{
		if((GlobalInfo.UpgradeFw.lastIndex == index))
		{
			CL_LOG("current pacakge is repeat transmate.[%d], [%d]!\n", len, index);
			return 0;
		}
	}
	GlobalInfo.UpgradeFw.lastIndex = index;
	
	return Upgrade_Write(pData, len);
}

void App_CB_SendData(CB_STR_t *ptk,uint16_t len, uint8_t  module, uint8_t  cmd)
{
    ptk->head.aa = 0xAA;
	ptk->head.five = 0x55;
    ptk->head.sn = GlobalInfo.packageSn++;
    ptk->head.len = 5 + len;
    ptk->head.ver = 0x01;
	ptk->head.module = module;
	ptk->head.cmd = cmd;
	
    ptk->data[len] = GetPktSum((uint8_t*)&ptk->head.ver, len + 4);
    ptk->data[len + 1] = 0x0d;
    ptk->data[len + 2] = 0x0a;
    
	UsartSend(RS232_INDEX, (void*)ptk, sizeof(CB_HEAD_STR) + len + 1);
}

//������Ӧ��Ϣ
int SendMsg_ACK(uint8_t type, uint8_t cmd, uint8_t result)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	REPORT_INFO_ACK_STR* ShakeHandAck = (REPORT_INFO_ACK_STR*)pBuff->data;

	ShakeHandAck->result = result;
	
	App_CB_SendData(pBuff, sizeof(REPORT_INFO_ACK_STR), type, cmd);
	PrintfData("SendMsg_ACK", (uint8_t*)pBuff, sizeof(REPORT_INFO_ACK_STR) + sizeof(CB_HEAD_STR) + 2);
	
    return CL_OK;
}

//�ϱ�PCB���
int PCBBumUpLoad(uint8_t *pcb)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	PCB_INFO_STR* PcbAck = (PCB_INFO_STR*)pBuff->data;
    FeedWatchDog();
	memcpy(PcbAck->Pcb, pcb, 8);
    App_CB_SendData(pBuff, sizeof(PCB_INFO_STR), MsgType_ALL, CMD_READ_PCB);
//	PrintfData("PCBBumUpLoad", (uint8_t*)pBuff, sizeof(PCB_INFO_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

//ͬ��������Ϣ
int SyncSystemInfo(void)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	SYSTEM_INFO_T* SystemInfoAck = (SYSTEM_INFO_T*)pBuff->data;
    FeedWatchDog();
	memcpy(SystemInfoAck, &SystemInfo, sizeof(SystemInfo));
    App_CB_SendData(pBuff, sizeof(SYSTEM_INFO_T), MsgType_ALL, CMD_UP_SYSTEM_INFO);
//	PrintfData("SyncSystemInfo", (uint8_t*)pBuff, sizeof(SYSTEM_INFO_T) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

//״̬ͬ��
int SyncSystemState(void)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	SYSTEM_STATUS_T* SystemStatusAck = (SYSTEM_STATUS_T*)pBuff->data;

	memcpy(&SystemStatusAck->system_state, &SystemStatus, sizeof(SystemStatus));
    App_CB_SendData(pBuff, sizeof(SYSTEM_STATUS_T), MsgType_ALL, CMD_UP_SYSTEM_STATE);
//	PrintfData("SyncSystemState", (uint8_t*)pBuff, sizeof(SYSTEM_STATUS_T) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

//�����ϱ�
int CardNumUpLoad(unsigned char result,unsigned char *cardNum)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	READ_CARDINFO_ACK_STR* CardAck = (READ_CARDINFO_ACK_STR*)pBuff->data;

	CardAck->result = result;
	memcpy(CardAck->Data, cardNum, 16);
	
    App_CB_SendData(pBuff, sizeof(READ_CARDINFO_ACK_STR), MsgType_CARD, CMD_CARD_READ);
//	PrintfData("CardNumUpLoad", (uint8_t*)pBuff, sizeof(READ_CARDINFO_ACK_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

//�������ϱ�
int CardTypeUpLoad(uint8_t cardType,uint8_t *serialNum)
{
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	CARDINFO_STR* CardAck = (CARDINFO_STR*)pBuff->data;

	CardAck->CardType = cardType;
	memcpy(CardAck->CardSn, serialNum, 4);
	
    App_CB_SendData(pBuff, sizeof(CARDINFO_STR), MsgType_CARD, CMD_CARD_UP);
//	PrintfData("CardTypeUpLoad", (uint8_t*)pBuff, sizeof(CARDINFO_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}

/*****************************************************************************
** Function name:       WriteCardBlockSuccessUpLoad
** Descriptions:        
** input parameters:    None
** output parameters:   None
** Returned value:	  None
** Author:              quqian
*****************************************************************************/
int WriteCardBlockSuccessUpLoad(uint8_t result)
{	
	uint8_t FrameBuff[128] = {0,};
	CB_STR_t * pBuff = (void*)FrameBuff;
	WRITE_CARDINFO_ACK_STR* WriteCardAck = (WRITE_CARDINFO_ACK_STR*)pBuff->data;

	WriteCardAck->result = result;
	
    App_CB_SendData(pBuff, sizeof(WRITE_CARDINFO_ACK_STR), MsgType_CARD, CMD_CARD_WRITE);
//	PrintfData("WriteCardBlockSuccessUpLoad", (uint8_t*)pBuff, sizeof(WRITE_CARDINFO_ACK_STR) + sizeof(CB_HEAD_STR) + 2);
    
    return 0;
}



void MainBoardBasicInfo(CB_STR_t *pBuff, uint16_t len)
{
	uint8_t pcb[8];
	
    switch (pBuff->head.cmd) 
	{
		case CMD_UP_SYSTEM_INFO:
			isSyncSystemInfo = 1;
            CL_LOG("recv system info ack..\n");
		return;
		case CMD_SYSTEM_OPERATE:
		{
            SendMsg_ACK(MsgType_ALL, CMD_SYSTEM_OPERATE, 0);

            //ϵͳ��λ
            if(pBuff->data[0] == 0x01)
			{
                CL_LOG("�յ�����ĸ�λϵͳ����.\n");
                NVIC_SystemReset();
            }
            else if(pBuff->data[0] == 0x02)		//������λ
			{
                CL_LOG("BlueBluetooth_Reset req .\n");
                //BlueBluetooth_Reset();
            }
            else if(pBuff->data[0] == 0x03)		//�����㲥����
			{
               // SetBluePair(st.msg[1]);
            }
        }
		return;
		case CMD_UP_SYSTEM_STATE:
            return;
		case CMD_WRITE_PCB:		//дPCB
			FlashWrite(PCB_INFO, &pBuff->data[0], 8);
			SendMsg_ACK(MsgType_ALL, CMD_WRITE_PCB, 0);
            return;
		case CMD_READ_PCB:		//��PCB
            FlashReadByte(PCB_INFO, pcb, 8);
            PCBBumUpLoad(pcb);
            return;
		default:
            CL_LOG("����.\n");
		return;
	}
}

#define M1_CARD_BLOCK_LENTH      16
void MainBoardCardInfo(CB_STR_t *pBuff, uint16_t len)
{
	uint8_t block_data[M1_CARD_BLOCK_LENTH] = {0,};
	
    switch (pBuff->head.cmd) 
	{
		case CMD_CARD_UP:
            if(pBuff->data[0] != 0)
			{
                TypeA_Halt();
            }
            return;
		case CMD_CARD_READ:
		{
            #if 1
            if(pBuff->head.len >= 8)
            {
                uint8_t keyA[6];
                uint8_t sector ,block;
//                uint8_t sectorBlock;
                uint8_t PICC_ATQA1[2],PICC_SAK1[3],PICC_UID1[4];

                memcpy(keyA, pBuff->data, 6);
                //Debug_Log("��Կ����Կ\r\n");
                PrintfData("keyA", keyA, 6);

                sector = pBuff->data[6];
                block = pBuff->data[7];
//                sectorBlock = sector*4 + block;
                printf("Ѱ��, sector = %d, block = %d \r\n", sector, block);
                //Ѱ��
                if (TypeA_CardActivate(PICC_ATQA1, PICC_UID1, PICC_SAK1) != OK)
                {
                    TypeA_Halt();
                    CardNumUpLoad(1, block_data);
                    printf("Card Author failed.\r\n");
                    return;
                }
			//	CL_LOG("Ѱ��ͨ��.\r\n");
			//	PrintfData("aaa PICC_UID1:", PICC_UID1, 4);
            //	PrintfData("aaa PICC_ATQA1:", PICC_ATQA1, 2);
                ////M1��
//                if(PICC_ATQA1[0]!=0x04)
//                {
//                    TypeA_Halt();
//                    CardNumUpLoad(1, block_data);
//                    CL_LOG("Card Author failed.\r\n");
//                    return;
//                }

                //Debug_Log("������֤\r\n");
                //������֤
                if(Mifare_Auth(0, sector, keyA, PICC_UID1) != CL_OK)
                {
                    TypeA_Halt();
                    CardNumUpLoad(1, block_data);
                    printf("������֤û��ͨ��\r\n");
                    return;
                }
             //   CL_LOG("������֤ͨ��\r\n");

                //Debug_Log("����");
                //Debug_Hex(&sectorBlock, 1);
                //Debug_Log("����\r\n");
                //�����ݿ�
                if(Mifare_Blockread(sector*4+block, block_data) != OK)
                {
                    TypeA_Halt();
                    CardNumUpLoad(1,block_data);
                    printf("Card Author failed.\r\n");
                    return;
                }
                //Debug_Log("����");
                //Debug_Hex(&sectorBlock, 1);
                //Debug_Log("����ͨ��\r\n");

                //Debug_Log("�����ݿ�:");
                //Debug_Hex(&sectorBlock, 1);
                //Debug_Log(":");
                //Debug_Hex(&block_data[0], 16);
                //Debug_Log("\r\n");

                //�ϴ�����
                CardNumUpLoad(0,&block_data[0]);
                TypeA_Halt();
            }
            #endif
		}
		return;
		case CMD_CARD_WRITE:
		{
            #if 1
            uint8_t keyA[6];
            uint8_t sector ,block, sectorBlock;
            uint8_t PICC_ATQA1[2], PICC_SAK1[3], PICC_UID1[4];

            memcpy(keyA, pBuff->data, 6);

            sector = pBuff->data[6];
            block = pBuff->data[7];

            sectorBlock = sector*4 + block;
            //Debug_Log("д��keyA :");
            //Debug_Hex(&keyA[0], 6);
            //Debug_Log("\r\n");

            ///Debug_Log("дshuju�������� :");
            //Debug_Hex(&st.msg[0],16+8);
            //Debug_Log("\r\n");

          //  CL_LOG("д��Ѱ��\r\n");
            TypeA_Halt();
            //Ѱ��
            if (TypeA_CardActivate(PICC_ATQA1,PICC_UID1,PICC_SAK1) != CL_OK)
            {
                printf("WRITE failed.\r\n");
                TypeA_Halt();
                WriteCardBlockSuccessUpLoad(WRITE_ERROR);
                return;
            }
         //   CL_LOG("д��Ѱ��ͨ��\r\n");
            //Debug_Log("д��������֤ \r\n");
            //������֤
            if(Mifare_Auth(0, sector, keyA, PICC_UID1) != OK)
            {
                printf("WRITE failed.\r\n");
                TypeA_Halt();
                WriteCardBlockSuccessUpLoad(WRITE_ERROR);
                return;
            }
         //   CL_LOG("д��������֤ͨ��\r\n");
		//	CL_LOG("д�����ݵ���: %d\r\n", sectorBlock);
		//	PrintfData("д�������� :", ((WRITE_CARDINFO_STR*)pBuff->data)->blockData, 16);
           
            switch(sectorBlock)
            {
                case 8:
                    //printf("д��8У��\r\n");
                    if(((WRITE_CARDINFO_STR*)pBuff->data)->blockData[15] == GetPktSum(&((WRITE_CARDINFO_STR*)pBuff->data)->blockData[0], 15))
                    {
                        //printf("д����8У��ͨ��\r\n");
                        //printf("д����8\r\n");
                        if(OK != Mifare_Blockwrite(8, &((WRITE_CARDINFO_STR*)pBuff->data)->blockData[0]))
                        {
                            printf("д����8ʧ��\r\n");
                            WriteCardBlockSuccessUpLoad(WRITE_ERROR);
                        }
                        else
						{
                            //printf("д����8�ɹ�\r\n");
                            WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
                        }
                    }
                break;
                default:
                    if(OK != Mifare_Blockwrite(sectorBlock, &((WRITE_CARDINFO_STR*)pBuff->data)->blockData[0]))
                    {
                        printf("д����xʧ��\r\n");
                        WriteCardBlockSuccessUpLoad(WRITE_ERROR);
                    }
                    else
					{
                      //  printf("д����x�ɹ�\r\n");
                        WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
                    }
                break;
            }
            TypeA_Halt();
            #endif
        }
		return;
		default:
            CL_LOG("����.\n");
		return;
	}
}

int SwipeCardWriteCard(CB_STR_t *pBuff, uint16_t len)
{
#if 0
	uint8_t keyA[6];
	uint8_t sector ,block, sectorBlock;
	uint8_t PICC_ATQA1[2],PICC_SAK1[3],PICC_UID1[4];

	memcpy(keyA,st.msg,6);

	sector = st.msg[6];
	block = st.msg[7];

	sectorBlock = sector*4 + block;
	//Debug_Log("д��keyA :");
	//Debug_Hex(&keyA[0], 6);
	//Debug_Log("\r\n");

	///Debug_Log("дshuju�������� :");
	//Debug_Hex(&st.msg[0],16+8);
	//Debug_Log("\r\n");

	Debug_Log("д��Ѱ��\r\n");
	TypeA_Halt();
	//Ѱ��
	if (TypeA_CardActivate(PICC_ATQA1,PICC_UID1,PICC_SAK1) != OK)
	{
	    Debug_Log("WRITE failed.\r\n");
	    TypeA_Halt();
	    WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	    return;
	}
	//Debug_Log("д��Ѱ��ͨ��\r\n");
	//Debug_Log("д��������֤ \r\n");
	//������֤
	if(Mifare_Auth(0, sector, keyA, PICC_UID1) != OK)
	{
	    Debug_Log("WRITE failed.\r\n");
	    TypeA_Halt();
	    WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	    return;
	}
	//Debug_Log("д��������֤ͨ��\r\n");

	// Debug_Log("д�����ݵ���");
	//Debug_Hex(&sectorBlock,1);
	//Debug_Log("\r\n");

	//Debug_Log("д�������� :");
	//Debug_Hex(((WRITE_CARD_REQ_STR*)st.msg)->blockData,16);
	//Debug_Log("\r\n");
	switch(sectorBlock)
	{
	    case 8:
	        //Debug_Log("д��8У��\r\n");
	        if(((WRITE_CARD_REQ_STR*)st.msg)->blockData[15] == GetPktSum(&((WRITE_CARD_REQ_STR*)st.msg)->blockData[0], 15))
	        {
	            //Debug_Log("д����8У��ͨ��\r\n");
	            //Debug_Log("д����8\r\n");
	            if(OK != Mifare_Blockwrite(8, &((WRITE_CARD_REQ_STR*)st.msg)->blockData[0]))
	            {
	                Debug_Log("д����8ʧ��\r\n");
	                WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	            }
	            else{
	                //Debug_Log("д����8�ɹ�\r\n");
	                WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
	            }
	        }
	    break;
	    default:
	        if(OK != Mifare_Blockwrite(sectorBlock, &((WRITE_CARD_REQ_STR*)st.msg)->blockData[0]))
	        {
	            Debug_Log("д����xʧ��\r\n");
	            WriteCardBlockSuccessUpLoad(WRITE_ERROR);
	        }
	        else{
	            //Debug_Log("д����x�ɹ�\r\n");
	            WriteCardBlockSuccessUpLoad(WRITE_SUCCESS);
	        }
	    break;
	}
	TypeA_Halt();
#endif
    
    return CL_OK;
}



int SwipeCardWriteCardAck(CB_STR_t *pBuff, uint16_t len)
{
	WRITE_CARDINFO_ACK_STR* WriteCardAck = (WRITE_CARDINFO_ACK_STR*)(pBuff->data);
			
	if(0 == WriteCardAck->result)
	{
		
	}
    
    return CL_OK;
}




int UpgradeRequestHandler(CB_STR_t *pBuff, uint16_t len)
{
	uint32_t i = 0;
	pSTART_UPGRADE_REQUEST_STR pUpgradeRequest = (START_UPGRADE_REQUEST_STR*)(pBuff->data);;
	
	UpgradeRequestArray.filesize = pUpgradeRequest->filesize;
	UpgradeRequestArray.package = pUpgradeRequest->package;
	UpgradeRequestArray.checkSum = pUpgradeRequest->checkSum;
	UpgradeRequestArray.fw_verson = pUpgradeRequest->fw_verson;
	CL_LOG("filesize[%#x], package[%#x], checkSum[%#x], fw_verson[%#x].\n", UpgradeRequestArray.filesize, UpgradeRequestArray.package, UpgradeRequestArray.checkSum, UpgradeRequestArray.fw_verson);
	
	GlobalInfo.UpgradeFw.CurrentPackage = 1;
	GlobalInfo.UpgradeFw.lastIndex = 0;
	GlobalInfo.UpgradeFw.ReceiveSize = 0;
	if((0 == UpgradeRequestArray.filesize) || (0 == UpgradeRequestArray.package)/* || 
		(UpgradeRequestArray.fw_verson == FW_VERSION)*/)
	{
        CL_LOG("filesize or package or fw_verson, error!!!");
		return CL_FAIL;
	}
#if 1
	fmc_unlock();
	for (i = 0; i < (uint32_t)(APP_FW_SIZE / FLASH_PAGE_SIZE); i++)     //����APP������ 
    {
		fmc_page_erase(AppUpBkpAddr + i * FLASH_PAGE_SIZE);//�����������
		fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }
	fmc_lock();
#endif

	return CL_OK;
}


void MainBoardPktProc(CB_STR_t *pBuff, uint16_t len)
{
    //PrintfData("MainBoardPktProc", (void*)pBuff, len);
    FeedWatchDog();
    switch (pBuff->head.module)
	{
		case MsgType_ALL:
			MainBoardBasicInfo(pBuff, len);
		break;
		case MsgType_KEY:
		break;
		case MsgType_CARD:
			MainBoardCardInfo(pBuff, len);
		break;
		case MsgType_BLUE:
			
		break;
	}
}

void ComRecvMainBoardData(void)
{
	uint8_t data;
	static uint8_t gCBRecvBuffer[256] = {0,};
    uint8_t *pktBuff = (void*)&gCBRecvBuffer[0];
	static uint8_t  step = CB_FIND_AA;
	static uint16_t pktLen;
	static uint16_t length;
	static uint8_t  sum;
	uint32_t TimeTicksFlag = GetTimeTicks();

	while(((TimeTicksFlag + 10) >= GetTimeTicks()) && (TimeTicksFlag <= GetTimeTicks()))
	{
		FeedWatchDog();
		while (CL_OK == FIFO_S_Get(&(UartPortHandleInfo[1].rxBuffCtrl), &data)) 
		{
			FeedWatchDog();
			TimeTicksFlag = GetTimeTicks();
			//printf("mm:%x", data);
			switch (step) 
			{
	    		case CB_FIND_AA:
	    		{
	    			if (data == 0xAA) 
					{
	    				pktLen=0;
	    				pktBuff[pktLen] = 0xAA;
	    				pktLen++;
	    				step = CB_FIND_FIVE;
	    			}
	    		}
	    		break;
	    		case CB_FIND_FIVE:
	    		{
	    			if (data == 0x55) 
					{
	    				pktBuff[pktLen] = 0x55;
	    				pktLen++;
	    				step = CB_FIND_LEN;
	    			} 
					else 
					{
	    				step = CB_FIND_AA;
	    			}
	    		}
	    		break;
	    		case CB_FIND_LEN:
	    		{
	    			if (pktLen == 2) 
					{
	    				length = data;
	    			} 
					else if (pktLen == 3) 
					{
	    				length |= (data << 8);
	    				sum = 0;
	                    if (5 > length) 
						{
	                        CL_LOG("length=%d,error.\n", length);
	                        step = CB_FIND_AA;
	                        break;
	                    }
	                    step = CB_FIND_VER;
	    			}
	    			pktBuff[pktLen] = data;
	    			pktLen++;
	    		}
	    		break;
	    		case CB_FIND_VER:
	    		{
	    			if (data == 0x01) 
					{
	    				sum += data;
	    				pktBuff[pktLen] = data;
	    				length--;
	    				pktLen++;
	    				step = CB_FIND_SN;
	    				//CL_LOG("CKB find version = %d.\n", data);
	    			} 
					else 
					{
	    				step = CB_FIND_AA;
	    			}
	    		}
	    		break;
	    		case CB_FIND_SN:
	    		{
	    			sum += data;
	    			pktBuff[pktLen] = data;
	    			length--;
	    			pktLen++;
	    			step = CB_FIND_MODULE;
	    			//CL_LOG("CKB find sn = %d.\n", sn);
	    		}
	    		break;
	    		case CB_FIND_MODULE:
	    		{
	    			sum += data;
	    			pktBuff[pktLen] = data;
	    			length--;
	    			pktLen++;
	    			step = CB_FIND_CMD;
	    			//CL_LOG("CKB find module = %d.\n", module);
	    		}
	    		break;
	    		case CB_FIND_CMD:
	    		{
	    			sum += data;
	    			pktBuff[pktLen] = data;
	    			if (--length) 
					{
	                    step = CB_FIND_DATA;
	                }
					else
					{
	                    step = CB_FIND_CHECKSUM;
	                }
	    			pktLen++;
	    			//CL_LOG("CKB find cmd = %d.\n", cmd);
	    		}
	    		break;
	    		case CB_FIND_DATA:
	    		{
	    			sum += data;
	    			pktBuff[pktLen] = data;
	    			pktLen++;
	    			length--;
	    			if (length==1) 
					{
	    				step = CB_FIND_CHECKSUM;
	    			}
	    		}
	    		break;
	    		case CB_FIND_CHECKSUM:
	    		{
	    			pktBuff[pktLen] = data;
	    			pktLen++;
	    			if (data == sum)
					{
	    				MainBoardPktProc((CB_STR_t*)pktBuff, pktLen);
	    			}
					else 
					{
	    				CL_LOG("CB checksum err.\n");
	    			}
	    			step = CB_FIND_AA;
	    		}
	    		break;
	    		default:
	    		{
	    			step = CB_FIND_AA;
	    		}
	    		break;
	    	}
		}
	}
}





