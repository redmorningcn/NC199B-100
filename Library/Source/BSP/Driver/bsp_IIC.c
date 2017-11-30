/*******************************************************************************
 *   Filename:      bsp_IIC.c
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
#include <includes.h>
#include <bsp_IIC.h>


#define     SCL     	407		//PE7
#define     SDA         408		//PE8


#ifndef FALSE
#define FALSE              1
#endif

#ifndef TRUE
#define TRUE               0
#endif

#define Delay_Factor       5                                        		//延时因子



/*******************************************************************************
* Function Name  : I2C_delay
* Description    : 延时程序，i决定延时时间
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{
	uint8_t i = Delay_Factor; 
	while(i) 
	{ 
		i--; 
	} 
//    #if DEBUG_MODEL_DISENABLE > 0 
//        WDT_Feed();                                               /* Determine CPU capacity                               */
//    #endif
}

//------------------------------------------------------------------------
//  名  称 ：		GPIO_Fram_Init
//  功  能 :    		初始化铁电的 IO口，为通用IO 口
// 入口参数：	无
// 出口参数：	无
//------------------------------------------------------------------------
void	GPIO_Fram_Init(void)
{
	GPIO_PinselConfig(SCL,0);
	GPIO_PinselConfig(SCL,0);
	GPIO_PinselConfig(SDA,0);
	GPIO_PinselConfig(SDA,0);
}


//------------------------------------------------------------------------
//  名  称 ：void WriteSCL( uint8 temp )
//  功  能 ：设置 SCL
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteSCL(uint8 temp)
{	
	if(temp)		
	{
		GPIO_SetOrClearValue(SCL,1);
	}
	else
	{
		GPIO_SetOrClearValue(SCL,0);		
	}	

	I2C_delay();
}

//------------------------------------------------------------------------
//  名  称 ：uint8 ReadSCL(void)
//  功  能 ：读取 SCL 的值
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
uint8 ReadSCL(void)
{
   uint8    temp;
    temp = 	GPIO_ReadIoVal(SCL);
    I2C_delay();
    return temp;
}

//------------------------------------------------------------------------
//  名  称 ：void WriteSDA( uint8 temp )
//  功  能 ：设置 SDA
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteSDA(uint8 temp)
{
	if(temp)		
	{
		GPIO_SetOrClearValue(SDA,1);
	}
	else
	{
		GPIO_SetOrClearValue(SDA,0);		
	}	
	I2C_delay();
}

//------------------------------------------------------------------------
//  名  称 ：uint8 ReadSDA(void)
//  功  能 ：读取 SDA 的值
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
uint8 ReadSDA(void)
{
    uint8 temp;
    
    temp = 	GPIO_ReadIoVal(SDA);
    I2C_delay();

    return temp;
}

//------------------------------------------------------------
//函数名称:         StartI2C()
//功    能:         I2C总线开始条件
//入口参数:         无
//出口参数:         无
//说明：            I2C总线的开始条件：
//                  SCL高电平时SDA的下降沿
//------------------------------------------------------------
void StartI2C(void)
{
    WriteSCL(0);                                // SCL=0;
    WriteSDA(1);                                //SDA=1;
    WriteSCL(1);                                //SCL=1;
    WriteSDA(0);                                //SDA=0;
    WriteSCL(0);                                // SCL=0;
}

//---------------------------------------------------------------
//函数名称:         StopI2C()
//功    能:         I2C总线结束条件
//入口参数:         无
//出口参数:         无
//说明：            I2C总线结束条件：SCL为高电平时SDA的上升沿
//--------------------------------------------------------------------
void StopI2C(void)
{
    WriteSCL(0);                                // SCL=0;
    WriteSDA(0);                                //SDA=0;
    WriteSCL(1);                                //SCL=1;
    WriteSDA(1);                                //SDA=1;
    WriteSCL(0);                                //SCL=0;
}

//-------------------------------------------------------------------------------
//函数名称:         MasterAckI2C()
//功    能:         主机接收应答，在接收到1字节数据后，返回应答，告诉从机接收完成
//入口参数:         无
//出口参数:         无
//说明：            I2C总线应答：读完一个字节后将SDA线拉低，即应答0
//---------------------------------------------------------------------------------
void MasterAckI2C(void)
{
    WriteSCL(0);                                // SCL=0;
    WriteSDA(0);                               //SDA=0;
    WriteSCL(1);                                //SCL=1;
    WriteSCL(0);                                 //SCL=0;
    WriteSDA(0);                                //SDA=0;
}

//-----------------------------------------------------------------------------------
//函数名称:         MasterNoAckI2C()
//功    能:         主机接收应答，告诉通信从机接收完成
//入口参数:         无
//出口参数:         无
//说明：            I2C总线应答：
//                  读完所要求的字节数据后将SDA线拉高，为结束IIC总线操作做准备
//-------------------------------------------------------------------------------------
void MasterNoAckI2C(void)
{
    WriteSCL(0);                                    // SCL=0;
    WriteSDA(1);                                    //SDA=1;
    WriteSCL(1);                                    //SCL=1;
    WriteSCL(0);                                    //SCL=0;
    WriteSDA(0);                                    //SDA=0;
}

//------------------------------------------------------------------------------------
//函数名称:         CheckSlaveAckI2C()
//功    能:         读I2C从机应答
//入口参数:         无
//出口参数:         ackflag
//                  如果从机产生应答，则返回0，
//                  从机没有产生应答,否则返回1
//说明：            检查I2C器件(从机)应答情况,应答返回1,否则为零
//these waw a bug befor 2008/05/17.
//------------------------------------------------------------------------------------
uint8 CheckSlaveAckI2C(void)
{
    uint8 ackflag;                                  //清除应答标志;
    
    ackflag = ReadSDA();
    WriteSCL(1);                                    //SCL=1;
    WriteSCL(0);                                    //SCL=0;
    
    return ackflag;
}

//------------------------------------------------------------------------------------
//函数名称:         WriteByteWithI2C()
//功    能:         	主机对I2C总线写1操作
//入口参数:         Data:要写出的数据
//出口参数:         无
//说明：            	写一字节数据Data
//------------------------------------------------------------------------------------
void WriteByteWithI2C(uint8 Data)  
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        if(Data & 0x80)                             //传输位从高位到低位,先传最高位 
        {
            WriteSDA(1);                            //SDA=1;
            WriteSCL(1);                            //SCL=1;
        }
        else
        {  
            WriteSDA(0);                            //SDA=0;
            WriteSCL(1);                            //SCL=1;
        }   
        WriteSCL(0);                                //SCL=0;
        WriteSDA(0);                                //SDA=0;
        Data <<= 1;                                 //数据左移             
    }
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadByteWithI2C()
//功    能:         主机对I2C总线写1操作
//入口参数:         无
//出口参数:         无
//说明：            读一字节数据：返回值即读的数据
//--------------------------------------------------------------------------------------------------------
uint8 ReadByteWithI2C(void)            
{
    uint8 readdata=0;                           // Read_Data为读入的数据，0
    uint8 j;
    
    for (j=0;j<8;j++)
    {
        readdata <<=1;
								  //数据读出，若是0则移位；若是1则与0x01或
        if(ReadSDA())
        {
            readdata =(readdata | 0x01);               
        }
        WriteSCL(1);                              //SCL=1;
        WriteSCL(0);                              //SCL=0;
    }
    
    return   readdata ;                         //返回读出的字节
}


