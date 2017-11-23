/*******************************************************************************
 *   Filename:       bsp_pwm.c
 *   Revised:        All copyrights reserved to Wuming Shen.
 *   Date:           2014-07-05
 *   Revision:       v1.0
 *   Writer:	     Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <bsp_pwm.h>

#define BSP_PWM_MODULE_EN 1
#if BSP_PWM_MODULE_EN > 0

/*******************************************************************************
 * ������ Ƶ�ʵ�λΪHZ
 */
#define FANPWM_FREQ            10000
#define LEDPWM_FREQ            10000
#define BEEPPWM_FREQ           2000

#define FANPWM_DUTY_MAX        100
#define LEDPWM_DUTY_MAX        100
#define BEEPPWM_DUTY_MAX       100
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
//StrPwm   FanPwm   = {
//    TIM5,
//    FANPWM_FREQ,        FANPWM_FREQ,        FANPWM_FREQ,
//    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,
//    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,
//    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,
//    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,    FANPWM_DUTY_MAX,
//    0,                  0,                  0,
//    0,                  2,                  0
//};

StrPwm   LedPwm   = {
    TIM1,
    LEDPWM_FREQ,        LEDPWM_FREQ,        LEDPWM_FREQ,
    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,
    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,
    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,
    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,    LEDPWM_DUTY_MAX,
    0,                  0,                  0,
    1,                  2,                  3
};

StrPwm   BeepPwm   = {
    TIM2,
    BEEPPWM_FREQ,       BEEPPWM_FREQ,       BEEPPWM_FREQ,
    BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,
    BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,
    BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,
    BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,   BEEPPWM_DUTY_MAX,
    0,                  0,                  0,
    1,                  0,                  0
};
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void    FanPWM_GPIO_Configuration      (void);
void    FanPWM_TIM_Configuration       (void);
void    BeepPWM_TIM_Configuration      (void);
void    LedPWM_GPIO_Configuration      (void);
void    LedPWM_TIM_Configuration       (void);
void    BeepPWM_GPIO_Configuration     (void);
void    BeepPWM_TIM_Configuration      (void);

