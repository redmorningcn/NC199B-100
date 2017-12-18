/*******************************************************************************
 *   Filename:       app_task_led.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmoringcn.
 *
 *   Description:    双击选中 led 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Led 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 LED 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_LED_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_LED_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskLedCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Led ）
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
#include <bsp_DM412.h>

//#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_led__c = "$Id: $";
#endif

#define APP_TASK_LED_EN     DEF_ENABLED
#if APP_TASK_LED_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
//#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 2u)

/*******************************************************************************
 * MACROS
 */
//#define     BSP_LED_NUM         8
////LED点亮次数 
//#define     LED_TOGGLE_CNT      15


/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
//
//uint8   LedWorkTimes[BSP_LED_NUM + 1] = 0;

#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* 描述： 任务控制块（TCB）
*/
static  OS_TCB   AppTaskLedTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
//static  CPU_STK  AppTaskLedStk[ APP_TASK_LED_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskLed           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************/
//red
#define     RGB_LED_COLOR_RED       0xffff,0x0000,0x0000
#define     RGB_LED_COLOR_GREEN     0x0000,0xffff,0x0000
#define     RGB_LED_COLOR_BLUE      0x0000,0x0000,0xffff
#define     RGB_LED_COLOR_WHITE     0xffff,0xffff,0xffff

//led
#define     HUM_LED         0
#define     DUST_LED        1
#define     OIL_LED         2

#define     GOOD_GRADE      0
#define     NORMAL_GRADE    1
#define     BAD_GRADE       2

/*******************************************************************************
 * 名    称： AppTaskLed
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/

//#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskLedEvtProcess(osalTid task_id, osalEvt task_event)
//#else
//static  void  AppTaskLed (void *p_arg)
//#endif
{
//    OS_ERR      err;
//    INT32U      ticks;
//    INT32S      dly;
//    CPU_SR_ALLOC();
//    
//    /***********************************************
//    * 描述：Task body, always written as an infinite loop.
//    */
//#if ( OSAL_EN == DEF_ENABLED )
//#else
//    TaskInitLed();
//    
//    while (DEF_TRUE) {
//#endif
//        /***********************************************
//        * 描述： 本任务看门狗标志置位
//        */
//        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
//                     (OS_FLAGS     ) WDT_FLAG_LED,
//                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                     (CPU_TS       ) 0,
//                     (OS_ERR      *) &err);
//        /***********************************************
//        * 描述： 得到系统当前时间
//        */
//        ticks = OSTimeGet(&err);
//        
//#if ( OSAL_EN == DEF_ENABLED )
        if( task_event & OS_EVT_LED_TICKS ) {
//#else
//#endif
            static  uint8   mode = 0;
            /***********************************************
            * 描述： 开机显示.red /green /blue
            */
            while(mode < 12)
            {
                mode++;
                switch(mode %4)
                {
                    case 0:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_RED);   break;
                    case 1:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_GREEN); break;
                    case 2:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_BLUE);  break;
                    case 3:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_WHITE); break;
                }
                    
                osal_start_timerEx( OS_TASK_ID_LED,
                                    OS_EVT_LED_TICKS,
                                    600);
                return ( task_event ^ OS_EVT_LED_TICKS );
            }

            /***********************************************
            * 描述： 湿度显示
            */
            switch(Ctrl.sRec.Air_Hum_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(HUM_LED,RGB_LED_COLOR_BLUE);    break;
            }
            
            /***********************************************
            * 描述： 油显示
            */
            switch(Ctrl.sRec.Air_Voc_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(OIL_LED,RGB_LED_COLOR_BLUE);    break;
            }
               
            /***********************************************
            * 描述： 粉尘显示
            */
            switch(Ctrl.sRec.Air_Dust_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(DUST_LED,RGB_LED_COLOR_BLUE);    break;
            }            
            
            /***********************************************
            * 描述： 定时器重启
            */
            osal_start_timerEx( OS_TASK_ID_LED,
                                OS_EVT_LED_TICKS,
                                100);
            return ( task_event ^ OS_EVT_LED_TICKS );
        }
        
        
        
        return  task_event;
}


/*******************************************************************************
 * 名    称：APP_LedInit
 * 功    能：任务初始化
 * 入口参数：无
 * 出口参数：无
 * 作　 　者：redmorningcn.
 * 创建日期：2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitLed(void)
{    
    /***********************************************
    * 描述： 初始化本任务用到的相关硬件
    */
    bsp_DM412Init();            //DM412初始化
        
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    //WdtFlags |= WDT_FLAG_LED;
    /*************************************************
    * 描述：启动事件查询
    */

    osal_start_timerEx( OS_TASK_ID_LED,
                        OS_EVT_LED_TICKS,
                        1000);
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif