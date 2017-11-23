/*******************************************************************************
 *   Filename:      app.c
 *   Revised:       $Date: 2013-06-30$
 *   Revision:      $
 *	 Writer:		Wuming Shen.
 *
 *   Description:
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <app.h>
#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *main__c = "$Id: $";
#endif

/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 2)

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 *
 * 返回类型         函数名称                入口参数
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */



void    app_init_sctrl(void)
{
    Ctrl.Para.dat.Password      =   6237;
//    Ctrl.Para.dat.SoftWareID    =   1705;   //年月，版本
    

//   	FRAM_StoreRunPara((stcRunPara *) &Ctrl.sRunPara);
    FRAM_ReadRunPara((stcRunPara *) &Ctrl.Para.dat.sRunPara);   //读取运行参数
 
//开始标示    //存储时间间隔
    Ctrl.Para.dat.sRunPara.StartFlg = 1;  
    //Ctrl.Para.dat.sRunPara.SysSta   = 0;        
        
    Ctrl.Para.dat.sRunPara.RecClear = 0;
    Ctrl.Para.dat.sRunPara.StoreTime= 60;       
    Ctrl.Para.dat.sRunPara.SysReset = 0;
    
    Ctrl.Para.dat.sAirPara.sStandard_Dust.GoodLimit     = 150;
    Ctrl.Para.dat.sAirPara.sStandard_Dust.NormalLimit   = 500;
    
    Ctrl.Para.dat.sAirPara.sStandard_VOC.GoodLimit      = 300;
    Ctrl.Para.dat.sAirPara.sStandard_VOC.NormalLimit    = 1000;
    
    Ctrl.Para.dat.sAirPara.sStandard_Hum.GoodLimit      = 50;
    Ctrl.Para.dat.sAirPara.sStandard_Hum.NormalLimit    = 70;
    
    Ctrl.Para.dat.sAirPara.Dust_modefy  = 0;
    Ctrl.Para.dat.sAirPara.Hum_modefy   = 0;
    Ctrl.Para.dat.sAirPara.Voc_modefy   = 0;
    
    FRAM_StoreRunPara((stcRunPara *) &Ctrl.Para.dat.sRunPara);   //读取运行参数
    
    
//测量装置1 初始化    
    Ctrl.Mtr.ConnCtrl[0].ConnFlg   = 1;
    Ctrl.Mtr.ConnCtrl[0].ErrFlg    = 0;
    Ctrl.Mtr.ConnCtrl[0].MasterAddr= MASTE_ADDR_HOST;
//    Ctrl.Mtr.ConnCtrl[0].SlaveAddr = MASTE_ADDR_HOST+0x10;
    Ctrl.Mtr.ConnCtrl[0].SlaveAddr = SLAVE_ADDR_DIP1;
    Ctrl.Mtr.ConnCtrl[0].SendFlg   = 0;
    Ctrl.Mtr.ConnCtrl[0].SendFramNum=1;
    Ctrl.Mtr.ConnCtrl[0].TimeOut   = 10;
    
//测量装置2 初始化    
    Ctrl.Mtr.ConnCtrl[1].ConnFlg   = 1;
    Ctrl.Mtr.ConnCtrl[1].ErrFlg    = 0;
    Ctrl.Mtr.ConnCtrl[1].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Mtr.ConnCtrl[1].SlaveAddr = SLAVE_ADDR_DIP2;
    Ctrl.Mtr.ConnCtrl[1].RecvEndFlg= 0;
    Ctrl.Mtr.ConnCtrl[1].SendFlg   = 0;
    Ctrl.Mtr.ConnCtrl[1].SendFramNum=1;
    Ctrl.Mtr.ConnCtrl[1].TimeOut   = 10;    
    
//无线模块 数据 初始化    
    Ctrl.Dtu.ConnCtrl[0].ConnFlg   = 1;
    Ctrl.Dtu.ConnCtrl[0].ErrFlg    = 0;
    Ctrl.Dtu.ConnCtrl[0].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Dtu.ConnCtrl[0].SlaveAddr = SLAVE_ADDR_DTU;
    Ctrl.Dtu.ConnCtrl[0].RecvEndFlg= 0;
    Ctrl.Dtu.ConnCtrl[0].SendFlg   = 0;
    Ctrl.Dtu.ConnCtrl[0].SendFramNum=1;
    Ctrl.Dtu.ConnCtrl[0].TimeOut   = 10;    
    
//无线模块 参数 初始化    
    Ctrl.Dtu.ConnCtrl[1].ConnFlg   = 1;
    Ctrl.Dtu.ConnCtrl[1].ErrFlg    = 0;
    Ctrl.Dtu.ConnCtrl[1].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Dtu.ConnCtrl[1].SlaveAddr = SLAVE_ADDR_SET;
    Ctrl.Dtu.ConnCtrl[1].RecvEndFlg= 0;
    Ctrl.Dtu.ConnCtrl[1].SendFlg   = 0;
    Ctrl.Dtu.ConnCtrl[1].SendFramNum=1;
    Ctrl.Dtu.ConnCtrl[1].TimeOut   = 10;   
}


