/*******************************************************************************
 *   Filename:       bsp_watercheck.c
 *   Revised:        All copyrights reserved to Snlion.
 *   Date:           2014-08-18
 *   Revision:       v1.0
 *   Writer:	     Snlion-WY.
 *
 *   Description:    ʹ���ĸ�IO��״̬���ˮλ
 *
 *
 *   Notes:          IO�����ߵ�ƽ��˵��ˮλ��������IO���ĸ߶ȡ�
 *
 *******************************************************************************/

#include "bsp_watercheck.h"


/*******************************************************************************
 * ��    �ƣ� WaterCheck_GPIO_Config
 * ��    �ܣ� ˮλ���IO������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Snlion-WY
 * �������ڣ� 2014-08-18
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void WaterCheck_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*******************************************************************************
 * ��    �ƣ� WaterCheck_Height
 * ��    �ܣ� ���ˮλ�ĸ߶�
 * ��ڲ����� ��
 * ���ڲ����� ��Ӧ�߶ȵ�ֵ
 * �������ߣ� Snlion-WY
 * �������ڣ� 2014-08-18
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ����˿�1λ������Ĳ����㡣����ֵԽ��ˮλԽ�ߡ�
 *******************************************************************************/
unsigned char WaterCheck_Height(void)
{
    uint8_t Port1Status = 0x00;
    uint8_t Port2Status = 0x00;
    uint8_t Port3Status = 0x00;
    uint8_t Port4Status = 0x00;

    uint8_t ReturnValue = 0;

    Port1Status = (uint8_t)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
    Port2Status = (uint8_t)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
    Port3Status = (uint8_t)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
    Port4Status = (uint8_t)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);


    if((Port1Status == 1)&&(Port2Status == 0)&&(Port3Status == 0)&&(Port4Status == 0))
    {
        ReturnValue = 2;
    }else if((Port1Status == 1)&&(Port2Status == 1)&&(Port3Status == 0)&&(Port4Status == 0))
    {
        ReturnValue = 3;
    }else if((Port1Status == 1)&&(Port2Status == 1)&&(Port3Status == 1)&&(Port4Status == 0))
    {
        ReturnValue = 4;
    }else if((Port1Status == 1)&&(Port2Status == 1)&&(Port3Status == 1)&&(Port4Status == 1))
    {
        ReturnValue = 5;
    }else
    {
        ReturnValue = 1;
    }

    return (ReturnValue);

}