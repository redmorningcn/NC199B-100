/*******************************************************************************
*   Filename:       bsp_control.c
*   Revised:        All copyrights reserved to Roger.
*   Date:           2014-08-18
*   Revision:       v1.0
*   Writer:	        wumingshen.
*
*   Description:    �����źţ����/RGB��ɫ�壩����ģ��
*
*
*   Notes:
*
*******************************************************************************/
#include "includes.h"
#include "global.h"
#include "bsp_control.h"



//============================================================================//

/*******************************************************************************
* ��    �ƣ� BeepGpioInit
* ��    �ܣ� ���������ų�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void BeepGpioInit(void)
{
    //GPIO_InitTypeDef  GPIO_InitStructure;
	////GPIOA Periph clock enable
	//RCC_APB2PeriphClockCmd(BEEP_RCC, ENABLE);
    ///* Set PA.1 GPIO_Mode_AF_PP*/
    //GPIO_InitStructure.GPIO_Pin     = BEEP_PIN;
    //GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;     // ͨ���������
    //GPIO_Init(BEEP_PROT, &GPIO_InitStructure);
}

/*******************************************************************************
* ��    �ƣ� ControlInit
* ��    �ܣ� �����ź���س�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void ControlInit(void)
{
   LedPWM_Init();
   FanPWM_Init();
}

/*******************************************************************************
* ��    �ƣ� FanSpeedControl
* ��    �ܣ� ����ȼ�����
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ������ 0-5 ����Խ��Խ��
*******************************************************************************/
void FanSpeedControl(uint8_t fanlevel)
{
//    static uint8_t lastlevel    = 0;

    switch(fanlevel) {
        /***********************************************
        * ������ �ػ�/����ģʽ
        */
        case FAN_LEVEL_OFF:{
            FanPWM_SetDuty(0,0);
        };break;
        /***********************************************
        * ������ ����ģʽ
        */
        case FAN_LEVEL_SILENT:{
            FanPWM_SetDuty(0,68);
        };break;
        /***********************************************
        * ������ ����ģʽ
        */
        case FAN_LEVEL_AQL1_LOW:{
             FanPWM_SetDuty(0,72);
        };break;
        /***********************************************
        * ������ ����ģʽ
        */
        case FAN_LEVEL_AQL2_MID:{
             FanPWM_SetDuty(0,82);
        };break;
        /***********************************************
        * ������ ����ģʽ
        */
        case FAN_LEVEL_AQL3_HIG:{
             FanPWM_SetDuty(0,85);
        };break;
        /***********************************************
        * ������ ����ģʽ
        */
        case FAN_LEVEL_MAX:{
             FanPWM_SetDuty(0,90);
        };break;
        default:break;
    }
}

/*******************************************************************************
* ��    �ƣ� FanSpeedControl
* ��    �ܣ� ����ȼ�����
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����RGB��ռ�ձ�
*******************************************************************************/
void RGBLedControl(uint8_t r_duty,uint8_t g_duty,uint8_t b_duty)
{
    LedPWM_SetDuty(0,r_duty);
    LedPWM_SetDuty(1,g_duty);
    LedPWM_SetDuty(2,b_duty);
}





/*************end of file***************/