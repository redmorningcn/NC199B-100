/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
 * �ļ���  ��SZ_STM32F107VC_LIB.h
 * ����    ���ṩSTM32F107VC����IV�ſ�����Ŀ⺯��
 * ʵ��ƽ̨��STM32���ۿ�����
 * ����    ��www.armjishu.com 
**********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* ���ܺ꿪��  ���δʹ����Щ���ܣ����򿪶�Ӧ�ĺ꿪�ؿ��Լ�С�������Ĵ�С */
//#define       USE_ADC
//#define       USE_RTC
//#define       USE_TIM5
//#define       USE_TIM3
//#define       USE_PVD
//#define       USE_IWDG
//#define       USE_RTC_ALARM

/* �������궨�� bitband macro ------------------------------------------------*/
/* ʹ��bitband����������bit�����ĳ����Ч�ʣ�����GPIO�ܽŵĿ���Ч����Ϊ���� */
/* ��������� ��32MB����������ķ���ӳ��Ϊ��1MB ����bit-band���ķ���(ʵ�ʴ�С����оƬ�й�) */
#define Periph_BASE         0x40000000  // �������ַ Peripheral 
#define Periph_BB_BASE      0x42000000  // �������������ַ Peripheral bitband

/* ע�⣺���볣������ʱ���ڱ���ʱ��������������������ַ�������ܴﵽ����Ч�ʵ�Ŀ��(�Ƽ�)
         ����������������ֻ��������ʱ��STM32�Լ������������ַ��Ч�ʻ����ۿ�(���Ƽ�) */
#define Periph_BB(PeriphAddr, BitNumber)    \
          *(__IO uint32_t *) (Periph_BB_BASE | ((PeriphAddr - Periph_BASE) << 5) | ((BitNumber) << 2))
	 
#define Periph_ResetBit_BB(PeriphAddr, BitNumber)    \
          (*(__IO uint32_t *) (Periph_BB_BASE | ((PeriphAddr - Periph_BASE) << 5) | ((BitNumber) << 2)) = 0)
   
#define Periph_SetBit_BB(PeriphAddr, BitNumber)       \
          (*(__IO uint32_t *) (Periph_BB_BASE | ((PeriphAddr - Periph_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define Periph_GetBit_BB(PeriphAddr, BitNumber)       \
          (*(__IO uint32_t *) (Periph_BB_BASE | ((PeriphAddr - Periph_BASE) << 5) | ((BitNumber) << 2)))

/* ����GPIO����������������壬nΪbitλ�÷�ΧΪ0��15    */
/* ���Ƕ�GPIOA.15����������Ҫ��ʼ��GPIO��Ȼ��ʹ�÷����� */
/* ��GPIOA.15����͵�ƽ��   PAOutBit(15) = 0;           */
/* ��GPIOA.15����͵�ƽ��   PAOutBit(15) = 1;           */
/* ��ȡGPIOA.15����ĵ�ƽ�� data = PAInBit(15);         */
#define PAOutBit(n)     Periph_BB((uint32_t)&GPIOA->IDR,n)  //��� 
#define PASetBit(n)     (PAOutBit(n) = 1)                   //��� ��
#define PAResetBit(n)   (PAOutBit(n) = 0)                   //��� ��
#define PAInBit(n)      Periph_BB((uint32_t)&GPIOA->IDR,n)  //���� 

#define PBOutBit(n)     Periph_BB((uint32_t)&GPIOB->ODR,n)  //��� 
#define PBSetBit(n)     (PBOutBit(n) = 1)                   //��� ��
#define PBResetBit(n)   (PBOutBit(n) = 0)                   //��� ��
#define PBInBit(n)      Periph_BB((uint32_t)&GPIOB->IDR,n)  //���� 

#define PCOutBit(n)     Periph_BB((uint32_t)&GPIOC->ODR,n)  //��� 
#define PCSetBit(n)     (PCOutBit(n) = 1)                   //��� ��
#define PCResetBit(n)   (PCOutBit(n) = 0)                   //��� ��
#define PCInBit(n)      Periph_BB((uint32_t)&GPIOC->IDR,n)  //���� 

#define PDOutBit(n)     Periph_BB((uint32_t)&GPIOD->ODR,n)  //��� 
#define PDSetBit(n)     (PDOutBit(n) = 1)                   //��� ��
#define PDResetBit(n)   (PDOutBit(n) = 0)                   //��� ��
#define PDInBit(n)      Periph_BB((uint32_t)&GPIOD->IDR,n)  //���� 

#define PEOutBit(n)     Periph_BB((uint32_t)&GPIOE->ODR,n)  //��� 
#define PESetBit(n)     (PEOutBit(n) = 1)                   //��� ��
#define PEResetBit(n)   (PEOutBit(n) = 0)                   //��� ��
#define PEInBit(n)      Periph_BB((uint32_t)&GPIOE->IDR,n)  //����

#define PFOutBit(n)     Periph_BB((uint32_t)&GPIOF->ODR,n)  //��� 
#define PFSetBit(n)     (PFOutBit(n) = 1)                   //��� ��
#define PFResetBit(n)   (PFOutBit(n) = 0)                   //��� ��
#define PFInBit(n)      Periph_BB((uint32_t)&GPIOF->IDR,n)  //����

#define PGOutBit(n)     Periph_BB((uint32_t)&GPIOG->ODR,n)  //��� 
#define PGSetBit(n)     (PGOutBit(n) = 1)                   //��� ��
#define PGResetBit(n)   (PGOutBit(n) = 0)                   //��� ��
#define PGInBit(n)      Periph_BB((uint32_t)&GPIOG->IDR,n)  //����


/* �ڲ�SRAM������ ��32MB SRAM�������ķ���ӳ��Ϊ��1MB SRAMbit-band���ķ���(ʵ�ʴ�С����оƬ�й�) */
#define RAM_BASE            0x20000000  // �ڲ�SRAM����ַ  
#define RAM_BB_BASE         0x22000000  // �ڲ�SRAM����������ַ

#define SRAM_ResetBit_BB(VarAddr, BitNumber)    \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
   
#define SRAM_SetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 1)

#define SRAM_GetBit_BB(VarAddr, BitNumber)       \
          (*(__IO uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))

typedef enum 
{
  COM1 = 0,   
  COM2 = 1
} COM_TypeDef;   

#define UART_RX_BUFFER_SIZE    11
typedef struct 
{
  USART_TypeDef* uart;   
  uint32_t uart_rx_length;
  uint32_t uart_rx_done;
  uint32_t uart_rx_write_length;
  uint32_t uart_rx_read_point;
  uint32_t uart_rx_write_point;
  uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
} SZ_USART_DATA_TypeDef;

extern SZ_USART_DATA_TypeDef SZ_USART1_DATA, SZ_USART2_DATA;

typedef struct
{
	uint8_t				buf[20];										//��������
	uint8_t				ptr;
	uint8_t             ok;
	uint32_t			timeptr;
}UART1;

extern UART1 rx,tx;

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/** ���ڹܽ���Դ��������  **/
#define COMn                             2

/** ����1�ܽ���Դ����  **/
#define SZ_STM32_COM1_STR                "USART1"
#define SZ_STM32_COM1                    USART1
#define SZ_STM32_COM1_CLK                RCC_APB2Periph_USART1
#define SZ_STM32_COM1_TX_PIN             GPIO_Pin_9
#define SZ_STM32_COM1_TX_GPIO_PORT       GPIOA
#define SZ_STM32_COM1_TX_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SZ_STM32_COM1_RX_PIN             GPIO_Pin_10
#define SZ_STM32_COM1_RX_GPIO_PORT       GPIOA
#define SZ_STM32_COM1_RX_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SZ_STM32_COM1_IRQn               USART1_IRQn

/** ����2�ܽ���Դ���� (USART2 pins remapped on GPIOD) **/
#define SZ_STM32_COM2_STR                "USART2"
#define SZ_STM32_COM2                    USART2
#define SZ_STM32_COM2_CLK                RCC_APB1Periph_USART2
#define SZ_STM32_COM2_TX_PIN             GPIO_Pin_2
#define SZ_STM32_COM2_TX_GPIO_PORT       GPIOA
#define SZ_STM32_COM2_TX_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SZ_STM32_COM2_RX_PIN             GPIO_Pin_3
#define SZ_STM32_COM2_RX_GPIO_PORT       GPIOA
#define SZ_STM32_COM2_RX_GPIO_CLK        RCC_APB2Periph_GPIOA
#define SZ_STM32_COM2_IRQn               USART2_IRQn

extern const uint8_t STM32F10x_STR[];
extern __IO uint32_t TimingDelay;
extern uint32_t STM32DeviceSerialID[3]; /* ȫ�ֱ���IntDeviceSerial��Ŷ������豸ID */
extern __IO uint16_t ADCConvertedValue;  /* 12λADCģ��ת�������ֻ��ADCConvertedValue�ĵ�12λ��Ч */
extern __IO uint32_t TimeDisplay;

/** ͨ�ú�������  **/  
void delay(__IO uint32_t nCount);
void NVIC_GroupConfig(void);
void SZ_STM32_SysTickInit(uint32_t HzPreSecond);
void SysTickDelay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void GetDeviceSerialID(void);

extern void __SZ_STM32_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
extern void SZ_STM32_COMInit(COM_TypeDef COM, uint32_t BaudRate);

/**-------------------------------------------------------
  * @������ NVIC_COMConfiguration
  * @����   ���ô���1�ʹ���2���жϲ���
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
extern void NVIC_COMConfiguration(void);


//ȡ������ɱ�־  ���+˼ά
extern uint8_t	GetFramRecEndFlg(void);

//�������ɱ�־  ���+˼ά
extern void  ClearFramRecEndFlg(void);

//ȡ485����  ���+˼ά
extern uint16_t  ReadRs485Data(uint8_t *Buf);

extern uint32_t UsartReadData(SZ_USART_DATA_TypeDef* SZ_USART,  uint8_t* Buffer);

extern void Uart_1Control(void);
/**-------------------------------------------------------
  * @������ USART1_IRQHandler
  * @����   ����1�жϴ�����
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
extern void USART1_IRQHandler(void);

/**-------------------------------------------------------
  * @������ USART2_IRQHandler
  * @����   ����2�жϴ�����
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
extern void USART2_IRQHandler(void);

void SZ_STM32_ADC_Configuration(void);
uint16_t GetADCConvertedValue(void);
 
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength);
    
void TIM5_Init(void);

void TIM3_PWM_Init(void);

void SZ_STM32_IWDGInit(uint32_t TimeoutFreMs);

void SZ_STM32_PVDConfiguration(void);

#ifdef __cplusplus
}
#endif
/******************* (C) COPYRIGHT 2013 www.armjishu.com *****END OF FILE****/

