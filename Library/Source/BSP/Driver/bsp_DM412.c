/*******************************************************************************
 *   Revised:       $Date: 2017-11-12$
 *   Revision:      $
 *   Writer:	    redmorningcn.
 *
 *   Description:
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/


/*******************************************************************************
* INCLUDES
*/
#include    <includes.h>
#include    <bsp_gpio.h>
//#include "bsp_DM412.h"

/*******************************************************************************
 * CONSTANTS
 */
/***********************************************/
//IO

#define LED1_MOSI               402
#define LED1_SCK                403

#define LED2_MOSI               405
#define LED2_SCK                404

#define LED3_MOSI               406
#define LED3_SCK                213

#define LED_MAX                 3


/**-------------------------------------------------------
* @函数名  bsp_DM412Init
* @功能    初始化LED1的端口
author:    redmorningcn
data:      2017-11-13
***------------------------------------------------------*/
void bsp_DM412Init(void)
{
    GPIO_PinselConfig(LED1_MOSI,0);
    GPIO_PinselConfig(LED1_SCK,0);

    GPIO_PinselConfig(LED2_MOSI,0);
    GPIO_PinselConfig(LED2_SCK,0);

    GPIO_PinselConfig(LED3_MOSI,0);
    GPIO_PinselConfig(LED3_SCK,0);
}

//---------------------------------------------------------------------------------
//功    能:             SPI发送Lantch,将前面发送的数据装置到DM412
//入口参数:             无
//出口参数:             无
//说    明:				以模拟SPI总线方式发送数据
//----------------------------------------------------------------------------------
void SentLantch_LED(uint8 lednum)
{
	uint8_t i; 
    uint16  MosiBuf[LED_MAX] = {    LED1_MOSI,
                                    LED2_MOSI,
                                    LED3_MOSI
                                };
    
    uint16  SckBuf[LED_MAX] = {     LED1_SCK,
                                    LED2_SCK,
                                    LED3_SCK
                                };
	
	Delay_Nus(2);
    
    GPIO_SetOrClearValue(SckBuf[lednum],1);       	

    for (i=0;i<8;i++)
    {
        GPIO_SetOrClearValue(MosiBuf[lednum],0);       	
		Delay_Nus(10);
        
        GPIO_SetOrClearValue(MosiBuf[lednum],1);       	
		Delay_Nus(10);	
    }
    
    GPIO_SetOrClearValue(SckBuf[lednum],1);       	
	Delay_Nus(20);

    GPIO_SetOrClearValue(MosiBuf[lednum],0);
    GPIO_SetOrClearValue(SckBuf[lednum],0);       	
    Delay_Nus(2);
}

//---------------------------------------------------------------------------------
//功    能:  对led的SPI发送数据
//入口参数:             
//author:    redmorningcn
//data:      2017-11-13
//----------------------------------------------------------------------------------
void SentTwoByte_LED(uint8 lednum,uint16 ch)
{
    uint32  i,temp = 0;   
    uint8   typelen;
    
    uint16   MosiBuf[LED_MAX] = {   LED1_MOSI,
                                    LED2_MOSI,
                                    LED3_MOSI
                                };
    
    uint16   SckBuf[LED_MAX] = {    LED1_SCK,
                                    LED2_SCK,
                                    LED3_SCK
                                };

    if(lednum > LED_MAX - 1)
        return;
	
    typelen = sizeof(ch);           //字符长度
    GPIO_SetOrClearValue(SckBuf[lednum],0);
        
	Delay_Nus(2);
    for (i=0;i<8*typelen;i++)           
    {
        temp = 0; 
		temp=ch&(0x01<<(8*typelen - 1));

    	ch=ch<<1;
        if(temp)
        {
            GPIO_SetOrClearValue(MosiBuf[lednum],1);       	
        }
        else
        {
            GPIO_SetOrClearValue(MosiBuf[lednum],0);
        }
        
        Delay_Nus(10);	

        GPIO_SetOrClearValue(SckBuf[lednum],0);
        Delay_Nus(5);

        GPIO_SetOrClearValue(SckBuf[lednum],1);
        Delay_Nus(20);	 
    }   
    
    GPIO_SetOrClearValue(SckBuf[lednum],0);
	Delay_Nus(2);
}

//---------------------------------------------------------------------------------
//功    能:  对led进行颜色设置
//入口参数:  需要连续发送三次，再发送一次装载指令，才能将数据下载至DM412           
//author:    redmorningcn，颜色为RGB，
//data:      2017-11-13
//----------------------------------------------------------------------------------
void    SetLedColor(uint8 lenuum,uint16  red,uint16  green,uint16  blue)
{
    for(uint8 i=0;i<3;i++)
    {
       SentTwoByte_LED(lenuum,red);
       SentTwoByte_LED(lenuum,green);
       SentTwoByte_LED(lenuum,blue);
    }
    
    SentLantch_LED(lenuum);
}


//---------------------------------------------------------------------------------
//功    能:  对led进行颜色设置
//入口参数:  需要连续发送三次，再发送一次装载指令，才能将数据下载至DM412           
//author:    redmorningcn，颜色为RGB，
//data:      2017-11-13
//----------------------------------------------------------------------------------
void test_DM412_Led(void)
{
    static  uint16   red = 0x5555,green =0xAAAA,blue =0xffff;
    
    bsp_DM412Init();
    
    uint8  i = 10;
    while(i--)
    {

        red     +=  0x7ff;
        green   +=  0x7ff;
        blue    -=  0x7ff;
        
        SetLedColor(0,red,green,blue);
        SetLedColor(1,red,green,blue);
        SetLedColor(2,red,green,blue);
    }
}
