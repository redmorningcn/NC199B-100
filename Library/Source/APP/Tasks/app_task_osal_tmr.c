/*******************************************************************************
 *   Filename:       app_task_tmr.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    双击选中 tmr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Tmr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 TMR 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_TMR_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_TMR_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskTmrCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Tmr ）
 *
 *   Notes:
 *     				E-mail: shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
//#include "DS3231.h"
#include "app.h"
#include "bsp_timer.h"
#include "os_cfg_app.h"
#include <osal_timer.h>


#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_tmr__c = "$Id: $";
#endif

#define APP_TASK_TMR_EN     DEF_ENABLED
#if APP_TASK_TMR_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
//#define CYCLE_TIME_TICKS            (OS_CFG_TICK_RATE_HZ * 1u)
#define CYCLE_SAMPLE_MSEC_TICKS     (OS_CFG_TICK_RATE_HZ / 5)
#define CYCLE_SAMPLE_SEC_TICKS      (OS_CFG_TICK_RATE_HZ * 5u)
#define CYCLE_SAMPLE_MIN_TICKS      (OS_CFG_TICK_RATE_HZ * 60u)
#define CYCLE_SAMPLE_TICKS          (OS_CFG_TICK_RATE_HZ * 2u)
     
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
static  OS_TCB   AppTaskTmrTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK  AppTaskTmrStk[ APP_TASK_TMR_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************/

/*******************************************************************************
 * 名    称： AppTaskTmr
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/
osalEvt  TaskTmrEvtProcess(osalTid task_id, osalEvt task_event)
{
    OS_ERR      err;
//    static BOOL     chang_flag = FALSE;
    
    /***********************************************
    * 描述： 本任务看门狗标志置位
    */
    OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                 (OS_FLAGS     ) WDT_FLAG_TMR,
                 (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                 (CPU_TS       ) 0,
                 (OS_ERR      *) &err);    
  
    /***************************************************************************
    * 描述： 统计模块和测量某块通讯定时器，统计装置定时发送数据查询指令，超时发送
    COMM_EVT_FLAG_MTR_TIMEOUT 标示。定时500ms通讯一次。
    */
    if( task_event & OS_EVT_TMR_MTR ) {     
        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                    ( OS_FLAGS     ) COMM_EVT_FLAG_MTR_TIMEOUT,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);        


           osal_start_timerEx(  OS_TASK_ID_TMR,     //计数器重新开始
                        OS_EVT_TMR_MTR,
                        OS_TICKS_PER_SEC);        
            
        return ( task_event ^ OS_EVT_TMR_MTR );
    }
    
    /***************************************************************************
    * 描述： 统计模块和无线发送模块通讯定时器，
    COMM_EVT_FLAG_DTU_TIMEOUT 标示。
    */
    if( task_event & OS_EVT_TMR_DTU ) { 
        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                    ( OS_FLAGS     ) COMM_EVT_FLAG_DTU_TIMEOUT,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);  
    
         osal_start_timerEx(  OS_TASK_ID_TMR,     //计数器重新开始
                              OS_EVT_TMR_DTU,
                              OS_TICKS_PER_SEC*15); 
        
       return ( task_event ^ OS_EVT_TMR_DTU );
    }
    
    /***************************************************************************
    * 描述： 统计模块和无线发送模块通讯定时器，
    COMM_EVT_FLAG_DTU_TIMEOUT 标示。
    */
    if( task_event & OS_EVT_TMR_OTR ) {        
        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                    ( OS_FLAGS     ) COMM_EVT_FLAG_OTR_TIMEOUT,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);        

         osal_start_timerEx(  OS_TASK_ID_TMR,     //计数器重新开始
                      OS_EVT_TMR_OTR,
                      OS_TICKS_PER_SEC*3);                             

        return ( task_event ^ OS_EVT_TMR_OTR );
    }
    
//    /***************************************************************************
//    * 描述： 超时操作 TAX 
//    COMM_EVT_FLAG_DTU_TIMEOUT 标示。
//    */
//    if( task_event & OS_EVT_TMR_TAX ) {            
//
//        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
//                    ( OS_FLAGS     ) COMM_EVT_FLAG_TAX_TIMEOUT,
//                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                    ( CPU_TS       ) 0,
//                    ( OS_ERR      *) &err);        
//        
//        osal_start_timerEx(  OS_TASK_ID_TMR,     //计数器重新开始
//                  OS_EVT_TMR_TAX,
//                  OS_TICKS_PER_SEC*10);                             
//
//        return ( task_event ^ OS_EVT_TMR_TAX );
//    }

    /***************************************************************************
    * 描述： 统计模块和无线发送模块通讯定时器，
    COMM_EVT_FLAG_DTU_TIMEOUT 标示。
    */
    if( task_event & OS_EVT_TMR_SEC ) {        
            Ctrl.Dtu.ConnectTimeOut++;
            Ctrl.Mtr.ConnectTimeOut++;
//            Ctrl.Otr.ConnectTimeOut++;
            
            if( Ctrl.Dtu.ConnectTimeOut > 1)
                Ctrl.Dtu.ConnectFlag = 0;
            
            if( Ctrl.Mtr.ConnectTimeOut > 1)
                Ctrl.Mtr.ConnectFlag = 0;
            
//            if( Ctrl.Otr.ConnectTimeOut > 1)
//                Ctrl.Otr.ConnectFlag = 0;

            osal_start_timerEx(  OS_TASK_ID_TMR,     //计数器重新开始
                    OS_EVT_TMR_SEC,
                    OS_TICKS_PER_SEC);        
            
        return ( task_event ^ OS_EVT_TMR_SEC );
    }
    return 0;
}

/*******************************************************************************
 * 名    称： APP_TmrInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： wumingshen.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitTmr(void)
{
    /***********************************************
    * 描述： 初始化
    */    
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    WdtFlags |= WDT_FLAG_TMR;
    /*************************************************
    * 描述：启动事件查询
    */
    osal_start_timerEx( OS_TASK_ID_TMR, OS_EVT_TMR_SEC,OS_TICKS_PER_SEC);   
    osal_start_timerEx( OS_TASK_ID_TMR, OS_EVT_TMR_MTR,OS_TICKS_PER_SEC*1 ); 
    osal_start_timerEx( OS_TASK_ID_TMR, OS_EVT_TMR_DTU,OS_TICKS_PER_SEC*2 ); 
    osal_start_timerEx( OS_TASK_ID_TMR, OS_EVT_TMR_OTR,OS_TICKS_PER_SEC*10);

}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif