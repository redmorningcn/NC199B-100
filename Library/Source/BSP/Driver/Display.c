#include "stm32f10x.h"
#include <stdio.h>
#include "SZ_STM32F107VC_LIB.h"
#include "DS3231.h"
#include "I2C_CLK.h"
#include "Display.h"
#include "delay.h"
#include "POWER_MACRO.h"
#include "SPI_CS5463_AC.h"
 
#include 	"Display.h"  
#include 	"string.h"

#include <includes.h>

//����˿ڼ���������---------------------------------------------------------------
//#define     DDR_7219    DDRB 
//#define     PINT_7219   PINB
//#define     PORT_7219   PORTB  
//#define     CS_7219     4                       //��������

//max7219�Ĵ�����ַ����-------------------------------------------------------------
#define     NoOp        0x00                    // �ղ����Ĵ���
#define     Digit0      0x01                    // �����1�Ĵ���
#define     Digit1      0x02                    // �����2�Ĵ���
#define     Digit2      0x03                    // �����3�Ĵ���
#define     Digit3      0x04                    // �����4�Ĵ���
#define     Digit4      0x05                    // �����5�Ĵ���
#define     Digit5      0x06                    // �����6�Ĵ���
#define     Digit6      0x07                    // �����7�Ĵ���
#define     Digit7      0x08                    // �����8�Ĵ���
/**/
#define     DecodeMode  0x09                    // ����ģʽ�Ĵ���
#define     Intensity   0x0a                    // ���ȼĴ���
#define     ScanLimit   0x0b                    // ɨ��λ���Ĵ���
#define     ShutDown    0x0c                    // �͹���ģʽ�Ĵ���
#define     DisplayTest 0x0f                    // ��ʾ���ԼĴ���
//max7219���ƼĴ�������
#define     ShutdownMode    0x00                // �͹��ķ�ʽ
#define     NormalOperation 0x01                // ����������ʽ
//#define     DecodeDigit     0xff                // �������ã�8λ��ΪBCD��;��8����������   
#define     NoDecodeDigit   0x00                // �������ã���8������������   
#define     ScanDigit       0x07                // ɨ��λ�����ã���ʾ8λ�����
//#define   ScanDigit 0x03                      // ɨ��λ�����ã���ʾ8λ�����
#define   	IntensityGrade  0x2a                // ���ȼ�������
//#define     IntensityGrade  0x03              // ���ȼ�������
#define     TestMode    0x01                    // ��ʾ����ģʽ 
#define     TextEnd     0x00                    // ��ʾ���Խ������ָ���������ģʽ


#define RCC_LED_DIS      		RCC_APB2Periph_GPIOD
#define MOSI_DIS                GPIO_Pin_8
#define SCK_DIS                 GPIO_Pin_10
#define CS_DIS                  GPIO_Pin_9
#define LED_DIS_PORT            GPIOD


/**-------------------------------------------------------
  * @������ LED_DIS_Config
  * @����   ��ʼ��LED�Ķ˿�
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
void LED_DIS_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOB, GPIOC and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_LED_DIS , ENABLE);
    
  /* LEDs pins configuration */
  GPIO_InitStructure.GPIO_Pin = MOSI_DIS | SCK_DIS | CS_DIS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_DIS_PORT, &GPIO_InitStructure);
}


//------------------------------------------------------------------------
//  ��  �� ��void WriteCS_DIS( uint8_t temp )
//  ��  �� ������ CS_DIS
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteCS_DIS(uint8_t temp)
{
	if( temp )						// �˿�����ߵ�ƽ
	{
		GPIO_SetBits(LED_DIS_PORT, CS_DIS);				//PD2 ��1
	}
	else
	{
		GPIO_ResetBits(LED_DIS_PORT, CS_DIS);			//PD2 ��0
	}
}
//------------------------------------------------------------------------
//  ��  �� ��void WriteSCK1( uint8_t temp )
//  ��  �� ������ SCK0
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteSCK1(uint8_t temp)
{
	if( temp )						// �˿�����ߵ�ƽ
	{
		GPIO_SetBits(LED_DIS_PORT, SCK_DIS);
	}
	else
	{
		GPIO_ResetBits(LED_DIS_PORT, SCK_DIS);
	}
}


