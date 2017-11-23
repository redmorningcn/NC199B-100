#include "stm32f10x.h"
#include <stdio.h>
#include "SZ_STM32F107VC_LIB.h"
#include "delay.h"
#include "WatchDog.h"

#define WDG_CLK      		       RCC_APB2Periph_GPIOA
#define WDG_PIN                    GPIO_Pin_8
#define WDG_PORT                   GPIOA

#define WDG_LOW()                  GPIO_ResetBits(WDG_PORT,WDG_PIN)
#define WDG_HIGH()                 GPIO_SetBits(WDG_PORT,WDG_PIN)

/**-------------------------------------------------------
  * @������ WDG_Init
  * @����   ��ʼ��WDG�Ķ˿�
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
void WDG_Init(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	//
	///* Enable GPIOD clock */
	//RCC_APB2PeriphClockCmd(WDG_CLK , ENABLE);
    //
	//GPIO_InitStructure.GPIO_Pin = WDG_PIN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_Init(WDG_PORT, &GPIO_InitStructure);
}

void FeedDog_LOW(void)
{
	//WDG_LOW();
}


void FeedDog_HIGH(void)
{
    //WDG_HIGH();
}


