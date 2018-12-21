#include "flash.h"
#include "includes.h"
#include "system.h"




/*****************************************************************************
** Function name:   	FlashReadByte
** Descriptions:        ��flash��ȡ����
** input parameters:    address : Flash��ַ
           				pBuffer : ���ݻ�����
            			readNum : ���ݴ�С
** Returned value:	    0-�ɹ���-1����
** Author:              quqian
*****************************************************************************/
int FlashReadByte(uint32_t address, uint8_t *pBuffer, uint16_t readNum)   	
{
	uint32_t i;
    uint32_t readAddr = address;
	
//    CL_LOG("address = [0x%x], readNum = [0x%x]\n", address, readNum);
	for(i = 0; i < readNum; i++)
	{
		pBuffer[i] = REG8(readAddr);
        readAddr += 1;
//		CL_LOG("readAddr = [0x%x], pBuffer[i] = [0x%x]\n", readAddr, pBuffer[i]);
	}

	return 0;
}


/*****************************************************************************
** Function name:   	FlashWriteWord
** Descriptions:        д���ݵ�оƬ�ڲ�Flash, ʹ�øú�����Ҫ�Ȳ�����Ӧ��flash
** input parameters:    address : Flash��ַ
           				pBuffer : ���ݻ�����
            			writeNum : ���ݴ�С(4�ı���)
** Returned value:	    0-�ɹ���-1����
** Author:              quqian
*****************************************************************************/
int FlashWriteWord(uint32_t address, uint8_t *pBuffer, uint16_t writeNum)
{
    uint32_t i;
	uint32_t OutTimeFlag = 0;
    uint32_t readAddr = address;
	
	fmc_unlock();
	for(i = 0; i < writeNum; i++)
    {
    	OutTimeFlag = 0;
    	fmc_flag_clear(FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_END);
        if (FMC_READY == fmc_word_program(readAddr, *((uint32_t *)(pBuffer + i * 4)))) 
		{
            readAddr += 4;
        }
		else
		{
            while(1)
			{
				FLASH_DELAY_MS(1);
				if(1000 < OutTimeFlag++)
				{
					CL_LOG("дflash��ʱ, [0x%x], ����!!! \n", readAddr);
					break;
				}
			}
        }
	}
	fmc_lock();
    
	return CL_OK;
}


/**********************************************************************************************************
�� �� ��:   FlashWrite
����˵��:   д���ݵ�CPU �ڲ�Flash�����б������е���������
��    ��:   address : Flash��ַ����ַ�����4�ı���
            writeBuffer : ���ݻ�����
            writeLen : ���ݴ�С����λ���ֽڣ�
�� �� ֵ:   0-�ɹ���-1����
**********************************************************************************************************/
uint8_t FlashBuffer[FLASH_PAGE_SIZE] = {0};
int FlashWrite(uint32_t address, uint8_t *writeBuffer, uint16_t Len)
#if 1
{
    uint32_t secpos;	   		//������ַ
	uint16_t secoff;	   		//������ƫ�Ƶ�ַ(16λ�ּ���)
	uint16_t secremain; 		//������ʣ���ַ(16λ�ּ���)	 
 	uint32_t i;    
	uint32_t offaddr;   		//ȥ��0X08000000��ĵ�ַ
	uint16_t writeLen = Len;
	
	FeedWatchDog();
    if (IS_PROTECTED_AREA(address)) 
	{	//�Ƿ���ַ
		CL_LOG("д��flash��ʼ��ַ[0x%x], ����!!! \n", address);
        return CL_FAIL;
    }
//    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//ϵͳ�Ѿ�����
//	{
//		xSemaphoreTake(FlashMutex,1000);
//	}
    fmc_unlock();
    
    offaddr = address - FLASH_BASE;		        	//ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr / FLASH_PAGE_SIZE;				//������ַ  
	secoff = (offaddr % FLASH_PAGE_SIZE);			//�������ڵ�ƫ��(4���ֽ�Ϊ������λ.)
	secremain = FLASH_PAGE_SIZE - secoff;			//����ʣ��ռ��С   
	if(writeLen <= secremain)                   	//�����ڸ�������Χ
	{
		secremain = writeLen;
	}

    while(1) 
	{
        FeedWatchDog();
		FlashReadByte(FLASH_BASE + secpos * FLASH_PAGE_SIZE, FlashBuffer, FLASH_PAGE_SIZE);		//������������������
		for(i = 0; i < secremain; i++)            	//У������
		{
			if(FlashBuffer[secoff + i] != 0XFF)   	//��Ҫ����  	  
			{
				CL_LOG("��Ҫ����.\n");
				break;
			}
		}
		if(i < secremain)                       	//��Ҫ����
		{
			fmc_page_erase(FLASH_BASE + secpos * FLASH_PAGE_SIZE);//�����������
			fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
			for(i = 0; i < secremain; i++)          //����
			{
				FlashBuffer[i + secoff] = writeBuffer[i];
			}

			FlashWriteWord(FLASH_BASE + (secpos * FLASH_PAGE_SIZE), FlashBuffer, (FLASH_PAGE_SIZE / 4));//д����������  
		}
        else 
		{
			FlashWriteWord(address, writeBuffer, ((0 == (secremain % 4)) ? (secremain / 4) : ((secremain / 4) + 1)));	//д�Ѿ������˵�, ֱ��д������ʣ������. 	
		}
        
		if(writeLen == secremain)               //д�������
		{
			CL_LOG("д��ɹ�.\n");
			break;
		}
		else                                    //д��δ����
		{
			secpos++;				            //������ַ��1
			secoff = 0;				            //ƫ��λ��Ϊ0 	 
		   	writeBuffer += secremain;  	        //ָ��ƫ�� 
			address += secremain;	            //д��ַƫ��	   
		   	writeLen -= secremain;	            //�ֽ�(16λ)���ݼ�
			if(writeLen > FLASH_PAGE_SIZE)
			{
				secremain = FLASH_PAGE_SIZE;		//��һ����������д����
			}
			else                                //��һ����������д����
			{
				secremain = writeLen;
			}
		}	 
	}

    fmc_lock();
//    if((xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) && (FlashMutex != NULL))//ϵͳ�Ѿ�����
//	{
//        xSemaphoreGive(FlashMutex);	
//	}

    return CL_OK;
}
#endif


