/*******************************************************************************
 *   Filename:       bsp_touch.c
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
#include <bsp_bpq.h>
   
#define BSP_BPQ_MODULE_EN 1
#if BSP_BPQ_MODULE_EN > 0
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define BPQ_GPIO                GPIOB

#define I0_0                    GPIO_Pin_11
#define I0_1                    GPIO_Pin_12

#define Q0_0                    GPIO_Pin_13
#define Q0_1                    GPIO_Pin_14
#define Q0_2                    GPIO_Pin_15

#define RD_I0_0()               GPIO_ReadInputDataBit(BPQ_GPIO,I0_0)
#define RD_I0_1()               GPIO_ReadInputDataBit(BPQ_GPIO,I0_1)

#define Q0_0_SET()              {GPIO_SetBits(BPQ_GPIO,Q0_0);}
#define Q0_0_RESET()            {GPIO_ResetBits(BPQ_GPIO,Q0_0);}

#define Q0_1_SET()              {GPIO_SetBits(BPQ_GPIO,Q0_1);}
#define Q0_1_RESET()            {GPIO_ResetBits(BPQ_GPIO,Q0_1);}

#define Q0_2_SET()              {GPIO_SetBits(BPQ_GPIO,Q0_2);}
#define Q0_2_RESET()            {GPIO_ResetBits(BPQ_GPIO,Q0_2);}
   
#define BPQ_SET_DIR_FORWARD()   {Q0_1_RESET(); Q0_0_SET();}
#define BPQ_SET_DIR_REVERSION() {Q0_0_RESET(); Q0_1_SET();}
#define BPQ_SET_DIR_STOP()      {Q0_0_RESET(); Q0_1_RESET();}

#define BPQ_SET_ERR_RST()       {Q0_2_RESET();BSP_OS_TimeDly(500);Q0_2_SET()}
   
#define BPQ_GET_RUN_STATUS      RD_I0_0
#define BPQ_GET_ERR_STATUS      RD_I0_1
   
/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
StrBpq   bpq    = { 0, 0.0, 50.0, 0.0};
StrPwm   *ppwm  = NULL;

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void    BPQ_GPIO_Configuration      (void);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */
 
/*******************************************************************************/
 
/*******************************************************************************
 * @fn      &:main
 * @brief   &:First function called after startup.
 * @return  &:don't care
 *******************************************************************************/
   

/*******************************************************************************
 * ��    �ƣ� BPQ_Init
 * ��    �ܣ� BPQ��ʼ��
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 StrBpq *BPQ_Init(void)
{
    ppwm        = PWM_Init();
    BPQ_GPIO_Configuration();
    BPQ_Stop();
    bpq.up      = TRUE;
    bpq.down    = FALSE;
    bpq.dir     = BPQ_DIR_FORWARD;
    
    return &bpq;
}

/*******************************************************************************
 * ��    �ƣ� BPQ_Configuration
 * ��    �ܣ� SPI����
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BPQ_GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIOA Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
  
    /* Set PB.12,PB.13 GPIO_Mode_IN_FLOATING */
    GPIO_InitStructure.GPIO_Pin     = I0_0 | I0_1;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IN_FLOATING;        // ����Ϊ���ø�������
    GPIO_Init(BPQ_GPIO, &GPIO_InitStructure);
  
    /* Set PB.14,PB.15 GPIO_Mode_AF_PP */
    GPIO_InitStructure.GPIO_Pin     = Q0_0 | Q0_1 | Q0_2;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;             // �����������
    GPIO_Init(BPQ_GPIO, &GPIO_InitStructure);
}

/*******************************************************************************
 * ��    �ƣ� BPQ_Start
 * ��    �ܣ� ����BPQ���
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL BPQ_Start(INT8U dir, float freq)
{
  if ( dir > 2 )
    return FALSE;
  
  if ( freq > bpq.max )
    freq = bpq.max;
  else if ( freq < bpq.min )
    freq    = bpq.min;
  
  bpq.freq = freq;
  BPQ_SetFreq(freq);
  BPQ_SetDir(dir);
 
  return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� BQP_SetFreq
 * ��    �ܣ� ����BPQ���Ƶ��
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BPQ_SetFreq(float freq)
{
  float duty = freq / 50.0;
  
  duty  = duty * pwm.maxDuty;
  PWM_SetDuty(duty);
}

/*******************************************************************************
 * ��    �ƣ� BQP_SetDir
 * ��    �ܣ� ����BPQ�������
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BPQ_SetDir(INT8U dir)
{
  if ( dir > 2 )
    return;
  
  if ( dir == 0 ) {
    BPQ_Stop();
  } else if ( dir == 1 ) {
    BPQ_SET_DIR_FORWARD();
    bpq.ofor    = 1;
    bpq.orev    = 0;
  } else {
    BPQ_SET_DIR_REVERSION();
    bpq.ofor    = 0;
    bpq.orev    = 1;
  }
  
  bpq.dir   = dir;
}

/*******************************************************************************
 * ��    �ƣ� BQP_SetDir
 * ��    �ܣ� ����BPQ�������
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL BPQ_GetStatus(StrBpq *pbpq )
{
  BPQ_GetInput(&bpq);
  
  pbpq->dir     = bpq.dir;
  pbpq->freq    = bpq.freq;
  pbpq->max     = bpq.max;
  pbpq->min     = bpq.min;
  pbpq->irun    = bpq.irun;
  pbpq->up      = bpq.up;
  pbpq->down    = bpq.down;
  
  return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� BPQ_GetInput
 * ��    �ܣ� ����BPQ�������
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL BPQ_GetInput(StrBpq *pbpq )
{
  pbpq->ierr       = BPQ_GET_ERR_STATUS();
  pbpq->irun       = BPQ_GET_RUN_STATUS();
  
  return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� BPQ_GetInput
 * ��    �ܣ� ����BPQ�������
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL BPQ_SetOutput(StrBpq *pbpq )
{
  bpq.ierr       = BPQ_GET_ERR_STATUS();
  bpq.irun       = BPQ_GET_RUN_STATUS();
  
  return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� BQP_SetDir
 * ��    �ܣ� ����BPQ�������
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL BPQ_SetStatus(StrBpq *pbpq )
{
  bpq.dir     = pbpq->dir;
  bpq.freq    = pbpq->freq;
  bpq.max     = pbpq->max;
  bpq.min     = pbpq->min;
  
  bpq.irun    = pbpq->irun;
  bpq.ierr    = pbpq->ierr;
  bpq.ofor    = pbpq->ofor;
  bpq.orev    = pbpq->orev;
  bpq.orst    = pbpq->orst;
  
  bpq.up      = pbpq->up;
  bpq.down    = pbpq->down;
  
  return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� BPQ_Start
 * ��    �ܣ� ֹͣBPQ���
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BPQ_Stop(void)
{
  BPQ_SET_DIR_STOP();
  bpq.dir   = BPQ_DIR_NONE;
  bpq.ofor  = 0;
  bpq.orev  = 0;
  bpq.orst  = 0;
}

/*******************************************************************************
 * ��    �ƣ� BPQ_ErrClr
 * ��    �ܣ� ���ϸ�λ
 * ��ڲ����� 
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-07-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BPQ_ErrClr(void)
{
  BPQ_Stop();
  BSP_OS_TimeDly(500);
  BPQ_SET_ERR_RST();
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/ 
#endif