/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : spi_flash.h
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : Header for spi_flash.c file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SPIFLASH_H
#define __BSP_SPIFLASH_H

#define _USING_AT25DF161_
/* Private typedef -----------------------------------------------------------*/



#include "stm32f10x.h"
#include "includes.h"


typedef  union __unionflashstorage{
   u8  buf1[512];
   u16 buf2[256];
   u32 buf3[128];
}unionflashstorage;

//============================================================================//
/***********************************************
* ������ ���������Ƿ�ʹ��Ӳ��SPI
     *   BSP_USE_SPI_MODE Ϊ 1  ʹ��Ӳ��SPI
     *   BSP_USE_SPI_MODE Ϊ 0  ʹ��IOģ��SPI
*/
#define  BSP_USE_SPI_MODE  1

//============================================================================//


/***********************************************
* ������ FLASH���Ŷ���
*/
#if (BSP_USE_SPI_MODE == 1)
#define BSP_FLASH_SPIx                          SPI1
#define BSP_FLASH_SPIx_RCC                      RCC_APB2Periph_SPI1
#endif

#define BSP_FLASH_CS_PIN                        GPIO_Pin_4
#define BSP_FLASH_CS_GPIO_PORT                  GPIOA
#define BSP_FLASH_CS_GPIO_RCC                   RCC_APB2Periph_GPIOA

#define BSP_FLASH_SPI_SCK_PIN                   GPIO_Pin_5
#define BSP_FLASH_SPI_SCK_GPIO_PORT             GPIOA
#define BSP_FLASH_SPI_SCK_GPIO_RCC              RCC_APB2Periph_GPIOA

#define BSP_FLASH_SPI_MISO_PIN                  GPIO_Pin_6
#define BSP_FLASH_SPI_MISO_GPIO_PORT            GPIOA
#define BSP_FLASH_SPI_MISO_GPIO_RCC             RCC_APB2Periph_GPIOA

#define BSP_FLASH_SPI_MOSI_PIN                  GPIO_Pin_7
#define BSP_FLASH_SPI_MOSI_GPIO_PORT            GPIOA
#define BSP_FLASH_SPI_MOSI_GPIO_RCC             RCC_APB2Periph_GPIOA

#define BSP_FLASH_WP_PIN                        GPIO_Pin_5
#define BSP_FLASH_WP_GPIO_PORT                  GPIOC
#define BSP_FLASH_WP_GPIO_RCC                   RCC_APB2Periph_GPIOC

#define BSP_FLASH_RST_PIN                       GPIO_Pin_4
#define BSP_FLASH_RST_GPIO_PORT                 GPIOC
#define BSP_FLASH_RST_GPIO_RCC                  RCC_APB2Periph_GPIOC

/* CSƬѡ �͵�ƽ��Ч */
#define BSP_FLASH_CS_LOW()      GPIO_ResetBits(BSP_FLASH_CS_GPIO_PORT, BSP_FLASH_CS_PIN)
#define BSP_FLASH_CS_HIGH()     GPIO_SetBits  (BSP_FLASH_CS_GPIO_PORT, BSP_FLASH_CS_PIN)
/* WPд���� �͵�ƽ��Ч */
#define BSP_FLASH_WP_LOW()      GPIO_ResetBits(BSP_FLASH_WP_GPIO_PORT, BSP_FLASH_WP_PIN)
#define BSP_FLASH_WP_HIGH()     GPIO_SetBits  (BSP_FLASH_WP_GPIO_PORT, BSP_FLASH_WP_PIN)
/* RST��λ �͵�ƽ��Ч */
#define BSP_FLASH_RST_LOW()     GPIO_ResetBits(BSP_FLASH_RST_GPIO_PORT, BSP_FLASH_RST_PIN)
#define BSP_FLASH_RST_HIGH()    GPIO_SetBits  (BSP_FLASH_RST_GPIO_PORT, BSP_FLASH_RST_PIN)

#if (BSP_USE_SPI_MODE == 0)
/* SPI  SCKʱ�� */
#define BSP_FLASH_SCK_LOW()      GPIO_ResetBits(BSP_FLASH_SPI_SCK_GPIO_PORT, BSP_FLASH_SPI_SCK_PIN)
#define BSP_FLASH_SCK_HIGH()     GPIO_SetBits  (BSP_FLASH_SPI_SCK_GPIO_PORT, BSP_FLASH_SPI_SCK_PIN)
/* SPI  MISO */
#define BSP_FLASH_MISO_READ()    GPIO_ReadInputDataBit(BSP_FLASH_SPI_MISO_GPIO_PORT, BSP_FLASH_SPI_MISO_PIN)
/* SPI  MOSI */
#define BSP_FLASH_MOSI_LOW()     GPIO_ResetBits(BSP_FLASH_SPI_MOSI_GPIO_PORT, BSP_FLASH_SPI_MOSI_PIN)
#define BSP_FLASH_MOSI_HIGH()    GPIO_SetBits  (BSP_FLASH_SPI_MOSI_GPIO_PORT, BSP_FLASH_SPI_MOSI_PIN)
#endif





