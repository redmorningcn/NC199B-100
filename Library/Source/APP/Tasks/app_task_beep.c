/*******************************************************************************
 *   Filename:       app_task_beep.c
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ˫��ѡ�� beep �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Beep �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� BEEP �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  ��APP_TASK_BEEP_PRIO��
 *                                            �� �����ջ��APP_TASK_BEEP_STK_SIZE����С
 *                   �� app.h �������������     ���������� void  App_TaskBeepCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Beep ��
 *
 *   Notes:
 *     				E-mail:261313062@qq.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
#include <bsp_pwm.h>
#include <bsp_os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_beep__c = "$Id: $";
#endif

#define APP_TASK_BEEP_EN     DEF_ENABLED
#if APP_TASK_BEEP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
//������Ŀ�������   (OS_CFG_TICK_RATE_HZ)Ϊ1S
#define BEEP_CYCLE_TIME_TICKS       (OS_CFG_TICK_RATE_HZ * 1u)

#define BEEP_FREQ_HIGH              3000.0
#define BEEP_FREQ_MID               2500.0
#define BEEP_FREQ_LOW               2000.0 
     
#define BEEP_FREQ_NORMAL            2500.0
#define BEEP_FREQ_LOCK              2500.0
#define BEEP_FREQ_PWR_ON            2500.0
     
#define BEEP_DUTY_ON_OFF            40.0
#define BEEP_DUTY_LOCK              40.0
#define BEEP_DUTY_NORMAL            40.0
#define BEEP_DUTY_PWR_ON            40.0
     
#define BEEP_DLY_NORMAL             200
#define BEEP_DLY_SHORT              10
#define BEEP_DLY_LOCK               100
#define BEEP_DLY_PWR_ON             10
     
/*******************************************************************************
 * TYPEDEFS
 */
OS_SEM          App_BeepSem;
OS_FLAG_GRP     BeepFlagGrp;                           //��������־��
OS_FLAGS        BeepFlags;

/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskBeepTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskBeepStk[ APP_TASK_BEEP_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskBeep             (void *p_arg);
static  void    APP_BeepInit            (void);

void            BeepOn                  (void);
void            BeepOff                 (void);
void            BeepLock                (void);
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
 * ��    �ƣ� App_TaskBeepCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskBeepCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskBeepTCB,               // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Beep",               // ��������
                 (OS_TASK_PTR ) AppTaskBeep,                  // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_BEEP_PRIO,           // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskBeepStk[0],            // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_BEEP_STK_SIZE / 10,  // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_BEEP_STK_SIZE,       // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
}

/*******************************************************************************
 * ��    �ƣ� AppTaskBeep
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskBeep (void *p_arg)
{
    OS_ERR          err;
    INT32S          dly = 1000;
    INT16S          ctr = 0;
    extern StrPwm   BeepPwm;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_BeepInit();

    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                     (OS_FLAGS     ) WDT_FLAG_BEEP,
                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                     (CPU_TS       ) 0,
                     (OS_ERR      *) &err);

        /********************************************************************
        * ������ �������Ź���־���飬 �ж��Ƿ�����������ι��
        */
        OS_FLAGS    flags =
        OSFlagPend(( OS_FLAG_GRP *)&BeepFlagGrp,
                   ( OS_FLAGS     ) BeepFlags,
                   ( OS_TICK      ) dly,
                   ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,   //����
                   ( CPU_TS      *) NULL,
                   ( OS_ERR      *)&err);

        if(err == OS_ERR_NONE) {
            switch ( flags ) {
            /***********************************************
            * ������ ������
            */
            case BEEP_FLAG_PWR_ON:
                ctr = 0;
                dly = 1000;
                BeepOn();
                // ��������������������ʱ��ʱ��
                osal_start_timerEx( OS_TASK_ID_CTRL, 
                                   OS_EVT_CTRL_FILTER, 
                                   (OS_TICKS_PER_SEC * 60 * FILTER_LIFE_SAVE_TIME) );
                // ͳ�Ƴ�����
                osal_start_timerEx( OS_TASK_ID_CTRL, 
                                   OS_EVT_CTRL_AIR_VOLUME, 
                                   (OS_TICKS_PER_SEC * 60) );
                break;
            /***********************************************
            * ������ �ػ���
            */
            case BEEP_FLAG_PWR_OFF:
                ctr = 0;
                dly = 1000;
                BeepOff();
                // �ػ�ֹͣ������������ʱ��ʱ��
                osal_stop_timerEx( OS_TASK_ID_CTRL, OS_EVT_CTRL_FILTER );
                osal_stop_timerEx( OS_TASK_ID_CTRL, OS_EVT_CTRL_AIR_VOLUME );
                break;
            /***********************************************
            * ������ ��ͯ����
            */
            case BEEP_FLAG_LOCK_EVT:
                ctr = 0;
                dly = 1000;
                BeepLock();
                break;
            /***********************************************
            * ������ ������
            */
            case BEEP_FLAG_KEY_EVT:
            default:
                PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, BEEP_DUTY_NORMAL);
                ctr = 100;
                dly = 10;
                break;
            }
            /***********************************************
            * ������ ���δ֪��־λ
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&BeepFlagGrp,
                        ( OS_FLAGS      )flags,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
        }

        /***********************************************
        * ������ ���Ű�����
        */
        if ( ctr >= 0 ) {
            PWM_SetDuty(&BeepPwm, PWM_CH_BEEP, ctr / ( 100 / BEEP_DUTY_NORMAL ));
            ctr--;
            dly = 7;
        } else {
            dly = 1000;
        }
    }
}

/*******************************************************************************
 * ��    �ƣ� APP_BeepInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static void APP_BeepInit(void)
{
    OS_ERR err;
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    BeepPWM_Init();

    /***********************************************
    * ������ �ϵ���һ��
    */
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_PWR_ON, BEEP_DUTY_PWR_ON);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    /***********************************************
    * ������ ������������ʧ
    */
    for ( int i = BEEP_DUTY_PWR_ON; i > 0 ; i-- ) {
        PWM_SetDuty(&BeepPwm, PWM_CH_BEEP, i);
        BSP_OS_TimeDly(BEEP_DLY_PWR_ON);
    }
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, 0.0);

    /***********************************************
    * ����������һ�����ڵ�һ�������ź���
    */
    OSSemCreate ((OS_SEM      *)&App_BeepSem,
                 (CPU_CHAR    *)"Beep On",
                 (OS_SEM_CTR   )0,
                 (OS_ERR      *)&err);
    /***********************************************
    * ������ ����������Ϣ���ͱ�־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&BeepFlagGrp,
                 ( CPU_CHAR     *)"BeepFlags",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    /***********************************************
    * ������ ��ӱ�־λ
    */
    BeepFlags   = BEEP_FLAG_PWR_ON
                + BEEP_FLAG_PWR_OFF
                + BEEP_FLAG_KEY_EVT
                + BEEP_FLAG_LOCK_EVT;

    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_BEEP;
}

/*******************************************************************************
 * ��    �ƣ� BeepOn
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepOn(void)
{
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOW, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_MID, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_HIGH, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    for ( int i = BEEP_DUTY_ON_OFF; i > 5; i-- ) {
        PWM_SetDuty(&BeepPwm, PWM_CH_BEEP, i);
        BSP_OS_TimeDly(BEEP_DLY_SHORT);
    }

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, 0.0);
}

/*******************************************************************************
 * ��    �ƣ� BeepOff
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepOff(void)
{
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_HIGH, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_MID, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOW, BEEP_DUTY_ON_OFF);
    BSP_OS_TimeDly(BEEP_DLY_NORMAL);
    
    for ( int i = BEEP_DUTY_ON_OFF; i > 5; i-- ) {
        PWM_SetDuty(&BeepPwm, PWM_CH_BEEP, i);
        BSP_OS_TimeDly(BEEP_DLY_SHORT);
    }

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, 0.0);
}

/*******************************************************************************
 * ��    �ƣ� BeepLock
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepLock(void)
{
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOCK, BEEP_DUTY_LOCK);
    BSP_OS_TimeDly(BEEP_DLY_LOCK);
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOCK, 0.0);
    BSP_OS_TimeDly(BEEP_DLY_LOCK);
    
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOCK, BEEP_DUTY_LOCK);
    BSP_OS_TimeDly(BEEP_DLY_LOCK);
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOCK, 0.0);
    BSP_OS_TimeDly(BEEP_DLY_LOCK);
    
    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_LOCK, BEEP_DUTY_LOCK);
    BSP_OS_TimeDly(BEEP_DLY_LOCK);

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, 0.0);
}

                     
/*******************************************************************************
 * ��    �ƣ� BeepLock
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BeepExt(u8 cnt, u16 freq, u16 dly)
{
    for ( int i = cnt; i > 0; i-- ) {
        PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, freq, BEEP_DUTY_LOCK);
        BSP_OS_TimeDly(dly);
        if ( i == 1 )
            break;
        PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, freq, 0.0);
        BSP_OS_TimeDly(dly);
    }

    PWM_SetProperty(&BeepPwm, PWM_CH_BEEP, BEEP_FREQ_NORMAL, 0.0);
}
/***********************************************
* ������ 
*/
void Beep(INT32U flag)
{   
    OS_ERR  err;
    /***********************************************
    * ������ ���Ͱ����¼���־λ
    */
    OSFlagPost(( OS_FLAG_GRP  *)&BeepFlagGrp,
               ( OS_FLAGS      )flag,
               ( OS_OPT        )OS_OPT_POST_FLAG_SET,
               ( OS_ERR       *)&err);
}
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif