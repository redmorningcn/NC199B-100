/*******************************************************************************
 *   Filename:       app_task_disp.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    双击选中 disp 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Disp 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 DISP 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_DISP_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_DISP_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskDispCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Disp ）
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
#include <global.h>
#include <app_ctrl.h>
#include <bsp_max7219.h>
#include <osal_timer.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_disp__c = "$Id: $";
#endif

#define APP_TASK_DISP_EN     DEF_ENABLED
#if APP_TASK_DISP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
//#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 2u)

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
static  OS_TCB   AppTaskDispTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
//static  CPU_STK  AppTaskDispStk[ APP_TASK_DISP_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void    comm_mtr_err_judye(void);

     
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskDisp           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
uint8 JudgeFlashIDErrFlg(void) ;  

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern stcCtrlDCB           Ctrl;


/*******************************************************************************/
static  uint8   dis_mode    = 0;

void    SetDispNow(void)
{
    BSP_DispEvtProcess();
    dis_mode = 13*2;
}


/*******************************************************************************
 * 名    称： AppTaskDisp
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/
extern  MODBUS_CH   *g_pch; 

//#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskDispEvtProcess(osalTid task_id, osalEvt task_event)
//#else
//static  void  AppTaskDisp (void *p_arg)
//#endif
{
/***********************************************
* 描述： Task body, always written as an infinite loop.
*/

    OSSetWdtFlag( WDT_FLAG_DISP );
    
    if( task_event & OS_EVT_DISP_TICKS ) {
        //#else
        //#endif
        static  uint32   times   = 0;
        static  uint8   disflg  = 0;
        
        //            stcTime         sTime;
        
        /***********************************************
        * 描述： 根据显示模式显示
        */
        switch(dis_mode/2) {
            /*******************************************************************
            * 描述： 显示装置时间
            */
        case 0:         //车型
            dis_mode++;
            uprintf("%d",Ctrl.Para.dat.sProductInfo.sLocoId.Type);  
            break;
        case 1:         //车号
            dis_mode++;
            uprintf("%d",Ctrl.Para.dat.sProductInfo.sLocoId.Num);  
            break;
            
            /*******************************************************************
            * 描述： 显示软件版本
            *        时间：开机显示一次，显示2s；
            */
        case 2: 
            dis_mode++;
            Ctrl.Para.dat.SoftWareID = 0x0104;
            uprintf("%02X.%02X.",(uint8)(Ctrl.Para.dat.SoftWareID>>8),(uint8)(Ctrl.Para.dat.SoftWareID));
            
            break;
            /*******************************************************************
            * 描述： 显示硬件版本
            *        第三版硬件    
            */
        case 3: 
            dis_mode++;
            uprintf("HV1.0");
            
            break;
            /*******************************************************************
            * 描述： 粉尘修正值
            */
        case 4: 
            dis_mode++;
            uprintf("%d.",Ctrl.Para.dat.sAirPara.Dust_modefy);  
            break;
            /*******************************************************************
            * 描述： 湿度修正值
            */
        case 5:  
            dis_mode++;
            uprintf("%d.",Ctrl.Para.dat.sAirPara.Hum_modefy);  
            break;  
            /*******************************************************************
            * 描述： Voc修正值
            */
        case 6:  
            dis_mode++;
            uprintf("%d.",Ctrl.Para.dat.sAirPara.Voc_modefy);  
            break; 
            /*******************************************************************
            * 描述： 车型车号
            */
        case 7:  
            dis_mode++;
            uprintf("      ");
            
            break; 
            /*******************************************************************
            * 描述： 全灭
            */
        case 8:  
            dis_mode++;
            uprintf("88888888");
            
            break;   
            /*******************************************************************
            * 描述： 高度值
            */
        case 9:    
            dis_mode++;
            
        case 13:  
            dis_mode++;
            //不赋值，只显示
            //disflg = BSP_DispEvtProcess();  //执行显示操作，有内容显示，不赋值。否者油量赋值
            
            break;   
            //            case 14:  
            //               dis_mode++;
            //                //不赋值，只显示
            //                //disflg = BSP_DispEvtProcess();  //执行显示操作，有内容显示，不赋值。否者油量赋值
            //                break;   
            //            case 15:  
            //                dis_mode++;
            //                //disflg = BSP_DispEvtProcess();  //执行显示操作，有内容显示，不赋值。否者油量赋值
            //                //不赋值，只显示
            //                times = 0;
            //                break;   
            /*******************************************************************
            * 描述： 开机后，显示油量，通讯故障判断及故障代码
            */
        default:
            
            if(disflg != 0)                                 //如果有其他内容，先显示其他内容
                break;
            
            switch(times++%9)                               //循环显示油水
            {
            case 0:uprintf("T");   
            break;    
            case 1:uprintf("%4d",Ctrl.sRec.sAir.Temperature);   
            break;     
            
            case 2:uprintf("H2o");   
            break;                              //显示湿度提示
            case 3:uprintf("%4d",Ctrl.sRec.sAir.Humidity);   
            break;                              //显示湿度值
            
            case 4:uprintf("o1L");   
            break;                              //显示油含量
            case 5:uprintf("%4d",Ctrl.sRec.sAir.VOCs);     
            break;                              //显示湿度值
            
            case 6:uprintf("dust");                            
            break;                              //显示粉尘提示
            case 7: uprintf("%4d",Ctrl.sRec.sAir.PM2D5_S);   
            break;                              //显示粉尘值  
            
            //                    case 8:  
            ////                        if(Ctrl.Para.dat.sRunPara.SysSta.Rsv ){
            ////                            Ctrl.Para.dat.sRunPara.SysSta.Rsv = 0;
            ////                            uprintf("tmr");
            ////
            ////                        }
            ////                        else if(Ctrl.Para.dat.sRunPara.SysSta.mtrsim ){
            ////                            Ctrl.Para.dat.sRunPara.SysSta.mtrsim =  0;
            ////                            uprintf("mtr");
            ////                            
            ////                            //return ( task_event ^ OS_EVT_DISP_TICKS );
            ////                        }else if(Ctrl.Para.dat.sRunPara.SysSta.mtrsend)
            ////                        {
            ////                            Ctrl.Para.dat.sRunPara.SysSta.mtrsend =  0;
            ////                            uprintf("send");
            ////
            ////                        }
            ////                        
            ////                        BSP_DispEvtProcess(); 
            ////                        goto exit;
            //        
            //                        break;                              //有故障显示故障代码
            }
            
            break;
        }
        
        disflg = BSP_DispEvtProcess();                      //执行显示操作，有内容显示，不赋值。否者油量赋值
    exit:            
        osal_start_timerEx( OS_TASK_ID_DISP,
                           OS_EVT_DISP_TICKS,
                           800);                           //0.8s显示一次
        return ( task_event ^ OS_EVT_DISP_TICKS );
    }
    return  task_event;
}


