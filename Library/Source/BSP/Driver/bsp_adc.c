/*******************************************************************************
 *   Filename:       bsp_adc.c
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2014-08-11
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ADC模数转换模块（使用DMA传输数据）
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
 * 名    称： ADCx_GPIO_Config
 * 功    能： 初始化ADC使用的引脚
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
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
 * 名    称： ADCx_Mode_Config
 * 功    能： 配置ADCx的工作模式
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
 *******************************************************************************/
static void  ADCx_Mode_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* DMA channel1 configuration */
    //给DMA配置通道1
    DMA_DeInit(DMA1_Channel1);

    //设置DMA源：内存地址或者串口数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
    //内存地址（要传输的变量的指针）
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Value;      //内存地址
    //方向：单向传输
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置传输时缓冲区的长度（1就是一个Half-word16位）
    DMA_InitStructure.DMA_BufferSize = Channel_Times * Channel_Number;
    //外设地址固定
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
    //内存地址递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;        //内存地址固定
    //DMA在访问每次操作 数据长度
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//半字（16位）
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    //设置DMA的传输方式：循环传输模数
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输模式
    //DMA通道x的优先等级:高
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //DMA通道x禁止内存到内存的传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    //初始化DMA
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA channel1 */
    //使能DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration */
    ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	    //独立模式ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	        //开启扫描模式，扫描模式用于多通道采集
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	    //开启连续转换模式，即不停地进行ADC转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = Channel_Number;	//要转换的通道数目Channel_Number
    ADC_Init(ADC1, &ADC_InitStructure);

    //----------------------------------------------------------------------
    //ADC的转换时间与ADC的时钟和采样周期相关，下面就是配置ADC转换时间的函数
    //ADC采样时间计算公式：T = 采样周期+12.5个周期

    /*配置ADC时钟，为PCLK2的6分频，即12MHz*/
    //ADC时钟频率越高，转换速度越快，但ADC时钟有上限值（不超过14MHZ）
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /*配置ADC1的通道11为55.	5个采样周期，序列为1 */
    //RANK值是指在多通道扫描模式时，本通道的扫描顺序
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_1Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);

    //----------------------------------------------------------------------

    /* Enable ADC1 DMA */
    //使能ADC1 的DMA
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    //使能ADC
    ADC_Cmd(ADC1, ENABLE);

    //----------------------------------------------------------------------
    //在开始ADC转换之前，需要启动ADC的自校准
    /*复位校准寄存器 */
    ADC_ResetCalibration(ADC1);
    /*等待校准寄存器复位完成 */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* ADC校准 */
    ADC_StartCalibration(ADC1);
    /* 等待校准完成*/
    while(ADC_GetCalibrationStatus(ADC1));

    /* 由于没有采用外部触发，所以使用软件触发ADC转换 */
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}


/*******************************************************************************
 * 名    称： Bsp_ADC_Init
 * 功    能： ADC初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
 *******************************************************************************/
void Bsp_ADC_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}

/*******************************************************************************
 * 名    称： ADC1Convert_Begin
 * 功    能： 开始ADC1的采集和转换
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
 *******************************************************************************/
void ADC1Convert_Begin(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*******************************************************************************
 * 名    称： ADC1Convert_Stop
 * 功    能： 停止ADC1的采集和转换
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
 *******************************************************************************/
void ADC1Convert_Stop(void)
{
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}


/*******************************************************************************
 * 名    称： Get_AD_AverageValue
 * 功    能： 获取AD采集的平均值
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈.
 * 创建日期： 2015-06-25
 * 修    改：
 * 修改日期：
 * 备    注：
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