//------------------------------------------------------------------------
//  ��  �� ��void WriteMOSI1( uint8_t temp )
//  ��  �� ������ MOSI1
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
void WriteMOSI1(uint8_t temp)
{	
	if( temp )					
	{
		GPIO_SetBits(LED_DIS_PORT, MOSI_DIS);
	}
	else
	{
		GPIO_ResetBits(LED_DIS_PORT, MOSI_DIS);
	}
}


//---------------------------------------------------------------------------------
//��������:             SentByte_SPI()
//��    ��:             SPI����һ�ֽ�����
//��ڲ���:             Data8
//���ڲ���:             ��
//˵    ��:				��ģ��SPI���߷�ʽ��������
//----------------------------------------------------------------------------------
void SentByte_SPI1(uint8_t ch)
{
	uint8_t i,temp;   
	
	WriteSCK1 ( 0 );
	
    for (i=0;i<8;i++)
    {
		temp=ch&0x80;
    	ch=ch<<1;
        if(temp)
        {
        	WriteMOSI1( 1 );	

        	WriteSCK1 ( 0 );

//            NOP();
  
        	WriteSCK1 ( 1 );	        	
        }
        else
        {
        	WriteMOSI1( 0 );	

            WriteSCK1 ( 0 );

//            NOP();
            
        	WriteSCK1 ( 1 );
        }
    }
    
//    NOP();
    
    WriteSCK1 ( 0 );
}

//----------------------------------------------------------------------------
//��������:   SentByteTo7219()
//��    ��:   ��7219����1Byte���ݻ�����
//��ڲ���:   addr:   ���ݽ��յ�ַ
//            Data:   Ҫ�������
//���ڲ���:   ��
//-----------------------------------------------------------------------------
void SentByteTo7219(uint8_t Addr,uint8_t Data) 
{
  	 WriteCS_DIS( 0 );			   					   		
  	 SentByte_SPI1( Addr );		                //�͵�ַ											
  	 SentByte_SPI1( Data );                    	//������
  	 WriteCS_DIS( 1 );							//��������,��7219����		  									
}


//-------------------------------------------------------------------
//��������:     Init7219 ()
//��    ��:     7219��ʼ��
//��ڲ���:     ��
//���ڲ���:     ��
//-------------------------------------------------------------------
void Init7219 (void)
{
//	InitIOForSPI1();								//SPI�˿ڳ�ʼ��IO��
	
    SentByteTo7219 (DisplayTest,TextEnd);         	// ���ù���ģʽ
	SentByteTo7219 (ScanLimit,ScanDigit);         	// ����ɨ�����
    SentByteTo7219 (DecodeMode,NoDecodeDigit);      // ���ò�����ģʽ
    SentByteTo7219 (Intensity,IntensityGrade);    	// ��������
    SentByteTo7219 (ShutDown,NormalOperation);    	// ����Ϊ��������ģʽ
}

uint8_t LedNumCode[]={0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B,0x77,
                   // 0    1    2    3    4    5    6    7    8    9    A 
                     0x1F,0x4E,0x3D,0x4F,0x47};  
                   // b   C    d     E    F      


	 /*			b
			  -----
			 |     |
		   g |	   | c
			 |	h  |
			  -----
			 |     |
		   f |	   | d
			 |	   |
			  -----	 .a
				e
          ������� abcd efgh

	 */


