#ifndef __MAIN_H__
#define __MAIN_H__


/*****************************************************************************
** @Author: quqian
** @Date: 2018-12-21 14:20:14 
** @File: main.c
** @MCU: GD32F350CBT6   
** @MCU max Speed: 108M
** @MCU Flash: 128K
** @MCU RAM: 16K
** @MCU Package: LQFP48
** @Last Modified by: quqian
** @Last Modified time: 2018-12-21 14:20:14 
*****************************************************************************/



#define FLASH_SIZE                      128     //设备flash大小128K
#define FLASH_PAGE_SIZE					1024

#define BOOT_SIZE	                	((uint32_t)0x2000)
#define AppFlashAddr	                (FLASH_BASE + BOOT_SIZE)		//App运行入口地址	//应用区	 58K
#define AppUpBkpAddr					(AppFlashAddr + ((uint32_t)58) * 1024)		//备份区	 58K

#define SystemInfoAddr 					(AppUpBkpAddr + ((uint32_t)58) * 1024)		//(FLASH_BASE + ((uint32_t)124) * 1024)
#define PCB_INFO						(FLASH_BASE + ((uint32_t)126) * 1024)				//PCB信息
#define UPGRADE_INFO					(FLASH_BASE + ((uint32_t)127) * 1024)				//升级信息
#define APP_FW_SIZE                     (AppUpBkpAddr - AppFlashAddr)




#define CL_OK                           0
#define CL_FAIL                         (-1)
#define CL_TRUE                         1
#define CL_FALSE                        0


#endif


