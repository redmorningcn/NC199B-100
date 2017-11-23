/*******************************************************************************
 *   Filename:      bsp_pm25.h
 *   Revised:       $Date: 2015-07-11
 *   Revision:      $V1.0
 *   Writer:        wumingshen.
 *
 *   Description:   PM2.5�ҳ���������ģ��ͷ�ļ�
 *   Notes:         ����������һ���ҳ���������ͨ���ɼ�ADֵ����ȡPM2.5��ֵ
 *
 *
 *   All copyrights reserved to wumingshen.
 *
 *******************************************************************************/
#ifndef __BSP_PM25_H__
#define __BSP_PM25_H__


#include "includes.h"

#define  PM25LED_RCC    RCC_APB2Periph_GPIOA
#define  PM25LED_PIN    GPIO_Pin_4
#define  PM25LED_PORT   GPIOA

#define  PM25LEDON()    GPIO_SetBits(PM25LED_PORT, PM25LED_PIN);
#define  PM25LEDOFF()   GPIO_ResetBits(PM25LED_PORT, PM25LED_PIN);


#define PM2D5_CHK_TYPE_DEF      0                   // Ĭ��
#define PM2D5_CHK_TYPE_K_Y1     1                   // �͵�
#define PM2D5_CHK_TYPE_K_Y2     2                   // �ߵ�
#define PM2D5_CHK_TYPE_B_OFFSET 3                   // ���ƫ��
#define PM2D5_CHK_TYPE_OFFSET   4                   // ����ƫ��
     
     
/***********************************************
* ������ ��ʼ��2������
*/
#define _k  6.0
#define _b  80
//1��Ĭ��ֵ
#define pm_x1          (4096 * 0.60 / 3.3)  //676 
#define pm_y1          (_k * 0 + _b)        // 0 
#define pm_x2          (4096 * 2.98 / 3.3)  //717  
#define pm_y2          (_k * 400 + _b)      // 46
#define pm_k           (pm_y2-pm_y1)/(pm_x2-pm_x1))//(1.5*(pm_y2-pm_y1)/(pm_x2-pm_x1))
#define pm_b           (pm_y1 - pm_k *pm_x1)
// 2��Ĭ��ֵ
#define pm_X1          (0 / 5)
#define pm_Y1           0
#define pm_X2          (1024 * ((1.4+0.78)/2.0) / 5.0)
#define pm_Y2           400
#define pm_K           ((pm_Y2-pm_Y1)/(pm_X2-pm_X1))
#define pm_B           (pm_Y1 - pm_K *pm_X1)

/***********************************************
* ������ PM2.5�ȼ��� �š������
*/
enum __pmlevel {
    EXCELLENCE = 0,
    FINE,
    BAD,
};

typedef struct {
    u8      Pm25Type            : 8;            // 0 Ϊ1����1Ϊ2��
    u16     Pm25AdVal;
    u16     Pm25RealVal;
} PmDataTypeDef;

typedef struct {
    s16     offset;
    s16     x1;
    s16     y1;
    s16     x2;
    s16     y2;
    s8      checked;
} StrChk;

extern u16      Pm25AdVal[];
extern StrChk   chk;

/***********************************************
* ������ �ⲿ���ú�������
*/
void        BSP_PM2D5Init           (void);
int16_t     BSP_PM2D5GetValue       (u8 type);
uint8_t     BSP_PM2D5GetLevel       (u16 __upm);

void        BSP_PM2D5Chk            (u8 type, s16 value);
#endif