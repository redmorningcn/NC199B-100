#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "stm32f10x.h"

/***********************************************
* ������ LED���Ŷ���
*/
#define  LED01_R_RCC          RCC_APB2Periph_GPIOA
#define  LED01_R_PORT         GPIOA
#define  LED01_R_PIN          GPIO_Pin_8

#define  LED01_G_RCC          RCC_APB2Periph_GPIOA
#define  LED01_G_PORT         GPIOA
#define  LED01_G_PIN          GPIO_Pin_9

#define  LED01_B_RCC          RCC_APB2Periph_GPIOA
#define  LED01_B_PORT         GPIOA
#define  LED01_B_PIN          GPIO_Pin_10

#define  LED02_R_RCC          RCC_APB2Periph_GPIOA
#define  LED02_R_PORT         GPIOA
#define  LED02_R_PIN          GPIO_Pin_15

#define  LED02_G_RCC          RCC_APB2Periph_GPIOA
#define  LED02_G_PORT         GPIOA
#define  LED02_G_PIN          GPIO_Pin_12

#define  LED02_B_RCC          RCC_APB2Periph_GPIOA
#define  LED02_B_PORT         GPIOA
#define  LED02_B_PIN          GPIO_Pin_11

/***********************************************
* ������ ѡ���ĸ�LED
*/
enum SelectLedxTypeDef {
    APP_ALLLED    ,
    APP_LED01     ,
    APP_LED02     ,
};

/***********************************************
* ������ LED����ɫ
*/
enum LedxColorTypeDef {
    Color_All     ,
    Color_Red     ,
    Color_Green   ,
    Color_Bule    ,
};

/***********************************************
* ������ ��������
*/
void App_Led_Init(void);
void App_Led_On(unsigned char Ledx,unsigned char color);
void App_Led_Off(unsigned char Ledx,unsigned char color);
void App_Led_Toggle(unsigned char Ledx,unsigned char color);


#endif