/*******************************************************************************
 * 名    称： APP_DispInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-12-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitDisp(void)
{    
//    /***********************************************
//    * 描述： 初始化本任务用到的相关硬件
//    */
    BSP_DispInit();

    BSP_DispOff();
    
    BSP_DispSetBrightness(10);
    BSP_DispClrAll();
    uprintf("88888888");
    BSP_DispEvtProcess();
    

    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    OSRegWdtFlag( WDT_FLAG_DISP );

    /*************************************************
    * 描述：启动事件查询
    */
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        100);
}

void    comm_mtr_err_judye(void)
{
    /*******************************************************************
    * 描述：油尺串口通讯故障判断
    */
//    static  uint32  comm_mtr_err_times = 0;
//    
//    if(         Ctrl.Mtr.ConnCtrl[0].ErrFlg == 1 
//       &&       Ctrl.Mtr.ConnCtrl[1].ErrFlg == 1){
//
//        comm_mtr_err_times++;                   //错误次数
//        
//        if(comm_mtr_err_times > 4 && Ctrl.sRunPara.SysSta == 0 ){
//            Ctrl.sRunPara.SysSta   = 83;       //测量装置1、测量装置2均有故障
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //错误代码赋值
//        }
//    }else if(   Ctrl.Mtr.ConnCtrl[0].ErrFlg == 1){
//        comm_mtr_err_times++;                   //错误次数
//        
//        if(comm_mtr_err_times > 8 && Ctrl.sRunPara.SysSta == 0 ){
//            Ctrl.sRunPara.SysSta   = 71;       //测量装置1故障
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //错误代码赋值
//        }
//        
//    }else if(   Ctrl.Mtr.ConnCtrl[1].ErrFlg == 1){
//        comm_mtr_err_times++;                   //错误次数
//        
//        if(comm_mtr_err_times > 8 && Ctrl.sRunPara.SysSta == 0 )
//            Ctrl.sRunPara.SysSta   = 81;       //测量装置2故障
//        
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //错误代码赋值
//    }else {
//        comm_mtr_err_times = 0;                 //错误次数
//
//        if(     Ctrl.sRunPara.SysSta == 81
//            ||  Ctrl.sRunPara.SysSta == 71
//            ||  Ctrl.sRunPara.SysSta == 83){
//            uprintf("E-%d",Ctrl.sRunPara.SysSta+100);  //显示故障代码+100
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta+100;//错误代码赋值
//            Ctrl.sRunPara.SysSta   = 0;        //清除故障代码
//        }
//    }
}
/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif