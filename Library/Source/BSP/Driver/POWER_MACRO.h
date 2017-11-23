/******************** (C) COPYRIGHT 2014 www.nanche.com  ********************
* �ļ���  ��POWER_MACRO.h
* ����    ��ʵ��CS5463�ײ�궨��
* ����    ��ZW
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _POWER_MACRO_H
#define _POWER_MACRO_H

#include <app_tax.h>
/*������*/
#define		VDCOFFC							0xD1
#define		VACGAINC						0xD6
#define		VACOFFC							0xD5
#define		IDCOFFC							0xC9
#define		IACGAINC						0xCE
#define		IACOFFC							0xCD
#define     VDCGAINC                        0xD2
#define     IDCGAINC                        0xCA
#define     P_rate_ADD                      0xCB
#define     P_rate_SUB                      0xCC
#define     P_rate_SET                      0xC0
#define     P_rate_DEL                      0xAA

#define		CONFIG							0x00				//   ���üĴ��� 
#define		IDCOFF							0x02				//   I       ����ͨ��ֱ��ƫ
#define		IG								0x04				//   I       ����ͨ�������
#define		VDCOFF							0x06				//   ��ѹͨ��ֱ��ƫ
#define		VG								0x08				//   ��ѹͨ�������
#define		CYCLE_COUNT						0x0A				//   ÿ���������ڵ�A
#define		PULSE_RATE						0x0C				//   Pulse-Rate   ��������
#define		INSTANT_I					    0x0E				//   I       ˲ʱ�����Ĵ���
#define		INSTANT_V						0x10				//   V       ˲ʱ��ѹ�Ĵ���
#define		INSTANT_P						0x12				//   P       ˲ʱ���ʼĴ���
#define		PA								0x14				//   E       ���ܼĴ�������
#define		IRMS							0x16				//   I       ������Чֵ�Ĵ�
#define		VRMS							0x18				//   V       ��ѹ��Чֵ�Ĵ�
#define		POFF							0x1C				//   Status  ����ƫ��
#define     FREQUENCY						0x1A                //   Freq    ����Ƶ�ʲ���
#define		STATUS							0x1E				//   Status  ״̬�Ĵ��� 
#define		IACOFF							0x20				//   ����ͨ������ƫ
#define		VACOFF							0x22				//   V       ��ѹͨ������ƫ
#define     MODE                            0x24				//   MODE    ����ģʽ�Ĵ���
#define     TEMP                            0x26				//   TEMP    �¶�
#define     PQ                              0x2A           
#define     PF                              0x32                 //   PF      ��������
#define		INTMASK     					0x34				//	 �ж����μĴ��� 
#define     S								0x36				//   S       ���ڹ���
#define		CTRL							0x38				//   Ctrl    ���ƼĴ���

#define     SINGLE_CYCLE					0xE0                // 	 ��������������
#define		CONTINUOUS_CYCLE				0xE8				//   ���������������� 
#define		SYNC0							0xFE				//   SYNC0 ���� 
#define		SYNC1 							0xFF				//   SYNC1 ���� 

#define		STOP							0xA0				//   �ϵ�/��ͣ���� 
#define		IDLE							0x88				//   ������� 

#define     SOFT_RESET                      0x00
#define 	WAIT                            0x01
#define     SLEEP                           0x10 

#define     PAGE 							0x3E				//   д����ʱΪ��ҳ�Ĵ���
#define 	PAGE_0                          0x000000
#define 	PAGE_1                          0x000001
#define 	PAGE_3                          0x000003
#define     PULSE_WIDTH                     0x00
#define     TGAIN                           0x04				//    �¶ȴ���������
#define     TOFF                            0x06				//    �¶ȴ�����ƫ��

#define 	Dummy_Byte           			0xFE	            //    ��ΪSYNC0  

__packed
typedef struct
{
	float				U_RMS;					//��ѹ	
	float				I_RMS;					//����
	uint32_t			U_SCL;					//ԭ�ߵ�ѹ	
	uint32_t			I_SCL;					//ԭ�ߵ���
	float               Power_Freq;			    //Ƶ��
	float               Power_Factor;			//��������
	int32_t             ACTIVE_POWER;			//�й�����
	int32_t             REACTIVE_POWER;		    //�޹�����
	uint32_t			PPpower_NUM;			//���й�����										
	uint32_t			NPpower_NUM;			//���й�����
	uint32_t			PPpulse;				//���й�������
	uint32_t            NPpulse;				//���й�������
	uint32_t            PPpower_base;			//���й�������
	uint32_t            NPpower_base;			//���й�������
	uint32_t            PPpower_temp;			//���й�����ʱ����
	uint32_t            NPpower_temp;			//���й�����ʱ����
	uint32_t			PQpower_NUM;			//���޹�����										
	uint32_t			NQpower_NUM;			//���޹�����
	uint32_t			PQpulse;				//���޹�������
	uint32_t            NQpulse;				//���޹�������
	uint32_t            PQpower_base;			//���޹�������
	uint32_t            NQpower_base;			//���޹�������
	uint32_t            PQpower_temp;			//���޹�����ʱ����
	uint32_t            NQpower_temp;			//���޹�����ʱ����

   float                U_O;//   u32 ��ѹ����ֵ
   float                U_K;//  f32 ��ѹ����ϵ��
   float                I_K;//  f32 ��������ϵ��
}ENERGY;

__packed
typedef struct
{ 
	uint32_t			PPPower;			                //���й�����    1kvarh      99999999 kvarh
	uint32_t			NPPower;			                //���й�����    1kvarh      99999999 kvarh 
	uint32_t			PQPower;			                //���޹�����    1kvarh      99999999 kvarh
	uint32_t			NQPower;			                //���޹�����    1kvarh      99999999 kvarh
	uint32_t			PrimVolt;					        //ԭ�ߵ�ѹ      0.001V      0��35000.000V
	uint32_t			PrimCurr;					        //ԭ�ߵ���      0.001A      0��600.000A
	uint32_t            PowerFreq;			                //Ƶ��          0.001Hz    
	int32_t             PowerFactor;			            //��������      0.001       -1.000��1.000
	int32_t             ActivePower;			            //�й�����      0.001kW     -12000.000  kW��12000.000  kW
	int32_t             ReactivePower;		                //�޹�����      0.001kvar   -12000.000  kvar��12000.000 
} StrEnergy;

__packed
typedef struct {
    uint32_t            RecordTime;
    uint32_t            Head;
    uint32_t            Tail;
} StrMaterRecord;

__packed
typedef struct
{    
    /***************************************************
    * ������ ͷ:10 bytes
    */
    uint8_t             CmdType;                            // ��������
    uint8_t             DataType;                           // ��������
    uint16_t            LocoTyp;                            // �����ͺ�
    uint16_t            LocoNbr;                            // ������
    uint32_t            RecordNbr;                          // ��¼��
    
    /***************************************************
    * ������ ʱ��:4 bytes
    *
    struct __sTIME__ {
    uint32_t            Sec                 : 6;            // D5~D0:��
    uint32_t            Min                 : 6;            // D11~D6:��
    uint32_t            Hour                : 5;            // D16~D12:ʱ
    uint32_t            Day                 : 5;            // D21~D17����
    uint32_t            Mon                 : 4;            // D25~D22����
    uint32_t            Year                : 6;            // D26~D31:��
    } Time;
    
    *//***************************************************
    * ������ ����:40 bytes
    */
    StrEnergy           Energy;
    uint32_t            Rssi                : 8;        // �ź�ǿ��		1
    uint32_t            Err1                : 9;
    uint32_t            ErrTax              : 1;
    uint32_t            ErrRsv              :14;        // ���ϴ���		3	
    uint8_t             Rsv[6];                         // Ԥ��		6	
    
    uint8_t             TaxType;                        // TAX�豸����	BYTE	1	
    uint8_t             TeZhengMa;                      // TAX2Ϊ��վ�����䣻TAX3ΪTAX������	BYTE	1	
    /***************************************************
    * ������ TAX:64 bytes
    */
    union __uTAX__ {
    _StrTax_II_Rec      Tax2;                           // ��¼����     
    struct __sTAX_III__ {
    StrTaxVariable      Vari;
    StrTaxConstant      Cons;
    } Tax3;
    uint8_t             buf1[64];
    uint16_t            buf2[64/2];
    uint32_t            buf3[64/4];
    } Tax;    
    uint16_t            chk;
    
    StrMaterRecord      Record;
} StrMater;

extern  StrMater    Mater,History;
extern  ENERGY      AC;

__packed
typedef struct
{
	uint32_t			Idcoff;
	uint32_t			Vdcoff;
	uint32_t			Iacoff;
	uint32_t			Vacoff;
	uint32_t			Igain;
	uint32_t			Vgain;
	uint32_t            P_rate;
	uint32_t			P_off;
	uint32_t			ID;
	uint32_t			Number;
	uint8_t				Ref[120];
}CONFIGSFR;
extern CONFIGSFR   configsfr_AC , configsfr_AC_RD;

__packed
typedef struct
{
	 uint8_t 	        Time[6];			    //ʱ��
	 uint8_t			POWER_FREQ;             //Ƶ��
	 uint8_t            POWER_FACTOR;           //��������
	uint32_t			PPpower_NUM;			//���й�����										
	uint32_t			NPpower_NUM;			//���й�����
	uint32_t			PQpower_NUM;			//���й�����										
	uint32_t			NQpower_NUM;			//���й�����
	uint16_t			VOLTAGE;				//ԭ�ߵ�ѹ
	uint16_t			CURRENT;				//ԭ�ߵ���
	uint16_t			RES;				    //Ԥ��2�ֽ�
	uint16_t 	        Crc16;					//CRCУ��
}SDAT;

extern  SDAT 	  recordsfr;


__packed
typedef struct
{
	uint8_t				buf[100];							//��������
	uint8_t				Ok;										//������
}UART;


extern  uint32_t  g_Flash_Adr;

#endif 


