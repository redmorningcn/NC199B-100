/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
* �ļ���  ��SZ_STM32F107VC_LIB.c
* ����    ���ṩSTM32F107VC����IV�ſ�����Ŀ⺯��
* ʵ��ƽ̨��STM32���ۿ�����
* ����    ��www.armjishu.com 
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "SZ_STM32F107VC_LIB.h"
#include "RS485.h"
#include "POWER_MACRO.h"
#include "SPI_CS5463_AC.h"
#include "FM24CL64.h"
#include "MX25.h"
#include "DS3231.h"
#include "crccheck.h"
#include "delay.h"
#include <includes.h>

__IO uint32_t TimingDelay;

USART_TypeDef* COM_USART[COMn] = {SZ_STM32_COM1, SZ_STM32_COM2}; 
GPIO_TypeDef* COM_TX_PORT[COMn] = {SZ_STM32_COM1_TX_GPIO_PORT, SZ_STM32_COM2_TX_GPIO_PORT};
GPIO_TypeDef* COM_RX_PORT[COMn] = {SZ_STM32_COM1_RX_GPIO_PORT, SZ_STM32_COM2_RX_GPIO_PORT};
const uint32_t COM_USART_CLK[COMn] = {SZ_STM32_COM1_CLK, SZ_STM32_COM2_CLK};
const uint32_t COM_TX_PORT_CLK[COMn] = {SZ_STM32_COM1_TX_GPIO_CLK, SZ_STM32_COM2_TX_GPIO_CLK};
const uint32_t COM_RX_PORT_CLK[COMn] = {SZ_STM32_COM1_RX_GPIO_CLK, SZ_STM32_COM2_RX_GPIO_CLK};
const uint16_t COM_TX_PIN[COMn] = {SZ_STM32_COM1_TX_PIN, SZ_STM32_COM2_TX_PIN};
const uint16_t COM_RX_PIN[COMn] = {SZ_STM32_COM1_RX_PIN, SZ_STM32_COM2_RX_PIN};

uint32_t STM32DeviceSerialID[3]; /* ȫ�ֱ���IntDeviceSerial��Ŷ������豸ID */

/**-------------------------------------------------------
* @������ delay
* @����   �򵥵�delay��ʱ����.
* @����   �ӳ������� 0--0xFFFFFFFF
* @����ֵ ��
***------------------------------------------------------*/
void delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

/**-------------------------------------------------------
* @������ NVIC_GroupConfig
* @����   ����NVIC�ж����ȼ����麯��.
*         Ĭ������Ϊ1���ر�ʾ�����ȼ�, 3���ر�ʾ�����ȼ�
*         �û����Ը�����Ҫ�޸�
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void NVIC_GroupConfig(void)
{
    /* ����NVIC�ж����ȼ�����:
    - 1���ر�ʾ�����ȼ�  �����ȼ��Ϸ�ȡֵΪ 0 �� 1 
    - 3���ر�ʾ�����ȼ�  �����ȼ��Ϸ�ȡֵΪ 0..7
    - ��ֵԽ�����ȼ�Խ�ߣ�ȡֵ�����Ϸ���Χʱȡ��bitλ 
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    /*==================================================================================
    NVIC_PriorityGroup   |  �����ȼ���Χ  |  �����ȼ���Χ  |   ����
    ==================================================================================
    NVIC_PriorityGroup_0  |      0         |      0-15      |   0 ���ر�ʾ�����ȼ�
    |                |                |   4 ���ر�ʾ�����ȼ� 
    ----------------------------------------------------------------------------------
    NVIC_PriorityGroup_1  |      0-1       |      0-7       |   1 ���ر�ʾ�����ȼ�
    |                |                |   3 ���ر�ʾ�����ȼ� 
    ----------------------------------------------------------------------------------
    NVIC_PriorityGroup_2  |      0-3       |      0-3       |   2 ���ر�ʾ�����ȼ�
    |                |                |   2 ���ر�ʾ�����ȼ� 
    ----------------------------------------------------------------------------------
    NVIC_PriorityGroup_3  |      0-7       |      0-1       |   3 ���ر�ʾ�����ȼ�
    |                |                |   1 ���ر�ʾ�����ȼ� 
    ----------------------------------------------------------------------------------
    NVIC_PriorityGroup_4  |      0-15      |      0         |   4 ���ر�ʾ�����ȼ�
    |                |                |   0 ���ر�ʾ�����ȼ�   
    ==================================================================================*/
}

