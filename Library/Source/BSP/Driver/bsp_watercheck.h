#ifndef __BSP_WATERCHECK_H__
#define __BSP_WATERCHECK_H__

#include "stm32f10x.h"
#include "global.h"

typedef struct {
    uint16 ShuiWei;         /*ˮλ*/
    uint16 ShuiXiangTemp;   /*ˮ���¶�*/
    uint16 JiReQiTemp;      /*�������¶�*/
    uint16 NeiXunHuanTemp;  /*��ѭ���¶�*/
    uint16 WaiXunHuanTemp;  /*��ѭ���¶�*/
    uint8  TemperatureError;/*�¶Ȳ��������̴���ָʾ*/
}SensorTypeDef;

void WaterCheck_GPIO_Config(void);
unsigned char WaterCheck_Height(void);

#endif