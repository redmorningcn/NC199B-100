#include "stm32f10x.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "SZ_STM32F107VC_LIB.h"
#include "DS3231.h"
#include "I2C_CLK.h"
#include "Display.h"
#include "DATA_STORAGE.h"
#include "DELAY.h"
#include "MX25.h"
#include "FM24CL64.h"
#include "POWER_MACRO.h"
#include "crccheck.h"
#include "WatchDog.h"


#include  <global.h>
#include  <includes.h>

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
#define SPI_FLASH_SPI                   SPI2
#define SPI_FLASH_SPI_CLK               RCC_APB1Periph_SPI2

#define SPI_FLASH_SPI_SCK_PIN           GPIO_Pin_13              /* PB.13 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT     GPIOB
#define SPI_FLASH_SPI_SCK_GPIO_CLK      RCC_APB2Periph_GPIOB

#define SPI_FLASH_SPI_MISO_PIN          GPIO_Pin_14              /* PB.14 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT    GPIOB
#define SPI_FLASH_SPI_MISO_GPIO_CLK     RCC_APB2Periph_GPIOB

#define SPI_FLASH_SPI_MOSI_PIN          GPIO_Pin_15              /* PB.15 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT    GPIOB
#define SPI_FLASH_SPI_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOB

#define SPI_FLASH_CS_PIN_NUM            12                       /* PB.12 */
#define SPI_FLASH_CS_PIN                GPIO_Pin_12  
#define SPI_FLASH_CS_GPIO_PORT          GPIOB
#define SPI_FLASH_CS_GPIO_CLK           RCC_APB2Periph_GPIOB

/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256


/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable                0x06 
#define W25X_WriteDisable               0x04 
#define W25X_ReadStatusReg              0x05 
#define W25X_WriteStatusReg             0x01 
#define W25X_ReadData                   0x03 
#define W25X_FastReadData               0x0B 
#define W25X_FastReadDual               0x3B 
#define W25X_PageProgram                0x02 
#define W25X_BlockErase                 0xD8 
#define W25X_SectorErase                0x20 
#define W25X_ChipErase                  0xC7 
#define W25X_PowerDown                  0xB9 
#define W25X_ReleasePowerDown           0xAB 
#define W25X_DeviceID                   0xAB 
#define W25X_ManufactDeviceID           0x90 
#define W25X_JedecDeviceID              0x9F 

#define WIP_FlagMask                    0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte1                     0xA5


/* Private macro -------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define CE_Low()       (Periph_BB((uint32_t)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 0)

/* Deselect SPI FLASH: Chip Select pin high */
#define CE_High()      (Periph_BB((uint32_t)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 1)

#define  UCOS_EN            DEF_ENABLED

/***********************************************
* ������ OS�ӿ�
*/
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
    static  OS_SEM                   Bsp_EepSem;    // �ź���
    #else
    static  OS_EVENT                *Bsp_EepSem;    // �ź���
    #endif
#endif
  
    
/*******************************************************************************
* ��    �ƣ� EEP_WaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ�����  0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static uint8_t EEP_WaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    return BSP_OS_SemWait(&Bsp_EepSem,0);           // �ȴ��ź���
#else
    uint8_t       err;
    OSSemPend(Bsp_EepSem,0,&err);                   // �ȴ��ź���
    if ( err = OS_ERR_NONE )
      return TRUE;
    else
      return FALSE;
#endif
}


/*******************************************************************************
* ��    �ƣ� EEP_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static void EEP_SendEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&Bsp_EepSem);                        // �����ź���
#else
    uint8_t       err;
    OSSemPost(Bsp_EepSem);                              // �����ź���
#endif
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_Init
  * @����   ��ʼ�����ⲿSPI FLASH�ӿڵ���������
  *         Initializes the peripherals used by the SPI FLASH driver.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable SPI2 and GPIO clocks */
    /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(SPI_FLASH_CS_GPIO_CLK | SPI_FLASH_SPI_MOSI_GPIO_CLK |
                         SPI_FLASH_SPI_MISO_GPIO_CLK | SPI_FLASH_SPI_SCK_GPIO_CLK, ENABLE);

    /*!< SPI_FLASH_SPI Periph clock enable */
    RCC_APB1PeriphClockCmd(SPI_FLASH_SPI_CLK, ENABLE);
    
    /*!< AFIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /*!< Configure SPI_FLASH_SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			   //��������Ϊ�������������
    GPIO_Init(SPI_FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_MOSI_PIN;
    GPIO_Init(SPI_FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                //GPIO_Mode_IN_FLOATING;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				   //����Ϊ����PP��OD����
    GPIO_Init(SPI_FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStructure);

    /* Deselect the FLASH: Chip Select high */
    CE_High();

    /* SPI2 configuration */
    // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
    // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI2  */
    SPI_Cmd(SPI2, ENABLE);

    SPI_Flash_WAKEUP();
    
    /***********************************************
    * ������ OS�ӿ�
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_EepSem,1, "Bsp EepSem");      // �����ź���
#else
    Bsp_EepSem     = OSSemCreate(1);                    // �����ź���
#endif
#endif
}

/**-----------------------------------------------------------------
  * @������ Send_Byte
  * @����   ͨ��SPI���߷���һ���ֽ�����(˳�����һ���ֽ�����)
  *         Sends a byte through the SPI interface and return the byte
  *         received from the SPI bus.
  * @����   Ҫд���һ���ֽ�����
  * @����ֵ �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ���ֽ�
***----------------------------------------------------------------*/
uint8_t Send_Byte(uint8_t byte)
{
    /* Loop while DR register in not empty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI2 peripheral */
    SPI_I2S_SendData(SPI2, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}

/**-----------------------------------------------------------------
  * @������ Get_Byte
  * @����   ��ȡSPI FLASH��һ���ֽڣ�δ�������Ͷ��������ʼ��ַ
  * @����   ��
  * @����ֵ ��SPI_FLASH��ȡ��һ���ֽ�
***----------------------------------------------------------------*/
uint8_t Get_Byte(void)
{
    return (Send_Byte(Dummy_Byte1));
}

/**-----------------------------------------------------------------
  * @������ SPI_Flash_WAKEUP
  * @����   ����SPI FLASH
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  CE_Low();

  /* Send "Power Down" instruction */
  Send_Byte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  CE_High();
}   
 
/* ���º�������ֲʱ�����޸� */
/************************************************************************
** ��������:MX25L1602_RD                                                                                                        
** ��������:MX25L1602�Ķ�����,��ѡ���ID�Ͷ����ݲ���                
** ��ڲ���:
**            uint32_t Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��                
**          uint32_t NByte:    Ҫ��ȡ�������ֽ���
**            uint8_t* RcvBufPt:���ջ����ָ��            
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
uint8_t MX25L3206_RD(uint32_t Dst, uint32_t NByte,uint8_t* RcvBufPt)
{
    uint32_t i = 0;
	uint32_t adr;
    
    EEP_WaitEvent();
    
	adr=Dst&MAX_ADDR ;
    CE_Low();            
    Send_Byte(0x0B);                                // ���Ͷ�����
    Send_Byte(((adr & 0xFFFFFF) >> 16));            // ���͵�ַ��Ϣ:�õ�ַ��3���ֽ����
    Send_Byte(((adr & 0xFFFF) >> 8));
    Send_Byte(adr & 0xFF);
    Send_Byte(0xFF);                                // ����һ�����ֽ��Զ�ȡ����
    for (i = 0; i < NByte; i++)        
    {
        RcvBufPt[i] = Get_Byte();        
    }
    CE_High();   
    EEP_SendEvent();
    return (1);
}

/************************************************************************
** ��������:MX25L1602_RdID                                                                                                        
** ��������:MX25L1602�Ķ�ID����,��ѡ���ID�Ͷ����ݲ���                
** ��ڲ���:
**            idtype IDType:ID���͡��û�����Jedec_ID,Dev_ID,Manu_ID������ѡ��
**            uint32_t* RcvbufPt:�洢ID������ָ��
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:������Ĳ���������Ҫ���򷵻�ERROR
************************************************************************/
uint32_t MX25L3206_RdID(void)
{
    uint32_t temp = 0;
    CE_Low();            
    Send_Byte(0x9F);                            // ���Ͷ�JEDEC ID����(9Fh)
    temp = (temp | Get_Byte()) << 8;            // �������� 
    temp = (temp | Get_Byte()) << 8;    
    temp = (temp | Get_Byte());                 // �ڱ�����,temp��ֵӦΪ0xBF2541
    CE_High();            
	return temp;
}

/************************************************************************
** ��������:MX25L1602_WR                                            
** ��������:MX25L1602��д��������д1���Ͷ�����ݵ�ָ����ַ                                    
** ��ڲ���:
**            uint32_t Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��                
**            uint8_t* SndbufPt:���ͻ�����ָ��
**          uint32_t NByte:Ҫд�������ֽ���
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
** ע     ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
uint8_t MX25L3206_WR(uint32_t Dst, uint8_t* SndbufPt, uint32_t NByte)
{
    uint8_t StatRgVal = 0;
    uint32_t i = 0;
	uint32_t adr;
    
    EEP_WaitEvent();
    
	adr=Dst&MAX_ADDR ;
    
    CE_Low();            
    Send_Byte(0x06);                                                    /* ����дʹ������               */
    CE_High();        
    //д���� 
    CE_Low();            
    Send_Byte(0x02);                                                    /* �����ֽ�������д����         */
    Send_Byte((((adr+i) & 0xFFFFFF) >> 16));                            /* ����3���ֽڵĵ�ַ��Ϣ        */
    Send_Byte((((adr+i) & 0xFFFF) >> 8));
    Send_Byte((adr+i) & 0xFF);
    for(i = 0; i < NByte; i++)
    {
        Send_Byte(SndbufPt[i]);                                     /* ���ͱ���д������             */
    }
    CE_High();    
    
    do
    {
        CE_Low();             
        Send_Byte(0x05);                                                /* ���Ͷ�״̬�Ĵ�������         */
        StatRgVal = Get_Byte();                                         /* ������õ�״̬�Ĵ���ֵ       */
        CE_High();                                
    }
    while (StatRgVal == 0x03);                                          /* һֱ�ȴ���ֱ��оƬ����       */
    EEP_SendEvent();
    return (1);        
}

/************************************************************************
** ��������:MX25L1602_Erase                                                
** ��������:����ָ����������ѡȡ���Ч���㷨����                                
** ��ڲ���:
**            uint32_t sec1����ʼ������,��Χ(0~499)
**            uint32_t sec2����ֹ������,��Χ(0~499)
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR        
************************************************************************/

uint8_t MX25L3206_Erase(uint32_t sec1, uint32_t sec2)
{
    uint8_t  temp2 = 0,StatRgVal = 0;
    uint32_t SecnHdAddr = 0;                  
    uint32_t no_SecsToEr = 0;                       // Ҫ������������Ŀ
    uint32_t CurSecToEr = 0;                        // ��ǰҪ������������
    
    /*  �����ڲ��� */
    if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX))    
    {
        return (0);    
    }       
    
    CE_Low();            
    Send_Byte(0x06);                                // ����дʹ������
    CE_High();            

    /* ����û��������ʼ�����Ŵ�����ֹ�����ţ������ڲ��������� */
    if (sec1 > sec2)
    {
       temp2 = sec1;
       sec1  = sec2;
       sec2  = temp2;
    } 
    /* ����ֹ���������������������� */
    if (sec1 == sec2)    
    {
        SecnHdAddr = SEC_SIZE * sec1;               // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x20);                            // ������������ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
           Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
           Send_Byte(SecnHdAddr & 0xFF);
          CE_High();            
        do
        {
              CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        return (1);            
    }
    
                     /*         ������ʼ��������ֹ��������������ٵĲ�������                             */    
    
    if (sec2 - sec1 == SEC_MAX)    
    {
        CE_Low();            
        Send_Byte(0x60);                            // ����оƬ����ָ��(60h or C7h)
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High(); 
			Delay(10);								//�˴���ʱ�Ƿ��Ǹ���
        }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        return (1);
    }
    
    no_SecsToEr = sec2 - sec1 +1;                   // ��ȡҪ������������Ŀ
    CurSecToEr  = sec1;                             // ����ʼ������ʼ����
    
    /* ����������֮��ļ���������ȡ16���������㷨 */
    while (no_SecsToEr >= 16)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0xD8);                            // ����64KB�����ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        CurSecToEr  += 16;                          // ���������16��������,�Ͳ����������ڵĴ�����������
        no_SecsToEr -=  16;                         // �������������������������
    }
    /* ����������֮��ļ���������ȡ8���������㷨 */
    while (no_SecsToEr >= 8)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x52);                            // ����32KB����ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();            
        do
        {
            CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����    
        CurSecToEr  += 8;
        no_SecsToEr -=  8;
    }
    /* �������������㷨����ʣ������� */
    while (no_SecsToEr >= 1)
    {
        SecnHdAddr = SEC_SIZE * CurSecToEr;         // ������������ʼ��ַ
        CE_Low();    
        Send_Byte(0x20);                            // ������������ָ��
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
           Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
           Send_Byte(SecnHdAddr & 0xFF);
          CE_High();            
        do
        {
              CE_Low();             
            Send_Byte(0x05);                        // ���Ͷ�״̬�Ĵ�������
            StatRgVal = Get_Byte();                 // ������õ�״̬�Ĵ���ֵ
            CE_High();                                
          }
        while (StatRgVal == 0x03);                  // һֱ�ȴ���ֱ��оƬ����
        CurSecToEr  += 1;
        no_SecsToEr -=  1;
    }
    return (1);
}


