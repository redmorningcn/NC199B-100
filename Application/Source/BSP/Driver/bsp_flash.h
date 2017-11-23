/*******************************************************************************
 *   Filename:       bsp_flash.h
 *   Revised:        $Date: 2015-08-23
 *   Revision:       $V1.0
 *	 Writer:		 Roger_WY.
 *
 *   Description:    STM32�ڲ�FLASH����ģ��ͷ�ļ�
 *
 *   Notes:
 *
 *
 *   All copyrights reserved to Roger_WY.
 *
 *******************************************************************************/
#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stm32f10x.h"
//���ڹ����ж���
#if !defined( STM32_FLASH_SIZE )
#if defined( STM32F103RB )
#define  STM32_FLASH_SIZE       128         // flash���� �����Լ�ѡ��оƬ������
#elif defined( STM32F103RC )
#define  STM32_FLASH_SIZE       256         // flash���� �����Լ�ѡ��оƬ������
#endif
#endif
     
#define  STM32_FLASH_BASE       0X08000000  // flash����ַ

#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE         1024        // �ֽ�
#else
#define STM_SECTOR_SIZE	        2048        // �ֽ�
#endif

#define STM_FLASH_PAGES         (STM32_FLASH_SIZE * 1024 / STM_SECTOR_SIZE )
#define EEPROM_START_PAGE       (STM_FLASH_PAGES-1)                             //������ҳ��
#define PARAM_START_ADDR        (STM32_FLASH_BASE + STM_SECTOR_SIZE * EEPROM_START_PAGE) //flashҳ�׵�ַ

/***********************************************
* �������ⲿ��������
*/
void    BSP_FlashRead           (u32 addr,u16 *pbuffer,u16 num);
void    BSP_FlashWrite          (u32 addr,u16 *pbuffer,u16 num);

u16     BSP_FlashReadBytes      (u32 addr, u8 *buf, u16 len);
u16     BSP_FlashWriteBytes     (u32 addr, u8 *buf, u16 len);


#endif