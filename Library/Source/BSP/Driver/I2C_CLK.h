/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : i2c_clk.h
* Author             : ZHOUWEI
* Version            : V3.5.0
* Date               : 01/02/2014
* Description        : Header for i2c_clk.c module
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __I2C_CLK_H
#define __I2C_CLK_H


extern void I2C_GPIO_Config(void);
extern void I2C_delay(void);
extern void I2C_Start(void);
extern void I2C_Stop(void);
extern void I2C_Ack(void);
extern void I2C_NoAck(void);
extern uint8_t I2C_WaitAck(void);
extern void I2C_SendByte(uint8_t SendByte);
extern uint8_t I2C_ReadByte(void);
extern void Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
//extern uint8_t Write(uint8_t SlaveAddress,uint8_t REG_Address);
extern uint8_t Single_Read(uint8_t SlaveAddress,uint8_t REG_Address);

/*******************************************************************************
* Function Name  : I2C_GPIO_Config
* Description    : Configration Simulation IIC GPIO
* Input          : None 
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_GPIO_Config(void);

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : ��ʱ����i������ʱʱ��
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_delay(void);

/*******************************************************************************
* Function Name  : void delay100us(void)
* Description    : ��ʱ600us
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void delay100us(void);

/*******************************************************************************
* Function Name  : I2C_Start
* Description    : ����I2C�� SCLΪ1ʱ��SDA��1��0����
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_Start(void);

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : ֹͣI2C�� SCLΪ1ʱ��SDA��0��1����
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_Stop(void);

/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : ���豸����Ӧ���ź�
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_Ack(void);
 
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : ���豸�������һ��Ӧ���ź�
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_NoAck(void);

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : ���豸����Ӧ���ź�
* Input          : None
* Output         : None
* Return         : Ӧ���źţ�FALSE = 1��Ӧ��TURE = 0��Ӧ��
****************************************************************************** */
extern uint8_t I2C_WaitAck(void);

/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ����һ���ֽڵ������豸
* Input          : SendByte����Ҫ���͵�����
* Output         : None
* Return         : None
****************************************************************************** */
extern void I2C_SendByte(uint8_t SendByte);

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : ͨ��I2C��ȡ1���ֽڵ�����
* Input          : None
* Output         : None
* Return         : ��ȡ�������� 
****************************************************************************** */
extern uint8_t I2C_ReadByte(void);
          
/*******************************************************************************
* Function Name  : Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data)
* Description    : ����һ���ֽ����ݵ�ָ���Ĵ��豸�ļĴ�����
* Input          : SlaveAddressΪ���豸��ַ��REG_AddressΪ�Ĵ�����ַ��REG_dataΪҪд������
* Output         : None
* Return         : None
****************************************************************************** */
extern void Single_Write(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);


/*******************************************************************************
* Function Name  : Single_Read(uint8_t SlaveAddress,uint8_t REG_Address)
* Description    : �Ӵ��豸��ȡ1���ֽڵ�����
* Input          : SlaveAddressΪ���豸��ַ��REG_AddressΪ�Ĵ�����ַ
* Output         : None
* Return         : None
****************************************************************************** */
extern uint8_t Single_Read(uint8_t SlaveAddress,uint8_t REG_Address);


#endif
/* end of i2c_clk.h */


