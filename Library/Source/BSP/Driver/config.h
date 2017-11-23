
#ifndef __CONFIG_H 
#define __CONFIG_H

//This segment should not be modified
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define NULL 0
typedef unsigned char  uint8;                   /* defined for unsigned 8-bits integer variable 	�޷���8λ���ͱ���  */
typedef signed   char  int8;                    /* defined for signed 8-bits integer variable		�з���8λ���ͱ���  */
typedef unsigned short uint16;                  /* defined for unsigned 16-bits integer variable 	�޷���16λ���ͱ��� */
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
typedef float          fp32;                    /* single precision floating point variable (32bits) �����ȸ�������32λ���ȣ� */
typedef double         fp64;                    /* double precision floating point variable (64bits) ˫���ȸ�������64λ���ȣ� */

/********************************/
/*      ARM specital code      */
/*      ARM���������           */
/********************************/
//This segment should not be modify
//��һ������Ķ�

#include    "stm32f10x.h"


/********************************/
/*Application Program Configurations*/
/*     Ӧ�ó�������             */
/********************************/
//This segment could be modified as needed.
//���¸�����Ҫ�Ķ�

/********************************/
/*Configuration of the example */
/*     �����ӵ�����             */
/********************************/
/* System configuration .Fosc��Fcclk��Fcco��Fpclk must be defined */
/* ϵͳ����, Fosc��Fcclk��Fcco��Fpclk���붨��*/
#define Fosc            12000000                 //Crystal frequence,10MHz~25MHz��should be the same as actual status. 
						    //Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
#define Fcclk           (Fosc *6)                  //System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz. 
						    //ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
#define Fcco            (Fcclk * 4)                 //CCO frequence,should be 2��4��8��16 multiples of Fcclk, ranged from 156MHz to 320MHz. 
						    //CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
#define Fpclk           (Fcclk / 4) * 1             //VPB clock frequence , must be 1��2��4 multiples of (Fcclk / 4).
						    //VPBʱ��Ƶ�ʣ�ֻ��Ϊ(Fcclk / 4)��1��2��4��

#include    "target.h"              //This line may not be deleted ��һ�䲻��ɾ��

#endif
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/

#define		VDCOFFC							0xD1
#define		VACGAINC						0xD6
#define		VACOFFC							0xD5
#define		IDCOFFC							0xC9
#define		IACGAINC						0xCE
#define		IACOFFC							0xCD

#define		CONFIG							0x00				//     ���üĴ��� 
#define		IDCOFF							0x02				//   I      ����ͨ��ֱ��ƫ
#define		IA								0x04				//   I        ����ͨ�������
#define		VDCOFF							0x06				//      ��ѹͨ��ֱ��ƫ
#define		VA								0x08				//      ��ѹͨ�������
#define		CYCLE							0x0A				// ÿ���������ڵ�A
#define		PULSE							0x0C				//  Pulse-Rate   ��������
#define		IG								0x0E				//   I       ˲ʱ�����Ĵ���
#define		VG								0x10				//   V       ˲ʱ��ѹ�Ĵ���
#define		PG								0x12				//   P       ˲ʱ���ʼĴ���
#define		EG								0x14				//   E       ���ܼĴ�������
#define		IRMS							0x16				//   I      ������Чֵ�Ĵ�
#define		VRMS							0x18				//   V      ��ѹ��Чֵ�Ĵ�
#define		POFF							0x1C				//   Status     ����ƫ��
#define		STATUS							0x1E				//   Status     ״̬�Ĵ��� 
#define		IACOFF							0x20				//   ����ͨ������ƫ
#define		VACOFF							0x22				//   V      ��ѹͨ������ƫ
#define		MASK     						0x34				//�ж����μĴ��� 
#define		CTRL							0x38				//   Ctrl     ���ƼĴ��� 

#define		START							0xE8				// ����ת�� 
#define		SYNC0							0xFE				// SYNC0 ���� 
#define		SYNC1 							0xFF
#define		STOP							0xA0				// �ϵ�/��ͣ���� 
#define		IDLE							0x88				//������� 


#define     DS3231_ADDR     		0xD0
#define     FRAM_ADDR     			0xA0



#define     RST_USB		0x01<<29					//P129
#define 	BUSY_USB	0x01<<21					//P121
#define 	CS_USB		0x01<<15					//P015
#define 	INT_USB		0x01<<16					//P016
#define		DO_USB		0x01<<18					//P018
#define		DI_USB		0x01<<19					//P019
#define		SCK_USB		0x01<<17					//P017



#define 	NOPS			    __asm { NOP ; NOP; NOP; NOP; NOP; NOP}

typedef struct
{
	volatile	uint16			Vrms;										//��ѹ										
	volatile	uint16			Irms;										//����
	volatile	long long		Ppower;										//������										
	volatile	uint32			Npower;										//������
	volatile	uint32			Pulse;										//����������										
	volatile	uint8			Time[8];									//ʱ��
	volatile	uint32			PagePtr;									//��Flash��ҳָ��
	volatile	uint32			RecordPtr;									//��ҳ��ļ�¼ָ��
	volatile	uint32			UsbPagePtr;									//�ϴ�ת��λ��
	volatile	uint32			ID;
	volatile	uint32			Status;
	volatile	uint8			Reserve[14];
	volatile	uint16			Crc16;
}SYSTEM;
typedef struct
{
	volatile	uint32				Idcoff;
	volatile	uint32				Vdcoff;
	volatile	uint32				Iacoff;
	volatile	uint32				Vacoff;
	volatile  	uint32				Igain;
	volatile	uint32				Vgain;
	volatile	uint32				Poff;
	volatile	uint32				ID;
	volatile	uint32				Number;
	volatile	uint8				Ref[124];
}CONFIGSFR;
typedef struct
{
	volatile	uint8				buf[100];							//��������
	volatile	uint8				Ok;										//������
}UART;
typedef struct
{
	uint8							buf[8*20];							//��ʾ����
	uint8							FPtr;
	uint8							CPtr;
	uint8							EPtr;
}SHOWSFR;

#define VER							1.0


#define	xEndCH376Cmd( )	{ IO0SET = CS_USB; }  /* SPIƬѡ��Ч,����CH376����,������SPI�ӿڷ�ʽ */


#define	LONG_NAME_BUF_LEN	( LONG_NAME_PER_DIR * 20 )	/* ���ж���ĳ��ļ�������������,��СֵΪLONG_NAME_PER_DIR*1 */
