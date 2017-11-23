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
//函数名称:         GPIO_SetOrClearValue()
//功    能:         	对指定的端口写1或0
//入口参数:         GpioNum:   引脚编号，由端口*100+ 引脚编号 组成。
//                  		Val:   设定值，0 或
//作		者:		redmorningcn
//日		期:		2017-05-15
//出口参数:         无
//说明：            
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
//函数名称:         GPIO_PinselConfig()
//功    能:         	配置引脚功能
//入口参数:         GpioNum:   引脚编号，由端口*100+ 引脚编号 组成。
//                  		FuncNum:    引脚功能号，一般功能0~4
//作		者:		redmorningcn
//日		期:		2017-05-15
//出口参数:         无
//说明：            
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
//函数名称:        GPIO_ReadIoVal()
//功    能:         	读取引脚上值
//入口参数:         GpioNum:   引脚编号，由端口*100+ 引脚编号 组成。
//出口参数:		0 或1                 		
//作		者:		redmorningcn
//日		期:		2017-05-15
//出口参数:         无
//说明：            
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