/*                   
uint8_t LedCharCode[]={0x77,0x1F,0x4E,0x3D,0x4F,0x47,0x7B,0x37,0x30,0x3c,0x87,0x0e,0x01,
                   //   A    B    C    d    E    F    g    H     I   J    K    L   Minus    
                        0x76,0x7E,0x67,0xfE,0x66,0x5B,0x0f,0x3e,0x3e,0x00,0x37,0x33,0x6D};  
                        //n   O    P    Q     r    S    t    U    V    W   X    Y    Z   
*/                        
uint8_t LedCharCode[] = {0x00,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0x01,0xE5,0xE5,
				  	   //  ''   !    "    #    $    %    &    '    (     )   *    +    ,    -    .   /
					  0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B, 
					 // 0    1   2	  3	    4    5    6    7    8    9	
					  0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,0xE5,
					 // :    ;    <    =     >   ?   @
					  0x77,0x1F,0x4E,0x3d,0x4F,0x47,0x5F,0x37,0x30,0x3c,0x87,0x0e,0xE5,
                    //  A    B   C    D     E    F    G    H   I    J    K    L    M    
                      0x76,0x7E,0x67,0xFE,0x66,0x5B,0x0F,0x3E,0x3E,0xE5,0x37,0x33,0x6D,
                    //  N    O    P   Q    R     S    T    U    V   W    X    Y    Z     
                      0xE5,0xE5,0xE5,0xE5,0x08,0xE5,
                     // [    \    ]    ^    _    `
                      0x77,0x1F,0x0D,0x3D,0x4F,0x47,0x5F,0x17,0x30,0x3c,0x87,0x0e,0xE5,
                     // a    b   c    d     e    f    g    h    i    j     k    l    m
					  0x15,0x1D,0x67,0x73,0x05,0x5B,0x0F,0x1C,0x1C,0xD3,0x37,0x33,0x6D,
                     // n    o   p    q     r    s    t    u    v   w    x    y    z     
                      0xE5,0xE5,0xE5,0xE5,0xE5,0x00//
                      // {    |    }    ~       ��
					   };     
           
#define     LED_CHAR_NNM    16                  //�������ʾ������� 
#define     DISPLAY_LED_NUM 8                   //����ܸ���   
        
//--------------------------------------------------------------------
//void          DisplayNum (uint32_t Unmber)
//��    ��:     ������ܽ�����������ʾ����
//��ڲ���:     number
//���ڲ���:     ��
//-------------------------------------------------------------------
void    DisplayNum (uint32_t Unmber)
{
    unsigned char i  = DISPLAY_LED_NUM;
	unsigned char digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
	
	Init7219();                                //��ʾ��ʼ��
	
	do
	{ 		
		i--;
		SentByteTo7219(digit[i],LedNumCode[Unmber % 10] );       //�ӵ�λ����λ��ʾ
		Unmber /= 10;                       
	} while(i && Unmber);										 //��λ��0����ʾ
	while(i)
	{
        i--;
        SentByteTo7219(digit[i],0x00);            //��û����λ����ʾ���        
    }	  
}

//--------------------------------------------------------------------
//void          DisplayNum_WithZero (uint32_t Unmber)
//��    ��:     ������ܽ�����������ʾ����
//��ڲ���:     number
//���ڲ���:     ��
//-------------------------------------------------------------------
void    DisplayNum_WithZero (uint32_t Unmber)
{
    unsigned char i  = DISPLAY_LED_NUM;
	unsigned char digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
	
	Init7219();                                //��ʾ��ʼ��
	
	do
	{ 		
		i--;
		SentByteTo7219(digit[i],LedNumCode[Unmber % 10] );       //�ӵ�λ����λ��ʾ
		Unmber /= 10;                       
	}while(i);													 //��λ��0Ҳ��ʾ����	
	while(i)
	{
        i--;
        SentByteTo7219(digit[i],0x00);            //��û����λ����ʾ���        
    }	  
}

//--------------------------------------------------------------------
//void          DisplayNumWithDoit (uint32_t Unmber)
//��    ��:     ������ܽ�����������ʾ����
//��ڲ���:     number
//���ڲ���:     ��
//-------------------------------------------------------------------
void    DisplayNumWithDot (uint32_t Unmber)
{
    unsigned char i  = DISPLAY_LED_NUM;
	unsigned char digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};

	do
	{ 		
		i--;
		SentByteTo7219(digit[i],LedNumCode[Unmber % 10] + 0x80);       //�ӵ�λ����λ��ʾ
		Unmber /= 10;                       
	}while(i && Unmber);
		
	while(i)
	{
        i--;
        SentByteTo7219(digit[i],0x00);            //��û����λ����ʾ���        
    }	  
}

