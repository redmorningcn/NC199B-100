/*******************************************************************************
*   Filename:       bsp_control.h
*   Revised:        All copyrights reserved to Roger.
*   Date:           2014-08-18
*   Revision:       v1.0
*   Writer:	        wumingshen.
*
*   Description:    �̵������� ͷ�ļ�
*
*
*   Notes:
*
*******************************************************************************/
#ifndef __BSP_CONTROL_H__
#define __BSP_CONTROL_H__

#include "stm32f10x.h"
#include "bsp_pwm.h"


#define BEEP_RCC                RCC_APB2Periph_GPIOA
#define BEEP_PROT               GPIOA
#define BEEP_PIN                GPIO_Pin_1

#define BEEP_ON()               GPIO_SetBits(BEEP_PROT, BEEP_PIN);
#define BEEP_OFF()              GPIO_ResetBits(BEEP_PROT, BEEP_PIN);

// �����λ	��������
enum __FAN_LEVEL {
    FAN_LEVEL_OFF               =   0,      // �ػ�/����״̬
    FAN_LEVEL_SILENT            =   1,	    // ����ģʽ
    FAN_LEVEL_AQL1_LOW          =   2,	    // ����ģʽ�¿�����/�ֶ�ģʽ����
    FAN_LEVEL_AQL2_MID          =   3,	    // ����ģʽ�¿�����/�ֶ�ģʽ����
    FAN_LEVEL_AQL3_HIG          =   4,	    // ����ģʽ�¿�����/�ֶ�ģʽ����
    FAN_LEVEL_MAX               =   5,	    // ����ģʽ
};

// ״̬��ģʽ����
enum __FAN_MODE {
    FAN_MODE_PWR_OFF            =   0,      // �ϵ�	δ�ϵ�״̬
    //FAN_MODE_PWR_ON             =   0,      // ����	�ϵ��Զ����У������λΪ0��������������
    FAN_MODE_SILENT             =   1,      // ����ģʽ	�����λ1��LED����ɫ������Ч��
    FAN_MODE_SMART              =   2,      // ����ģʽ	�����λ���ݴ����������Ŀ���������̬�仯2-4��LED����������ı�
    FAN_MODE_MAX_SPEED          =   3,      // ����ģʽ	�����λ5��LED����ɫ
    FAN_MODE_HAND               =   4,      // �ֶ�ģʽ	�ֶ��л�ģʽ2-4��LED������������ı�
};

// LED��ɫ��Ӧ������������
enum __LED_COLOR {
    LED_COLOR_BLE               =   0,      // ��ɫ	����ģʽ/�ֶ�ģʽ�µĿ����ţ���λ2
    LED_COLOR_YEL               =   1,      // ��ɫ	����ģʽ/�ֶ�ģʽ�µĿ���������λ3
    LED_COLOR_RED               =   2,      // ��ɫ	����ģʽ/�ֶ�ģʽ�µĿ������λ4
    LED_COLOR_GRE               =   3,      // ��ɫ	����ģʽ����λ5
    LED_COLOR_BLE_BRE           =   4,      // ��ɫ������	����ģʽ����λ1
    LED_COLOR_BLK               =   5,      // Ϩ��	����״̬/�ϵ�״̬
};

//enum __fanlevel {
//    OFFMODE = 0,        //�ػ�/����ģʽ
//    QUIETMODE,          //����ģʽ
//    LOWSPEEDMODE,       //����ģʽ
//    MEDIUMSPEEDMODE,    //����ģʽ
//    HIGHSPEEDMODE,      //����ģʽ
//    EXTREMESPEEDMODE,   //����ģʽ
//};
/***********************************************
* ������ ��������
*/
void ControlInit(void);
void FanSpeedControl(uint8_t fanlevel);
void RGBLedControl(uint8_t r_duty,uint8_t g_duty,uint8_t b_duty);



#endif