/*******************************************************************************
 * ��    �ƣ� FanPWM_Init
 * ��    �ܣ� ���PWM��ʼ��
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
StrPwm *FanPWM_Init(void)
{
    //memcpy(&LedPwm,&FanPwm,sizeof(StrPwm));
    FanPWM_GPIO_Configuration();
    FanPWM_TIM_Configuration();

    //PWM_Start(&FanPwm);
#if ( PWM_OUT_NEGTIVE == DEF_ENABLED )
    PWM_SetProperty(&FanPwm, PWM_CH_FAN, FANPWM_FREQ, 100.0);
#else
    PWM_SetProperty(&FanPwm, PWM_CH_FAN, FANPWM_FREQ, 0.0);
#endif
    return &FanPwm;
}
/*******************************************************************************
 * ��    �ƣ� LedPWM_Init
 * ��    �ܣ� RGB��ɫLEDPWM
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
StrPwm *LedPWM_Init(void)
{
    LedPWM_GPIO_Configuration();
    LedPWM_TIM_Configuration();

    PWM_Start(&LedPwm);
    PWM_SetProperty(&LedPwm, 0, LEDPWM_FREQ, 0);
    PWM_SetProperty(&LedPwm, 1, LEDPWM_FREQ, 0);
    PWM_SetProperty(&LedPwm, 2, LEDPWM_FREQ, 0);
    PWM_SetProperty(&LedPwm, 3, LEDPWM_FREQ, 0);

    return &LedPwm;
}

/*******************************************************************************
 * ��    �ƣ� BeepPWM_Init
 * ��    �ܣ� ������PWM��ʼ��
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
StrPwm *BeepPWM_Init(void)
{
    BeepPWM_GPIO_Configuration();
    BeepPWM_TIM_Configuration();
    PWM_Start(&BeepPwm);

    PWM_SetProperty(&BeepPwm, 1, BEEPPWM_FREQ, 0);

    return &BeepPwm;
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_GPIO_Configuration
 * ��    �ܣ� �����������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void FanPWM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
#if ( PWM_OUT_NEGTIVE == DEF_ENABLED )
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;     // ��©���
#else
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_OD;     // ��©���
#endif
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��    �ƣ� LedPWM_GPIO_Configuration
 * ��    �ܣ� RGB��ɫLED��������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void LedPWM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9
                                    | GPIO_Pin_10
                                    | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;     // �����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��    �ƣ� BeepPWM_GPIO_Configuration
 * ��    �ܣ� ��������������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepPWM_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;     // ��©���
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_TIM_Configuration
 * ��    �ܣ� ���PWM��ʱ������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void FanPWM_TIM_Configuration (void)
{
    /***********************************************
    * ������ 
    */
    //TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    //TIM_OCInitTypeDef         TIM_OCInitStructure;
    //TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;

    /***********************************************
    * ������ 
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    /***********************************************
    * ������ 
    */
    FanPwm.freq[0]   = FanPwm.freq[1]   = FanPwm.freq[2]    = FanPwm.freq[3]    = FANPWM_FREQ;
    FanPwm.period[0] = FanPwm.period[1] = FanPwm.period[2]  = FanPwm.period[3]  = 1000000 / (uint32_t)FanPwm.freq[0]-1;
    FanPwm.TIMx      = TIM1;

    /***********************************************
    * ������ ����������ʱ����������
    */
    //TIM_TimeBaseStructure.TIM_Prescaler         = SYS_CLK-1;                     //�Զ�ʱ��ʱ��TIMxCLK����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
    //TIM_TimeBaseStructure.TIM_Period            = FanPwm.period[0];         //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����  FanPwm.period[0]
    //TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;             //������Ƶ(����Ƶ)
    //TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;       //���ϼ���
    //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                        //�ظ��Ĵ����������Զ�����FanPwmռ�ձ�
    //TIM_TimeBaseInit(LedPwm.TIMx, &TIM_TimeBaseStructure);

    /***********************************************
    * ������ ���Ĳ�LedPwm�������
    */
    //TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM2;          //����ΪPwm1���ģʽ(100%����ߵ�ƽ)
    //TIM_OCInitStructure.TIM_Pulse               = LedPwm.compare[0] ;       //����ռ�ձ�ʱ��
    //TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_High;      //��������ֵ�������������������ֵʱ����ƽ�������� FanPwm.compare[0]
    //TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;   //��Ч��ƽ�ļ��ԣ�����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ
    /***********************************************
    * ������ ���漸�������Ǹ߼���ʱ���Ż��õ���ͨ�ö�ʱ����������
    */
    //TIM_OCInitStructure.TIM_OCNPolarity         = TIM_OCNPolarity_High;     //���û������������
    //TIM_OCInitStructure.TIM_OutputNState        = TIM_OutputNState_Disable; //ʹ�ܻ��������
    //TIM_OCInitStructure.TIM_OCIdleState         = TIM_OCIdleState_Reset;    //���������״̬
    //TIM_OCInitStructure.TIM_OCNIdleState        = TIM_OCNIdleState_Reset;   //�����󻥲������״̬
    
    /***********************************************
    * ������ 
    */
    //TIM_OC1Init(FanPwm.TIMx,&TIM_OCInitStructure);                          //����ָ��������ʼ��
}

/*******************************************************************************
 * ��    �ƣ� LedPWM_TIM_Configuration
 * ��    �ܣ� RGB��ɫLED-PWM��ʱ������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void LedPWM_TIM_Configuration (void)
{
    /***********************************************
    * ������ ��ʱ��������
    */
    //TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;

    /***********************************************
    * ������ ʹ��ʱ��
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    //TIM1 ʹ���ڲ�ʱ��
    //TIM_InternalClockConfig(TIM1);
    //TIM1��������
    //����Ԥ��Ƶ����Ƶϵ��71����APB2=72M, TIM1_CLK=72/72=1MHz
    //TIM_Period��TIM1_ARR��=1000�����������ϼ�����1000����������¼�������ֵ����
    //���ϼ���ģʽ
    //    TIM_RepetitionCounter(TIM1_RCR)   =0��ÿ��������������������¼�
    //    TIM_BaseInitStructure.TIM_Period  = 50000;
    //    TIM_BaseInitStructure.TIM_Prescaler = 71;
    //    TIM_BaseInitStructure.TIM_ClockDivision = 0;
    //    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    //    TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
    //    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
    //    //���жϣ�����һ�����жϺ����������ж�
    //    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    //    //ʹ��TIM1�ж�Դ
    //    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    //    //TIM1�ܿ��أ�����
    //    TIM_Cmd(TIM1, ENABLE);TIMx->ARR
    /***********************************************
    * ������ ��ʼ���ṹ��
    */
    LedPwm.freq[0]   = LedPwm.freq[1]   = LedPwm.freq[2]    = LedPwm.freq[3]    = LEDPWM_FREQ;
    LedPwm.period[0] = LedPwm.period[1] = LedPwm.period[2]  = LedPwm.period[3]  = 1000000 / (uint32_t)LedPwm.freq[0]-1;
    LedPwm.TIMx      = TIM1;

    /***********************************************
    * ������ ����������ʱ����������
    */
    TIM_TimeBaseStructure.TIM_Prescaler         = BSP_CPU_ClkFreq_MHz-1;    //ʱ��Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Period            = LedPwm.period[0];         //�Զ���װ�ؼĴ�����ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;             //������Ƶ(����Ƶ)
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;       //���ϼ���
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                        //�ظ��Ĵ����������Զ�����FanPwmռ�ձ�
    TIM_TimeBaseInit(LedPwm.TIMx, &TIM_TimeBaseStructure);

    /***********************************************
    * ������ ���Ĳ�FanPwm�������
    */