//------------------------------------------------------
//��������:     DisplayString ()
//��    ��:     ���������ʾ�ַ�������
//��ڲ���:     String: �ַ�����
//���ڲ���:     ��
//��ʾ����:     String
//���ݴ���:	    ���ø�ʽ  DisplayString("ABCD");
//-------------------------------------------------------
void DisplayString(char * String)
{
	uint8_t   i = 0;
	uint8_t   digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
	uint8_t   CharTemp;
	
	while( i < DISPLAY_LED_NUM)
	{
        SentByteTo7219(digit[i],0x00);                      //��û����λ����ʾ���  
          
        i++;    
    }	
	
	i = 0;
	while(i < DISPLAY_LED_NUM && strlen((char *)String) - i > 0)
	{ 		
		i++;
		if(String[strlen((char *)String) - i] >= ' ')					//��ʾ��ĸ
		{
			CharTemp = String[strlen((char *)String)- i] - ' ';                       	//����ʾ�ַ�λ��,LedCharCodeλ��
			SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedCharCode[CharTemp]);  	//�ӵ�λ����λ��ʾ
//			SentByteTo7219(digit[i],LedCharCode[CharTemp]);  	//�Ӹ�λ����λ��ʾ       
		}   
		else													//��ʾ����
		{
			CharTemp = String[strlen((char *)String) - i] - '0';                      	//����ʾ�ַ�λ��,LedCharCodeλ��
			SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedNumCode[CharTemp]);   	//�ӵ�λ����λ��ʾ 
//			SentByteTo7219(digit[i],LedNumCode[CharTemp]);   	//�Ӹ�λ����λ��ʾ     			
		}        
	}
}


//------------------------------------------------------
//��������:     DisplayString_WithDot ()
//��    ��:     ���������ʾ�ַ�������,��С����
//��ڲ���:     String: �ַ�����
//���ڲ���:     ��
//��ʾ����:     String
//���ݴ���:	    ���ø�ʽ  DisplayString("ABCD");
//-------------------------------------------------------
void DisplayString_WithDot(char * String)
{
	uint8_t   i = 0;
	uint8_t   digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
	uint8_t   CharTemp;
	
	while( i < DISPLAY_LED_NUM)
	{
        SentByteTo7219(digit[i],0x00);                      //��û����λ����ʾ���  
          
        i++;    
    }	
	
	i = 0;
	while(i < DISPLAY_LED_NUM && strlen((char *)String) - i > 0)
	{ 		
		i++;
		if(i == 4)
		{
			if(String[strlen((char *)String) - i] >= ' ')					//��ʾ��ĸ
			{
				CharTemp = String[strlen((char *)String)- i] - ' ';                       	//����ʾ�ַ�λ��,LedCharCodeλ��
				SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedCharCode[CharTemp] + 0x80);  	//�ӵ�λ����λ��ʾ
//				SentByteTo7219(digit[i],LedCharCode[CharTemp]);  	//�Ӹ�λ����λ��ʾ       
			}   
			else													//��ʾ����
			{
				CharTemp = String[strlen((char *)String) - i] - '0';                      	//����ʾ�ַ�λ��,LedCharCodeλ��
				SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedNumCode[CharTemp] + 0x80);   	//�ӵ�λ����λ��ʾ 
//				SentByteTo7219(digit[i],LedNumCode[CharTemp]);   	//�Ӹ�λ����λ��ʾ     			
			}
		}
		else
		{
			if(String[strlen((char *)String) - i] >= ' ')					//��ʾ��ĸ
			{
				CharTemp = String[strlen((char *)String)- i] - ' ';                       	//����ʾ�ַ�λ��,LedCharCodeλ��
				SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedCharCode[CharTemp]);  	//�ӵ�λ����λ��ʾ
//				SentByteTo7219(digit[i],LedCharCode[CharTemp]);  	//�Ӹ�λ����λ��ʾ       
			}   
			else													//��ʾ����
			{
				CharTemp = String[strlen((char *)String) - i] - '0';                      	//����ʾ�ַ�λ��,LedCharCodeλ��
				SentByteTo7219(digit[DISPLAY_LED_NUM - i],LedNumCode[CharTemp]);   	//�ӵ�λ����λ��ʾ 
//				SentByteTo7219(digit[i],LedNumCode[CharTemp]);   	//�Ӹ�λ����λ��ʾ     			
			}
		}			        
	}
}

//------------------------------------------------------
//��������:     DisplayABC_2 ()
//��    ��:     ���������ʾ�ַ�������E1��E2��E3��E4
//��ڲ���:     String: �ַ�����
//���ڲ���:     ��
//��ʾ����:     String
//���ݴ���:	    ���ø�ʽ  DisplayString("ABCD");
//-------------------------------------------------------
void DisplayABC_2(uint_least8_t * String)
{
	uint8_t   i = 0;
	uint8_t   digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
	uint8_t   CharTemp;
	
	while( i < DISPLAY_LED_NUM)
	{
        SentByteTo7219(digit[i],0x00);                      //��û����λ����ʾ���  
          
        i++;    
    }	
	
	i = 0;
	while(i < DISPLAY_LED_NUM && strlen((char *)String) - i > 0)
	{ 		
		i++;
		if(String[strlen((char *)String) - i] >= ' ')					//��ʾ��ĸ
		{
			CharTemp = String[strlen((char *)String)- i] - ' ';                       	//����ʾ�ַ�λ��,LedCharCodeλ��
			SentByteTo7219(digit[i-1],LedCharCode[CharTemp]);  	//�ӵ�λ����λ��ʾ
//			SentByteTo7219(digit[i],LedCharCode[CharTemp]);  	//�Ӹ�λ����λ��ʾ       
		}   
		else													//��ʾ����
		{
			CharTemp = String[strlen((char *)String) - i] - '0';                      	//����ʾ�ַ�λ��,LedCharCodeλ��
			SentByteTo7219(digit[i-1],LedNumCode[CharTemp]);   	//�ӵ�λ����λ��ʾ 
//			SentByteTo7219(digit[i],LedNumCode[CharTemp]);   	//�Ӹ�λ����λ��ʾ     			
		}        
	}
}


//------------------------------------------------------
//��������:MovDisplayChar ()
//��    ��:���������ʾ�ַ����Ѷ��ַ�
//��ڲ���:character: �ַ�����
//���ڲ���:��
//��ʾ����:   -   E   H   L   P   Black
//���ݴ���:	  A	  B	  C	  D	  E	  F	  
//-------------------------------------------------------
void MovDisplayChar (uint8_t character)
{
	uint8_t i = DISPLAY_LED_NUM ,j;
	uint8_t digit[8] = {Digit0,Digit1,Digit2,Digit3,Digit4,Digit5,Digit6,Digit7};
		
	for(j = 0;j < 12; j++)
	{
	 	 i = DISPLAY_LED_NUM;
		 while(i)
		 {
            i--;
            SentByteTo7219(digit[i],0x00);            	//��û����λ����ʾ���
         }	
		 
		SentByteTo7219(digit[j%4],LedNumCode[character%LED_CHAR_NNM]);       //�ӵ�λ����λ��ʾ   
//		DelayX10ms(20);
		Delay(2000000);                
	}
}

void Dis_Test(void)
{
	uint8_t i = 0;
	for(i=0;i<10;i++)
	{
		DisplayNum_WithZero(i*11111111);
		//Delay(400000);
        BSP_OS_TimeDly(OS_TICKS_PER_SEC/2);
	}
}


