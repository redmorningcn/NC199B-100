/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : i2c_clk.c
* Author             : ZHOUWEI
* Version            : V3.5.0
* Date               : 01/07/2014
* Description        : This file provides a set of functions needed to manage the
*                      communication between I2C peripheral and I2C DS3231SN.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "I2C_CLK.h"
#include "DS3231.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#define RCC_GPIO_I2C1      RCC_APB2Periph_GPIOE
#define SCL                GPIO_Pin_7
#define SDA                GPIO_Pin_8
#define GPIO_I2C1_PORT     GPIOE

#define FALSE              1
#define TRUE               0
#define Delay_Factor       5                                        		//��ʱ����

#define SCL_SET            GPIO_SetBits(GPIO_I2C1_PORT, SCL)				//SCL=1
#define SCL_RESET          GPIO_ResetBits(GPIO_I2C1_PORT, SCL)			    //SCL=0
   
#define SDA_SET            GPIO_SetBits(GPIO_I2C1_PORT, SDA)				//SDA=1
#define SDA_RESET          GPIO_ResetBits(GPIO_I2C1_PORT, SDA)			    //SDA=0
                            
#define SDA_read()         GPIO_ReadInputDataBit(GPIO_I2C1_PORT, SDA)	    //��ȡSDA״̬


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 			     //����ṹ��
  
  RCC_APB2PeriphClockCmd(RCC_GPIO_I2C1 , ENABLE);	     //ʹ��GPIOBʱ��

  GPIO_InitStructure.GPIO_Pin =  SCL;				 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	     //����SCL���ٶ�Ϊ2MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;       //����SCL��Ϊ��©�����
  GPIO_Init(GPIO_I2C1_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	     //����SDA���ٶ�Ϊ2MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	     //����SDA��Ϊ��©�����
  GPIO_Init(GPIO_I2C1_PORT, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delayms
* Description    : ��ʱ����i������ʱʱ��
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delayms(void)
{
	uint8_t i = Delay_Factor; 
	while(i) 
	{ 
		i--; 
	}  
}

/*******************************************************************************
* Function Name  : SCL_L
* Description    : ����SCLΪ�͵�ƽ����������ʱ
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void SCL_L(void)
{
  SCL_RESET;
  I2C_delayms();  
}

/*******************************************************************************
* Function Name  : SCL_H
* Description    : ����SCLΪ�ߵ�ƽ����������ʱ
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void SCL_H(void)
{
  SCL_SET;
  I2C_delayms();  
}

/*******************************************************************************
* Function Name  : SDA_L
* Description    : ����SDAΪ�͵�ƽ����������ʱ
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void SDA_L(void)
{
  SDA_RESET;
  I2C_delayms();  
}

/*******************************************************************************
* Function Name  : SDA_H
* Description    : ����SDAΪ�ߵ�ƽ����������ʱ
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void SDA_H(void)
{
  SDA_SET;
  I2C_delayms();  
}

/*******************************************************************************
* Function Name  : I2C_Start
* Description    : ����I2C�� SCLΪ1ʱ��SDA��1��0����
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Start(void)
{
	SCL_L();	                  //SCL = 0
	SDA_H();					  //SDA = 1
	SCL_H();					  //SCL = 1
	SDA_L();					  //SDA = 0
	SCL_L();					  //SCL = 0
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : ֹͣI2C�� SCLΪ1ʱ��SDA��0��1����
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{
	SCL_L();					  //SCL = 0
	SDA_L();					  //SDA = 0
	SCL_H();					  //SCL = 1
	SDA_H();					  //SDA = 1
	SCL_L();					  //SCL = 0
} 
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : ���豸����Ӧ���ź�
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{	
	SCL_L();					  //SCL = 0
	SDA_L();					  //SDA = 0    
	SCL_H();					  //SCL = 1
	SCL_L();					  //SCL = 0
	SDA_L();					  //SDA = 0
}   
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : ���豸�������һ��Ӧ���ź�
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{	
	SCL_L();				      //SCL = 0
	SDA_H();					  //SDA = 1 
	SCL_H();					  //SCL = 1   
	SCL_L();					  //SCL = 0
	SDA_L();					  //SDA = 0
} 
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : ���豸����Ӧ���ź�
* Input          : None
* Output         : None
* Return         : Ӧ���źţ�FALSE = 1��Ӧ��TURE = 0��Ӧ��
****************************************************************************** */
uint8_t I2C_WaitAck(void) 	                      //����Ϊ:=0��ACK,=1��ACK
{
	SCL_L();				                      //SCL = 0
	SDA_H();					                  //SDA = 1
	SCL_H();					                  //SCL = 1
	
	SCL_L();					                  //SCL = 0

	if(SDA_read())			                      //��SDA״̬�������Ƿ�ռ�ã�
	{
    	SCL_L();				                  //SCL = 0
		I2C_delayms();
//   	return FALSE;			                  //����FALSE = 1����Ӧ��     ?????   δŪ����
    	return TRUE;
	}
	else
	{
		SCL_L();					              //SCL = 0
		I2C_delayms();
		return TRUE;			                  //����TRUE = 0����Ӧ��
	}
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ����һ���ֽڵ������豸
* Input          : SendByte����Ҫ���͵�����
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(uint8_t SendByte) //���ݴӸ�λ����λ
{
	uint8_t i = 8;
	
	while(i--)
	{
		SCL_L();				 	             //SCL = 0
		if(SendByte&0x80)		 	             //�ȷ���λ
		{                         
	    	SDA_H();				 	         //SDA = 1
		}  
		else 
		{
	  		SDA_L();  			 	            //SDA = 0
		}
		SendByte<<=1;				            //����1λ
		I2C_delayms();
		SCL_H();					            //SCL = 1
		I2C_delayms();
	}
	SCL_L();					 	            //SCL = 0
}  
/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : ͨ��I2C��ȡ1���ֽڵ�����
* Input          : None
* Output         : None
* Return         : ��ȡ�������� 
****************************************************************************** */
uint8_t I2C_ReadByte(void) 		 	       		//���ݴӵ�λ����λ
{ 
	uint8_t i = 8;
	uint8_t ReceiveByte = 0;
	
	SDA_H();				    		        //SDA = 1
	while(i--)
	{
		ReceiveByte<<=1;      		            //�Ƚ��յ�λ 
		SCL_L();			    		        //SCL = 0
		SCL_H();			   		 	        //SCL = 1
		if(SDA_read())	    		            //��SDA״̬���Ƿ�Ϊ1��Ϊ1��ֵ��Ϊ0�������
		{
			ReceiveByte|=0x01;  	  	        //�����λ��ֵ
		}                         
	}
	SCL_L();				    		        //SCL = 0���ͷ�����
	return ReceiveByte;	    		       		//��������
} 
          
/*******************************************************************************
* Function Name  : Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data)
* Description    : ����һ���ֽ����ݵ�ָ���Ĵ��豸�ļĴ�����
* Input          : SlaveAddressΪ���豸��ַ��REG_AddressΪ�Ĵ�����ַ��REG_dataΪҪд������
* Output         : None
* Return         : None
****************************************************************************** */

void Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data)
{
	I2C_Start();				           //����I2C
	I2C_SendByte(SlaveAddress);            //д���豸��ַ(DS3231дģʽΪ0xD0) 
	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
	I2C_SendByte(REG_Address );            //д���豸�Ĵ�����ַ      
	while(I2C_WaitAck());				   //�ȴ����豸Ӧ��
	I2C_SendByte(REG_data);		           //д���ݵ��Ĵ���
	while(I2C_WaitAck());				   //�ȴ����豸Ӧ��
	I2C_Stop(); 				           //ֹͣI2C
}


/*******************************************************************************
* Function Name  : Single_Read(uint8_t SlaveAddress,uint8_t REG_Address)
* Description    : �Ӵ��豸��ȡ1���ֽڵ�����
* Input          : SlaveAddressΪ���豸��ַ��REG_AddressΪ�Ĵ�����ַ
* Output         : None
* Return         : None
****************************************************************************** */
uint8_t Single_Read(uint8_t SlaveAddress,uint8_t REG_Address)
{   
	uint8_t REG_data;     	
	I2C_Start();				        	//����I2C
	I2C_SendByte(SlaveAddress); 	        //д���豸��ַ(DS3231дģʽΪ0xD0) 
	while(I2C_WaitAck());				    //�ȴ����豸Ӧ��
	I2C_SendByte(REG_Address);              //д���豸�Ĵ�����ַ      
	while(I2C_WaitAck());				    //�ȴ����豸Ӧ��
	I2C_Start();					        //����I2C
	I2C_SendByte(SlaveAddress+1);	        //д���豸��ַ(DS3231дģʽΪ0xD1=0xD0+1)
	while(I2C_WaitAck());				    //�ȴ����豸Ӧ��
	
	REG_data= I2C_ReadByte();		        //������������
	I2C_NoAck();					        //�ȴ�����Ӧ��
	I2C_Stop();						        //ֹͣI2C
	return REG_data;				        //���ض�ȡ��������
}
 
/* END OF I2C_CLK.C */



