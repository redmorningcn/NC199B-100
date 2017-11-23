/*******************************************************************************
 *   Filename:       app_task_key.c
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ˫��ѡ�� key �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Key �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� KEY �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_KEY_PRIO ��
 *                                            �� �����ջ�� APP_TASK_KEY_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskKeyCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Key ��
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
#include "bsp_key.h"
#include <bsp_control.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_key__c = "$Id: $";
#endif

#define APP_TASK_KEY_EN     DEF_ENABLED
#if APP_TASK_KEY_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
//������Ŀ�������   (OS_CFG_TICK_RATE_HZ)Ϊ1S
#define  KEY_CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 1u)

/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskKeyTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskKeyStk[ APP_TASK_KEY_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskKey          (void *p_arg);
static  void    APP_KeyInit         (void);
static  void    KeyValueHandle      (StrCtrlDCB *dcb);


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern OS_SEM   App_BeepSem;

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskKeyCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskKeyCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskKeyTCB,                  // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Key",                  // ��������
                 (OS_TASK_PTR ) AppTaskKey,                     // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,              // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskKeyStk[0],               // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,     // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,          // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������

}

/*******************************************************************************
 * ��    �ƣ� AppTaskKey
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskKey (void *p_arg)
{
    OS_ERR      err;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_KeyInit();

    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {

        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                     (OS_FLAGS     ) WDT_FLAG_KEY,
                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                     (CPU_TS       ) 0,
                     (OS_ERR      *) &err);
        /***********************************************
        * ������ ���а����¼�ʱ��ȡ��ֵ�Ͱ����¼�
        */
        if( KEY_GetKey(Ctrl.Key) != KEY_STATUS_NULL ) {
            KeyValueHandle(&Ctrl);
        }
    }
}

/*******************************************************************************
 * ��    �ƣ� APP_KeyInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static void APP_KeyInit(void)
{
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    KEY_Init();

    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_KEY;
}

/*******************************************************************************
 * ��    �ƣ� KeyValueHandle
 * ��    �ܣ� ����������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
extern OS_SEM  SemReportStatus;            //�����ϱ�״̬�ź���
static void KeyValueHandle(StrCtrlDCB *dcb)
{
    OS_ERR err;
    static INT08U  LongPressedFlag  = 0;
    /***********************************************
    * ������ ��ȡ�����¼�
    */
    switch(dcb->Key->evt){
    /***************************************************************************
    * ������ ���������¼�
    */
    case KEY_EVT_DOWN: {
        switch(dcb->Key->val) {
        /***********************************************
        * ������ WIFI��λ��
        */
        case KEY_NRESET: 
            break;
        /***********************************************
        * ������ WIFI���ü�
        */
        case KEY_NRELOAD:
            osal_stop_timerEx( OS_TASK_ID_SEN,
                               OS_EVT_SEN_PM2D5_CHK);
            break;
        /***********************************************
        * ������ ��Դ��
        */
        case KEY_PWR: 
            break;
        }
    };break;
    
    /***************************************************************************
    * ������ ���������¼�
    */
    case KEY_EVT_UP: {        
        /***********************************************
        * ������ ���������־
        */
        if ( LongPressedFlag ) {
            LongPressedFlag    = 0;
        } else {
            switch(dcb->Key->val) {
            /***********************************************
            * ������ WIFI��λ��
            */
            case KEY_NRESET: 
                if ( Ctrl.Sys.FirstPwrFlag == TRUE ) {
                    if ( ++Ctrl.PM2D5.ChkVal > 35 ) {
                        Ctrl.PM2D5.ChkVal   = 20;
                        BeepExt(3, 2000, 100);
                    } else {
                        BeepExt(1, 1500, 200);                        
                    }
                } else {
                    /***********************************************
                    * ������ ���Ͱ����¼���־λ
                    */
                    Beep( BEEP_FLAG_KEY_EVT );
                }
                break;
            /***********************************************
            * ������ WIFI���ü�
            */
            case KEY_NRELOAD: 
                if ( Ctrl.Sys.FirstPwrFlag == TRUE ) {
                    osal_stop_timerEx( OS_TASK_ID_SEN,
                                       OS_EVT_SEN_PM2D5_CHK);
                    osal_start_timerEx( OS_TASK_ID_SEN,
                                       OS_EVT_SEN_PM2D5_CHK,
                                        OS_TICKS_PER_SEC * 1);
                    
                    BeepExt(2, 1500, 200);
                /***********************************************
                * ������ ���Ͱ����¼���־λ
                */
                } else {
                    /***********************************************
                    * ������ PM2.5��ֵУ��
                    */
                    osal_start_timerEx( OS_TASK_ID_SEN,
                                       OS_EVT_SEN_CHK_TIMEOUT,
                                       OS_TICKS_PER_SEC);
                    
                    if ( ++Ctrl.PM2D5.ChkCtr > 10 ) {  
                        osal_start_timerEx( OS_TASK_ID_SEN,
                                           OS_EVT_SEN_PM2D5_CHK,
                                           OS_TICKS_PER_SEC * 10); 
                        BeepExt(3, 1500, 200);
                    } else {                
                        /***********************************************
                        * ������ WIFI���ü�
                        */
                        Ctrl.Wifi.W2dWifiSts.WifiStatus.ustatus.SoftApMode  = 1;
                        Ctrl.Wifi.W2dWifiSts.WifiStatus.ustatus.ConnRouter  = 0;
                        Ctrl.Sys.WifiSts = WIFI_STS_CONFIG;
                        
                        osal_set_event( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_CFG) );
                        osal_start_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_TMO), 60*OS_TICKS_PER_SEC);
                        /***********************************************
                        * ������ ���Ͱ����¼���־λ
                        */
                        Beep( BEEP_FLAG_KEY_EVT );
                        /***********************************************
                        * ������ ֪ͨWIFIģ���������ģʽ
                        */
                        OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                                   ( OS_FLAGS      )WIFI_EVT_FLAG_CONFIG,
                                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                                   ( OS_ERR       *)&err);
                    }
                }
                break;
            /***********************************************
            * ������ ��Դ��
            */
            case KEY_PWR:
                /***********************************************
                * ������ ��û�п�����ͯ����ʱ�򣬵����޸�ģʽ
                */
                if( !Ctrl.Wifi.Wr.ChildSecurityLock ) {
                    /***********************************************
                    * ������ ����ǹػ�״̬���򿪻�
                    */
                    if( SYS_PWR_ON != Ctrl.Sys.SysSts ) {
                        /***********************************************
                        * ������ ������
                        */
                        Power(ON);
                    /***********************************************
                    * ������ ����ǿ���״̬���ı�ģʽ
                    */
                    } else {
                        if ( ++Ctrl.Fan.Mode > FAN_MODE_MAX_SPEED ){
                            Ctrl.Fan.Mode  = FAN_MODE_SILENT;
                        }
                        /***********************************************
                        * ������ ���Ͱ����¼���־λ
                        */
                        Beep( BEEP_FLAG_KEY_EVT );
                    }
                /***********************************************
                * ������ ���п�����ͯ����ʱ�򣬲��޸�ģʽ������������
                */
                } else {
                    /***********************************************
                    * ������ ���Ͷ�ͯ���¼���־λ
                    */
                    OSFlagPost(( OS_FLAG_GRP  *)&BeepFlagGrp,
                               ( OS_FLAGS      )BEEP_FLAG_LOCK_EVT,
                               ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                               ( OS_ERR       *)&err);
                }
                break;
            }
        }
    } break;
    
    /***************************************************************************
    * ������ ������ס�¼�
    */
    case KEY_EVT_PRESSED: {
        switch(dcb->Key->val) {
        /***********************************************
        * ������ WIFI��λ��
        */
        case KEY_NRESET:
            if ( !LongPressedFlag++ ) {
                BeepExt(3, 1500, 200);
            }
            break;
        /***********************************************
        * ������ WIFI���ü�
        */
        case KEY_NRELOAD:{
            if ( !LongPressedFlag++ ) {
                Ctrl.Sys.WifiSts = WIFI_STS_CONFIG;
                osal_set_event( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_CFG) );
                osal_start_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_TMO), 60*OS_TICKS_PER_SEC);
                /***********************************************
                * ������ ���Ͱ����¼���־λ
                */
                Beep( BEEP_FLAG_KEY_EVT );
                /***********************************************
                * ������ ֪ͨWIFIģ���������ģʽ
                */
                OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                           ( OS_FLAGS      )WIFI_EVT_FLAG_CONFIG,
                           ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                           ( OS_ERR       *)&err);
                BSP_OS_TimeDly(OS_TICKS_PER_SEC);           // ��ʱ���ȴ������������
            }
        } break;
        
        /***********************************************
        * ������ ��Դ��
        */
        case KEY_PWR:{
            /***********************************************
            * ������ �ó�����־λ
            */
            if ( !LongPressedFlag ) {
                LongPressedFlag++;
                /***********************************************
                * ������ ��������ͯ��ʱ������Ϊ����
                */
                if( Ctrl.Wifi.Wr.ChildSecurityLock == 1 ) {
                    Ctrl.Wifi.Wr.ChildSecurityLock = 0;
                    /***********************************************
                    * ������ ���Ͱ����¼���־λ
                    */
                    Beep( BEEP_FLAG_KEY_EVT );
                /***********************************************
                * ������ ����
                */
                } else {
                    /***********************************************
                    * ������ ����״̬�������ػ�
                    */
                    if( Ctrl.Sys.SysSts == SYS_PWR_ON) {
                        /***********************************************
                        * ������ �ػ�
                        */
                        Power(OFF);
                        BSP_OS_TimeDly(OS_TICKS_PER_SEC);           // ��ʱ���ȴ������������
                    /***********************************************
                    * ������ �ػ�״̬������������
                    */
                    } else {
                        /***********************************************
                        * ������ 
                        */
                        //BeepExt(2, 1500, 100);
                    }
                }
            }
            /***********************************************
            * ������ �����ϱ�״̬
            */
            OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                       ( OS_FLAGS      )WIFI_EVT_FLAG_REPORT,
                       ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                       ( OS_ERR       *)&err);
        } break;
        }
    };break;
    
    default:break;
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif