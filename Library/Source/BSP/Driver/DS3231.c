
#include    "stm32f10x.h"
#include	"I2C_CLK.h"
#include	"DS3231.h"
#include    <stdio.h>
#include    "Display.h"
#include    "DELAY.h"
#include    "POWER_MACRO.h"
#include  <includes.h>


#define     DS3231_ADDR     		0xD0

#define     DS3231_SEC      		0x00
#define     DS3231_MIN      		0x01
#define     DS3231_HOUR     		0x02
#define     DS3231_DAT      		0x03	  //����
#define     DS3231_DAY     		    0x04
#define     DS3231_MONTH    		0x05
#define     DS3231_YEAR     		0x06

#define     DS3231_CONTROL      	0x0E
#define     DS3231_STATUS       	0x0F
#define     DS3231_AGING_OFFSET     0x10
#define     DS3231_TEMP_MSB     	0x11
#define     DS3231_TEMP_LSB     	0x12

#define     RCC_GPIO_RST_CLK        RCC_APB2Periph_GPIOB
#define     RST_CLK                 GPIO_Pin_0
#define     GPIO_RST_CLK_PORT       GPIOB


/***********************************************
* ������ OS�ӿ�
*/
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
    static  OS_SEM                   Bsp_Sem;    // �ź���
    #else
    static  OS_EVENT                *Bsp_Sem;    // �ź���
    #endif
#endif
    
    
#if (UCOS_EN     == DEF_ENABLED)
/*******************************************************************************
* ��    �ƣ� EEP_WaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ�����  0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static uint8_t EEP_WaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */

#if OS_VERSION > 30000U
    return BSP_OS_SemWait(&Bsp_Sem,0);           // �ȴ��ź���
#else
    uint8_t       err;
    OSSemPend(Bsp_Sem,0,&err);                   // �ȴ��ź���
    if ( err = OS_ERR_NONE )
      return TRUE;
    else
      return FALSE;
#endif
}

/*******************************************************************************
* ��    �ƣ� EEP_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static void EEP_SendEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&Bsp_Sem);                        // �����ź���
#else
    uint8_t       err;
    OSSemPost(Bsp_Sem);                              // �����ź���
#endif
}
#endif /* end of (UCOS_EN     == DEF_ENABLED)*/