uint8_t SaveOneREcord(uint32_t adr,uint8_t *buf,uint32_t len)
{
	if((adr&0x0FFF)==0)							//�ڿ�߽磬��Ҫ������
	{
        EEP_WaitEvent();
		MX25L3206_Erase((adr>>12)&0x3FF,(adr>>12)&0x3FF);
        EEP_SendEvent();
	}
	return MX25L3206_WR(adr, buf, len);
}
void FASTREAD(uint32_t adr,uint8_t *buf,uint32_t len)
{
	MX25L3206_RD(adr, len,buf);
	
}
uint8_t RDSR(void)
{
	uint8_t dat;
    CE_Low();             
    Send_Byte(0x05);                                // ���Ͷ�״̬�Ĵ�������
    dat = Get_Byte();                             // ������õ�״̬�Ĵ���ֵ
    CE_High();                                
	return dat;
}
void WRSR(uint8_t dat)
{
    CE_Low();             
    Send_Byte(0x06);                                // ���Ͷ�״̬�Ĵ�������
    CE_High();                                
    CE_Low();             
    Send_Byte(0x01);                                // ���Ͷ�״̬�Ĵ�������
    Send_Byte(dat);                                // ���Ͷ�״̬�Ĵ�������
    CE_High();                                
}

void SaveRecord(void)
{
	uint8_t   AddressBuf[4] = {0};
	uint32_t  Address = 0;
	
	GetTime((TIME *)&recordsfr.Time[0]);
    
	recordsfr.Crc16 = GetCrc16Chk1((uint8_t *)&recordsfr.Time[0],sizeof(SDAT)-2);
   	SaveOneREcord(g_Flash_Adr,(uint8_t *)&recordsfr.Time[0],sizeof(SDAT));
    // 	printf("g_Flash_Adr = %d",g_Flash_Adr);
	g_Flash_Adr += sizeof(SDAT);
    //	printf("g_Flash_Adr = %d",g_Flash_Adr);
	Address = (MAX_ADDR & g_Flash_Adr)|0xAA000000;	 //FLASH��С��128Mbit
	TO_Arayy(AddressBuf , Address);
	WriteFM24CL64(60, AddressBuf, 4);
}


/*  ����ת��                    */
/*  ���ݵ��ֽ���ǰ�����ֽ��ں�  */
void DOWNLOAD(uint32_t NRecord)
{	
	uint32_t i;
	static uint8_t  DATA_OUT[32] = {0};
    
	uint32_t PP_Power = 0;
	uint32_t NP_Power = 0;
	uint32_t PQ_Power = 0;
	uint32_t NQ_Power = 0;
	uint16_t CRC_sum1 = 0;
	uint16_t CRC_sum2 = 0;
    
	for(i=0;i<NRecord;i++) {
		FeedDog_HIGH();
		MX25L3206_RD(i*32, 32 ,DATA_OUT);				  //ÿ�ζ�ȡ32�ֽڣ���һ����¼
		FeedDog_LOW();
		
		CRC_sum1 = GetCrc16Chk1((uint8_t *)&DATA_OUT[0],sizeof(SDAT)-2);		
		CRC_sum2 = DATA_OUT[30] + (DATA_OUT[31] << 8);
		
        if(CRC_sum1 == CRC_sum2) {						   //�����ۼӺͼ���
			printf("\r\n��%05d����¼��20%02d-%02d-%02d %02d:%02d:%02d  ", (i+1),
                   DATA_OUT[0],DATA_OUT[1],DATA_OUT[2],
                   DATA_OUT[3],DATA_OUT[4],DATA_OUT[5]);
			FeedDog_HIGH();
			PP_Power =  DATA_OUT[11]<<24 |  DATA_OUT[10]<<16 | DATA_OUT[9]<<8  | DATA_OUT[8];
			FeedDog_LOW();
			printf("���й�������%10dkW.h  ", PP_Power);
			FeedDog_HIGH();
			NP_Power =  DATA_OUT[15]<<24 |  DATA_OUT[14]<<16 | DATA_OUT[13]<<8 | DATA_OUT[12];
			FeedDog_LOW();
			printf("���й�������%10dkW.h  ", NP_Power);
			PQ_Power =  DATA_OUT[19]<<24 |  DATA_OUT[18]<<16 | DATA_OUT[17]<<8 | DATA_OUT[16];
			FeedDog_HIGH();
			printf("���޹�������%10dkvar.h  ", PQ_Power);
			FeedDog_LOW();
			NQ_Power =  DATA_OUT[23]<<24 |  DATA_OUT[22]<<16 | DATA_OUT[21]<<8 | DATA_OUT[20];
			FeedDog_HIGH();
			printf("���޹�������%10dkvar.h  ", NQ_Power);
			FeedDog_LOW();
		}
		else 
			printf("�ۼӺʹ���");
	}
}

