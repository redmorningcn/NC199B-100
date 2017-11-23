/******************** (C) COPYRIGHT 2013 www.armjishu.com  ********************
 * �ļ���  ��DATA_STORAGE.c
 * ����    ��ʵ��SPI�洢��MX25L12805�Ĳ��ԡ��洢����д
 * ��׼��  ��STM32F10x_StdPeriph_Driver V3.5.0
 * ����    ��www.armjishu.com 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "SZ_STM32F107VC_LIB.h"
#include "SZ_SPI_FLASH.h"
#include "DATA_STORAGE.h"
#include "stdio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define  FLASH_WriteAddress         0x000000
#define  FLASH_ReadAddress          FLASH_WriteAddress
#define  FLASH_SectorToErase        FLASH_WriteAddress

#define  BufferSize (countof(Tx_Buffer)-1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx_Buffer[] = "\n\r www.nanche.com STM32F107 SPI Flash Test Example: \n\r communication with an MX25L12805 SPI FLASH";
uint8_t Index, Rx_Buffer[BufferSize];
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
__IO uint32_t FlashID = 0;
__IO uint32_t DeviceID = 0;
__IO uint32_t ManufactDeviceID =0;


void FLASH_TEST(void)
{
  printf("\r\n ���ܵ���·�� SPI�洢��MX25L12805����ʵ��: \r\n");

    /* Initialize the SPI FLASH driver */
    
  SPI_FLASH_Init(); 
  /* Get SPI Flash ID */  
  FlashID = SPI_FLASH_ReadID();
  printf("\r\n Flash JEDEC Manufacturer ID and Device ID is 0x%X", FlashID);

  ManufactDeviceID = SPI_FLASH_Read_ManufactDeviceID();
  printf("\r\n ManufactDeviceID is 0x%X", ManufactDeviceID);
  
  DeviceID = SPI_FLASH_ReadDeviceID();
  printf("\r\n Flash Device ID is 0x%X", DeviceID);

  /* Check the SPI Flash ID */
  if ((FlashID == MX25L12805) || (FlashID == W25Q16_FLASH_ID))
  {
      if ((FlashID == MX25L12805))
      {
          printf("\r\n Winbond Serial Flash MX25L12805 Identitied!");
      }
      else
      {
          printf("\r\n Winbond Serial Flash W25Q16 Identitied!");
      }           
      printf("\r\n Size:128M-bit(16M-byte),4K-bytes/Sector,64K-bytes/Block,256-bytes/page!");
  }   
  else
  {
      printf("\r\n MX25L12805 Test Failed!\n\r");
  }
      /* Perform a write in the Flash followed by a read of the written data */
      /* Erase SPI FLASH Sector to write on */

  SPI_FLASH_SectorErase(FLASH_SectorToErase);

  printf("\r\n д��: %s", Tx_Buffer);

  /* Write Tx_Buffer data to SPI FLASH memory д�� */
  SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);

  /* Read data from SPI FLASH memory  ��ȡ*/
  SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);

  /* Check the corectness of written dada ��� */
  TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

  if(PASSED == TransferStatus1)
  {
     printf("\r\n ����������д������һ��!\n\r");
  }
  else
  {
     printf("\r\n ����-->����������д�����ݲ�һ��!\n\r");
  }
      
  /* Perform an erase in the Flash followed by a read of the written data */
  /* Erase SPI FLASH Sector to write on */
  SPI_FLASH_SectorErase(FLASH_SectorToErase);

  /* Read data from SPI FLASH memory */
  SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);

  /* Check the corectness of erasing operation dada */
  for (Index = 0; Index < BufferSize; Index++)
  {
    if (Rx_Buffer[Index] != 0xFF)
    {
      TransferStatus2 = FAILED;
    }
  }

  if(PASSED == TransferStatus2)
  {
    printf("\r\n �������������Ϊ0xFF����ȷ!\n\r");
  }
  else
  {
    printf("\r\n ����-->������������ݲ�Ϊ0xFF!\n\r");
  }

  /* TransferStatus2 = PASSED, if the specified sector part is erased */
  /* TransferStatus2 = FAILED, if the specified sector part is not well erased */

  if((PASSED == TransferStatus1) && (PASSED == TransferStatus2))
  {
    if ((FlashID == MX25L12805))         /* #define  MX25L12805       0xC22018 */
    {
      printf("\r\n MX25L12805 Test Sucessed!\n\r");
    }
    else
    {
      printf("\r\n MX25L12805 Test Sucessed!\n\r");
    } 

  }
  else
  {
    printf("\r\n -->Failed: MX25L12805 Test Failed!\n\r");
  }
  
  SPI_Flash_PowerDown();    
}