/*******************************************************************************
* Function Name  : GPIO_RST_CLK_Configuration
* Description    : ʱ�Ӹ�λ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_RST_CLK_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_APB2PeriphClockCmd(RCC_GPIO_RST_CLK,ENABLE);   

  GPIO_InitStructure.GPIO_Pin =  RST_CLK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_RST_CLK_PORT, &GPIO_InitStructure);
}


//-------------------------------------------------------------------------------------------------------
//��������:         uint8 BCD2HEX(uint8 Bcd)  
//��    ��:         ��BCD���Ϊ��������
//��ڲ���:         BCD������  
//���ڲ���:         ����������
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8_t BCD2HEX(uint8_t Bcd)  
{
    uint8_t   Hex;
    Hex = (Bcd & 0x0f) + ((Bcd>>4) & 0x0f)*10;
    
    return Hex;
}

//-------------------------------------------------------------------------------------------------------
//��������:         uint8 BCD2HEX(uint8 Bcd)  
//��    ��:         ��BCD���Ϊ��������
//��ڲ���:         BCD������  
//���ڲ���:         ����������
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8_t HEX2BCD(uint8_t Hex)  
{
    uint8_t   Bcd;
    
    Bcd = Hex %10 + (((Hex/10)%10)<<4);
    
    return Bcd;
}

//-------------------------------------------------------------------------------------------------------
//��������:         ReadDS3231Byte()
//��    ��:         ��DS3231�ж���һ���ֽ�
//��ڲ���:         DS3231�豸�ţ� ��ȡ���ݵĵ�ַ
//���ڲ���:         ���ض�����ֵ
//˵����            
//--------------------------------------------------------------------------------------------------------
uint8_t ReadDS3231Byte(uint8_t addr)  
{
    uint8_t   SlaveAddr;
    uint8_t   Data;	
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //�ӻ���ַ
    
	Data = Single_Read(SlaveAddr , addr);
    
    return  Data;
}    

//-------------------------------------------------------------------------------------------------------
//��������:         void WriteDS3231Byte(uint8 addr,uint8   Data)  
//��    ��:         дһ���ֽڵ�DS3231��
//��ڲ���:         addr:DS3231��ַ�� Data:Ҫд�������
//���ڲ���:         ���ض�����ֵ
//--------------------------------------------------------------------------------------------------------
void WriteDS3231Byte(uint8_t Addr,uint8_t Data)  
{
    uint8_t   SlaveAddr;
    
    SlaveAddr = DS3231_ADDR & (~(0x01));            //���豸��ַ
   
    Single_Write(SlaveAddr , Addr , Data);
}      


//-------------------------------------------------------------------------------------------------------
//��������:         ReadTemp(void)  
//��    ��:         ��DS3231�ж����¶�
//��ڲ���:         ��
//���ڲ���:         ���ض������¶�
//˵����            
//--------------------------------------------------------------------------------------------------------
float ReadTemp(void)  
{
	float         Temp;
	uint8_t       TempMsb;
	uint8_t       TempLsb;
	
	WriteDS3231Byte(DS3231_CONTROL,0x20);
	
	TempMsb  = ReadDS3231Byte(DS3231_TEMP_MSB);      //����λ
	TempLsb  = ReadDS3231Byte(DS3231_TEMP_LSB);      //����λ
	
	//	Temp = TempMsb + TempLsb/64*0.25;
	//	Temp = Temp + 0.25;
	Temp = (float)(TempMsb) + (float)(TempLsb)/256;
	//	tem = TempMsb + TempLsb/64*0.25;
	//  printf("\r\n��ǰ�¶ȣ�tem = %d,Temp = %d,TempMsb = %d,TempLsb = %d,",tem,Temp,TempMsb,TempLsb);
	printf("\r\nʱ��оƬDS3231SN��ǰ�¶ȣ�Temp = %5.2f��,TempMsb = %d,TempLsb = %d",Temp,TempMsb,TempLsb);		
	return  Temp;
}

//-------------------------------------------------------------------------------------------------------
//��������:         stcTime     ReadTime(void)  
//��    ��:         дDS3231ʱ��
//��ڲ���:         дʱ��
//���ڲ���:         
//--------------------------------------------------------------------------------------------------------
void    WriteTime(TIME  sTime)  
{
	uint8_t   	Sec;
	uint8_t   	Min;
	uint8_t     Hour;	
	uint8_t     Day;
	uint8_t     Month;
	uint8_t     Year;
	
#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif
	Sec     =   HEX2BCD(sTime.Sec);		
	Min     =   HEX2BCD(sTime.Min);		
	Hour    =   HEX2BCD(sTime.Hour);	
	Day     =   HEX2BCD(sTime.Day);		
	Month   =   HEX2BCD(sTime.Month);		
	Year    =   HEX2BCD(sTime.Year);
	
	WriteDS3231Byte(DS3231_SEC,Sec);     
	WriteDS3231Byte(DS3231_MIN,Min);        
	WriteDS3231Byte(DS3231_HOUR,Hour);
	WriteDS3231Byte(DS3231_DAY,Day);    
	WriteDS3231Byte(DS3231_MONTH,Month);  
	WriteDS3231Byte(DS3231_YEAR,Year);     
#if (UCOS_EN     == DEF_ENABLED)
    EEP_SendEvent();                            // �ͷ��ź���
#endif     
}

//-------------------------------------------------------------------------------------------------------
//��������:         void    SetTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec)   
//��    ��:         ����ʱ��
//��ڲ���:         
//���ڲ���:         
//˵����            
//--------------------------------------------------------------------------------------------------------
void SetTime(uint8_t Year,uint8_t Month,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec)  
{
  TIME  sTime;

  sTime.Sec   = Sec;  				
  sTime.Min   = Min;  				
  sTime.Hour  = Hour; 			
  sTime.Day  = Day; 			
  sTime.Month = Month;    			
  sTime.Year  = Year;	
  
  WriteTime(sTime);	
}

//-------------------------------------------------------------------------------------------------------
//��������:         void    InitDS3231(void)  
//��    ��:         ��ʼ��DS3231�豸ģʽ
//��ڲ���:         ��
//���ڲ���:         ��
//--------------------------------------------------------------------------------------------------------
void    InitDS3231(void)  
{
    
	WriteDS3231Byte(DS3231_CONTROL,0);     
	
	WriteDS3231Byte(DS3231_STATUS,0);

    
    /***********************************************
    * ������ OS�ӿ�
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_Sem,1, "Bsp EepSem");      // �����ź���
#else
    Bsp_Sem     = OSSemCreate(1);                    // �����ź���
#endif
#endif
      	
}


void GetTime(TIME *t)
{
	uint8_t   	Sec;
    uint8_t   	Min;
    uint8_t     Hour;
    uint8_t     Day;
    uint8_t     Month;
    uint8_t     Year;
    	
#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif
    Sec     = ReadDS3231Byte(DS3231_SEC); 
    Min     = ReadDS3231Byte(DS3231_MIN);  
    Hour    = ReadDS3231Byte(DS3231_HOUR);  
    Day     = ReadDS3231Byte(DS3231_DAY); 
    Month   = ReadDS3231Byte(DS3231_MONTH); 
    Year    = ReadDS3231Byte(DS3231_YEAR);
    	
    t->Sec   = BCD2HEX(Sec); 				
    t->Min   = BCD2HEX(Min); 				
    t->Hour  = BCD2HEX(Hour);    			
    t->Day   = BCD2HEX(Day);    			
    t->Month = BCD2HEX(Month);   			
    t->Year  = BCD2HEX(Year);
#if (UCOS_EN     == DEF_ENABLED)
    EEP_SendEvent();                            // �ͷ��ź���
#endif
}


//-------------------------------------------------------------------------------------------------------
//��������:         void   DisplayTime()   
//��    ��:         ��ʾʱ��
//��ڲ���:         ��
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
void DisplayTime(void)
{   
#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif

	GetTime((TIME *)&recordsfr.Time[0]);
	uartprintf("\r\n ��ǰʱ��Ϊ��20%d-%d-%d  %d:%u:%d", recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
	                                                recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);				
    
#if (UCOS_EN     == DEF_ENABLED)
    EEP_SendEvent();                            // �ͷ��ź���
#endif
}