#ifdef _USING_AT25DF161_

typedef enum
{
  SECTOR_UNPROTECT = 0,
  SECTOR_PROTECT = 1
}FlashPretectStatue;
/***********************************************
* ������ FLASH���в���
*/
#define  AT45DB161E_FLASH_ID            0x1F260001  //Flash��оƬID
#define  FLASH_ID                       AT45DB161E_FLASH_ID
#define  BSP_FLASH_PageSize             512         //ҳ��СĬ��528���ֽڣ����޸�Ϊ512��ÿ��оƬֻ���޸�һ��
#define  BSP_FLASH_PageNum              4096

/***********************************************
* ������ FLASHָ���
*/
#define Dummy_Byte                      0xA5     //�������ֽ�,����SPI��ȡ����

#define FLASH_IDREAD                    0x9F     //��ȡоƬIDָ��
#define FLASH_STATUSREAD                0xD7     //��ȡоƬ״̬�Ĵ���ָ��  Ҳ����ʹ��ָ����0x57

#define FLASH_CHREAD                    0x0B     //������ ����ģʽ ���Դﵽ66MHZ
#define FLASH_CLREAD                    0x03     //������ ����ģʽ ���Դﵽ33MHZ
#define FLASH_PAGEREAD	                0xD2     //���洢��ҳ�������洢��ҳ������ֱ�Ӵ� 4096 ��ҳ��ĳһ��ҳ�ж�ȡ���ݣ� ����Ӱ�컺�����е�����
#define FLASH_BUF1READ                  0x54     //��ȡ������1������
#define FLASH_BUF2READ                  0x56     //��ȡ������2������

#define FLASH_PAGEERASE                 0x81     //ҳ����ָ��
#define FLASH_BLOCKERASE                0x50     //�����ָ��
#define FLASH_SECTORERASE               0x7C     //��������ָ��

#define FLASH_BUFWRITE1                 0x84     //������1д ��������1д����
#define FLASH_BUFWRITE2                 0x87     //������2д ��������2д����
#define B1_TO_MM_PAGE_PROG_WITH_ERASE   0x83	 // ����һ������������д�����洢��������ģʽ��
#define B2_TO_MM_PAGE_PROG_WITH_ERASE   0x86	 // ���ڶ�������������д�����洢��������ģʽ��


#define MM_PAGE_TO_B1_XFER              0x53	 // �����洢����ָ��ҳ���ݼ��ص���һ������
#define MM_PAGE_TO_B2_XFER              0x55	 // �����洢����ָ��ҳ���ݼ��ص��ڶ�������


//#define FLASH_ID  0x1F2601
//
//#define AT25DF161_FLASH_ID    FLASH_ID
//
//#define SPI_FLASH_PageSize    512
//
//#define WREN        0x06  /* Write enable instruction */
//#define READ        0x03  /* Read from Memory instruction */
//#define WRITE       0x84  /* Write to Memory instruction */
//#define RDID        0x9F  /* Read identification */
//#define SE          0x7C  /* Sector Erase instruction */
//#define PE          0x81  /* Page Erase instruction */
//#define RDSR        0xD7  /* Read Statue instruction */
//#define WIP_Flag    0x80  /* Write In Progress (WIP) flag */
//#define BE          0x60  /* Bulk Erase instruction */
//#define BLE         0x20  /* Block(size:4K/block) Erase instruction */
//#define RA          0x1b  /* Read arrary */
//
//#define Dummy_Byte 0xA5
#endif

#ifdef _USING_M25P64_

#define SPI_FLASH_PageSize    0x100
/* Private define ------------------------------------------------------------*/
#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0xD8  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xA5

#endif

/***********************************************
* ������ ��������
*/
/*----- Low layer function -----*/
#if (BSP_USE_SPI_MODE == 1)
uint8_t BSP_FLASH_ReadByte(void);
uint8_t BSP_FLASH_SendByte(uint8_t byte);
uint16_t BSP_FLASH_SendHalfWord(uint16_t HalfWord);
#else
u8   BSP_FLASH_ReadByte(void);
void BSP_FLASH_SendByte(uint8_t byte);
#endif

