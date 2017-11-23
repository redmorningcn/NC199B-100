/*******************************************************************************
*   Filename:       bsp_led.c
*   Revised:        All copyrights reserved to Roger-WY.
*   Date:           2014-08-18
*   Revision:       v1.0
*   Writer:	        Roger-WY.
*
*   Description:    LED����
*
*
*   Notes:
*
*******************************************************************************/

#include "bsp_led.h"
#include "includes.h"
#include "global.h"

#define BSP_LED_EN 1
#if BSP_LED_EN > 0

//============================================================================//

/*******************************************************************************
* ��    �ƣ� App_Led_Init
* ��    �ܣ� �û�LED��ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void App_Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED01_R_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED01_R_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED01_R_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(LED01_G_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED01_G_PIN ;
    GPIO_Init(LED01_G_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(LED01_B_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED01_B_PIN ;
    GPIO_Init(LED01_B_PORT, &GPIO_InitStructure);
    /***********************************************
     * ������LED02��R���Ÿ�����JTAG���������ţ������������䣬��JTAG������Ϊ��ͨIOʹ��
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //Ȼ�����GPIO��ӳ�亯������������ʵ����ӳ�䣺
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    RCC_APB2PeriphClockCmd(LED02_R_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED02_G_PIN ;
    GPIO_Init(LED02_G_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(LED02_G_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED02_B_PIN ;
    GPIO_Init(LED02_B_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(LED02_B_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = LED01_B_PIN ;
    GPIO_Init(LED02_B_PORT, &GPIO_InitStructure);

    /***********************************************
    * ������ �ر�����LED
    */
    GPIO_SetBits(LED01_R_PORT , LED01_R_PIN );
    GPIO_SetBits(LED01_G_PORT , LED01_G_PIN );
    GPIO_SetBits(LED01_B_PORT , LED01_B_PIN );
    GPIO_SetBits(LED02_R_PORT , LED01_R_PIN );
    GPIO_SetBits(LED02_G_PORT , LED01_G_PIN );
    GPIO_SetBits(LED02_B_PORT , LED01_B_PIN );
}

/*******************************************************************************
* ��    �ƣ� App_Led_On
* ��    �ܣ� ����LED��
* ��ڲ����� Ledx  ѡ���ĸ�LED  ��ȡֵ��Χ��APP_ALLLED/APP_LED01/APP_LED02��
*            color ��ѡ���LED���Ǹ���ɫ��ȡֵ��Χ��Color_All/Color_Red/Color_Green/Color_Bule��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void App_Led_On(unsigned char Ledx,unsigned char color)
{
    if(Ledx == APP_ALLLED) {
       switch(color)
       {
            case Color_All  :
                GPIO_ResetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_ResetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_ResetBits(LED01_B_PORT , LED01_B_PIN );
                GPIO_ResetBits(LED02_R_PORT , LED02_R_PIN );
                GPIO_ResetBits(LED02_G_PORT , LED02_G_PIN );
                GPIO_ResetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            case Color_Red  :
                GPIO_ResetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_ResetBits(LED02_R_PORT , LED02_R_PIN );
                break;
            case Color_Green:
                GPIO_ResetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_ResetBits(LED02_G_PORT , LED02_G_PIN );
                break;
            case Color_Bule :
                GPIO_ResetBits(LED01_B_PORT , LED01_B_PIN );
                GPIO_ResetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            default:break;
       }
    } else if (Ledx == APP_LED01) {
        switch(color)
        {
            case Color_All  :
                GPIO_ResetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_ResetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_ResetBits(LED01_B_PORT , LED01_B_PIN );
                break;
            case Color_Red  :
                GPIO_ResetBits(LED01_R_PORT , LED01_R_PIN );
                break;
            case Color_Green:
                GPIO_ResetBits(LED01_G_PORT , LED01_G_PIN );
                break;
            case Color_Bule :
                GPIO_ResetBits(LED01_B_PORT , LED01_B_PIN );
                break;
            default:break;
        }
    } else if(Ledx == APP_LED02) {
       switch(color)
       {
            case Color_All  :
                GPIO_ResetBits(LED02_R_PORT , LED02_R_PIN );
                GPIO_ResetBits(LED02_G_PORT , LED02_G_PIN );
                GPIO_ResetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            case Color_Red  :
                GPIO_ResetBits(LED02_R_PORT , LED02_R_PIN );
                break;
            case Color_Green:
                GPIO_ResetBits(LED02_G_PORT , LED02_G_PIN );
                break;
            case Color_Bule :
                GPIO_ResetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            default:break;
       }
    } else {
        //������
    }
}

/*******************************************************************************
* ��    �ƣ� App_Led_Off
* ��    �ܣ� ����LED��
* ��ڲ����� Ledx  ѡ���ĸ�LED  ��ȡֵ��Χ��APP_ALLLED/APP_LED01/APP_LED02��
*            color ��ѡ���LED���Ǹ���ɫ��ȡֵ��Χ��Color_All/Color_Red/Color_Green/Color_Bule��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void App_Led_Off(unsigned char Ledx,unsigned char color)
{
    if(Ledx == APP_ALLLED) {
       switch(color)
       {
            case Color_All  :
                GPIO_SetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_SetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_SetBits(LED01_B_PORT , LED01_B_PIN );
                GPIO_SetBits(LED02_R_PORT , LED02_R_PIN );
                GPIO_SetBits(LED02_G_PORT , LED02_G_PIN );
                GPIO_SetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            case Color_Red  :
                GPIO_SetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_SetBits(LED02_R_PORT , LED02_R_PIN );
                break;
            case Color_Green:
                GPIO_SetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_SetBits(LED02_G_PORT , LED02_G_PIN );
                break;
            case Color_Bule :
                GPIO_SetBits(LED01_B_PORT , LED01_B_PIN );
                GPIO_SetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            default:break;
       }
    } else if (Ledx == APP_LED01) {
        switch(color)
        {
            case Color_All  :
                GPIO_SetBits(LED01_R_PORT , LED01_R_PIN );
                GPIO_SetBits(LED01_G_PORT , LED01_G_PIN );
                GPIO_SetBits(LED01_B_PORT , LED01_B_PIN );
                break;
            case Color_Red  :
                GPIO_SetBits(LED01_R_PORT , LED01_R_PIN );
                break;
            case Color_Green:
                GPIO_SetBits(LED01_G_PORT , LED01_G_PIN );
                break;
            case Color_Bule :
                GPIO_SetBits(LED01_B_PORT , LED01_B_PIN );
                break;
            default:break;
        }
    } else if(Ledx == APP_LED02) {
       switch(color)
       {
            case Color_All  :
                GPIO_SetBits(LED02_R_PORT , LED02_R_PIN );
                GPIO_SetBits(LED02_G_PORT , LED02_G_PIN );
                GPIO_SetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            case Color_Red  :
                GPIO_SetBits(LED02_R_PORT , LED02_R_PIN );
                break;
            case Color_Green:
                GPIO_SetBits(LED02_G_PORT , LED02_G_PIN );
                break;
            case Color_Bule :
                GPIO_SetBits(LED02_B_PORT , LED02_B_PIN );
                break;
            default:break;
       }
    } else {
        //������
    }
}

/*******************************************************************************
* ��    �ƣ� App_Led_Toggle
* ��    �ܣ� ��Led״̬ȡ��
* ��ڲ����� Ledx  ѡ���ĸ�LED  ��ȡֵ��Χ��APP_ALLLED/APP_LED01/APP_LED02��
*            color ��ѡ���LED���Ǹ���ɫ��ȡֵ��Χ��Color_All/Color_Red/Color_Green/Color_Bule��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
void App_Led_Toggle(unsigned char Ledx,unsigned char color)
{
    if(Ledx == APP_ALLLED) {
       switch(color)
       {
            case Color_All  :
                GPIO_WriteBit(LED01_R_PORT, LED01_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_R_PORT, LED01_R_PIN))));
                GPIO_WriteBit(LED01_G_PORT, LED01_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_G_PORT, LED01_G_PIN))));
                GPIO_WriteBit(LED01_B_PORT, LED01_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_B_PORT, LED01_B_PIN))));
                GPIO_WriteBit(LED02_R_PORT, LED02_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_R_PORT, LED02_R_PIN))));
                GPIO_WriteBit(LED02_G_PORT, LED02_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_G_PORT, LED02_G_PIN))));
                GPIO_WriteBit(LED02_B_PORT, LED02_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_B_PORT, LED02_B_PIN))));
                break;
            case Color_Red  :
                GPIO_WriteBit(LED01_R_PORT, LED01_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_R_PORT, LED01_R_PIN))));
                GPIO_WriteBit(LED02_R_PORT, LED02_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_R_PORT, LED02_R_PIN))));
                break;
            case Color_Green:
                GPIO_WriteBit(LED01_G_PORT, LED01_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_G_PORT, LED01_G_PIN))));
                GPIO_WriteBit(LED02_G_PORT, LED02_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_G_PORT, LED02_G_PIN))));
                break;
            case Color_Bule :
                GPIO_WriteBit(LED01_B_PORT, LED01_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_B_PORT, LED01_B_PIN))));
                GPIO_WriteBit(LED02_B_PORT, LED02_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_B_PORT, LED02_B_PIN))));
                break;
            default:break;
       }
    } else if (Ledx == APP_LED01) {
        switch(color)
        {
            case Color_All  :
                GPIO_WriteBit(LED01_R_PORT, LED01_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_R_PORT, LED01_R_PIN))));
                GPIO_WriteBit(LED01_G_PORT, LED01_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_G_PORT, LED01_G_PIN))));
                GPIO_WriteBit(LED01_B_PORT, LED01_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_B_PORT, LED01_B_PIN))));
                break;
            case Color_Red  :
                GPIO_WriteBit(LED01_R_PORT, LED01_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_R_PORT, LED01_R_PIN))));
                break;
            case Color_Green:
                GPIO_WriteBit(LED01_G_PORT, LED01_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_G_PORT, LED01_G_PIN))));
                break;
            case Color_Bule :
                GPIO_WriteBit(LED01_B_PORT, LED01_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED01_B_PORT, LED01_B_PIN))));
                break;
            default:break;
        }
    } else if(Ledx == APP_LED02) {
       switch(color)
       {
            case Color_All  :
                GPIO_WriteBit(LED02_R_PORT, LED02_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_R_PORT, LED02_R_PIN))));
                GPIO_WriteBit(LED02_G_PORT, LED02_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_G_PORT, LED02_G_PIN))));
                GPIO_WriteBit(LED02_B_PORT, LED02_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_B_PORT, LED02_B_PIN))));
                break;
            case Color_Red  :
                GPIO_WriteBit(LED02_R_PORT, LED02_R_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_R_PORT, LED02_R_PIN))));
                break;
            case Color_Green:
                GPIO_WriteBit(LED02_G_PORT, LED02_G_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_G_PORT, LED02_G_PIN))));
                break;
            case Color_Bule :
                GPIO_WriteBit(LED02_B_PORT, LED02_B_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED02_B_PORT, LED02_B_PIN))));
                break;
            default:break;
       }
    } else {
        //������
    }
}

#endif


/*************************end of file*****************************************/
