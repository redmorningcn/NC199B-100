#ifndef __BSP_HT1621__
#define __BSP_HT1621__

/***********************************************
 * ��������Ҫ��ӵ�ͷ�ļ�
 */
#include "stm32f10x.h"

#ifndef  u8
#define  u8    unsigned char
#endif

/***********************************************
 * �������Ƿ�ʹ��HT1621B�Ĳ��Ժ��� 0Ϊ��ʹ�� 1Ϊʹ��
 */
#define  HT1621B_TEST_EN  1

/***********************************************
 * ��������ʱ����
 */
#define _Nop() __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()


/***********************************************
 * ������ ����HT1621������
 */
#define BIAS    0x52    //0b1000 0101 0010 1/3bis 4com

#define SYSDIS  0X00    //0b1000 0000 0000 ����ϵͳ������LCDƫѹ������
#define SYSEN   0X02    //0b1000 0000 0010 ��ϵͳ����

#define LCDOFF  0X04    //0b1000 0000 0100 ��LCDƫѹ
#define LCDON   0X06    //0b1000 0000 0110 ��LCDƫѹ

#define XTAL    0x28    //0b1000 0010 1000 �ⲿ��ʱ��
#define RC256   0X30    //0b1000 0011 0000 �ڲ�ʱ��

#define TONEON  0X12    //0b1000 0001 0010 ���������
#define TONEOFF 0X10    //0b1000 0001 0000 �ر��������

#define WDTDIS  0X0A    //0b1000 0000 1010 ��ֹ���Ź�


/***********************************************
 * ������HT1621B�����Ŷ���
 */
/*----------����ʹ�ö˿ڶ���----------------*/
#define HT1621B_CS_RCC            RCC_APB2Periph_GPIOD
#define HT1621B_CS_PORT           GPIOD
#define HT1621B_CS_PIN            GPIO_Pin_2

#define HT1621B_WR_RCC            RCC_APB2Periph_GPIOC
#define HT1621B_WR_PORT           GPIOC
#define HT1621B_WR_PIN            GPIO_Pin_11

#define HT1621B_DATA_RCC          RCC_APB2Periph_GPIOC
#define HT1621B_DATA_PORT         GPIOC
#define HT1621B_DATA_PIN          GPIO_Pin_10

/*----------���������ƽ����----------------*/
#define HT1621B_CS_HIGH()         GPIO_SetBits  (HT1621B_CS_PORT,HT1621B_CS_PIN)
#define HT1621B_CS_LOW()          GPIO_ResetBits(HT1621B_CS_PORT,HT1621B_CS_PIN)

#define HT1621B_WR_HIGH()         GPIO_SetBits  (HT1621B_WR_PORT,HT1621B_WR_PIN)
#define HT1621B_WR_LOW()          GPIO_ResetBits(HT1621B_WR_PORT,HT1621B_WR_PIN)

#define HT1621B_DATA_HIGH()       GPIO_SetBits  (HT1621B_DATA_PORT,HT1621B_DATA_PIN)
#define HT1621B_DATA_LOW()        GPIO_ResetBits(HT1621B_DATA_PORT,HT1621B_DATA_PIN)



/***********************************************
 * ��������������
 */
void HT1621B_Init(void);
void HT1621BWrCmd(u8 Cmd);
void HT1621BWrOneData(u8 Addr,u8 Data);
void HT1621BWrStrData(u8 Addr,u8 *p,u8 cnt);
void HT1821B_AllClean(void);

#if  HT1621B_TEST_EN > 0
void HT1621B_TEST(void);
#endif



#endif /* end of ht1621b*/