//#include    "config.h"
//#include    "string.h"
//#include    "CpuDrive.h"
//#include    "HardDrive.h"

#include    <includes.h>
#include    "bsp_DS3231.h"
#include    "bsp_IIC.h"


#define     DS3231_ADDR     		0xd0
#define     DS3231_SEC      		0x00
#define     DS3231_MIN      		0x01
#define     DS3231_HOUR     		0x02
#define     DS3231_DAY      		0x03
#define     DS3231_DATA     		0x04
#define     DS3231_MONTH    		0x05
#define     DS3231_YEAR     		0x06

#define     DS3231_CONTROL      	0x0E
#define     DS3231_STATUS       	0x0F
#define     DS3231_AGING_OFFSET     0x10
#define     DS3231_TEMP_MSB     	0x11
#define     DS3231_TEMP_LSB     	0x12

//-------------------------------------------------------------------------------------------------------
//函数名称:         uint8 BCD2HEX(uint8 Bcd)  
//功    能:         将BCD码变为二进制码
//入口参数:         BCD码数据  
//出口参数:         二进行制数
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 BCD2HEX(uint8 Bcd)  
{
    uint8   Hex;
    Hex = (Bcd & 0x0f) + ((Bcd>>4) & 0x0f)*10;
    
    return Hex;
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         uint8 BCD2HEX(uint8 Bcd)  
//功    能:         将BCD码变为二进制码
//入口参数:         BCD码数据  
//出口参数:         二进行制数
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 HEX2BCD(uint8 Hex)  
{
    uint8   Bcd;
    
    Bcd = Hex %10 + (((Hex/10)%10)<<4);
    
    return Bcd;
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadDS3231Byte()
//功    能:         从DS3231中读出一个字节
//入口参数:         DS3231设备号， 读取数据的地址
//出口参数:         返回读出的值
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 ReadDS3231Byte(uint8 addr)  
{
    uint8   SlaveAddr;
    uint8   RecData;	
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //从机地址
    
    StartI2C();                                     //启动IIC总线
   
    WriteByteWithI2C(SlaveAddr);                    //写DS3231读取
    
    CheckSlaveAckI2C();                             //读从机应答信号
    
    WriteByteWithI2C(addr);                         //写DS3231读取

    CheckSlaveAckI2C();                             //读从机应答信号

    StartI2C();                                     //启动IIC总线
    
    SlaveAddr = DS3231_ADDR | 0x01;             	//从机地址

    WriteByteWithI2C(SlaveAddr);                    //写DS3231设备地址

    CheckSlaveAckI2C();                             //读从机应答信号

    RecData = ReadByteWithI2C();                	//接收到的数据
    
    MasterNoAckI2C();                               //主机应答从机
    
    StopI2C();                          			//结束IIC总线
    
    return  RecData;
}    

//-------------------------------------------------------------------------------------------------------
//函数名称:         void WriteDS3231Byte(uint8 addr,uint8   Data)  
//功    能:         写一个字节到DS3231中
//入口参数:         addr:DS3231地址， Data:要写入的数据
//出口参数:         返回读出的值
//--------------------------------------------------------------------------------------------------------
void WriteDS3231Byte(uint8 Addr,uint8   Data)  
{
    uint8   SlaveAddr;
    
    StartI2C();                                     //启动IIC总线
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //从机地址
   
    WriteByteWithI2C(SlaveAddr);                    //写DS3231读取
    
    CheckSlaveAckI2C();                             //读从机应答信号
    
    WriteByteWithI2C(Addr);                         //写DS3231操作地址
    
    CheckSlaveAckI2C();                             //读从机应答信号
    
    WriteByteWithI2C(Data);                         //写DS3231数据
    
    CheckSlaveAckI2C();                             //读从机应答信号
    
    StopI2C();                          			//结束IIC总线
}      

//-------------------------------------------------------------------------------------------------------
//函数名称:         stcTime     ReadTime(void)  
//功    能:         从DS3231中读出时间
//入口参数:         顾虑读出来的时间数据
//出口参数:         返回读出的时间
//--------------------------------------------------------------------------------------------------------
uint8	ReadTime(stcTime  *  sTime)  
{
    uint8   	Sec;
    uint8   	Min;
    uint8       Hour;	
    uint8       Data;
    uint8       Month;
    uint8       Year;
    	
    Sec     = ReadDS3231Byte(DS3231_SEC); 
    Min     = ReadDS3231Byte(DS3231_MIN);  
    Hour    = ReadDS3231Byte(DS3231_HOUR);  
    Data    = ReadDS3231Byte(DS3231_DATA); 
    Month   = ReadDS3231Byte(DS3231_MONTH); 
    Year    = ReadDS3231Byte(DS3231_YEAR);    
    	

    sTime->Sec   = BCD2HEX(Sec); 				
    sTime->Min   = BCD2HEX(Min); 				
    sTime->Hour  = BCD2HEX(Hour);    			
    sTime->Date  = BCD2HEX(Data);    			
    sTime->Month = BCD2HEX(Month);   			
    sTime->Year  = BCD2HEX(Year);		
        		
    return  1;
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         stcTime     ReadTime(void)  
//功    能:         从DS3231中读出时间
//入口参数:         无
//出口参数:         返回读出的时间
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8   ReadTemp(void)  
{
    int16       Temp;
    int8        TempMsb;
    uint8       TempLsb;
    	
    TempMsb     = ReadDS3231Byte(DS3231_TEMP_MSB);      //读高位
    TempLsb     = ReadDS3231Byte(DS3231_TEMP_LSB);      //读低位
    
    Temp = TempMsb * 4 + (TempLsb>>6);

    //printfcom0("\r\nTemp = %d,TempMsb = %d,TempLsb = %d,",Temp,TempMsb,TempLsb);
    		
    return  Temp;
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         stcTime     ReadTime(void)  
//功    能:         写DS3231时间
//入口参数:         写时间
//出口参数:         
//--------------------------------------------------------------------------------------------------------
void    WriteTime(stcTime  sTime)  
{
    uint8   	Sec;
    uint8   	Min;
    uint8       Hour;	
    uint8       Data;
    uint8       Month;
    uint8       Year;
    	
    Sec     = HEX2BCD(sTime.Sec);		
    Min     = HEX2BCD(sTime.Min);		
    Hour    = HEX2BCD(sTime.Hour);	
    Data    = HEX2BCD(sTime.Date);		
    Month   = HEX2BCD(sTime.Month);		
    Year    = HEX2BCD(sTime.Year);				

    WriteDS3231Byte(DS3231_SEC,Sec);     
    WriteDS3231Byte(DS3231_MIN,Min);        
    WriteDS3231Byte(DS3231_HOUR,Hour);
    WriteDS3231Byte(DS3231_DATA,Data);    
    WriteDS3231Byte(DS3231_MONTH,Month);  
    WriteDS3231Byte(DS3231_YEAR,Year);         
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         void    SetTime(uint8 Year,uint8 Month,uint8 Data,uint8 Hour,uint8 Min,uint8 Sec)   
//功    能:         设置时间
//入口参数:         
//出口参数:         
//说明：            
//--------------------------------------------------------------------------------------------------------
void    SetTime(uint8 Year,uint8 Month,uint8 Date,uint8 Hour,uint8 Min,uint8 Sec)  
{
    stcTime  sTime;

    sTime.Sec   = Sec;  				
    sTime.Min   = Min;  				
    sTime.Hour  = Hour; 			
    sTime.Date  = Date; 			
    sTime.Month = Month;    			
    sTime.Year  = Year;	
    
    WriteTime(sTime);	
}


//-------------------------------------------------------------------------------------------------------
//函数名称:         void    InitDS3231(void)  
//功    能:         初始化DS3231设备模式
//入口参数:         无
//出口参数:         无
//--------------------------------------------------------------------------------------------------------
void    InitDS3231(void)  
{
//    InitI2CIO();
    
    GPIO_Fram_Init();       //初始化IO
    
    WriteDS3231Byte(DS3231_CONTROL,0);     
    
    WriteDS3231Byte(DS3231_STATUS,0);       	
}

