/*******************************************************************************
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� store �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Store �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� STORE �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_STORE_PRIO ��
 *                                            �� �����ջ�� APP_TASK_STORE_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskStoreCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Store ��
 *
 *   Notes:
 *     				E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
#include <app_ctrl.h>
#include <os.h>
#include <osal.h>
#include <osal_event.h>
#include <os_cfg_app.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_osal__c = "$Id: $";
#endif

#define APP_TASK_OSAL_EN     DEF_ENABLED
#if APP_TASK_OSAL_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define  OSAL_CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ / 1u)

/*******************************************************************************
 * TYPEDEFS
 */
#if ( OSAL_EN == DEF_ENABLED )
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskOsalTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskOsalStk[ APP_TASK_OSAL_STK_SIZE ];
#endif
/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskOsal             (void *p_arg);
static  void    APP_OsalInit            (void);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern void     OSAL_TaskCreateHook     (void);
    
 /*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskOsalCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskOsalCreate(void)
{
#if ( OSAL_EN == DEF_ENABLED )
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskOsalTCB,                 // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Osal",                 // ��������
                 (OS_TASK_PTR ) AppTaskOsal,                    // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_OSAL_PRIO,             // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskOsalStk[0],              // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_OSAL_STK_SIZE / 10,    // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_OSAL_STK_SIZE,         // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
#else
#endif
}

/*******************************************************************************
 * ��    �ƣ� AppTaskOsal
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskOsal (void *p_arg)
{    
#if ( OSAL_EN == DEF_ENABLED )
    OS_ERR          err;
    INT32U          ticks;
    static INT32S   dly     = 1;
    
    osalEvt         events;
    halIntState_t   cpu_sr;
    
    /***********************************************
    * ������ �����ʼ��
    */
    APP_OsalInit();
    
    /***********************************************
    * ������OSALϵͳ��ʼ��
    */
    osal_init_system();
    
    /***********************************************
    * ������ OSAL��������
    */
    OSAL_TaskCreateHook();
    
    /***********************************************
    * ������OSAL�����ʼ��
    */
    osal_Task_init();
    
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    *        OSAL�����ѯ��ִ��
    */
    while (DEF_TRUE) {
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        ticks = OSTimeGet(&err);
        
#if (UCOS_EN == DEF_ENABLED ) && ( OS_VERSION > 30000U )
        BOOL    ret = BSP_OS_SemWait(&Osal_EvtSem, dly);    // �ȴ��ź���
#endif
        do {
            /***********************************************
            * ������������Ҫϵͳ���ǲ�ѯ�ĵ��¼����磺
            *       ���ڽ����¼�����ťɨ���¼���
            */
            //HAL_ProcessPoll();
            
            /***********************************************
            * �����������Ƿ�����������¼�
            */
            TaskActive = osalNextActiveTask();
            
            /***********************************************
            * �����������ǰ�������¼�����
            */
            if ( TaskActive ) {
                CPU_CRITICAL_ENTER();
                events = TaskActive->events;
                
                /***********************************************
                * �����������ǰ�����¼�
                */
                TaskActive->events = 0;
                CPU_CRITICAL_EXIT();
                
                /***********************************************
                * �������ٴμ���¼��Ƿ����
                */
                if ( events != 0 ) {
                    /***********************************************
                    * �������ж��������Ƿ����
                    */
                    if ( TaskActive->pfnEventProcessor ) {
                        /***********************************************
                        * ������ִ������������ȼ��¼�������δ��������¼�
                        *       �¼�ֵԽС�����ȼ�Խ��
                        */
                        events = (TaskActive->pfnEventProcessor)( TaskActive->taskID, events );
                        
                        /***********************************************
                        * ��������ӷ���δ��������¼�����ǰ���񣬵ȴ��´�ִ��
                        */
                        CPU_CRITICAL_ENTER();
                        TaskActive->events |= events;
                        CPU_CRITICAL_EXIT();
                    }
                }
            }
        } while (TaskActive);                           // �ȴ���������ִ����
        
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                     (OS_FLAGS     ) WDT_FLAG_OSAL,
                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                     (CPU_TS       ) 0,
                     (OS_ERR      *) &err);
        
        /***********************************************
        * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
        */
        dly   = OSAL_CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  <= 0 ) {
            dly   = 1;
        }
    }
#else
#endif
}

/*******************************************************************************
 * ��    �ƣ� APP_OsalInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static void APP_OsalInit(void)
{
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_OSAL;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif