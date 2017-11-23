/*******************************************************************************
 *   Filename:       bsp_usart.h
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ��������ģ��
 *   Notes:
 *     				  E-mail:261313062@qq.com
 *
 *******************************************************************************/

#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "stm32f10x.h"

/***********************************************
* �������õ��ĸ����ھͶ����ĸ�����
*/
#define  Open_UART1
#define  Open_UART2
#define  Open_UART3


/***********************************************
* �������ⲿ��������
*/
void BSP_UartCfg(u8 port_nbr,u32  baud);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void USART_OPen_INT(USART_TypeDef* USARTx);
void USART_Close_INT(USART_TypeDef* USARTx);
void USART_Send_Char(USART_TypeDef* USARTx,unsigned char data);
void USART_WriteBytes(USART_TypeDef* USARTx,unsigned char *buffer,unsigned int len);

#endif