/*******************************************************************************
 * 名    称： App_Main
 * 功    能： 系统初开始前调用此函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈
 * 创建日期： 2014-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void App_Main(void)
 {
    /***********************************************
    * 描述： Disable all interrupts.
    */
    BSP_IntDisAll();
    /***********************************************
    * 描述： 重新设置应用程序中断向量表，如果没有BOOTLOADER
    *        NVIC_VectTab_FLASH_OFFSET = 0
    */
#if defined     (IMAGE_A)
    /***********************************************
    * 描述： 程序A
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR_A-FLASH_BOOT_ADDR);
    IAP_SetStatus(IAP_STS_SUCCEED); 
#elif defined   (IMAGE_B)
    /***********************************************
    * 描述： 程序B
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR_B-FLASH_BOOT_ADDR);
    IAP_SetStatus(IAP_STS_SUCCEED); 
#else
    /***********************************************
    * 描述： 程序
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X0000);
#endif
 
    
    //iap  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X10000);

    //init  ctrl
    app_init_sctrl();
 }

/*******************************************************************************
 * 名    称： App_InitStartHook
 * 功    能： 系统初始化开始前调用此函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈
 * 创建日期： 2014-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void App_InitStartHook(void)
 {
 
 }
 
/*******************************************************************************
 * 名    称： App_InitEndHook
 * 功    能： 系统初始化结速后调用此函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈
 * 创建日期： 2014-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void App_InitEndHook(void)
 {
 
 }
 
 /*******************************************************************************
 * 名    称： OS_TaskCreateHook
 * 功    能： OS创建任务回调函数
 * 入口参数： 无
 * 出口参数： 无
 * 作  　者： 无名沈
 * 创建日期： 2014-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void OS_TaskCreateHook(void)
 {     
//    App_TaskMaterCreate();
//    App_TaskDtuCreate();
    //App_TaskKeyCreate();
    //App_TaskBeepCreate();
 }
 
 /*******************************************************************************
 * 名    称： OSAL_TaskCreateHook
 * 功    能： OSAL创建任务回调函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： 无名沈
 * 创建日期： 2014-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void OSAL_TaskCreateHook(void)
 {
    osal_add_Task(TaskInitTmr,                      // 任务初始化函数指针
                  TaskTmrEvtProcess,                // 任务事件处理函数指针
                  OS_TASK_PRO_TMR,                  // 任务优先级
                  OS_TASK_ID_TMR);                  // 任务ID
    
    osal_add_Task(TaskInitDisp,                     // 任务初始化函数指针
                  TaskDispEvtProcess,               // 任务事件处理函数指针
                  OS_TASK_PRO_DISP,                 // 任务优先级
                  OS_TASK_ID_DISP);                 // 任务ID 
    
    osal_add_Task(TaskInitLed,                      // 任务初始化函数指针
                  TaskLedEvtProcess,                // 任务事件处理函数指针
                  OS_TASK_PRO_LED,                  // 任务优先级
                  OS_TASK_ID_LED);                  // 任务ID    
    
    osal_add_Task(TaskInitStore,                    // 任务初始化函数指针
                  TaskStoreEvtProcess,              // 任务事件处理函数指针
                  OS_TASK_PRO_STORE,                // 任务优先级
                  OS_TASK_ID_STORE);                // 任务ID
 }
/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
