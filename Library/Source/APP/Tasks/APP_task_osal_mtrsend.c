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
#include <includes.h>
#include <global.h>
#include <app_ctrl.h>
#include <cpu.h>

     
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* ������ ������ƿ飨TCB��
*/
//static  OS_TCB   AppTaskStoreTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
//static  CPU_STK  AppTaskStoreStk[ APP_TASK_STORE_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
//extern	void    BSP_StoreInit(void);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
/*******************************************************************************/

/*******************************************************************************
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-12-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitMtrSend(void)
{    
//    /***********************************************
//    * ������ ��ʼ���������õ������Ӳ��
//    */

//��ʼ����ʱ��
    osal_start_timerEx( OS_TASK_ID_MTRSNED,
                      OS_EVT_MTRSEND_TICKS,
                      1000);                                 //1����ٴ洢
}

/*******************************************************************************
 * ���� ���ߣ� 	redmorningcn.
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
osalEvt  TaskMtrSendEvtProcess(osalTid task_id, osalEvt task_event)
{
    OS_ERR      err;

    if( task_event & OS_EVT_STORE_TICKS ) {

        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                    ( OS_FLAGS     ) COMM_EVT_FLAG_MTR_TIMEOUT,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);        
        
        osal_start_timerEx( OS_TASK_ID_MTRSNED,
                           OS_EVT_MTRSEND_TICKS,
                           1000);
        //1000);
        return ( task_event ^ OS_EVT_MTRSEND_TICKS );
    }
    
    return  task_event;
}



/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
//#endif