#if ( PWM_OUT_NEGTIVE == DEF_ENABLED )
    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;          //����ΪPwm2���ģʽ(0%����ߵ�ƽ)
    TIM_OCInitStructure.TIM_Pulse               = LedPwm.compare[0] ;       //����ռ�ձ�ʱ��
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_High;      //�����������
#else
    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM2;          //����ΪPwm2���ģʽ(0%����ߵ�ƽ)
    TIM_OCInitStructure.TIM_Pulse               = LedPwm.compare[0] ;       //����ռ�ձ�ʱ��
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_High;      //�����������
#endif
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;   //ʹ�ܸ�ͨ�����

    /***********************************************
    * ������ ���漸�������Ǹ߼���ʱ���Ż��õ���ͨ�ö�ʱ����������
    */
    TIM_OCInitStructure.TIM_OCNPolarity         = TIM_OCNPolarity_High;     //���û������������
    TIM_OCInitStructure.TIM_OutputNState        = TIM_OutputNState_Disable; //ʹ�ܻ��������
    TIM_OCInitStructure.TIM_OCIdleState         = TIM_OCIdleState_Reset;    //���������״̬
    TIM_OCInitStructure.TIM_OCNIdleState        = TIM_OCNIdleState_Reset;   //�����󻥲������״̬

    /***********************************************
    * ������ ��ʼ����ͨ��1���
    */
    TIM_OC1Init(LedPwm.TIMx,&TIM_OCInitStructure);                          //����ָ��������ʼ��
   
    /***********************************************
    * ������ ���Ĳ�LedPwm�������
    */
    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;          //����ΪPwm1���ģʽ(100%����ߵ�ƽ)
    TIM_OCInitStructure.TIM_Pulse               = LedPwm.compare[0] ;       //����ռ�ձ�ʱ��
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_High;      //�����������
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;   //ʹ�ܸ�ͨ�����

    /***********************************************
    * ������ ���漸�������Ǹ߼���ʱ���Ż��õ���ͨ�ö�ʱ����������
    */
    TIM_OCInitStructure.TIM_OCNPolarity         = TIM_OCNPolarity_High;     //���û������������
    TIM_OCInitStructure.TIM_OutputNState        = TIM_OutputNState_Disable; //ʹ�ܻ��������
    TIM_OCInitStructure.TIM_OCIdleState         = TIM_OCIdleState_Reset;    //���������״̬
    TIM_OCInitStructure.TIM_OCNIdleState        = TIM_OCNIdleState_Reset;   //�����󻥲������״̬

    /***********************************************
    * ������ ��ʼ��ͨ��2��3��4�Ƚ����
    */
    TIM_OC2Init(LedPwm.TIMx,&TIM_OCInitStructure);
    TIM_OC3Init(LedPwm.TIMx,&TIM_OCInitStructure);
    TIM_OC4Init(LedPwm.TIMx,&TIM_OCInitStructure);

    /***********************************************
    * ������ ���岽��������ɲ���������ã��߼���ʱ�����еģ�ͨ�ö�ʱ����������
    */
    TIM_BDTRInitStructure.TIM_OSSRState         = TIM_OSSRState_Disable;    //����ģʽ�����ѡ��
    TIM_BDTRInitStructure.TIM_OSSIState         = TIM_OSSIState_Disable;    //����ģʽ�����ѡ��
    TIM_BDTRInitStructure.TIM_LOCKLevel         = TIM_LOCKLevel_OFF;        //��������
    TIM_BDTRInitStructure.TIM_DeadTime          = 0x90;                     //����ʱ������
    TIM_BDTRInitStructure.TIM_Break             = TIM_Break_Disable;        //ɲ������ʹ��
    TIM_BDTRInitStructure.TIM_BreakPolarity     = TIM_BreakPolarity_High;   //ɲ�����뼫��
    TIM_BDTRInitStructure.TIM_AutomaticOutput   = TIM_AutomaticOutput_Enable;//�Զ����ʹ��
    TIM_BDTRConfig(LedPwm.TIMx,&TIM_BDTRInitStructure);

    /***********************************************
    * ������ ��������ʹ�ܶ˵Ĵ�
    */
    TIM_OC1PreloadConfig(LedPwm.TIMx, TIM_OCPreload_Enable);                //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
    TIM_OC2PreloadConfig(LedPwm.TIMx, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(LedPwm.TIMx, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(LedPwm.TIMx, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(LedPwm.TIMx, ENABLE);                              //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
    TIM_Cmd(LedPwm.TIMx,ENABLE);                                            //��TIM1
    
    /***********************************************
    * ������ ��������Ǹ߼���ʱ�����еģ����FanPwm�����
    */
    TIM_CtrlPWMOutputs(LedPwm.TIMx, ENABLE);                                //LedPwm���ʹ�ܣ�һ��Ҫ�ǵô�
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_TIM_Configuration
 * ��    �ܣ� ���PWM��ʱ������
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepPWM_TIM_Configuration (void)
{
    /***********************************************
    * ������ 
    */
    //TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    //TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;

    /***********************************************
    * ������ 
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /***********************************************
    * ������ 
    */
    BeepPwm.freq[0]   = BeepPwm.freq[1]   = BeepPwm.freq[2]   = BeepPwm.freq[3]   = BEEPPWM_FREQ;
    BeepPwm.period[0] = BeepPwm.period[1] = BeepPwm.period[2] = BeepPwm.period[3] = 1000000 / (uint32_t)BeepPwm.freq[0] - 1;
    BeepPwm.TIMx      = TIM2;

    /***********************************************
    * ������ 
    */
    TIM_TimeBaseStructure.TIM_Prescaler = BSP_CPU_ClkFreq_MHz-1;    //�Զ�ʱ��ʱ��TIMxCLK����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
    TIM_TimeBaseStructure.TIM_Period = BeepPwm.period[0];           //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����  FanPwm.period[0]
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        //����ʱ�ӷ�Ƶϵ��������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //���ϼ���ģʽ
    TIM_TimeBaseInit(BeepPwm.TIMx, &TIM_TimeBaseStructure);         //ʱ����ʼ��

    /***********************************************
    * ������ 
    */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	            //����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�������״̬Ϊʹ�����
    TIM_OCInitStructure.TIM_Pulse = BeepPwm.compare[0];	            //��������ֵ�������������������ֵʱ����ƽ�������� FanPwm.compare[0]
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       //��Ч��ƽ�ļ��ԣ�����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

    /***********************************************
    * ������ 
    */
    TIM_OC2Init(BeepPwm.TIMx, &TIM_OCInitStructure);	            //ʹ�ܶ�ʱ����ͨ��2
    TIM_OC2PreloadConfig(BeepPwm.TIMx, TIM_OCPreload_Enable);       //���ø�ͨ���ıȽϼĴ���TIM_CCRԤװ��ʹ��

    TIM_ARRPreloadConfig(BeepPwm.TIMx, ENABLE);			            // ʹ��TIM5���ؼĴ���ARR

    /***********************************************
    * ������ 
    */
    TIM_Cmd(BeepPwm.TIMx, ENABLE);                                  //ʹ�ܶ�ʱ��5
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_SetFreq
 * ��    �ܣ� ���÷��PWM���Ƶ��
 * ��ڲ����� Ƶ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PWM_SetFreq(StrPwm *pwm, uint8_t ch, float freq)
{
    if ( ch > 3 )
        return;

    if ( freq > 10000.0 )
      freq = 10000.0;
    else if ( freq < 1.0 )
      freq  = 1.0;

    pwm->freq[ch]    = freq;
    pwm->period[ch]  = 1000000 / (uint32_t)pwm->freq[ch] - 1;

    PWM_SetDuty(pwm, ch,pwm->duty[ch]);
    TIM_SetAutoreload(pwm->TIMx, pwm->period[ch]);
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_SetDuty
 * ��    �ܣ� ���÷��PWM���ռ�ձ�
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PWM_SetDuty(StrPwm *pwm, uint8_t ch, float duty)
{
    if ( ch > 3 )
        return;

    if ( duty > 100.0 )
      duty  = 100.0;
    else if ( duty < 0.0 )
      duty  = 0.0;

    pwm->duty[ch]    = duty;
    pwm->compare[ch] = (uint32_t)( pwm->duty[ch] * pwm->period[ch] / 100.0 );

    switch(ch) {
    case 0:
        TIM_SetCompare1(pwm->TIMx, pwm->compare[ch]);
        break;
    case 1:
        TIM_SetCompare2(pwm->TIMx, pwm->compare[ch]);
        break;
    case 2:
        TIM_SetCompare3(pwm->TIMx, pwm->compare[ch]);
        break;
    case 3:
        TIM_SetCompare4(pwm->TIMx, pwm->compare[ch]);
        break;
    default:break;
    }
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_SetProperty
 * ��    �ܣ� �������PWM���
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PWM_SetProperty(StrPwm *pwm, uint8_t ch, float freq, float duty)
{
  PWM_SetFreq(pwm, ch, freq);
  PWM_SetDuty(pwm, ch, duty);
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_Start
 * ��    �ܣ� �������PWM���
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PWM_Start(StrPwm *pwm)
{
    //��������ʹ�ܶ˵Ĵ�
    for ( int i = 0; i < PWM_CH_NBR; i++ ) {
        switch (pwm->chEn[i]) {
        case 1:
            TIM_OC1PreloadConfig(pwm->TIMx, TIM_OCPreload_Enable);   //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
            break;
        case 2:
            TIM_OC2PreloadConfig(pwm->TIMx, TIM_OCPreload_Enable);   //ʹ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���
            break;
        case 3:
            TIM_OC3PreloadConfig(pwm->TIMx, TIM_OCPreload_Enable);   //ʹ��TIMx��CCR3�ϵ�Ԥװ�ؼĴ���
            break;
        case 4:
            TIM_OC4PreloadConfig(pwm->TIMx, TIM_OCPreload_Enable);   //ʹ��TIMx��CCR4�ϵ�Ԥװ�ؼĴ���
            break;
        default:
            break;
        }
    }
    TIM_ARRPreloadConfig(pwm->TIMx, ENABLE);                 //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
    TIM_Cmd(pwm->TIMx,ENABLE);                               //��TIM2
}

/*******************************************************************************
 * ��    �ƣ� FanPWM_Stop
 * ��    �ܣ� ֹͣ���PWM���
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PWM_Stop(StrPwm *pwm)
{
    //��������ʹ�ܶ˵Ĺر�
    for ( int i = 0; i < PWM_CH_NBR; i++ ) {
        switch (pwm->chEn[i]) {
        case 0:
            TIM_OC1PreloadConfig(pwm->TIMx, TIM_OCPreload_Disable);   //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
            break;
        case 1:
            TIM_OC2PreloadConfig(pwm->TIMx, TIM_OCPreload_Disable);   //ʹ��TIMx��CCR2�ϵ�Ԥװ�ؼĴ���
            break;
        case 2:
            TIM_OC3PreloadConfig(pwm->TIMx, TIM_OCPreload_Enable);   //ʹ��TIMx��CCR3�ϵ�Ԥװ�ؼĴ���
            break;
        case 3:
            TIM_OC4PreloadConfig(pwm->TIMx, TIM_OCPreload_Disable);   //ʹ��TIMx��CCR4�ϵ�Ԥװ�ؼĴ���
            break;
        default:
            break;
        }
    }
    TIM_ARRPreloadConfig(pwm->TIMx, DISABLE);                     //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
    TIM_CtrlPWMOutputs(pwm->TIMx, DISABLE);                         //�߼���ʱ��Pwm���ʹ�ܣ�һ��Ҫ�ǵô�

}

/*******************************************************************************
 * ��    �ƣ� LED_SetColor
 * ��    �ܣ� ����LED��ɫ
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void LED_SetColor(INT32U color ) 
{
    INT08U red  = B2_INT32U(color);
    INT08U gre  = B1_INT32U(color);
    INT08U ble  = B0_INT32U(color);
    
    PWM_SetDuty(&LedPwm, LED_CH_R, red / 2.55);
    PWM_SetDuty(&LedPwm, LED_CH_G, gre / 2.55);
    PWM_SetDuty(&LedPwm, LED_CH_B, ble / 2.55);
}
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif