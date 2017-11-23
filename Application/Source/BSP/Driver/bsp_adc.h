/*******************************************************************************
 *   Filename:       bsp_adc.h
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2015-08-11
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ADCģ��ת��ģ�飨ʹ��DMA�������ݣ�ͷ�ļ�
 *
 *
 *   Notes:
 *
 *   All copyrights reserved to Roger-WY
 *******************************************************************************/
#ifndef __BSP_ADC_H__
#define	__BSP_ADC_H__


#include "stm32f10x.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

/*******************************************************************************
 * ������ �궨��ɼ�ͨ��������ÿ��ͨ���ɼ��Ĵ���
 */
#define Channel_Number 3    //2��ͨ��
#define Channel_Times  1    //ÿ��ͨ���ɼ�1��


extern __IO uint16_t ADC_Value[Channel_Times][Channel_Number];
extern __IO uint16_t ADC_AverageValue[Channel_Number];


/*******************************************************************************
 * ������ �ⲿ��������
 */
void Bsp_ADC_Init(void);
void ADC1Convert_Begin(void);
void ADC1Convert_Stop(void);
void Get_AD_AverageValue(void);


#endif /* __ADC_H */

