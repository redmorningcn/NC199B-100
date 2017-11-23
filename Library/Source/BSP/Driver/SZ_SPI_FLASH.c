/******************** (C) COPYRIGHT 2013 www.armjishu.com  ********************
 * �ļ���  ��SZ_SPI_FLASH.c
 * ����    ��ʵ��STM32F107VC����IV�ſ������SPI�洢��W25Q16_W2XQ16�ײ㺯��
 * ʵ��ƽ̨��STM32���ۿ�����
 * ��׼��  ��STM32F10x_StdPeriph_Driver V3.5.0
 * ����    ��www.armjishu.com 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "SZ_SPI_FLASH.h"
#include "SZ_STM32F107VC_LIB.h"
#include "DELAY.h"


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

#define Dummy_Byte                      0xA5

/* Private macro -------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
//#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
#define SPI_FLASH_CS_LOW()       (Periph_BB((uint32_t)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 0)

/* Deselect SPI FLASH: Chip Select pin high */
//#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN)
#define SPI_FLASH_CS_HIGH()      (Periph_BB((uint32_t)&SPI_FLASH_CS_GPIO_PORT->ODR, SPI_FLASH_CS_PIN_NUM) = 1)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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
    SPI_FLASH_CS_HIGH();

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
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_SectorErase
  * @����   ����SPI FLASHһ����������������
  *         Erases the specified FLASH sector.
  * @����   SectorAddr: ������ַ address of the sector to erase.
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
    /* Send write enable instruction */
    SPI_FLASH_WriteEnable();

    /* Sector Erase */
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /* Send Sector Erase instruction */
    SPI_FLASH_SendByte(W25X_SectorErase);
    /* Send SectorAddr high nibble address byte */
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    /* Send SectorAddr medium nibble address byte */
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    /* Send SectorAddr low nibble address byte */
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_ChipErase
  * @����   ��������SPI FLASH����������
  *         Erases the entire FLASH.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_ChipErase(void)
{
    /* Send write enable instruction */
    SPI_FLASH_WriteEnable();

    /* Bulk Erase */
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /* Send Bulk Erase instruction  */
    SPI_FLASH_SendByte(W25X_ChipErase);
    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_PageWrite
  * @����   ����д����д�����һ���ֽڶ�С�ڵ���һҳ��С������
  *         Writes more than one byte to the FLASH with a single 
  *         WRITE cycle(Page WRITE sequence). The number of byte 
  *         can't exceed the FLASH page size.
  * @����   - pBuffer : ָ�����д�����ݻ������ĵ�ַָ��
  *             pointer to the buffer  containing the data to be
  *             written to the FLASH.
  *         - WriteAddr : flash��д���ַ
  *             FLASH's internal address to write to.
  *         - NumByteToWrite : д����ֽ���
  *             number of bytes to write to the FLASH, must be
  *             equal or less than "SPI_FLASH_PageSize" value.
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    /* Enable the write access to the FLASH */
    SPI_FLASH_WriteEnable();

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();
    /* Send "Write to Memory " instruction */
    SPI_FLASH_SendByte(W25X_PageProgram);
    /* Send WriteAddr high nibble address byte to write to */
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    /* Send WriteAddr medium nibble address byte to write to */
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    /* Send WriteAddr low nibble address byte to write to */
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
    }

    /* while there is data to be written on the FLASH */
    while (NumByteToWrite--)
    {
        /* Send the current byte */
        SPI_FLASH_SendByte(*pBuffer);
        /* Point on the next byte to be written */
        pBuffer++;
    }

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    /* Wait the end of Flash writing */
    SPI_FLASH_WaitForWriteEnd();
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_BufferWrite
  * @����   ��SPI FLASHд��һ�����ݣ�д����ֽ������Դ���һҳ����
  *         Writes block of data to the FLASH. In this function,
  *         the number of WRITE cycles are reduced,
  *         using Page WRITE sequence.
  * @����   - pBuffer : ָ�����д�����ݻ������ĵ�ַָ��
  *             pointer to the buffer  containing the data to be
  *             written to the FLASH.
  *         - WriteAddr : flash��д���ַ
  *             FLASH's internal address to write to.
  *         - NumByteToWrite : д����ֽ���
  *             number of bytes to write to the FLASH.
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
            {
                temp = NumOfSingle - count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
                WriteAddr +=  count;
                pBuffer += count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_BufferRead
  * @����   ��SPI FLASH����һ�����ݣ�д����ֽ������Դ���һҳ����
  *         Reads a block of data from the FLASH.
  * @����   - pBuffer : ָ�����д�����ݻ������ĵ�ַָ��
  *             pointer to the buffer that receives the data read
  *             from the FLASH.
  *         - ReadAddr : flash�Ķ���ʼ��ַ
  *             FLASH's internal address to read from.
  *         - NumByteToWrite : �������ֽ���
  *             number of bytes to read from the FLASH.
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


/**-----------------------------------------------------------------
  * @������ SPI_FLASH_ReadRManufactDeviceID
  * @����   ��ȡSPI FLASH����ID���豸ID(�豸ID�������ͺ�����)
  *         Reads Manufacturer ID and  Device ID
  * @����   ��
  * @����ֵ 16bit���ߵ�������Ϊ����ID������ID
***----------------------------------------------------------------*/
u32 SPI_FLASH_Read_ManufactDeviceID(void)
{
    u32 Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "RDID " instruction */
    SPI_FLASH_SendByte(W25X_ManufactDeviceID);

    /* Read a byte from the FLASH */
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

    SPI_FLASH_SendByte(0x00);

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1;

    return Temp;
}


/**-----------------------------------------------------------------
  * @������ SPI_FLASH_ReadID
  * @����   ��ȡSPI FLASH����ID���豸ID(�豸ID�������ͺ�����)
  *         Reads Manufacturer ID and two Device ID bytes
  * @����   ��
  * @����ֵ 24bit���ߵ�������Ϊ����ID�����ͺ�����
***----------------------------------------------------------------*/
u32 SPI_FLASH_ReadID(void)
{
    u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "RDID " instruction */
    SPI_FLASH_SendByte(W25X_JedecDeviceID);

    /* Read a byte from the FLASH */
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_ReadDeviceID
  * @����   ��ȡSPI FLASH�豸ID
  *         Read one Device ID bytes
  * @����   ��
  * @����ֵ һ���ֽڵ�Device ID
***----------------------------------------------------------------*/
u32 SPI_FLASH_ReadDeviceID(void)
{
    u32 Temp = 0;

    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "RDID " instruction */
    SPI_FLASH_SendByte(W25X_DeviceID);
    Temp = SPI_FLASH_SendByte(Dummy_Byte);
    Temp = SPI_FLASH_SendByte(Dummy_Byte);
    Temp = SPI_FLASH_SendByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp = SPI_FLASH_SendByte(Dummy_Byte);

    /* Deselect the FLASH: Chip Select high */
    SPI_FLASH_CS_HIGH();

    return Temp;
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_StartReadSequence
  * @����   ����һ����ȡSPI FLASH�ķ��ʣ��������Ͷ��������ʼ��ַ
  *         Initiates a read data byte (READ) sequence from the Flash.
  *         This is done by driving the /CS line low to select the device,
  *         then the READ instruction is transmitted followed by 3 bytes
  *         address. This function exit and keep the /CS line low, so the
  *         Flash still being selected. With this technique the whole
  *         content of the Flash is read with a single READ instruction.
  *         Read one Device ID bytes
  * @����   ReadAddr FLASH�ķ��ʵ�ַ
  *                  FLASH's internal address to read from.
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
    /* Select the FLASH: Chip Select low */
    SPI_FLASH_CS_LOW();

    /* Send "Read from Memory " instruction */
    SPI_FLASH_SendByte(W25X_ReadData);

    /* Send the 24-bit address of the address to read from -----------------------*/
    /* Send ReadAddr high nibble address byte */
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /* Send ReadAddr medium nibble address byte */
    SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
    /* Send ReadAddr low nibble address byte */
    SPI_FLASH_SendByte(ReadAddr & 0xFF);
}


/**-----------------------------------------------------------------
  * @������ SPI_FLASH_ReadByte
  * @����   ��ȡSPI FLASH��һ���ֽڣ�δ�������Ͷ��������ʼ��ַ
  * @����   ��
  * @����ֵ ��SPI_FLASH��ȡ��һ���ֽ�
***----------------------------------------------------------------*/
u8 SPI_FLASH_ReadByte(void)
{
    return (SPI_FLASH_SendByte(Dummy_Byte));
}


/**-----------------------------------------------------------------
  * @������ SPI_FLASH_SendByte
  * @����   ͨ��SPI���߷���һ���ֽ�����(˳�����һ���ֽ�����)
  *         Sends a byte through the SPI interface and return the byte
  *         received from the SPI bus.
  * @����   Ҫд���һ���ֽ�����
  * @����ֵ �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ���ֽ�
***----------------------------------------------------------------*/
u8 SPI_FLASH_SendByte(u8 byte)
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
  * @������ SPI_FLASH_SendHalfWord
  * @����   ͨ��SPI���߷���һ������(16bit=2���ֽ�����)(˳���������)
  *         Sends a Half Word through the SPI interface and return the
  *         Half Word received from the SPI bus.
  * @����   Ҫд���һ����������(16bit)
  * @����ֵ �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ����������(16bit)
***----------------------------------------------------------------*/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /* Send Half Word through the SPI2 peripheral */
    SPI_I2S_SendData(SPI2, HalfWord);

    /* Wait to receive a Half Word */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the Half Word read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI2);
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_WriteEnable
  * @����   SPI FLASHдʹ��
  *         Enables the write access to the FLASH.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_WriteDisable
  * @����   SPI FLASHд��ֹ
  *         Disable the write access to the FLASH.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_WriteDisable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteDisable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**-----------------------------------------------------------------
  * @������ SPI_FLASH_WaitForWriteEnd
  * @����   ͨ��������ȡSPI FLASH��״̬�Ĵ����ж�д���Ƿ�ִ�н���
  *         Polls the status of the Write In Progress (WIP) flag in the
  *         FLASH's status  register  and  loop  until write  opertaion
  *         has completed.
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);

  }
  while ((FLASH_Status & WIP_FlagMask) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/**-----------------------------------------------------------------
  * @������ SPI_Flash_PowerDown
  * @����   SPI FLASH�������ģʽ
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
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
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   

/******************* (C) COPYRIGHT 2010 www.armjishu.com *****END OF FILE****/