/**-------------------------------------------------------
* @������ SZ_STM32_SysTickInit
* @����   ��ʼ��ϵͳ��ʱ��SysTick
*         �û����Ը�����Ҫ�޸�
* @����   ÿ���жϴ���
* @����ֵ ��
***------------------------------------------------------*/
void SZ_STM32_SysTickInit(uint32_t HzPreSecond)
{
    /* HzPreSecond = 1000 to Setup SysTick Timer for 1 msec interrupts.
    ------------------------------------------
    1. The SysTick_Config() function is a CMSIS function which configure:
    - The SysTick Reload register with value passed as function parameter.
    - Configure the SysTick IRQ priority to the lowest value (0x0F).
    - Reset the SysTick Counter register.
    - Configure the SysTick Counter clock source to be Core Clock Source (HCLK).
    - Enable the SysTick Interrupt.
    - Start the SysTick Counter.
    
    2. You can change the SysTick Clock source to be HCLK_Div8 by calling the
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8) just after the
    SysTick_Config() function call. The SysTick_CLKSourceConfig() is defined
    inside the misc.c file.
    
    3. You can change the SysTick IRQ priority by calling the
    NVIC_SetPriority(SysTick_IRQn,...) just after the SysTick_Config() function 
    call. The NVIC_SetPriority() is defined inside the core_cm3.h file.
    
    4. To adjust the SysTick time base, use the following formula:
    
    Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
    - Reload Value is the parameter to be passed for SysTick_Config() function
    - Reload Value should not exceed 0xFFFFFF
    */
    if (SysTick_Config(SystemCoreClock / HzPreSecond))
    { 
        /* Capture error */ 
        while (1);
    }
}

/**-------------------------------------------------------
* @������ SysTickDelay
* @����   ϵͳ��ʱ��ʵ�ֵ���ʱ�������������û������е���
*         �û����Ը�����Ҫ�޸�
* @����   �ӳ����������ӳ�ʱ��ΪnTime/HzPreSecond
*         ���Ϊ -1/HzPreSecond
* @����ֵ ��
***------------------------------------------------------*/
void SysTickDelay(__IO uint32_t nTime)
{ 
    TimingDelay = nTime;
    
    while(TimingDelay != 0);
}

/**-------------------------------------------------------
* @������ TimingDelay_Decrement
* @����   ϵͳ���Ķ�ʱ�����������õ��Ӻ���
*         ��ȫ�ֱ���TimingDelay��һ������ʵ����ʱ
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}

extern void SysTick_Handler_User(void);
/**-------------------------------------------------------
* @������ SysTick_Handler
* @����   ϵͳ���Ķ�ʱ��������������
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
    
    SysTick_Handler_User();
}

void GetDeviceSerialID(void)
{
    STM32DeviceSerialID[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    STM32DeviceSerialID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    STM32DeviceSerialID[2] = *(__IO uint32_t*)(0x1FFFF7F0);
}

/**-------------------------------------------------------
* @������ __SZ_STM32_COMInit
* @����   ��STM32��USART��ʼ���ײ㺯��
* @����1  COM1  ��ӦSTM32��USART1 ��Ӧ�������ϴ���1
*         COM2  ��ӦSTM32��USART2 ��Ӧ�������ϴ���2
* @����2  ָ��һ����Ա�Ѹ�ֵUSART_InitTypeDef�ṹ���ָ��
* @����ֵ ��
***------------------------------------------------------*/
void __SZ_STM32_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIO clock */
    /* ʹ��STM32��USART��ӦGPIO��Clockʱ�� */
    RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);
    
    if (COM == COM1)
    {
        /* ʹ��STM32��USART1/USART2/USART3��Clockʱ�� */
        RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
    }
    else
    {
        /* Enable the USART2 Pins Software Remapping */
        /* ʹ��STM32��USART2�Ĺܽ���ӳ�� */
        //        GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);    //����·������ӳ��
        /* ʹ��STM32��USART��Clockʱ�� */
        RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
    }
    
    /* Configure USART Tx as alternate function push-pull */
    /* ��ʼ��STM32��USART��TX�ܽţ�����Ϊ���ù���������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);
    
    /* Configure USART Rx as input floating */
    /* ��ʼ��STM32��USART��RX�ܽţ�����Ϊ���ù������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
    GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
    
    /* USART configuration */
    /* ���ݴ���Ĳ�����ʼ��STM32��USART���� */
    USART_Init(COM_USART[COM], USART_InitStruct);
    
    /* Enable USART */
    /* ʹ��STM32��USART����ģ�� */
    USART_Cmd(COM_USART[COM], ENABLE);
}

/**-------------------------------------------------------
* @������ __SZ_STM32_COMInit
* @����   �����û���STM32��USART��ʼ������
* @����1  COM1  ��ӦSTM32��USART1 ��Ӧ�������ϴ���1
*         COM2  ��ӦSTM32��USART2 ��Ӧ�������ϴ���2
* @����2  BaudRate ���ڵĲ����ʣ�����"115200"
* @����ֵ ��
***------------------------------------------------------*/
void SZ_STM32_COMInit(COM_TypeDef COM, uint32_t BaudRate)
{
    
    USART_InitTypeDef USART_InitStructure;
    
    /* USARTx Ĭ������:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = BaudRate;              //���ڵĲ����ʣ�����115200 ��ߴ�4.5Mbits/s
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�����ֳ���(8λ��9λ)
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //�����õ�ֹͣλ-֧��1��2��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;         //����żУ��  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //˫��ģʽ��ʹ�ܷ��ͺͽ���
    
    __SZ_STM32_COMInit(COM, &USART_InitStructure);  // ����STM32��USART��ʼ���ײ㺯��
    
    SystemCoreClockUpdate();
    //    printf("\n\r ϵͳ�ں�ʱ��Ƶ��(SystemCoreClock)Ϊ��%dHz.\n\r",
    //            SystemCoreClock);
}

/**-------------------------------------------------------
* @������ NVIC_COMConfiguration
* @����   ���ô���1�ʹ���2���жϲ���
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void NVIC_COMConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    USART_ITConfig(SZ_STM32_COM1, USART_IT_RXNE, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(SZ_STM32_COM2, USART_IT_RXNE, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = SZ_STM32_COM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}

UART1 rx,tx;
//
//void USART_GetInputString(void)
//{
//	uint8_t mychar = 0;
//
//	mychar = USART_ReceiveData(USART1);
//	if(rx.ok == 0)
//	{
//		if(mychar == '\r')
//		{
//			rx.buf[rx.ptr] = '\0';
//			rx.ok = 1;
//		}
//		else if(mychar == '\b')
//		{
//			if(rx.ptr > 0)
//			{	
//				rx.ptr--;
//			}
//		}
//	    else 
//		{
//			rx.buf[rx.ptr++] = mychar;
//			printf("%d,  %x ",rx.ptr,mychar);
//		}
//		if(rx.ptr >= 3)
//		{
//		 	rx.buf[rx.ptr] = '\0';
//			rx.ok = 1;
//		}
//	}
//}
//
//
////Usart_1IrqHandlerUser()
////{
////	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
////    {
////         /* clear interrupt */
////		USART_GetInputString();	
////		while(USART_GetITStatus(USART1, USART_IT_RXNE) == RESET);
////	}
////	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
////	{
////		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
////    }
////
////    /* If overrun condition occurs, clear the ORE flag 
////                              and recover communication */
////     /*�������*/
////    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
////    {
//////		rs = USART_GetITStatus(USART1, USART_IT_RXNE);
//////		printf("RXNE״̬Ϊ%d��",rs);
//////        printf("�������");
//////		USART_ClearFlag(USART1,USART_FLAG_ORE);
//////		rx.buf[rx.ptr++] = 	USART1->DR & 0x01FF;
//////		printf("%d  , %x  ,  ",rx.ptr,rx.buf[rx.ptr-1]);
////		(void)USART_ReceiveData(USART1);
////    }
////  	 /*�������*/
////	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
////	{
////		/* clear interrupt */
////		USART_ClearITPendingBit(USART1, USART_IT_TC);
////	}
//////	while (USART1->SR & USART_FLAG_RXNE == 0);
////}
//
//void Usart_1IrqHandlerUser()
//{
//	uint8_t l_COM0TempRev = 0;
// 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
//         /* clear interrupt */
//        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		/* save on rx buffer */		
//		/* save character */
//        l_COM0TempRev = USART1->DR & 0x01ff;
//				printf("%x-", l_COM0TempRev);
//        rx.buf[rx.ptr] = l_COM0TempRev; 
//		rx.ptr++;
//		if(rx.ptr == 6)
//			rx.ptr = 0;
//		while (USART1->SR & USART_FLAG_RXNE == 0);
//    }
//
//    /* If overrun condition occurs, clear the ORE flag 
//                              and recover communication */
//     /*�������*/
//    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//    {
//        (void)USART_ReceiveData(USART1);
//    }
//  	 /*�������*/
//	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
//	{
//		/* clear interrupt */
//		USART_ClearITPendingBit(USART1, USART_IT_TC);
//	}
//}
//
//void Uart_1Control(void)
//{
//	uint8_t     fram_clr[4]     = {0};
//	uint8_t     IDBuf[4]        = {0};
//	uint8_t     clear_buf[24]   = {0};
//	uint32_t    ID              = 0;
//    
//	uint16_t    crc16_Cal;
//	uint16_t    crc16_Rec;
//	
//	TIME        system;    
//    
//    /***********************************************
//    * ������ 
//    */ 
//	if(rx.ok == 1) {
//		crc16_Cal = AppGetCrc16Chk1((uint8_t *)&rx.buf[0],9);
//		crc16_Rec = rx.buf[9] | rx.buf[10]<<8;		  //���յ���AppGetCrc16Chk1
//        
//		if((rx.buf[0]==0xAA)&&(rx.buf[1]==0x55)&&(crc16_Cal == crc16_Rec)) {
//			switch(rx.buf[2])
//			{
//			case 0xA1:				
//				ID = rx.buf[3]<<24 | rx.buf[4]<<16 | rx.buf[5]<<8 | rx.buf[6];
//				TO_Arayy(IDBuf , ID);
//				WriteFM24CL64(64 , IDBuf , 4);
//				printf("\r\n ID���óɹ���\r\n ");
//				ReadFM24CL64(64 , IDBuf , 4);
//				printf("\r\n ��װ��ID����Ϊ��%02d%02d%02d%02d\r\n" , IDBuf[3],IDBuf[2],IDBuf[1],IDBuf[0]);
//				break;
//                
//			case 0xA2:				
//				ID = rx.buf[3]<<16 | rx.buf[4]<<8 | rx.buf[5];
//				ID = ID | 0xAA000000;
//				TO_Arayy(IDBuf , ID);
//				WriteFM24CL64(68 , IDBuf , 4);
//				printf("\r\n ���ͳ������óɹ���\r\n ");
//				ReadFM24CL64(68 , IDBuf , 4);
//				if(IDBuf[2] == 0x01)
//					printf("\r\n װ������Ϊ:HXD1D %02d%02d\r\n" , IDBuf[1],IDBuf[0]);
//				break;
//                
//			case 0xA3:				//����ʱ��
//				system.Year=rx.buf[3];
//				system.Month=rx.buf[4];
//				system.Day=rx.buf[5];
//				system.Hour=rx.buf[6];
//				system.Min=rx.buf[7];
//				system.Sec=rx.buf[8];
//				WriteTime(system);
//				printf("\r\nϵͳʱ�����óɹ�!\r\n");
//				DisplayTime();
//				break;
//                
//			case 0xA4:				//��ѹֱ�����У׼
//				CS5463_AC_Adjust(VDCOFFC);												
//				break;
//			case 0xA5:				//����ֱ�����У׼
//				CS5463_AC_Adjust(IDCOFFC);										
//				break;
//			case 0xA6:				//��ѹ�������У׼
//				CS5463_AC_Adjust(VACOFFC);												
//				break;
//			case 0xA7:				//�����������У׼
//				CS5463_AC_Adjust(IACOFFC);											
//				break;
//			case 0xA8:				//��ѹֱ������У׼
//				CS5463_AC_Adjust(VACGAINC);									
//				break;
//			case 0xA9:				//����ֱ������У׼
//				CS5463_AC_Adjust(IACGAINC);									
//				break;
//                
//			case 0xAB:
//				WriteFM24CL64(32, clear_buf, 24);
//				printf("\r\n �������������\n");
//				break;
//			case 0xAC:				//���й�����
//				AC.PPpulse = 0;
//				AC.PPpower_base = 0;
//				AC.PPpower_temp = 0;
//				printf("\r\n ���й�����������\n");
//				break;
//			case 0xAD:				//���й�����
//				AC.NPpulse = 0;
//				AC.NPpower_base = 0;
//				AC.NPpower_temp = 0;
//				printf("\r\n ���й�����������\n");
//				break;
//			case 0xAE:				//���޹�����
//				AC.PQpulse = 0;
//				AC.PQpower_base = 0;
//				AC.PQpower_temp = 0;
//				printf("\r\n ���޹�����������\n");
//				break;
//			case 0xAF:				//���޹�����
//				AC.NQpulse = 0;
//				AC.NQpower_base = 0;
//				AC.NQpower_temp = 0;
//				printf("\r\n ���޹�����������\n");
//				break;
//                
//			case 0x5A:
//				CS5463_AC_Adjust(P_rate_ADD);
//				break;
//			case 0x5B:
//				CS5463_AC_Adjust(P_rate_SUB);
//				break;
//			case 0x5C:
//				CS5463_AC_Adjust(P_rate_DEL);
//				break;
//                
//			case 0x51:				
//				ReadFM24CL64(64 , IDBuf , 4);
//				printf("\r\n ��װ��IDΪ��%02d%02d%02d%02d\r\n" , IDBuf[3],IDBuf[2],IDBuf[1],IDBuf[0]);
//				break;
//                
//		    case 0x52:
//				ReadFM24CL64(68 , IDBuf , 4);
//				if(IDBuf[3] == 0xAA)
//				{
//					if(IDBuf[2] == 0x01)
//						printf("\r\n װ������Ϊ��HXD1D %02d%02d\r\n" , IDBuf[1],IDBuf[0]);
//				}
//				else 
//					printf("\r\n δ����װ�����ͣ�\r\n");
//				break;
//                
//			case 0x53:				//��ȡʱ��
//				DisplayTime();
//				break;
//                
//			case 0x54:						  //ͨ������ת��FLASH�е�������Ч����
//				DisplayTime();
//				ReadFM24CL64(64 , IDBuf , 4);
//				printf("\r\n װ�ñ�ţ�%02d%02d%02d%02d\r\n" , IDBuf[3],IDBuf[2],IDBuf[1],IDBuf[0]);
//				ReadFM24CL64(68 , IDBuf , 4);
//				if(IDBuf[3] == 0xAA)
//				{
//					if(IDBuf[2] == 0x01)
//                        printf("\r\n װ������Ϊ��SS4 %02d%02d\r\n" , IDBuf[1],IDBuf[0]);
//				}
//				else 
//					printf("\r\n δ����װ�����ͣ�\r\n");
//				DOWNLOAD(g_Flash_Adr/32);				
//				if(g_Flash_Adr == 0)
//					printf("\r\n FLASHоƬ������Ч���ݣ�\r\n");
//				break;
//                
//			case 0x55:				
//				printf("\r\n FLASHоƬ������............\r\n");
//				printf("\r\n ��Լ��Ҫ30�룬�����ĵȴ���\r\n");
//				MX25L3206_Erase(0, 4096);
//				printf("\r\n FLASHоƬ�Ѳ���\r\n");
//				break;
//                
//			case 0x56:
//				printf("\r\n FRAMоƬ������...\r\n");
//				WriteFM24CL64(60, fram_clr, 4);
//				g_Flash_Adr = 0;
//				printf("\r\n FRAMоƬ�Ѳ���\r\n");
//                
//			default:
//				break;
//			}
//		}
//	}
//	rx.ok = 0;
//	rx.ptr = 0;
//}	

uint8_t     l_COM0RdTmp;        					//����2״̬	
uint8_t     l_COM0TempRev;      					//����2�����ֽ�

uint8_t		RecTmpFlg0 =0,RecTmpFlg1 =0,RecTmpFlg2 =0;
uint8_t		l_FramRecEndFlg = 0;
uint32_t	COM0RecNumTmp0 = 0,l_COM0RecNumEnd = 0;
uint32_t	l_COM0RecLen = 0;

#define	    RS485_BUF_SIZE  	256
uint8_t 	Rs485RecDataBuf[RS485_BUF_SIZE];
/**-------------------------------------------------------
* @������ Usart_2IrqHandlerUser
* @����   ����2�жϴ�����
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void Usart_2IrqHandlerUser()
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        /* clear interrupt */
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		/* save on rx buffer */		
		/* save character */
        l_COM0TempRev = USART2->DR & 0x01ff;
        //				printf("%c-", l_COM0TempRev);
        Rs485RecDataBuf[COM0RecNumTmp0 % RS485_BUF_SIZE] = l_COM0TempRev; 
		COM0RecNumTmp0++;
		if(COM0RecNumTmp0 == RS485_BUF_SIZE)
			COM0RecNumTmp0 = 0;
		RecTmpFlg0 = l_COM0TempRev;
        if(RecTmpFlg0 == 0x7E) {
        	printf("\n\r���յ�֡β\n");
			//if(l_FramRecEndFlg == 0)							//����δ����������
        	{
	        	l_FramRecEndFlg = 1;							//֡������ɣ���־λ��ֵ
	        	
	        	if(COM0RecNumTmp0 > l_COM0RecNumEnd)
	        	{
	           		l_COM0RecLen 	= COM0RecNumTmp0 - l_COM0RecNumEnd;
				}
				else
				{
	           		l_COM0RecLen 	= COM0RecNumTmp0 + (255 - l_COM0RecNumEnd) + 1; //�����������
				}
				
	        	l_COM0RecNumEnd 	= COM0RecNumTmp0;
        	}
            
			RecRs485();
    	}
		while (USART2->SR & USART_FLAG_RXNE == 0);
    }
    
    /* If overrun condition occurs, clear the ORE flag 
    and recover communication */
    /*�������*/
    if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
    {
        (void)USART_ReceiveData(USART2);
    }
    /*�������*/
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
}


//ȡ������ɱ�־  ���+˼�S
uint8_t	GetFramRecEndFlg(void)
{
	return	l_FramRecEndFlg;
}

//�������ɱ�־  ���+˼�S
void  ClearFramRecEndFlg(void)
{
	l_FramRecEndFlg = 0;
}

//ȡ485����  ���+˼�S
uint16_t  ReadRs485Data(uint8_t *Buf)
{
	uint32_t  i;
	
	printf("\r\n RS485 %d %d ",l_COM0RecNumEnd,l_COM0RecLen);
	
	if(l_COM0RecLen > RS485_BUF_SIZE)
	{
		l_COM0RecLen = RS485_BUF_SIZE;
	}
    //	printf("ȥ�ظ�ͨѶǰ����:");
	for(i = 0;i < l_COM0RecLen;i++)
	{
		Buf[i] = Rs485RecDataBuf[(l_COM0RecNumEnd + RS485_BUF_SIZE - l_COM0RecLen + i) 
            % RS485_BUF_SIZE];
        //		printf("%x ",Buf[i]);				
	}
    //	Sendrs485(Buf,l_COM0RecLen);						//��ӡ���յ�������
	
	return	l_COM0RecLen;
}

///**-------------------------------------------------------
//  * @������ USART1_IRQHandler
//  * @����   ����1�жϴ�����
//  * @����   ��
//  * @����ֵ ��
//***------------------------------------------------------*/
//void USART1_IRQHandler(void)
//{
//    /* ��ʼ�ٽ���Դ���ʣ���ֹ�ж� */
////	__disable_irq(); 
//
//    /* ���ô���1�жϴ����Ӻ��� */
//    Usart_1IrqHandlerUser();
//    
//    /* �����ٽ���Դ���ʣ������ж� */
////    __enable_irq();
//}

///**-------------------------------------------------------
//  * @������ USART2_IRQHandler
//  * @����   ����2�жϴ�����
//  * @����   ��
//  * @����ֵ ��
//***------------------------------------------------------*/
//void USART2_IRQHandler(void)
//{
//	/* ��ʼ�ٽ���Դ���ʣ���ֹ�ж� */
////	__disable_irq(); 
//
//    /* ���ô���2�жϴ����Ӻ��� */
//    Usart_2IrqHandlerUser();
////	RecRs485();
//    /* �����ٽ���Դ���ʣ������ж� */
////   __enable_irq();
//}

//uint32_t GetSysTime(void)
//{
//	return(SysTime);
//}

SZ_USART_DATA_TypeDef SZ_USART1_DATA, SZ_USART2_DATA;

/**-------------------------------------------------------
* @������ UsartIrqHandlerUser
* @����   ���õĴ����жϴ�����
* @����   SZ_USART �������ݽṹ��ָ��
* @����ֵ ��
***------------------------------------------------------*/
void UsartIrqHandlerUser(SZ_USART_DATA_TypeDef* SZ_USART)
{
    uint8_t uartRxData = 0;
    //	static uint32_t Time = 0;
    //	Time = GetSysTime();
    if(USART_GetITStatus(SZ_USART->uart, USART_IT_RXNE) != RESET) {
        /* save on rx buffer */
        while (SZ_USART->uart->SR & USART_FLAG_RXNE) {
            /* save character */
            uartRxData      = SZ_USART->uart->DR & 0xff;
            rx.buf[rx.ptr]  = uartRxData;
            //			printf("%x",uartRxData);
            rx.ptr++;
        }
        
        /* clear interrupt */
        USART_ClearITPendingBit(SZ_USART->uart, USART_IT_RXNE);
        
        /* ������� */
        if ((uartRxData == '\r') || (rx.ptr >= UART_RX_BUFFER_SIZE)) {
			rx.ok = 1;
            //			printf("�������");
        }
    }
    
    
    /* If overrun condition occurs, clear the ORE flag 
    and recover communication */
    if (USART_GetFlagStatus(SZ_USART->uart, USART_FLAG_ORE) != RESET)
    {
        (void)USART_ReceiveData(SZ_USART->uart);
    }
    
	if (USART_GetITStatus(SZ_USART->uart, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(SZ_USART->uart, USART_IT_TC);
	}
}

/**-------------------------------------------------------
* @������ UsartReadData
* @����   ���õĴ������ݶ�ȡ�Ӻ���
* @����1  SZ_USART �������ݽṹ��ָ��
* @����2  �������ݴ�ŵ�Buffer
* @����ֵ ��
***------------------------------------------------------*/
uint32_t UsartReadData(SZ_USART_DATA_TypeDef* SZ_USART,  uint8_t* Buffer)
{
    uint32_t uartRxLength = 0, i;
    
    /* if the next position is read index, discard this 'read char' */
    if (SZ_USART->uart_rx_length != 0)
    {
        /* ��ʼ�ٽ���Դ���ʣ���ֹ�ж� */
        __disable_irq(); 
        
        uartRxLength = SZ_USART->uart_rx_length;
        SZ_USART->uart_rx_length = 0;
        SZ_USART->uart_rx_done = 0;
        
        /* �����ٽ���Դ���ʣ������ж� */
        __enable_irq();
        
        for(i=0; i<uartRxLength; i++)
        {
            Buffer[i] = SZ_USART->uartRxBuffer[SZ_USART->uart_rx_read_point];
            SZ_USART->uart_rx_read_point++;
            
            if (SZ_USART->uart_rx_read_point >= UART_RX_BUFFER_SIZE)
            {
                SZ_USART->uart_rx_read_point = 0;
            }
        }
        
        Buffer[i] = '\0';
    }
    
    return uartRxLength;
}

/**-------------------------------------------------------
* @������ USART1_IRQHandler
* @����   ����1�жϴ�����
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
//void USART1_IRQHandler(void)
//{
//    /* ��ʼ�ٽ���Դ���ʣ���ֹ�ж� */
//	__disable_irq(); 
//
//    /* ����1�ж�ʱ��ָʾ��1״̬ȡ�� */
////	LED1OBB = !LED1OBB;
//
//    /* ���ù��õĴ����жϴ����Ӻ��� */
//    UsartIrqHandlerUser(&SZ_USART1_DATA);
//    
//    /* �����ٽ���Դ���ʣ������ж� */
//    __enable_irq();
//}

//
///*
//    �������´���,֧��printf����,����Ҫѡ��use MicroLIB	  
//*/
//#ifndef MicroLIB
//#pragma import(__use_no_semihosting)   //û��ʵ��fgetcʱ��Ҫ�����ò���          
///* ��׼����Ҫ��֧�ֺ��� ʹ��printf()���Դ�ӡ����Ҫʵ�ָú��� */               
//struct _Filet//__FILE 
//{ 
//	int handle; 
//    /* Whatever you require here. If the only file you are using is */    
//    /* standard output using printf() for debugging, no file handling */    
//    /* is required. */
//}; 
//
//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//_sys_exit(int x) 
//{ 
//	x = x; 
//} 
///* �ض���fputc���� ���ʹ��MicroLIBֻ��Ҫ�ض���fputc�������� */  
//int fputc(int ch, FILE *f)
//{
//    /* Place your implementation of fputc here */
//    /* Loop until the end of transmission */
//    while (USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_TC) == RESET)
//    {}
//
//    /* e.g. write a character to the USART */
//    USART_SendData(SZ_STM32_COM1, (uint8_t) ch);
//
//    return ch;
//}
///*
//����ֱ��ʹ��putchar
//����Ҫ�ٶ��� int putchar(int ch)����Ϊstdio.h�������¶���
// #define putchar(c) putc(c, stdout)
//*/
//
//int ferror(FILE *f) {  
//    /* Your implementation of ferror */  
//    return EOF;  
//} 
//#endif 
//
//FILE __stdin;
//
//int fgetc(FILE *fp)
//{
//	int ch = 0;
//	
//    while(USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_RXNE) == RESET)
//    {
//    }
//
//    ch = (int)SZ_STM32_COM1->DR & 0xFF;
//	
//    putchar(ch); //����
//	
//	return ch;
//}
#define USE_IAR
#ifdef  USE_IAR
#define PUTCHAR_PROTOTYPE int putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch,FILE *f)   
#endif

PUTCHAR_PROTOTYPE{
    //HAL_UART_Transmit(&huart1, (char *)(&(ch)), 1, 10);
    //return ch;
    int8_t retrys  = 5;
    
    while (USART_GetFlagStatus(SZ_STM32_COM1, USART_FLAG_TC) == RESET){
        if ( retrys -- < 0 )
            break;
        else
            BSP_OS_TimeDly(1);
    }
    
    /* e.g. write a character to the USART */
    USART_SendData(SZ_STM32_COM1, (uint8_t) ch);
    
    return ch;
} 

#ifdef USE_ADC
/**-------------------------------------------------------
* @������ ADC_GPIO_Configuration
* @����   ADCģ��ת����GPIO���ó�ʼ������
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void ADC_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOC clock */
    /* ʹ��GPIOCʱ��      */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	  //
    
    /* Configure PA.01 (ADC Channel12) as analog input -------------------------*/
    //PC0 ��Ϊģ��ͨ��11��������                         
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //�ܽ�0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   //����ģʽ
    GPIO_Init(GPIOC, &GPIO_InitStructure);          //GPIO��
}

/* STM32оƬADCת�����DR�Ĵ�������ַ */
#define DR_ADDRESS                  ((uint32_t)0x4001244C) 
/* ���ADCΪ12λģ��ת��������ı�����ֻ��ADCConvertedValue�ĵ�12λ��Ч */
__IO uint16_t ADCConvertedValue;     // 

/**-------------------------------------------------------
* @������ SZ_STM32_ADC_Configuration
* @����   ADCģ��ת���Ĳ������ú���
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void SZ_STM32_ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;        //ADC��ʼ���ṹ������
    DMA_InitTypeDef DMA_InitStructure;        //DMA��ʼ���ṹ������     
    
    ADC_GPIO_Configuration();
    
    /* Enable DMA1 clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 //ʹ��DMAʱ��
    
    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);		  //����DMA1�ĵ�һͨ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = DR_ADDRESS;		  //DMA��Ӧ���������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;   //�ڴ�洢����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
    DMA_InitStructure.DMA_BufferSize = 1;		   //DMA�����С��1��
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����һ�����ݺ��豸��ַ��ֹ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;	//�رս���һ�����ݺ�Ŀ���ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�����������ݿ��Ϊ16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA�������ݳߴ磬HalfWord����Ϊ16λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;   //ת��ģʽ��ѭ������ģʽ��
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//DMA���ȼ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		  //M2Mģʽ����
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��ADCʱ��
    
    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//������ת��ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;		  //����ɨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;   //��������ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ADC�ⲿ���أ��ر�״̬
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   //���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ
    ADC_InitStructure.ADC_NbrOfChannel = 1;	 //����ͨ������1��
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /* ADC1 regular channel10 configuration */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);
    //ADCͨ���飬 ��1��ͨ�� ����˳��1��ת��ʱ�� 
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);	  //ADC���ʹ��
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);  //����ADC1
    
    /* Enable ADC1 reset calibaration register */   
    ADC_ResetCalibration(ADC1);	  //����У׼
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ�����У׼���
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);		//��ʼУ׼
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));	   //�ȴ�У׼���
    /* Start ADC1 Software Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM����
}

/**-------------------------------------------------------
* @������ GetADCConvertedValue
* @����   ��ȡADCģ��ת���Ľ��
* @����   ��
* @����ֵ ADCת������ı�����ֻ�еĵ�12λ��Ч
***------------------------------------------------------*/
uint16_t GetADCConvertedValue(void)
{
    return ADCConvertedValue;
}
#endif

/**-------------------------------------------------------
* @������ Buffercmp
* @����   �Ƚ������������������ȵ������Ƿ���ͬ�ĺ���
* @����1  pBuffer1 �� pBuffer2 ��Ҫ�Ƚ�������������ʼ��ַ
* @����2  BufferLength �����ıȽϳ��ȣ��ֽ�Ϊ��λ
* @����ֵ PASSED: �������������ȵ�������ͬ
*         FAILED: �������������ȵ����ݲ���ͬ
***------------------------------------------------------*/
TestStatus Buffercmp(u8* pBuffer1, u8* pBuffer2, u16 BufferLength)
{
    while(BufferLength--)
    {
        if(*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }
        
        pBuffer1++;
        pBuffer2++;
    }
    
    return PASSED;
}

#ifdef USE_TIM5
/**-------------------------------------------------------
* @������ NVIC_TIM5Configuration
* @����   ����TIM5�ж�������������
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
static void NVIC_TIM5Configuration(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* Set the Vector Table base address at 0x08000000 */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
    
    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}

/**-------------------------------------------------------
* @������ NVIC_TIM5Configuration
* @����   ����TIM5����������ÿ��������ж�һ�� 
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void TIM5_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    /* TIM5 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    
    /* ---------------------------------------------------------------
    TIM4 Configuration: Output Compare Timing Mode:
    TIM2CLK = 36 MHz, Prescaler = 7200, TIM2 counter clock = 7.2 MHz
    --------------------------------------------------------------- */
    
    /* Time base configuration */
    //��������Զ�װ�صļ���ֵ�����ڼ����Ǵ�0��ʼ�ģ�����10000�κ�Ϊ9999
    TIM_TimeBaseStructure.TIM_Period = (10000 - 1);
    // �������Ԥ��Ƶϵ����������Ϊ0ʱ��ʾ����Ƶ����Ҫ��1
    TIM_TimeBaseStructure.TIM_Prescaler = (7200 - 1);
    // �߼�Ӧ�ñ��β��漰�������ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR,TIx)
    // ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    //���ϼ���
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //��ʼ����ʱ��5
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    
    /* Clear TIM5 update pending flag[���TIM5����жϱ�־] */
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    
    /* TIM IT enable */ //������ж�
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    
    /* TIM5 enable counter */
    TIM_Cmd(TIM5, ENABLE);  //������ʹ�ܣ���ʼ����
    
    /* �жϲ������� */
    NVIC_TIM5Configuration();
}


/**-------------------------------------------------------
* @������ TIM5_IRQHandler
* @����   TIM5�жϴ�������ÿ���ж�һ�� 
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void TIM5_IRQHandler(void)
{
    /* www.armjishu.com ARM������̳ */
    static u32 counter = 0;
    
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        /* LED1ָʾ��״̬ȡ�� */
        //        SZ_STM32_LEDToggle(LED1);
        
        /* armjishu.com������: ���������ж���ʹ��Printf, ��ʾ��ֻ����ʾ�� */
        printf("\n\rarmjishu.com��ʾ��: ���������ж���ʹ��Printf, ��ʾ��ֻ����ʾ��\n\r");
        printf("ARMJISHU.COM-->TIM5:%d\n\r", counter++);
    }
}
#endif

#ifdef USE_TIM3
/**-------------------------------------------------------
* @������ NVIC_TIM3Configuration
* @����   ����TIM3�ж�������������
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
static void NVIC_TIM3Configuration(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* Set the Vector Table base address at 0x08000000 */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
    
    /* Enable the TIM5 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}

/**-------------------------------------------------------
* @������ TIM3_PWM_Init
* @����   ����TIM3ΪPWMģʽ
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void TIM3_PWM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
  	const uint16_t Period_Val = 1020;
    __IO uint16_t CCR1_Val = Period_Val/2;
    __IO uint16_t CCR2_Val = Period_Val-20;
    uint16_t PrescalerValue = 0;
    
    /* System Clocks Configuration */ /* PCLK1 = HCLK/2 = 36MHz */
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    /* -----------------------------------------------------------------------
    TIM3 Configuration: generate 4 PWM signals with 4 different duty cycles:
    The TIM3CLK frequency is set to SystemCoreClock (Hz), to get TIM3 counter
    clock at 24 MHz the Prescaler is computed as following:
    - Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices
    
    The TIM3 is running at 36 KHz: TIM3 Frequency = TIM3 counter clock/(ARR + 1)
    = 24 MHz / 1000 = 24 KHz
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
    ----------------------------------------------------------------------- */
    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = Period_Val;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
    
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    
    /* TIM IT enable */
    TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
    
    //TIM_ARRPreloadConfig(TIM3, ENABLE);
    
    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);
    
    NVIC_TIM3Configuration();
}



/**-------------------------------------------------------
* @������ TIM3_IRQHandler
* @����   TIM3�жϴ�������PWM����ָʾ������
* @����   ��
* @����ֵ ��
***------------------------------------------------------*/
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    }
    
    if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    }
}
#endif

#ifdef USE_IWDG
/**-------------------------------------------------------
* @������ SZ_STM32_IWDGInit
* @����   STM32�������Ź���ʼ�����ú���
* @����   TimeoutFreMs  ���Ź���ʱ�����ٺ������һ��
* @����ֵ ��
* @ι����ʽ(Reload IWDG counter) IWDG_ReloadCounter(); 
***------------------------------------------------------*/
void SZ_STM32_IWDGInit(uint32_t TimeoutFreMs)/* 2 -  3276*/
{
    uint32_t ReloadValue;
    
    /* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency
    dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    
    /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    
    /* Set counter reload value to 349 *//* range from 0 to 4095 */
    //IWDG_SetReload(349);
    if(TimeoutFreMs < 2)
    {
        TimeoutFreMs = 2;
    }
    
    if(TimeoutFreMs > 3276)
    {
        TimeoutFreMs = 3276;
    }
    
    ReloadValue = ((TimeoutFreMs * 1250)/1000) - 1;
    IWDG_SetReload(ReloadValue);
    
    printf("\r\n IWDG Timeout frequency %dms, ReloadValue is set to %d.", TimeoutFreMs, ReloadValue);
    
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
    
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
    
}
#endif


#ifdef  USE_FULL_ASSERT
// ��Ҫ�ڹ�������Option(��ݼ�ALT+F7)C++����ҳ��define������"USE_FULL_ASSERT"
/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    
    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/******************* (C) COPYRIGHT 2013 www.armjishu.com *****END OF FILE****/
