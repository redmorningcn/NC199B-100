/*******************************************************************************
 *   Filename:       bsp_sht11.h
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    STH11��ʪ�ȴ���������ģ��
 *                   ������ͬ������ ��SHT10��
 *   Notes:
 *
 *     				 E-mail:261313062@qq.com
 *
 *******************************************************************************/
#ifndef __BSP_SHT11_H__
#define __BSP_SHT11_H__

#include "stm32f10x.h"
#include "global.h"
#include "math.h"


/**************** Ӳ��ƽ̨���� ************************************************/
extern  GPIO_InitTypeDef    GPIO_InitStructure;


#define RCC_GPIO_SHT        RCC_APB2Periph_GPIOC
#define SHT_PORT            GPIOC
#define GPIO_Pin_SHT_DAT    GPIO_Pin_10
#define GPIO_Pin_SHT_CLK    GPIO_Pin_11


#define SHT_DATA_H()        GPIO_SetBits(SHT_PORT,GPIO_Pin_SHT_DAT)
#define SHT_DATA_L()        GPIO_ResetBits(SHT_PORT,GPIO_Pin_SHT_DAT)
#define SHT_CLK_H()         GPIO_SetBits(SHT_PORT,GPIO_Pin_SHT_CLK)
#define SHT_CLK_L()         GPIO_ResetBits(SHT_PORT,GPIO_Pin_SHT_CLK)

#define SHT_DATA_INPUT()  st(   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SHT_DAT;\
                                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;\
                                GPIO_Init(SHT_PORT, &GPIO_InitStructure);\
                            )

#define SHT_DATA_OUPUT()  st(   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SHT_DAT;\
                                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;\
                                GPIO_Init(SHT_PORT, &GPIO_InitStructure);\
                            )

#define SHT_CLK_OUPUT()   st(   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SHT_CLK;\
                                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                                GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;\
                                GPIO_Init(SHT_PORT, &GPIO_InitStructure);\
                            )

#define SHT_INIT()        st(   RCC_APB2PeriphClockCmd(RCC_GPIO_SHT,ENABLE);\
                                SHT_CLK_OUPUT();\
                                SHT_DATA_OUPUT();\
                                SHT_CLK_L();\
                                SHT_DATA_H();\
                            )


/******************************************************************************/
enum {TEMP,HUMI};

#define noACK 0                          // ��Ӧ���־ �����ж��Ƿ����ͨѶ
#define ACK   1                          // Ӧ���־   �������ݴ���
                                                            // adr  command  r/w
#define SHTCMD_WRITE_STATUS     0x06     // "д״̬�Ĵ���"���� 000   0011    0
#define SHTCMD_READ_STATUES     0x07     // "��״̬�Ĵ���"���� 000   0011    1
#define SHTCMD_MEASURE_TEMP     0x03     // "�����¶�"����     000   0001    1
#define SHTCMD_MEASURE_HUMI     0x05     // "�������ʪ��"���� 000   0010    1
#define SHTCMD_RESET            0x1e     // "��λ"����         000   1111    0

/***********************************************
* ������ ����һ��ֵ������
*/
typedef union
{
  long unsigned int i;
  float f;
} value;


/***********************************************
* ������ SHTxx�������ݽ���ṹ��
*/
typedef struct
{
    /***********************************************
    * ������ SHTxx�������ݽ���Ŵ�10��֮���ֵ�����ͣ�
    */
    int16_t Temperature;
    int16_t Humidity;
    int16_t DewPoint;
    /***********************************************
    * ������ SHTxx�������ݽ���������ͣ�
    */
    float fTemp;
    float fHumi;
    float fDewpoint;
}SHT_DATA_TYPE;


/***********************************************
* ������ ���ⲿ���õĺ�������
*/
void Bsp_ShtInit(void);
u8   Bsp_ShtMeasureOnce(SHT_DATA_TYPE *t_shtval);


#endif