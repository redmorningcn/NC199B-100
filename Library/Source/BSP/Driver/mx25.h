
/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:            MX25L1602.c
** Descriptions:        SPI�µ�SST25VF016B���������� 
**
**------------------------------------------------------------------------------------------------------
** Created by:            Litiantian
** Created date:        2007-04-16
** Version:                1.0
** Descriptions:        The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:            
** Modified date:    
** Version:
** Descriptions:        
********************************************************************************************************/         

#define MAX_ADDR		(128/8*1024*1024-128)	// ����оƬ�ڲ�����ַ 
#define	SEC_MAX     	4096		// �������������
#define SEC_SIZE		0x1000      // ������С		4kByte

extern void SPI_FLASH_Init(void) ;
extern uint8_t Send_Byte(uint8_t byte);
extern uint8_t Get_Byte(void);
extern void SPI_Flash_WAKEUP(void);
extern uint8_t MX25L3206_RD(uint32_t Dst, uint32_t NByte,uint8_t* RcvBufPt);
extern uint32_t MX25L3206_RdID(void);
extern uint8_t MX25L3206_WR(uint32_t Dst, uint8_t* SndbufPt, uint32_t NByte);
extern uint8_t MX25L3206_Erase(uint32_t sec1, uint32_t sec2);
extern void FASTREAD(uint32_t adr,uint8_t *buf,uint32_t len);
extern uint8_t SaveOneREcord(uint32_t adr,uint8_t *buf,uint32_t len);
extern uint8_t RDSR(void);
extern void WRSR(uint8_t dat);

extern void SaveRecord(void);
extern void DOWNLOAD(uint32_t NRecord);