/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_Ram1Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32  SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);


FlashPretectStatue SPI_FLASH_GetProtectStatue(u8 sectorNo);
void SPI_FLASH_BlockErase(u32 BlockAddr);
void SPI_FLASH_ArraryRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);//wisure
void SPI_FLASH_ProtectSet(u8 setValue);//wisure added

/*----- Low layer function -----*/
u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
u8 SPI_FLASH_GetStatueByte(void);
void SPI_FLASH_SetAt45DB161_512BytesOnePage(void);
void SPI_FLASH_EnableSecProtect(void);
void SPI_FLASH_DisableSecProtect(void);
void SPI_FLASH_Ram1Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_PageErase(u32 Addr);





extern unionflashstorage FlashStorageBuf;




#endif /* __SPI_FLASH_H */
/*
|------------------------------------------------------------------------------------------------------------------------------|
|    ����	|                �����빦��	                                                         |  ��Ч��ƽ   |	  ����     |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|#CS	    |    Ƭѡ��#CS����ѡ��оƬ����#CS������Ϊ��Ч״̬ʱ��оƬ�򲻱�ѡ�У�                |             |               |
|           |    ���Ҵ�������״̬���������˯��״̬�����������SO���ڸ���̬��                    |             |               |
|           |    ��оƬδ��ѡ��ʱ������������SI��������ݽ��������ܡ�                            |    LOW      |   	����       |
|           |    #CS�����ϵ��½��ؽ�������һ����������������������һ��������                   |             |               |
|           |    ��һ���ڲ�������оƬ�ڲ��ı�̻���������ڣ�оƬ�����������״̬��ֱ��������ϡ�|	           |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|SCK	    |    ����ʱ�ӣ�������������оƬ�ṩʱ���źţ����������������ĳ��롣                  |             |               |
|           |    SI�����ϵ������ַ������������ʱ��SCK�������ر�д�룬                         |      -      |   ����        |
|           |    ��SO�����ϵ������������ʱ�ӵ��½��ر仯��		                                 |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|SI	        |    �������룺SI����������оƬ����λ��ʽд�����ݡ�                                  |             |               |
|           |    SI�����ϵ�����������������������ַ��                                          |      -      |   ����        |
|           |    SI�ϵ�������ʱ�ӵ�������д��оƬ��                                              |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|SO	        |    ���������SO����������оƬ����λ��ʽ������ݡ�                                  |      -      |   ���        |
|           |    SI�ϵ�������ʱ�ӵ��½��ر仯��	                                                 |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|#WP	    |    д��������#WP������Ϊ��ЧʱоƬ��������������������                             |             |               |
|           |    �Է�ֹ�������������ݵ��ƻ���                                                  |     LOW     |   	����       |
|           |    ������������ʹ������������������Ȼ���Ա�оƬʶ��                              |             |               |
|           |    #WP�������ڲ������ߣ��������ա�������Ȼ�������ⲿ�ӵ�VCC��	                     |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|#RESET	    |    ��λ��#RESET�����ϵĵ͵�ƽ����ֹ���ڴ���Ĳ�������λ�ڲ�״̬��������״̬��      |             |               |
|           |    #RESET�����ϵĵ͵�ƽ��ʹоƬһֱ���ڸ�λ״̬��                                  |     LOW     |  	����       |
|           |    ��#RESET��תΪ�ߵ�ƽ�󣬲��ܽ��������Ĳ�����                                    |             |               |
|           |    оƬ�ڲ������ϵ縴λ��·���������Ų���ʱ���ⲿ�ӵ��ߵ�ƽ��                      |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|RDY/#BUSY	|    ����/æµ״ָ̬ʾ����������©����·��������š�                                 |             |               |
|           |    ��оƬ����æ״̬ʱ���ڲ����������У�������Ϊ�͵�ƽ��                            |             |               |
|           |    ������������״̬��Ϊ�ߵ�ƽ���ⲿ���������裩��                                  |      -      |   ���        |
|           |    �����ڽ��б��/�����������Ƚϲ�����ҳ-����������ʱ�������͡�                    |             |               |
|           |    æ״ָ̬ʾFLASH����������ĳһ�����������ܱ�������                               |             |               |
|           |    ������һ���������Ķ���д������Ȼ���Խ��С�	                                     |             |               |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|VCC	    |    оƬ��Դ����	                                                                 |      -      |   ��Դ        |
|------------------------------------------------------------------------------------------------|-------------|---------------|
|GND	    |    �أ�������Ӧ��ϵͳ�ؽ���һ��                                                  |      -      |   ��          |
|------------------------------------------------------------------------------------------------|-------------|---------------|


*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
