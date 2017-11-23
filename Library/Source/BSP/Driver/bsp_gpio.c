/*******************************************************************************
 *   Filename:      bsp_gpio.c
 *   Revised:       $Date: 2017-05-15$
 *   Revision:      $
 *   Writer:	    redmorningcn.
 *
 *   Description:
 *   Notes:
 *     			E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/
#include 		<includes.h>  
#include 		"string.h"
#include    	"stdio.h" 
#include   	    "stdarg.h"

//-------------------------------------------------------------------------------------------------------
//��������:         GPIO_SetOrClearValue()
//��    ��:         	��ָ���Ķ˿�д1��0
//��ڲ���:         GpioNum:   ���ű�ţ��ɶ˿�*100+ ���ű�� ��ɡ�
//                  		Val:   �趨ֵ��0 ��
//��		��:		redmorningcn
//��		��:		2017-05-15
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
void	GPIO_SetOrClearValue(uint32_t GpioNum,uint8_t Val)
{
	GPIO_TypeDef 	*PortBuf[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE};
    GPIO_InitTypeDef  GPIO_InitStructure;

	uint8		PortNum;
	uint8		IoNum;

	PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;
    
    GPIO_InitStructure.GPIO_Pin     =  0x01<<IoNum;;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
    
    GPIO_Init(PortBuf[PortNum], &GPIO_InitStructure);  //io output 
    
    //GPIO_WriteBit(PortBuf[PortNum],IoNum,(BitAction)Val);
    GPIO_WriteBit(PortBuf[PortNum],0x01<<IoNum,(BitAction)Val);
}

//-------------------------------------------------------------------------------------------------------
//��������:         GPIO_PinselConfig()
//��    ��:         	�������Ź���
//��ڲ���:         GpioNum:   ���ű�ţ��ɶ˿�*100+ ���ű�� ��ɡ�
//                  		FuncNum:    ���Ź��ܺţ�һ�㹦��0~4
//��		��:		redmorningcn
//��		��:		2017-05-15
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
void	GPIO_PinselConfig(uint32_t GpioNum,uint8_t FuncNum)
{
	GPIO_InitTypeDef	gpio_init;

	GPIO_TypeDef 	*PortBuf[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE};
    uint32          RCC_APPBuf[] = {RCC_APB2Periph_GPIOA,
                                    RCC_APB2Periph_GPIOB,
                                    RCC_APB2Periph_GPIOC,
                                    RCC_APB2Periph_GPIOD,
                                    RCC_APB2Periph_GPIOE};
	uint8		PortNum;
	uint8		IoNum;

        
	PortNum 	= GpioNum/100;
	IoNum	    = GpioNum%100;

    RCC_APB2PeriphClockCmd(RCC_APPBuf[PortNum] , ENABLE);
    
    gpio_init.GPIO_Pin   = 0x01<<IoNum;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    //gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
        
    GPIO_Init(PortBuf[PortNum], &gpio_init);

}

//-------------------------------------------------------------------------------------------------------
//��������:        GPIO_ReadIoVal()
//��    ��:         	��ȡ������ֵ
//��ڲ���:         GpioNum:   ���ű�ţ��ɶ˿�*100+ ���ű�� ��ɡ�
//���ڲ���:		0 ��1                 		
//��		��:		redmorningcn
//��		��:		2017-05-15
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8	GPIO_ReadIoVal(uint32_t GpioNum)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_TypeDef 	*PortBuf[] = {GPIOA,GPIOB,GPIOC,GPIOD,GPIOE};
	uint8		    PortNum;
	uint8		    IoNum;
	uint32 		    PortVal;	

	PortNum = GpioNum/100;
	IoNum	= GpioNum%100;
    
    GPIO_InitStructure.GPIO_Pin     =  0x01<<IoNum;;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
    
    GPIO_Init(PortBuf[PortNum], &GPIO_InitStructure);   //io input
    
    PortVal = GPIO_ReadInputDataBit(PortBuf[PortNum],0x01<<IoNum);
//
//	if(PortVal & 1ul<<IoNum)
//	{
//		return	1;
//	}
    
    if(PortVal)
       return	1;
    
	return	0;
}
