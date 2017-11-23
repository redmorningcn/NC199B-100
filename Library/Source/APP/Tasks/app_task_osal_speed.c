/*******************************************************************************
 *   Filename:       app_task_store.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 speed 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Speed 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 SPEED 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_SPEED_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_SPEED_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskSpeedCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Speed ）
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
#include <global.h>
#include <app_ctrl.h>
#include <cpu.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_speed__c = "$Id: $";
#endif

#define APP_TASK_SPEED_EN     DEF_ENABLED
#if APP_TASK_SPEED_EN == DEF_ENABLED				//
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
extern  uint16  l_speed1;
extern  uint16  l_speed2;
extern  uint16  l_speedcs;
extern  uint8   l_locoworksta;


#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* 描述： 任务控制块（TCB）
*/
static  OS_TCB   AppTaskSpeedTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK  AppTaskSpeedStk[ APP_TASK_SPEED_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
extern	void    BSP_SpeedInit(void);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern	stcSysCtrl       sCtrl;       	//	全局结构控制体
/*******************************************************************************/



/*******************************************************************************
 * 名    称：  TaskInitSpeed
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void    BSP_SpeedInit(void)
{
	
    Time0Init();        //初始化速度传感器1
    
	Time1Init();        //初始化速度传感器2
    
	Time2Init();        //初始化柴速检测
    
    InitLocoStaIO();    //初始化工况检测IO脚
}

/*******************************************************************************
 * 名    称：  TaskInitSpeed
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitSpeed(void)
{    
//    /***********************************************
//    * 描述： 初始化本任务用到的相关硬件
//    */
    BSP_SpeedInit();			//初始化Flash底层相关函数

//初始化定时器
    osal_start_timerEx( OS_TASK_ID_SPEED,
                      OS_EVT_SPEED_TICKS,
                      1000);
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * 名    称： 		AppTaskSpeed
 * 功    能： 		控制任务
 * 入口参数： 	p_arg - 由任务创建函数传入
 * 出口参数： 	无
 * 作　 　者： 	redmorningcn.
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
osalEvt  TaskSpeedEvtProcess(INT8U task_id, osalEvt task_event)
{
    OS_ERR      err;
    INT32U      ticks;
    INT32S      dly;
    CPU_SR_ALLOC();

    if( task_event & OS_EVT_SPEED_TICKS ) {

            SpeedAndDistTast();         //速度计算，内定义了 l_speed1，l_speed2，l_speedcs，全局变量
            
            CheckLocoStaTask();         //工况检测，定义了l_locoworksta全局变量
            
            //给速度、工况等赋值
            sCtrl.sRec.Myspeed1         = l_speed1;
            sCtrl.sRec.Myspeed2         = l_speed2;
            sCtrl.sRec.MyEngRotSpd      = l_speedcs;
            
            sCtrl.sRec.MyLocoWorkState  = l_locoworksta;
            
            osal_start_timerEx( OS_TASK_ID_SPEED,
                                OS_EVT_SPEED_TICKS,
                                1000);
            
            return ( task_event ^ OS_EVT_SPEED_TICKS );
        }
}



/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif
