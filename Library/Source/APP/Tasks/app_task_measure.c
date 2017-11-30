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
#define  SNL_APP_SOURCE
#include <includes.h>


//#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
//const  CPU_CHAR  *app_task_key__c = "$Id: $";
//#endif

#define APP_TASK_KEY_EN     DEF_ENABLED
#if APP_TASK_KEY_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
//该任务的控制周期   (OS_CFG_TICK_RATE_HZ)为1S
//#define  KEY_CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 1u)

//进气口电磁阀  pd.3
#define     DCF_GPIO_AIR_IN      303  
//出气口电磁阀 pd.2
#define     DCF_GPIO_AIR_OUT     302   
     
     
/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* 描述： 任务控制块（TCB）
*/
static  OS_TCB   AppTaskMeasureTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK  AppTaskMeasureStk[ APP_TASK_MEASURE_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskMeasure          (void *p_arg);
static  void    APP_MeasureInit         (void);
void            StartAirMeasure         (void);

//static  void    MeasureValueHandle      (StrCtrlDCB *dcb);


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
 * 名    称： App_TaskMeasureCreate
 * 功    能： **任务创建
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： Roger-WY.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
void  App_TaskMeasureCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * 描述： 任务创建
    */
    OSTaskCreate((OS_TCB     *)&AppTaskMeasureTCB,                  // 任务控制块  （当前文件中定义）
                 (CPU_CHAR   *)"App Task Measure",                  // 任务名称
                 (OS_TASK_PTR ) AppTaskMeasure,                     // 任务函数指针（当前文件中定义）
                 (void       *) 0,                              // 任务函数参数
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,              // 任务优先级，不同任务优先级可以相同，0 < 优先级 < OS_CFG_PRIO_MAX - 2（app_cfg.h中定义）
                 (CPU_STK    *)&AppTaskMeasureStk[0],               // 任务栈顶
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,     // 任务栈溢出报警值
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,          // 任务栈大小（CPU数据宽度 * 8 * size = 4 * 8 * size(字节)）（app_cfg.h中定义）
                 (OS_MSG_QTY  ) 5u,                             // 可以发送给任务的最大消息队列数量
                 (OS_TICK     ) 0u,                             // 相同优先级任务的轮循时间（ms），0为默认
                 (void       *) 0,                              // 是一个指向它被用作一个TCB扩展用户提供的存储器位置
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // 允许堆栈检查该任务
                                OS_OPT_TASK_STK_CLR),           // 创建任务时堆栈清零
                 (OS_ERR     *)&err);                           // 指向错误代码的指针，用于创建结果处理

}

/*******************************************************************************
 * 名    称： AppTaskMeasure
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作　　者： Roger-WY.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/
static  void  AppTaskMeasure (void *p_arg)
{
//    OS_ERR      err;
    /***********************************************
    * 描述： 任务初始化
    */
    APP_MeasureInit();

    /***********************************************
    * 描述：Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {

        /***********************************************
        * 描述： 本任务看门狗标志置位
        */
//        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
//                     (OS_FLAGS     ) WDT_FLAG_KEY,
//                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                     (CPU_TS       ) 0,
//                     (OS_ERR      *) &err);
        /***********************************************
        * 描述： 当有按键事件时获取键值和按键事件
        */
        StartAirMeasure();
        
        BSP_OS_TimeDlyHMSM(0,10,0,0);       //10min
    }
}
/*******************************************************************************
 * 作　　者： redmorningcn.
 * 创建日期： 2017-11-22
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void        Measure_GPIO_Init(void)
{
    //端口初始化为I/O口
    GPIO_PinselConfig(DCF_GPIO_AIR_IN,0);
    GPIO_PinselConfig(DCF_GPIO_AIR_OUT,0);
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN, 0);   //关闭电磁阀
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,0);   //关闭电磁阀
    
    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 0;  //气路打开标识置1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 1;         
}

/*******************************************************************************
 * 作　　者： redmorningcn.
 * 创建日期： 2017-11-23
 * 修    改： 打开 电磁阀，先打开输出，再打开输入。然后置测量标识
              关闭 电磁阀，先关输入，在关输出。然后置测试标识
 * 修改日期：
 *******************************************************************************/
void    CloseAir(void)
{
    OS_ERR  err;

    ////////关闭测试
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN,0);    //先关输入
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //延时0.1s
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,0);   //再关输出
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //延时0.5s    
}

/*******************************************************************************
 * 作　　者： redmorningcn.
 * 创建日期： 2017-11-22
 * 修    改： 打开 电磁阀，先打开输出，再打开输入。然后置测量标识
              关闭 电磁阀，先关输入，在关输出。然后置测试标识
 * 修改日期：
 *******************************************************************************/
void    StartAirMeasure(void)
{
    OS_ERR  err;

    ////////启动测试
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,1);   //打开输出
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //延时0.5s
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN,1);    //打开输出
    OSTimeDly(100, OS_OPT_TIME_DLY, &err);      //延时0.1s

    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 1;  //气路打开标识置1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 0; 
    
    OSTimeDly(10000, OS_OPT_TIME_DLY, &err);    //延时10s
    Ctrl.Para.dat.sRunPara.SysSta.StartMeasure = 1;
    
    OSTimeDly(2000, OS_OPT_TIME_DLY, &err);     //延时2s    
    //初始化定时器
    osal_start_timerEx( OS_TASK_ID_STORE,
                      OS_EVT_STORE_TICKS,
                      1);                       //置存储定时器1，马上启动存储
    Ctrl.Para.dat.sRunPara.SysSta.Store   = 1;  //可以进行数据储存（测量过程的数据）
    
    OSTimeDly(2000, OS_OPT_TIME_DLY, &err);     //延时2s   
    
    CloseAir();                                 //关闭气路
      
    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 0;  //气路打开标识置1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 1;     
    
    Ctrl.Para.dat.sRunPara.SysSta.StartMeasure = 0;
}


/*******************************************************************************
 * 名    称： APP_MeasureInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： Roger-WY.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
static void APP_MeasureInit(void)
{
    /***********************************************
    * 描述： 初始化本任务用到的相关硬件
    */
    Measure_GPIO_Init();            //初始化端口

    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    //WdtFlags |= WDT_FLAG_KEY;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif