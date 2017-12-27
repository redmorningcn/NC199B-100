/*******************************************************************************
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 store 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Store 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 STORE 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_STORE_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_STORE_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskStoreCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Store ）
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
* 描述： 任务控制块（TCB）
*/
//static  OS_TCB   AppTaskStoreTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
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
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-12-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitMtrSend(void)
{    
//    /***********************************************
//    * 描述： 初始化本任务用到的相关硬件
//    */

//初始化定时器
    osal_start_timerEx( OS_TASK_ID_MTRSNED,
                      OS_EVT_MTRSEND_TICKS,
                      1000);                                 //1秒后再存储
}

/*******************************************************************************
 * 作　 　者： 	redmorningcn.
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
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
