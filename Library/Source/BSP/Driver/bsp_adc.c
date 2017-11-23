/*******************************************************************************
 *   Filename:       bsp_adc.c
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2014-08-11
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ADCģ��ת��ģ�飨ʹ��DMA�������ݣ�
 *
 *
 *   Notes:
 *
 *   All copyrights reserved to wumingshen
 *******************************************************************************/
#include "bsp_adc.h"


__IO uint16_t ADC_Value[Channel_Times][Channel_Number];
__IO uint16_t ADC_AverageValue[Channel_Number];

//============================================================================//


/*******************************************************************************
 * ��    �ƣ� ADCx_GPIO_Config
 * ��    �ܣ� ��ʼ��ADCʹ�õ�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5
                                    | GPIO_Pin_6
                                    | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��    �ƣ� ADCx_Mode_Config
 * ��    �ܣ� ����ADCx�Ĺ���ģʽ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
static void  ADCx_Mode_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* DMA channel1 configuration */
    //��DMA����ͨ��1
    DMA_DeInit(DMA1_Channel1);

    //����DMAԴ���ڴ��ַ���ߴ������ݼĴ�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
    //�ڴ��ַ��Ҫ����ı�����ָ�룩
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Value;      //�ڴ��ַ
    //���򣺵�����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //���ô���ʱ�������ĳ��ȣ�1����һ��Half-word16λ��
    DMA_InitStructure.DMA_BufferSize = Channel_Times * Channel_Number;
    //�����ַ�̶�
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
    //�ڴ��ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;        //�ڴ��ַ�̶�
    //DMA�ڷ���ÿ�β��� ���ݳ���
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���֣�16λ��
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    //����DMA�Ĵ��䷽ʽ��ѭ������ģ��
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������ģʽ
    //DMAͨ��x�����ȵȼ�:��
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //DMAͨ��x��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    //��ʼ��DMA
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA channel1 */
    //ʹ��DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration */
    ADC_DeInit(ADC1); //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	    //����ģʽADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	        //����ɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	    //��������ת��ģʽ������ͣ�ؽ���ADCת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = Channel_Number;	//Ҫת����ͨ����ĿChannel_Number
    ADC_Init(ADC1, &ADC_InitStructure);

    //----------------------------------------------------------------------
    //ADC��ת��ʱ����ADC��ʱ�ӺͲ���������أ������������ADCת��ʱ��ĺ���
    //ADC����ʱ����㹫ʽ��T = ��������+12.5������

    /*����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz*/
    //ADCʱ��Ƶ��Խ�ߣ�ת���ٶ�Խ�죬��ADCʱ��������ֵ��������14MHZ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /*����ADC1��ͨ��11Ϊ55.	5���������ڣ�����Ϊ1 */
    //RANKֵ��ָ�ڶ�ͨ��ɨ��ģʽʱ����ͨ����ɨ��˳��
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_1Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);

    //----------------------------------------------------------------------

    /* Enable ADC1 DMA */
    //ʹ��ADC1 ��DMA
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    //ʹ��ADC
    ADC_Cmd(ADC1, ENABLE);

    //----------------------------------------------------------------------
    //�ڿ�ʼADCת��֮ǰ����Ҫ����ADC����У׼
    /*��λУ׼�Ĵ��� */
    ADC_ResetCalibration(ADC1);
    /*�ȴ�У׼�Ĵ�����λ��� */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ADCУ׼ */
    ADC_StartCalibration(ADC1);
    /* �ȴ�У׼���*/
    while(ADC_GetCalibrationStatus(ADC1));

    /* ����û�в����ⲿ����������ʹ���������ADCת�� */
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}


/*******************************************************************************
 * ��    �ƣ� Bsp_ADC_Init
 * ��    �ܣ� ADC��ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void Bsp_ADC_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}

/*******************************************************************************
 * ��    �ƣ� ADC1Convert_Begin
 * ��    �ܣ� ��ʼADC1�Ĳɼ���ת��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void ADC1Convert_Begin(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*******************************************************************************
 * ��    �ƣ� ADC1Convert_Stop
 * ��    �ܣ� ֹͣADC1�Ĳɼ���ת��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void ADC1Convert_Stop(void)
{
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}


/*******************************************************************************
 * ��    �ƣ� Get_AD_AverageValue
 * ��    �ܣ� ��ȡAD�ɼ���ƽ��ֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void Get_AD_AverageValue(void)
{
    uint8_t count;
    uint8_t i;
    uint32_t sum = 0;

    for(i=0;i<Channel_Number;i++) {
        for(count=0;count<Channel_Times;count++) {
            sum += ADC_Value[count][i];
        }
        ADC_AverageValue[i] = sum/Channel_Times;
        sum = 0;
    }
}

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/