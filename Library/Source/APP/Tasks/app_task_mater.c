/*******************************************************************************
 *   Filename:       app_task_mater.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    双击选中 mater 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Mater 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 MATER 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_MATER_PRIO     ）
 *                                            和 任务堆栈（ APP_TASK_MATER_STK_SIZE ）大小
 *
 *   Notes:
 *     				 E-mail: shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
//#include <app_mater_protocol.h>
#include <bsp_flash.h>
#include <iap.h>
     
#include "stm32f10x.h"
#include <stdio.h>
#include "SZ_STM32F107VC_LIB.h"
#include "DS3231.h"
#include "I2C_CLK.h"
#include "Display.h"
#include "DATA_STORAGE.h"
#include "DELAY.h"
#include "RS485.h"
#include "POWER_MACRO.h"
#include "MX25.h"
#include "FM24CL64.h"
#include "string.h"
#include "xprintf.h"
#include "WatchDog.h"
#include <app_comm_protocol.h>
#include <crccheck.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_mater__c = "$Id: $";
#endif

#define APP_TASK_MATER_EN     DEF_ENABLED
#if APP_TASK_MATER_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
/***********************************************
* 描述： 数据记录地址
*
地址	长度	类型	名称
AC.PPpower_NUM
0
4
8
AC.NPpower_NUM
12
16
20
调试数据
32	4		直流电压偏移
36	4		直流电流偏移
40	4		交流电压偏移
44	4		交流电流偏移
48	4	u32	交流电压增益
52	4	u32	交流电流增益
56	4	u32	脉冲速率
//60	4	u32	数据地址
64	4	u32	设备ID
68	4	U8	车型车号
72	4	u32	记录起始地址
76	4	u32	记录结束地址
80	4	u32	记录流水号
84	4	u32	记录时间间隔
88	4	f32	电压比例
92	4	f32	电流比例
96  4   u32 零压处理阀值
AC.PQpower_NUM
100
104
108
AC.NQpower_NUM
112	4		负有功电量数据
116	4
120	4

//
160 4   f32 电压调节系数
164 4   f32 电流调节系数
*/
/*******************************************************************************
 * MACROS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
 * TYPEDEFS
 */

/***********************************************
* 描述： 任务控制块（TCB）
*/
static  OS_TCB      AppTaskMaterTCB;
OS_Q                MTR_RxQ;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK     AppTaskMaterStk[ APP_TASK_MATER_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */
ENERGY              AC;
SDAT 	            recordsfr;	
StrMater            Mater;
StrMater            History;

static volatile uint32_t            SysTime = 0;

uint8_t             g_DipDisVal[40];
uint32_t            g_Flash_Adr;

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void     AppTaskMater                (void *p_arg);
static void     APP_MaterInit               (void);

void            DC_COUNTInit                (void);
void            AC_COUNTInit                (void);
INT08U          APP_SevrRxDataDealCB        (MODBUS_CH  *pch);
void            APP_MaterSendMsgToServer    (void);
void            EXTI0_ISRHandler            (void);
void            EXTI9_5_ISRHandler          (void);

CPU_BOOLEAN     APP_CSNC_CommHandler        (MODBUS_CH  *pch);
CPU_BOOLEAN     APP_MaterCommHandler        (MODBUS_CH  *pch);

void            SavePowerData               (void);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern void     CS5463_AC_Adjust        (MODBUS_CH  *pch,uint8_t mode)	;
extern void     uartprintf              (MODBUS_CH  *pch,const char *fmt, ...);
/*******************************************************************************/

/*******************************************************************************
 * 名    称： App_TaskMaterCreate
 * 功    能： 任务创建
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
void  App_TaskMaterCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * 描述： 任务创建
    */
    OSTaskCreate((OS_TCB     *)&AppTaskMaterTCB,                    // 任务控制块  （当前文件中定义）
                 (CPU_CHAR   *)"App Task Mater",                    // 任务名称
                 (OS_TASK_PTR ) AppTaskMater,                       // 任务函数指针（当前文件中定义）
                 (void       *) 0,                                  // 任务函数参数
                 (OS_PRIO     ) APP_TASK_MATER_PRIO,                // 任务优先级，不同任务优先级可以相同，0 < 优先级 < OS_CFG_PRIO_MAX - 2（app_cfg.h中定义）
                 (CPU_STK    *)&AppTaskMaterStk[0],                 // 任务栈顶
                 (CPU_STK_SIZE) APP_TASK_MATER_STK_SIZE / 10,       // 任务栈溢出报警值
                 (CPU_STK_SIZE) APP_TASK_MATER_STK_SIZE,            // 任务栈大小（CPU数据宽度 * 8 * size = 4 * 8 * size(字节)）（app_cfg.h中定义）
                 (OS_MSG_QTY  ) 0u,                                 // 可以发送给任务的最大消息队列数量
                 (OS_TICK     ) 0u,                                 // 相同优先级任务的轮循时间（ms），0为默认
                 (void       *) 0,                                  // 是一个指向它被用作一个TCB扩展用户提供的存储器位置
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // 允许堆栈检查该任务
                                OS_OPT_TASK_STK_CLR),               // 创建任务时堆栈清零
                 (OS_ERR     *)&err);                               // 指向错误代码的指针，用于创建结果处理
}

/*******************************************************************************
 * 名    称： AppTaskMater
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/
static  void  AppTaskMater (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly     = CYCLE_TIME_TICKS;
    OS_TICK     ticks;
        
    /***********************************************
    * 描述： 创建事件标志组
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.MaterEvtFlag= COMM_EVT_FLAG_HEART       // 心跳包发送
                        + COMM_EVT_FLAG_RESET       // COMM复位
                        + COMM_EVT_FLAG_CONNECT     // COMM连接
                        + COMM_EVT_FLAG_RECV        // 串口接收
                        + COMM_EVT_FLAG_REPORT      // 串口发送
                        + COMM_EVT_FLAG_CLOSE       // 断开
                        + COMM_EVT_FLAG_TIMEOUT     // 超时
                        + COMM_EVT_FLAG_CONFIG;     // 配置
    
    OSQCreate ( (OS_Q        *)&MTR_RxQ,
                (CPU_CHAR    *)"RxQ",
                (OS_MSG_QTY   ) OS_CFG_INT_Q_SIZE,
                (OS_ERR      *)&err);
    /***********************************************
    * 描述： 任务初始化    
    */ 
  	float   Vac_RMS_F           = 0;
  	float   Iac_RMS_F           = 0;
        
 	uint32_t PP_Power_temp1     = 0;
	uint32_t PP_Power_temp2     = 0;
	uint32_t PP_Power_temp3     = 0;
    
	uint32_t NP_Power_temp1     = 0;
	uint32_t NP_Power_temp2     = 0;
	uint32_t NP_Power_temp3     = 0;
    
	uint8_t  PP_fram_temp[16]   = {0};
	uint8_t  NP_fram_temp[16]   = {0};
    
	int32_t  PQ_Power_temp1     = 0;
	uint32_t PQ_Power_temp2     = 0;
	uint32_t PQ_Power_temp3     = 0;
    
	uint32_t NQ_Power_temp1     = 0;
	uint32_t NQ_Power_temp2     = 0;
	uint32_t NQ_Power_temp3     = 0;
    
	uint8_t  PQ_fram_temp[16]   = {0};
	uint8_t  NQ_fram_temp[16]   = {0};
    
	uint32_t i                  = 0;
        
    /***********************************************
    * 描述： 电表初始化
    */   
    APP_MaterInit();
    
    /***********************************************
    * 描述： 读取机车型号，机车号
    */     
    ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
    ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
    ReadFM24CL64(64 , (uint8_t *)&Ctrl.ID, 4);
    /***********************************************
    * 描述： 记录时间间隔
    */  
    ReadFM24CL64(84 , (uint8_t *)&Mater.Record.RecordTime, 4);
    if ( Mater.Record.RecordTime < OS_TICKS_PER_SEC * 10 )
        Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
    else if ( Mater.Record.RecordTime > OS_TICKS_PER_SEC * 300 )
        Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
        
    ReadFM24CL64(88 , (uint8_t *)&AC.U_SCL, 4);
    if ( AC.U_SCL < 1 )
        AC.U_SCL = 1.0;
    else if ( AC.U_SCL > 50000 )
        AC.U_SCL    = 50000.0;
    
    ReadFM24CL64(92 , (uint8_t *)&AC.I_SCL, 4);
    if ( AC.I_SCL < 1.0 )
        AC.I_SCL = 1.0;
    else if ( AC.I_SCL > 1000.0 )
        AC.I_SCL    = 1000.0;
    //*
    float fdat;
    ReadFM24CL64(96 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 5000.0;    
    else if ( fdat > 20000.0 )
        fdat    = 20000.0;
    AC.U_O      = fdat;
    
    ReadFM24CL64(160 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 1.0;    
    else if ( fdat< 0.5 )
        fdat = 0.5;
    else if ( fdat > 2.0 )
        fdat    = 2.0;
    AC.U_K      = fdat;
    
    ReadFM24CL64(164 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 1.0;    
    else if ( fdat < 0.5 )
        fdat = 0.5;
    else if ( fdat > 2.0 )
        fdat    = 2.0;
    AC.I_K      = fdat;
    /***********************************************
    * 描述： 读取参数
    */ 
	ReadFM24CL64(  0, PP_fram_temp, 12);
	ReadFM24CL64( 12, NP_fram_temp, 12);
    
	ReadFM24CL64(100, PQ_fram_temp, 12);
	ReadFM24CL64(112, NQ_fram_temp, 12);
    /***********************************************
    * 描述： 正有功电能
    */ 
	PP_Power_temp1 =  PP_fram_temp[3]<<24 |  PP_fram_temp[2]<<16 | PP_fram_temp[1]<<8 | PP_fram_temp[0];
	PP_Power_temp2 =  PP_fram_temp[7]<<24 |  PP_fram_temp[6]<<16 | PP_fram_temp[5]<<8 | PP_fram_temp[4];
	PP_Power_temp3 = PP_fram_temp[11]<<24 | PP_fram_temp[10]<<16 | PP_fram_temp[9]<<8 | PP_fram_temp[8];
	
    if(PP_Power_temp1 == PP_Power_temp2)
		AC.PPpower_base = PP_Power_temp1;
	else if(PP_Power_temp2 == PP_Power_temp3)
		AC.PPpower_base = PP_Power_temp2;
	else
		AC.PPpower_base = PP_Power_temp1;
   	
    /***********************************************
    * 描述： 负有功电能
    */ 
	NP_Power_temp1 =  NP_fram_temp[3]<<24 |  NP_fram_temp[2]<<16 | NP_fram_temp[1]<<8 | NP_fram_temp[0];
	NP_Power_temp2 =  NP_fram_temp[7]<<24 |  NP_fram_temp[6]<<16 | NP_fram_temp[5]<<8 | NP_fram_temp[4];
	NP_Power_temp3 = NP_fram_temp[11]<<24 | NP_fram_temp[10]<<16 | NP_fram_temp[9]<<8 | NP_fram_temp[8];
    
	if(NP_Power_temp1 == NP_Power_temp2)
		AC.NPpower_base = NP_Power_temp1;
	else if(NP_Power_temp2 == NP_Power_temp3)
		AC.NPpower_base = NP_Power_temp2;
	else
		AC.NPpower_base = NP_Power_temp1;
    
    /***********************************************
    * 描述： 正无功电能
    */
	PQ_Power_temp1 =  PQ_fram_temp[3]<<24 |  PQ_fram_temp[2]<<16 | PQ_fram_temp[1]<<8 | PQ_fram_temp[0];
	PQ_Power_temp2 =  PQ_fram_temp[7]<<24 |  PQ_fram_temp[6]<<16 | PQ_fram_temp[5]<<8 | PQ_fram_temp[4];
	PQ_Power_temp3 = PQ_fram_temp[11]<<24 | PQ_fram_temp[10]<<16 | PQ_fram_temp[9]<<8 | PQ_fram_temp[8];
    
	if(PQ_Power_temp1 == PQ_Power_temp2)
		AC.PQpower_base = PQ_Power_temp1;
	else if(PQ_Power_temp2 == PQ_Power_temp3)
		AC.PQpower_base = PQ_Power_temp2;
	else
		AC.PQpower_base = PQ_Power_temp1;
    
    /***********************************************
    * 描述： 负无功电能
    */ 
	NQ_Power_temp1 =  NQ_fram_temp[3]<<24 |  NQ_fram_temp[2]<<16  | NQ_fram_temp[1]<<8 | NQ_fram_temp[0];
	NQ_Power_temp2 =  NQ_fram_temp[7]<<24 |  NQ_fram_temp[6]<<16  | NQ_fram_temp[5]<<8 | NQ_fram_temp[4];
	NQ_Power_temp3 = NQ_fram_temp[11]<<24 | NQ_fram_temp[10]<<16  | NQ_fram_temp[9]<<8 | NQ_fram_temp[8];
    
	if(NQ_Power_temp1 == NQ_Power_temp2)
		AC.NQpower_base = NQ_Power_temp1;
	else if(NQ_Power_temp2 == NQ_Power_temp3)
		AC.NQpower_base = NQ_Power_temp2;
	else
		AC.NQpower_base = NQ_Power_temp1;
    
	ReadFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
	ReadFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
	ReadFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
    if ( Mater.Record.Head > MAX_ADDR ) {
        Mater.Record.Head  = 0;
    } else {
        Mater.Record.Head  = Mater.Record.Head / sizeof(Mater);
        Mater.Record.Head  = Mater.Record.Head * sizeof(Mater);
    }
    if ( Mater.Record.Tail > MAX_ADDR ) {
        Mater.Record.Tail  = 0;
    } else {
        Mater.Record.Tail  = Mater.Record.Tail / sizeof(Mater);
        Mater.Record.Tail  = Mater.Record.Tail * sizeof(Mater);
    }
    if ( Mater.RecordNbr == 0xffffffff )
        Mater.RecordNbr  = 0;
    
    BSP_OS_TimeDly(OS_TICKS_PER_SEC / 2);
    /***********************************************
    * 描述： 
    */ 
	uartprintf(Ctrl.Com.pch,"\r\n当前头地址：0x%08X\r\n当前尾地址：0x%08X\r\n当前记录号：%d",
               Mater.Record.Head,
               Mater.Record.Tail,
               Mater.RecordNbr);
                
    /***********************************************
    * 描述： Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {
        /***********************************************
        * 描述： 本任务看门狗标志置位
        */
        OS_ERR      terr;
        ticks       = OSTimeGet(&terr);                 // 获取当前OSTick
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_MATER,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);
        
        /***********************************************
        * 描述： 等待DTU数据接收消息队列
        */
        OS_MSG_SIZE p_msg_size;
        
        MODBUS_CH *pch = 
       (MODBUS_CH *)OSQPend ((OS_Q*)&MTR_RxQ,
                    (OS_TICK       )dly,
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,//OS_OPT_PEND_NON_BLOCKING,
                    (OS_MSG_SIZE  *)&p_msg_size,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
        /***********************************************
        * 描述： 2017/8/19：收到电表串口数据
        */
        if ( OS_ERR_NONE == err ) {
            /***********************************************
            * 描述： 2017/8/19：保存接收到的数据数量并处理数据
            */
            pch->RxBufByteCnt   = p_msg_size;
            APP_MaterCommHandler(pch);
        }
        
        /***********************************************
        * 描述： 等待COMM操作标志位
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.MaterEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.MaterEvtFlag,
                    ( OS_TICK      ) dly,
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING,
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        /***********************************************
        * 描述： 没有错误,有事件发生
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
            /***********************************************
            * 描述： 心跳包发送
            */
            if       ( flags & COMM_EVT_FLAG_HEART  ) {
                flagClr |= COMM_EVT_FLAG_HEART;
                // 保存电量
                SavePowerData();
            /***********************************************
            * 描述： COMM复位
            */
            } 
            if ( flags & COMM_EVT_FLAG_RESET ) {
                flagClr |= COMM_EVT_FLAG_RESET;
                
            /***********************************************
            * 描述： COMM连接
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONNECT ) {
                flagClr |= COMM_EVT_FLAG_CONNECT;
                
            /***********************************************
            * 描述： 串口接收
            */
            } 
            if ( flags & COMM_EVT_FLAG_RECV ) {
                flagClr |= COMM_EVT_FLAG_RECV;
                
            /***********************************************
            * 描述： 上报消息
            */
            } 
            if ( flags & COMM_EVT_FLAG_REPORT ) {
                flagClr |= COMM_EVT_FLAG_REPORT; 
                
            /***********************************************
            * 描述： 断开
            */
            } 
            if ( flags & COMM_EVT_FLAG_CLOSE ) {
                flagClr |= COMM_EVT_FLAG_CLOSE;
                
            /***********************************************
            * 描述： 超时
            */
            } 
            if ( flags & COMM_EVT_FLAG_TIMEOUT ) {
                flagClr |= COMM_EVT_FLAG_TIMEOUT;
                
            /***********************************************
            * 描述： 配置
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONFIG ) {
                flagClr |= COMM_EVT_FLAG_CONFIG;
                /***********************************************
                * 描述： 保存数据
                */                 
                SavePowerData();
                //SaveRecord();
                void  App_SaveDataToHistory( void );
                App_SaveDataToHistory();
                
                uartprintf(Ctrl.Com.pch,"\r\n当前时间：20%02d-%02d-%02d  %02d:%02d:%02d", 
                           Mater.Tax.Tax3.Vari.LKJTime.Year, 
                           Mater.Tax.Tax3.Vari.LKJTime.Mon, 
                           Mater.Tax.Tax3.Vari.LKJTime.Day,
                           Mater.Tax.Tax3.Vari.LKJTime.Hour, 
                           Mater.Tax.Tax3.Vari.LKJTime.Min, 
                           Mater.Tax.Tax3.Vari.LKJTime.Sec);
                uartprintf(Ctrl.Com.pch,
                           "\r\n存储数据成功 ID = %d\r\n正有功 = %d;\r\n负有功 = %d;\r\n正无功 = %d;\r\n负无功 = %d;\r\n电  压 = %d;\r\n电  流 = %d;\r\n频  率 = %d;\r\n功率因素 = %d;\r\n有功功率 = %d;\r\n无功功率 = %d\r\n",
                            Mater.RecordNbr,
                            Mater.Energy.PPPower,      
                            Mater.Energy.NPPower,      
                            Mater.Energy.PQPower,      
                            Mater.Energy.NQPower,      
                            Mater.Energy.PrimVolt/1000,
                            Mater.Energy.PrimCurr/1000,
                            Mater.Energy.PowerFreq/1000,
                            Mater.Energy.PowerFactor,
                            Mater.Energy.ActivePower,  
                            Mater.Energy.ReactivePower);
                /***********************************************
                * 描述： 发送数据处理
                */
            } 
            //exit:
            /***********************************************
            * 描述： 清除标志
            */
            if ( !flagClr ) {
                flagClr = flags;
            }
            
            /***********************************************
            * 描述： 清除标志位
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
        /***********************************************
        * 描述： 如果超时，则发送一次心跳包
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                        ( OS_FLAGS      )Ctrl.Os.MaterEvtFlag,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err );
        }
        /***********************************************
        * 描述： 正有功、负有功、正无功、负无功电量
        */
        i++;
		AC.PPpower_NUM = (uint32_t)(AC.PPpower_temp + AC.PPpower_base);
		AC.NPpower_NUM = (uint32_t)(AC.NPpower_temp + AC.NPpower_base);
		AC.PQpower_NUM = (uint32_t)(AC.PQpower_temp + AC.PQpower_base);
		AC.NQpower_NUM = (uint32_t)(AC.NQpower_temp + AC.NQpower_base);
		
        /***********************************************
        * 描述： 循环显示，最大值为99999999
        */ 
        if(AC.PPpower_NUM >= 100000000)	{
		
			AC.PPpulse = 0;
			AC.PPpower_temp = 0;
			AC.PPpower_base = 1; 
		}
        /***********************************************
        * 描述： 循环显示，最大值为99999999
        */ 
		if(AC.NPpower_NUM >= 100000000)	{
		
			AC.NPpulse = 0;
			AC.NPpower_temp = 0;
			AC.NPpower_base = 1; 
		}
        /***********************************************
        * 描述： 循环显示，最大值为99999999
        */ 
		if(AC.PQpower_NUM >= 100000000)	{
		
			AC.PQpulse = 0;
			AC.PQpower_temp = 0;
			AC.PQpower_base = 1; 
		}
        /***********************************************
        * 描述： 循环显示，最大值为99999999
        */ 
		if(AC.NQpower_NUM >= 100000000)	{
		
			AC.NQpulse = 0;
			AC.NQpower_temp = 0;
			AC.NQpower_base = 1; 
		}
        /***********************************************
        * 描述： 有功功率、无功功率、功率因素、频率
        */ 
//		AC.ACTIVE_POWER     = (int32_t)(SPI_CS5463_AC_Read_Else_FLOAT(PA) * 20000);
//		AC.REACTIVE_POWER   = (int32_t)(SPI_CS5463_AC_Read_Else_FLOAT(PQ) * 20000);
//		AC.Power_Factor     = SPI_CS5463_AC_Read_Else_FLOAT(PF);
//		AC.Power_Freq       = SPI_CS5463_AC_Read_Else_FLOAT(FREQUENCY) * 4000;
//        
        /***********************************************
        * 描述： 功率因素
        */
        if ( fabs( AC.Power_Factor ) < 0.0005 )
            AC.Power_Factor     = 0;
         
        /**************** *******************************
        * 描述： 电压、电流
        */ 
//		Vac_RMS_F = SPI_CS5463_AC_Read_VIrms_FLOAT(VRMS);
//		Iac_RMS_F = SPI_CS5463_AC_Read_VIrms_FLOAT(IRMS);
		
        /***********************************************
        * 描述： 瞬时电压、瞬时电流:17020003
        *
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//比列系数：25000V/0.6(SS4车型)
        //AC.I_RMS = Iac_RMS_F * 500;				    //比列系数：300A/0.6(SS4车型)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * 0.99960016;		    //比列系数：25000V/0.6(HXD1D车型)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6);       //1000;	    //比列系数：600A/0.6(HXD1D车型)
		
		//if(Vac_RMS_F < 0.0012)							    //电压比例系数较大，去掉浮电压
		//	AC.U_RMS = 0;
        
        if ( AC.I_RMS  > 80 ) {
             AC.I_RMS = AC.I_RMS * 0.975609756;
        }
        
        /***********************************************
        * 描述： 瞬时电压、瞬时电流:17020001
        */
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//比列系数：25000V/0.6(SS4车型)
        //AC.I_RMS = Iac_RMS_F * 500;				    //比列系数：300A/0.6(SS4车型)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * AC.U_K;  //0.995421063;		//比列系数：25000V/0.6(HXD1D车型)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6) * AC.I_K;       //1000;	    //比列系数：600A/0.6(HXD1D车型)
		
		//if(Vac_RMS_F < 0.0012)							    //电压比例系数较大，去掉浮电压
		//	AC.U_RMS = 0;
        
        //if ( AC.I_RMS  > 80 ) {
        //     AC.I_RMS = AC.I_RMS * 0.975609756;
        //}
        /***********************************************
        * 描述： 瞬时电压、瞬时电流:17020002
        * 
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//比列系数：25000V/0.6(SS4车型)
        //AC.I_RMS = Iac_RMS_F * 500;				    //比列系数：300A/0.6(SS4车型)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6);		    //比列系数：25000V/0.6(HXD1D车型)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6);       //1000;	    //比列系数：600A/0.6(HXD1D车型)
		
		//if(Vac_RMS_F < 0.0012)							    //电压比例系数较大，去掉浮电压
		//	AC.U_RMS = 0;
        
        //if ( AC.I_RMS  > 80 ) {
        //     AC.I_RMS = AC.I_RMS * 0.975609756;
        //}
        */
        AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * AC.U_K;		    //比列系数：25000V/0.6(HXD1D车型)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6) * AC.I_K;       //1000;	    //比列系数：600A/0.6(HXD1D车型)
		
        /***********************************************
        * 描述： 
        */
        if ( AC.U_RMS  < AC.U_O ) {
             AC.U_RMS = 0;
        }
        
        Mater.Energy.PPPower        = (uint32_t)((float)AC.PPpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //正有功电量    1kvarh      99999999 kvarh
        Mater.Energy.NPPower        = (uint32_t)((float)AC.NPpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //负有功电量    1kvarh      99999999 kvarh 
        Mater.Energy.PQPower        = (uint32_t)((float)AC.PQpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //正无功电量    1kvarh      99999999 kvarh
        Mater.Energy.NQPower        = (uint32_t)((float)AC.NQpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //负无功电量    1kvarh      99999999 kvarh
        Mater.Energy.PrimVolt       = (uint32_t)(AC.U_RMS * 1000);				//原边电压      0.001V      0～35000.000V
        Mater.Energy.PrimCurr       = (uint32_t)(AC.I_RMS * 1000);				//原边电流      0.001A      0～600.000A
        Mater.Energy.PowerFreq      = (uint32_t)(AC.Power_Freq * 1000);			//频率          0.001Hz    
        Mater.Energy.PowerFactor    = (int32_t)(AC.Power_Factor * 1000);		//功率因数      0.001       -1.000～1.000
        Mater.Energy.ActivePower    = AC.ACTIVE_POWER;			                //有功功率      0.001kW     -12000.000  kW～12000.000  kW
        Mater.Energy.ReactivePower  = AC.REACTIVE_POWER;		                //无功功率      0.001kvar   -12000.000  kvar～12000.000 
        
//        SPI_CS5463_AC_GetDrdy();
//        SPI_CS5463_AC_ClearDrdy();
        /***********************************************
        * 描述： 计算剩余时间
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        }
    }
}

/*******************************************************************************
 * 名    称： APP_MaterInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2016-11-11
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void APP_MaterInit(void)
{
    /***********************************************
    * 描述： 创建事件标志组
    */    
	//SZ_USART1_DATA.uart = SZ_STM32_COM1;
    
	AC.U_RMS = 0;
	AC.I_RMS = 0;
    
	AC.Power_Factor = 0;
	AC.Power_Freq = 0;
    
	AC.ACTIVE_POWER = 0;
	AC.REACTIVE_POWER = 0;
    
	AC.PPpower_NUM = 0;								
	AC.NPpower_NUM = 0;			
	AC.PPpulse = 0;				
	AC.NPpulse = 0;				
	AC.PPpower_base = 0;		
	AC.NPpower_base = 0;		
	AC.PPpower_temp = 0;		
	AC.NPpower_temp = 0;
    
	AC.PQpower_NUM = 0;								
	AC.NQpower_NUM = 0;			
	AC.PQpulse = 0;				
	AC.NQpulse = 0;				
	AC.PQpower_base = 0;		
	AC.NQpower_base = 0;		
	AC.PQpower_temp = 0;		
	AC.NQpower_temp = 0;
        
    BSP_OS_TimeDly(OS_TICKS_PER_SEC);
    /***********************************************
    * 描述： 
    */
	//SZ_STM32_COMInit(COM1, 115200);
	//RS485_SET_RX_Mode();      /* 默认配置RS485的方向为接收，以免引起总线冲突 */
	//xPrintf_Init(19200);           //COM2对应的是RS485
	//LED_DIS_Config();
	I2C_GPIO_Config();
    void EEP_OS_Init(void);
    EEP_OS_Init();
	InitDS3231();
//	SPI_AC_INIT();
//	CS5463_AC_INIT();
	SPI_FLASH_Init();
    
    /***********************************************
    * 描述： 
    */
    //DC_COUNTInit();
	AC_COUNTInit();
    /***********************************************
    * 描述： 
    */
	//NVIC_GroupConfig();
	//NVIC_COMConfiguration();
    
    /***********************************************
    * 描述： 
    */
	//Dis_Test();							 //数码管上电测试
   	//SZ_STM32_SysTickInit(100);
    void PWR_PVD_Init(void);
	PWR_PVD_Init();
    //WDG_Init();
        
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    WdtFlags |= WDT_FLAG_MATER;
}

/*******************************************************************************
 * 名    称： DC_COUNTInit
 * 功    能： 初始化PE9的外部中断功能，进行脉冲计数
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2016-11-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void DC_COUNTInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能GPIOB的Clock时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* 初始化GPIOB8管脚，配置为带上拉的输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* 初始化GPIOA0为中断模式 */
    /* 将GPIOA0对应的管脚连接到内部中断线 */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource9);
    
    /* Configure Button EXTI line */
    /* 将GPIOA0配置为中断模式，下降沿触发中断 */    
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line9); 
    
    /***********************************************
    * 描述： 外部中断9 ~ 5向量
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI9_5, EXTI9_5_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI9_5);
}

/*******************************************************************************
 * 名    称： AC_COUNTInit
 * 功    能： 初始化PE0的外部中断功能，进行脉冲计数
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2016-11-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void AC_COUNTInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    /***********************************************
    * 描述： 电量计量方向控制引脚初始化
    */   
    /* 初始化GPIOD2为GPIO */   
	/* 使能GPIOD的Clock时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    /* Configure Button pin as input floating */
    /* 初始化GPIOD2管脚，配置为带上拉的输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /***********************************************
    * 描述： 有功电量输出引脚初始化
    */  
    /* 使能GPIOE的Clock时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* 初始化GPIOE0管脚，配置为带上拉的输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
	/* 初始化GPIOE0为中断模式 */
    /* 将GPIOE0对应的管脚连接到内部中断线 */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line0); 
    
    /***********************************************
    * 描述： 设置外部中断向量及开启中断
    */    
    /***********************************************
    * 描述： 外部中断0 ~ 4向量
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI0, EXTI0_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI0);
    
    /***********************************************
    * 描述： 无功电量输出引脚初始化
    */  
    /* 使能GPIOB的Clock时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* 初始化GPIOB8管脚，配置为带上拉的输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* 初始化GPIOA0为中断模式 */
    /* 将GPIOA0对应的管脚连接到内部中断线 */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource9);
    
    /* Configure Button EXTI line */
    /* 将GPIOA0配置为中断模式，下降沿触发中断 */    
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line9); 
    
    /***********************************************
    * 描述： 外部中断9 ~ 5向量
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI9_5, EXTI9_5_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI9_5);
}

/*******************************************************************************
 * 名    称： EXTI0_IRQHandler
 * 功    能： 外部中断0处理函数，计量正、负有功功率
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2016-11-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void EXTI0_ISRHandler(void) 
{   
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /***********************************************
        * 描述： 
        */ 
        if ( ( Mater.Energy.PrimVolt        == 0 ) ||
             ( Mater.Energy.PrimCurr        == 0 ) ||
             ( Mater.Energy.PowerFreq       == 0 ) ||
             ( Mater.Energy.PowerFactor     == 0 ) ) {
        } else {
            /***********************************************
            * 描述： 正有功
            */ 
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)) {
                AC.PPpulse++;
                AC.PPpower_temp = AC.PPpulse;                                   //脉冲常数100000，25kV/150V---600A/1A(HXD1D车型)
            /***********************************************
            * 描述： 负有功
            */ 
            } else {
                AC.NPpulse++;
                AC.NPpower_temp = AC.NPpulse;                                   //脉冲常数100000，25kV/150V---600A/1A(HXD1D车型)
            }
        }
        /* Clear the EXTI Line 0 */  
        EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*******************************************************************************
 * 名    称： EXTI1_IRQHandler
 * 功    能： 外部中断1处理函数，计量正、负无功功率
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2016-11-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void EXTI9_5_ISRHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /***********************************************
        * 描述： 
        */ 
        if ( ( Mater.Energy.PrimVolt        == 0 ) ||
             ( Mater.Energy.PrimCurr        == 0 ) ||
             ( Mater.Energy.PowerFreq       == 0 ) ||
             ( Mater.Energy.PowerFactor     == 0 ) ) {
        } else {
            /***********************************************
            * 描述： 正无功
            */ 
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)) {
                AC.PQpulse++;
                AC.PQpower_temp = AC.PQpulse;                                   //脉冲常数100000，25kV/150V---600A/1A(HXD1D车型)
            /***********************************************
            * 描述： 负无功
            */ 
            } else {
                AC.NQpulse++;
                AC.NQpower_temp = AC.NQpulse;                                   //脉冲常数100000，25kV/150V---600A/1A(HXD1D车型)
            }
        }
        /* Clear the EXTI Line 9 */  
        EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

/*******************************************************************************
 * 名    称： APP_MaterDispHandler
 * 功    能： 电表显示
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： wumingshen.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void APP_MaterDispHandler(INT08U *step)
{
	//static uint8_t step = 0;
    
    BSP_DispClrAll();
    switch ( *step ) {
    /***********************************************
    * 描述： 
    */
    case 0:     uprintf("E1      "); 	            break;                      // 
    case 1:     uprintf("%8d",Mater.Energy.PPPower);		break;              //显示正有功电量
    case 2:     uprintf("E2      ");		        break;
    case 3:     uprintf("%8d",Mater.Energy.NPPower);*step = 7;		break;	    //显示负有功电量
    case 4:     uprintf("E3      ");		        break;
    case 5:     uprintf("%8d",Mater.Energy.PQPower);break;	                    //显示正无功电量
    case 6:     uprintf("E4      ");		        break;
    case 7:     uprintf("%8d",Mater.Energy.NQPower);break;                     //显示负无功电量
    case 8:     uprintf("U %6d",(uint32_t)AC.U_RMS);break;                     //显示电压
    case 9:     uprintf("A %6d",(uint32_t)AC.I_RMS);break;                     //显示电流
    case 10:    
        if ( AC.Power_Factor == 1.0 )
            uprintf("P   .1000",AC.Power_Factor);	                            //显示功率
        else if ( AC.Power_Factor == -1.0 )
            uprintf("P  -.1000",AC.Power_Factor);	                            //显示功率
        else if(AC.Power_Factor < 0 )
            uprintf("P  -.0%03d",abs(AC.Power_Factor * 1000.0));	            //显示功率
        else
            uprintf("P   .0%03d",(int)(AC.Power_Factor * 1000.0));	            //显示功率
        break;                                                                  //显示功率
            
    default:
        *step   = 0;
        break;
    }
    *step  += 1;
    if ( *step > 10 )
        *step   = 0;    
}

/*******************************************************************************
 * 名    称： APP_MaterCommHandler
 * 功    能： 电表串口数据处理函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： wumingshen.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
/***********************************************
* 描述： 2015/12/07增加，用于非MODBBUS通信
*        非MODBUS通信，带帧头帧尾的通信数据处理
*/
#if MB_NONMODBUS_EN == DEF_ENABLED
CPU_BOOLEAN APP_MaterCommHandler (MODBUS_CH  *pch)
{    
    uint8_t     fram_clr[4]     = {0};
	uint8_t     IDBuf[4]        = {0};
	uint8_t     clear_buf[24]   = {0};
	uint32_t    ID              = 0;
	uint32_t    utemp;
    	
	TIME        system; 
    
    /***********************************************
    * 描述： 拷贝数据到数据区
    */
    //CPU_SR_ALLOC();
    //CPU_CRITICAL_ENTER();
    //uint32_t  Len     = pch->RxBufByteCnt;
    //memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    //CPU_CRITICAL_EXIT();
    uint32_t  Len       = pch->RxBufByteCnt;
    INT08U  *DataBuf    = pch->RxFrameData;
    
    
    CPU_INT16U  head    = BUILD_INT16U(pch->RxBuf[1], pch->RxBuf[0]);
    CPU_INT16U  tail    = BUILD_INT16U(pch->RxBuf[pch->RxBufByteCnt-1],
                                       pch->RxBuf[pch->RxBufByteCnt-2]);
    /***********************************************
    * 描述： 长沙南车DTU协议处理
    */
    if ( ( pch->RxFrameHead == head ) &&
         ( pch->RxFrameTail == tail ) ) {
        DataBuf = &pch->RxFrameData[8];
        //Len     = Ctrl.Com.Rd.Head.DataLen;        
        head    = BUILD_INT16U(DataBuf[1], DataBuf[0]);
    }
    
    /***********************************************
    * 描述： 
    序号    内容       字节数 内容说明
    1       帧头       2      0xAA 0xAA
    2       帧长度     1      0x08 帧长度固定为8字节
    3       命令类型   2      0x00 0x01
    4       预留       2      0x00 0x00
    5       累加和     1      前7个字节的累加和
    */ 
    if ( 0xAAAA == head ) {
        Len            -= 1;
        int8_t chkSum   = GetSumChk(DataBuf,Len);
        
        if ( chkSum ==  DataBuf[Len] ) {
            pch->TxFrameData[0]     = 0xAA;
            pch->TxFrameData[1]     = 0xAA;
            pch->TxFrameData[2]     = 0x30;
            pch->TxFrameData[3]     = 0x00;
            pch->TxFrameData[4]     = 0x71;
            
            CPU_SR  cpu_sr;
            OS_CRITICAL_ENTER();
            memcpy(&pch->TxFrameData[5],(uint8_t *)&Mater.Energy, sizeof(StrEnergy));
            OS_CRITICAL_EXIT();
            
            pch->TxFrameData[45]    = 0x00;
            pch->TxFrameData[46]    = 0x00;
            
            pch->TxFrameData[47]    = GetSumChk(pch->TxFrameData,47);
            
            NMB_Tx((MODBUS_CH   *)pch,
                   (CPU_INT08U  *)pch->TxFrameData,
                   (CPU_INT16U   )48);
            return TRUE;
        }
    /***********************************************
    * 描述： 调试参数
    */
    } else if ( ( 0xAA55 == head ) && ( Len > 10 )) {
        /***********************************************
        * 描述： 检测是否有帧尾，没有帧尾则进行CRC校验
        */
        if ( tail != 0x55AA  ) {
            uint16_t crc16_Cal = GetCrc16Chk1((uint8_t *)&DataBuf[0],9);
            uint16_t crc16_Rec = BUILD_INT16U(DataBuf[9],DataBuf[10]);
            if(crc16_Cal != crc16_Rec) {                
                return TRUE;
            }
        }
        switch(DataBuf[2])
        {
        /***********************************************
        * 描述： 设置序列号
        */
        case 0xA1:
            ID = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(64 , (uint8_t *)&ID , 4);
            ReadFM24CL64(64 , (uint8_t *)&ID , 4);
            uartprintf(pch,"\r\n本装置ID设置为：%d\r\n" , ID);
            break;            
        /***********************************************
        * 描述： 设置机车型号/机车号
        */
        case 0xA2:	
            Mater.LocoTyp   = BUILD_INT16U(DataBuf[3],DataBuf[4]);
            Mater.LocoNbr   = BUILD_INT16U(DataBuf[5],DataBuf[6]);
            WriteFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            WriteFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            ReadFM24CL64(68 ,  (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 ,  (uint8_t *)&Mater.LocoNbr, 2);
            uartprintf(pch,"\r\n当前车型：%d；当前车号：%d\r\n", Mater.LocoTyp,Mater.LocoNbr);
            break; 
        /***********************************************
        * 描述： 设置时间
        */
        case 0xA3:
            system.Year     =DataBuf[3];
            system.Month    =DataBuf[4];
            system.Day      =DataBuf[5];
            system.Hour     =DataBuf[6];
            system.Min      =DataBuf[7];
            system.Sec      =DataBuf[8];
            WriteTime(system);
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n当前时间为：20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);
            break;
        // 交流电表校准顺序
        /***********************************************
        * 描述： 1、电压直流零点校准
        */
//        case 0xA4:
//            CS5463_AC_Adjust(pch,VDCOFFC);												
//            break;
//        /***********************************************
//        * 描述： 2、电流直流零点校准
//        */
//        case 0xA5:
//            CS5463_AC_Adjust(pch,IDCOFFC);										
//            break;
//        /***********************************************
//        * 描述： 4、电压交流零点校准
//        */
//        case 0xA6:
//            CS5463_AC_Adjust(pch,VACOFFC);												
//            break;
//        /***********************************************
//        * 描述： 5、电流交流零点校准
//        */
//        case 0xA7:
//            CS5463_AC_Adjust(pch,IACOFFC);											
//            break;
//        /***********************************************
//        * 描述： 3、电压直流增益校准（交流表不校准）
//        */
//        case 0xA8:
//            CS5463_AC_Adjust(pch,VACGAINC);									
//            break;
//        /***********************************************
//        * 描述： 3、电流直流增益校准（直流表不校准）
//        */
//        case 0xA9:
//            CS5463_AC_Adjust(pch,IACGAINC);									
//            break;            
        /***********************************************
        * 描述： 清除所有数据
        */
        case 0xAB:
            WriteFM24CL64(32, clear_buf, 24);
            uartprintf(pch,"\r\n调试数据已清除\n");
            break;
        /***********************************************
        * 描述： 正有功清零
        */
        case 0xAC:
            AC.PPpulse = 0;
            AC.PPpower_base = 0;
            AC.PPpower_temp = 0;
            uartprintf(pch,"\r\n正有功电量已清零\n");
            break;
        /***********************************************
        * 描述： 负有功清零
        */
        case 0xAD:
            AC.NPpulse = 0;
            AC.NPpower_base = 0;
            AC.NPpower_temp = 0;
            uartprintf(pch,"\r\n负有功电量已清零\n");
            break;
        /***********************************************
        * 描述： 正无功清零
        */
        case 0xAE:
            AC.PQpulse = 0;
            AC.PQpower_base = 0;
            AC.PQpower_temp = 0;
            uartprintf(pch,"\r\n正无功电量已清零\n");
            break;
        /***********************************************
        * 描述： 负无功清零
        */
        case 0xAF:
            AC.NQpulse = 0;
            AC.NQpower_base = 0;
            AC.NQpower_temp = 0;
            uartprintf(pch,"\r\n负无功电量已清零\n");
            break;            
//        case 0x5A:
//            CS5463_AC_Adjust(pch,P_rate_ADD);
//            break;
//        case 0x5B:
//            CS5463_AC_Adjust(pch,P_rate_SUB);
//            break;
//        case 0x5C:
//            CS5463_AC_Adjust(pch,P_rate_DEL);
//            break;
//        case 0x5d:
//            CS5463_AC_Adjust(pch,P_rate_SET);
//            break;
        /***********************************************
        * 描述： 设置原边电压
        */
        case 0x5E:
            AC.U_SCL = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(88 , (uint8_t *)&AC.U_SCL , 4);
            ReadFM24CL64(88 , (uint8_t *)&AC.U_SCL , 4);
            uartprintf(pch,"\r\n原边额定电压设置为：%dV\r\n" , AC.U_SCL);
            break;
        /***********************************************
        * 描述： 设置原边电流
        */
        case 0x5F:
            AC.I_SCL = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(92 , (uint8_t *)&AC.I_SCL , 4);
            ReadFM24CL64(92 , (uint8_t *)&AC.I_SCL , 4);
            uartprintf(pch,"\r\n原边额定电流设置为：%dA\r\n" , AC.I_SCL);
            break;
        /***********************************************
        * 描述： 电压为零的阀值，当电压小于该值时电压设置为零
        */
        case 0x50:
            AC.U_O = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(96 , (uint8_t *)&AC.U_O , 4);
            ReadFM24CL64(96 , (uint8_t *)&AC.U_O , 4);
            uartprintf(pch,"\r\n零压阀值设置为：%fV\r\n" , AC.U_O);
            break;
        /***********************************************
        * 描述： 读取ID
        */
        case 0x51:				
            ReadFM24CL64(64 , (uint8_t *)&ID , 4);
            uartprintf(pch,"\r\n本装置ID为：%d\r\n" , ID);
            break;
        /***********************************************
        * 描述： 
        */
        case 0x52:
            ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            if( ( Mater.LocoTyp > 0 ) &&
                ( Mater.LocoTyp < 10000 ) &&
                ( Mater.LocoNbr > 0 ) && 
                ( Mater.LocoNbr < 10000 ) ) {
                uartprintf(pch,"\r\n当前车型：%d； 当前车号：%04d\r\n" , Mater.LocoTyp,Mater.LocoNbr);
            } else {
                uartprintf(pch,"\r\n未设置装车车型！\r\n");
            }
            break;            
        /***********************************************
        * 描述： 读取时间
        */
        case 0x53:
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n当前时间为：20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);				
            break;            
        /***********************************************
        * 描述： 通过串口转储FLASH中的所有有效数据
        */
        case 0x54:
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n当前时间为：20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);
            
            ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            if( ( Mater.LocoTyp > 0 ) &&
                ( Mater.LocoTyp < 10000 ) &&
                ( Mater.LocoNbr > 0 ) && 
                ( Mater.LocoNbr < 10000 ) ) {
                uartprintf(pch,"\r\n当前车型：%d； 当前车号：%04d\r\n" , Mater.LocoTyp,Mater.LocoNbr);
            } else {
                uartprintf(pch,"\r\n未设置装车车型！\r\n");
            }
            
            //DOWNLOAD(g_Flash_Adr/sizeof(Mater));
            if(g_Flash_Adr == 0)
                uartprintf(pch,"\r\nFLASH芯片中无有效数据！\r\n");
            break;
            
        case 0x55:
            uartprintf(pch,"\r\nFLASH芯片擦除中............\r\n");
            uartprintf(pch,"\r\n大约需要30秒，请耐心等待！\r\n");
            MX25L3206_Erase(0, 4096);
            uartprintf(pch,"\r\nFLASH芯片已擦除\r\n");
            break;
            
        case 0x56:
            uartprintf(pch,"\r\nFRAM芯片擦除中...\r\n");
            WriteFM24CL64(60, fram_clr, 4);
            g_Flash_Adr = 0;
            Mater.Record.Head  = 0;
            Mater.Record.Tail  = 0;
            Mater.RecordNbr  = 0;
            WriteFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
            WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
            WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
            uartprintf(pch,"\r\nFRAM芯片已擦除\r\n");
            break;
        case 0x57:{
            float x =(float)((pch->RxFrameData[4] >> 4) & 0x0f) * 1
                    + (float)((pch->RxFrameData[4] >> 0) & 0x0f) * 0.1
                    + (float)((pch->RxFrameData[5] >> 4) & 0x0f) * 0.01
                    + (float)((pch->RxFrameData[5] >> 0) & 0x0f) * 0.001
                    + (float)((pch->RxFrameData[6] >> 4) & 0x0f) * 0.0001
                    + (float)((pch->RxFrameData[6] >> 0) & 0x0f) * 0.00001
                    + (float)((pch->RxFrameData[7] >> 4) & 0x0f) * 0.000001
                    + (float)((pch->RxFrameData[7] >> 0) & 0x0f) * 0.0000001
                    + (float)((pch->RxFrameData[8] >> 4) & 0x0f) * 0.00000001
                    + (float)((pch->RxFrameData[8] >> 0) & 0x0f) * 0.000000001;
            
            AC.U_K = x; 
            WriteFM24CL64(160, (uint8_t *)&AC.U_K, 4);
            uartprintf(pch,"\r\n电压补偿斜率 = %f\n", AC.U_K);
            } break;
        case 0x58:{
            float x =(float)((pch->RxFrameData[4] >> 4) & 0x0f) * 1
                    + (float)((pch->RxFrameData[4] >> 0) & 0x0f) * 0.1
                    + (float)((pch->RxFrameData[5] >> 4) & 0x0f) * 0.01
                    + (float)((pch->RxFrameData[5] >> 0) & 0x0f) * 0.001
                    + (float)((pch->RxFrameData[6] >> 4) & 0x0f) * 0.0001
                    + (float)((pch->RxFrameData[6] >> 0) & 0x0f) * 0.00001
                    + (float)((pch->RxFrameData[7] >> 4) & 0x0f) * 0.000001
                    + (float)((pch->RxFrameData[7] >> 0) & 0x0f) * 0.0000001
                    + (float)((pch->RxFrameData[8] >> 4) & 0x0f) * 0.00000001
                    + (float)((pch->RxFrameData[8] >> 0) & 0x0f) * 0.000000001;
            
            AC.I_K = x; 
            WriteFM24CL64(164, (uint8_t *)&AC.I_K, 4);
            uartprintf(pch,"\r\n电流补偿斜率 = %f\n", AC.I_K);
            } break;
        case 0x00: 
            utemp   = BUILD_INT32U(DataBuf[3],DataBuf[4],DataBuf[5],DataBuf[6]); 
            if ( utemp < 10 )
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
            else if ( utemp > 300 )
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
            else
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * utemp;
            
            WriteFM24CL64(84, (uint8_t *)&Mater.Record.RecordTime, 4);
            ReadFM24CL64(84 , (uint8_t *)&utemp, 4);
            osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN ,Mater.Record.RecordTime);
            uartprintf(pch,"\r\n设置数据记录/发送间隔时间为：%d秒\r\n",utemp / OS_TICKS_PER_SEC);
            break;
        case 0x01:    
            ReadFM24CL64(84 , (uint8_t *)&utemp, 4);
            uartprintf(pch,"\r\n当前数据记录/发送间隔时间为：%d秒\r\n",utemp / OS_TICKS_PER_SEC);
            break;
        case 0xf0:
            SystemReset();							    //系统重启
            break;
        default:
            break;
        }
    }
    /***********************************************
    * 描述： 调用串口数据处理回调函数
    */ 
    return TRUE;
}

/*******************************************************************************
 * 名    称： APP_CSNC_CommHandler
 * 功    能： 串口数据处理函数
 * 入口参数： 无
 * 出口参数： 无
 * 作　　者： wumingshen.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
CPU_BOOLEAN APP_CSNC_CommHandler (MODBUS_CH  *pch)
{   
    if ( pch->PortNbr == Ctrl.Dtu.pch->PortNbr ) {
//        if ( GetRecvFrameNbr() == Ctrl.Com.Rd.Head.PacketSn ) {
//            /***********************************************
//            * 描述： 清除定时上传标志
//            */
//            SetSendFrameNbr();
//            extern void UpdateRecordPoint(uint8_t storeflag);
//            UpdateRecordPoint(1);
//            pch->StatNoRespCtr  = 0;
//            
//            osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
//        }
    } else {
        /***********************************************
        * 描述： 将数据打包给发送结构
        */ 
        APP_MaterCommHandler(pch);
    }
    
    /***********************************************
    * 描述： 串口接受COMM模组的消息并处理
    */
    Ctrl.Com.ConnectTimeOut    = 0;                // 超时计数器清零
    Ctrl.Com.ConnectFlag       = TRUE;             // 转连接成功标志
    
    return TRUE;
}
#endif

void SavePowerData(void)
{
    /***********************************************
    * 描述： 保存正有功电量数据到FRAM
    *       正有功电量    1kvarh      99999999 kvarh
    */
    if ( recordsfr.PPpower_NUM != AC.PPpower_NUM ) {	
        WriteFM24CL64(0, (uint8_t *)&AC.PPpower_NUM,4);
        WriteFM24CL64(4, (uint8_t *)&AC.PPpower_NUM,4);
        WriteFM24CL64(8, (uint8_t *)&AC.PPpower_NUM,4);
    }
    /***********************************************
    * 描述： 保存负有功电量数据到FRAM
    *        负有功电量    1kvarh      99999999 kvarh 
    */ 
    if ( recordsfr.NPpower_NUM != AC.NPpower_NUM ) { 
        WriteFM24CL64(12, (uint8_t *)&AC.NPpower_NUM,4);
        WriteFM24CL64(16, (uint8_t *)&AC.NPpower_NUM,4);
        WriteFM24CL64(20, (uint8_t *)&AC.NPpower_NUM,4);
    }
    /***********************************************
    * 描述： 保存负有功电量数据到FRAM
    *        正无功电量    1kvarh      99999999 kvarh 
    */
    if ( recordsfr.PQpower_NUM != AC.PQpower_NUM ) {
        WriteFM24CL64(100, (uint8_t *)&AC.PQpower_NUM,4);
        WriteFM24CL64(104, (uint8_t *)&AC.PQpower_NUM,4);
        WriteFM24CL64(108, (uint8_t *)&AC.PQpower_NUM,4);
    }
    /***********************************************
    * 描述： 保存负有功电量数据到FRAM
    *       负无功电量    1kvarh      99999999 kvarh 
    */ 
    if ( recordsfr.NQpower_NUM != AC.NQpower_NUM ) {
        WriteFM24CL64(112, (uint8_t *)&AC.NQpower_NUM,4);			  
        WriteFM24CL64(116, (uint8_t *)&AC.NQpower_NUM,4);
        WriteFM24CL64(120, (uint8_t *)&AC.NQpower_NUM,4);
    }    
    
    recordsfr.PPpower_NUM       = AC.PPpower_NUM;
    recordsfr.NPpower_NUM       = AC.NPpower_NUM;
    recordsfr.PQpower_NUM       = AC.PQpower_NUM;
    recordsfr.NQpower_NUM       = AC.NQpower_NUM;
}

void PVD_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line16);            //清中断
    
    //num = BKP_ReadBackupRegister(BKP_DR10);
    //num++;   
    
    //用户添加紧急处理代码处    
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//使能PWR和BKP外设时钟
    //PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问
    
    //BKP_WriteBackupRegister(BKP_DR10, (u8)num);//启动界面
    
    WriteFM24CL64(0, (uint8_t *)&AC.PPpower_NUM,4);
    WriteFM24CL64(4, (uint8_t *)&AC.PPpower_NUM,4);
    WriteFM24CL64(8, (uint8_t *)&AC.PPpower_NUM,4);
    
    //将负有功电量数据转存到缓冲区
    WriteFM24CL64(12, (uint8_t *)&AC.NPpower_NUM,4);			  //低字节在前，高字节在后
    WriteFM24CL64(16, (uint8_t *)&AC.NPpower_NUM,4);
    WriteFM24CL64(20, (uint8_t *)&AC.NPpower_NUM,4);                
    
    WriteFM24CL64(100, (uint8_t *)&AC.PQpower_NUM,4);
    WriteFM24CL64(104, (uint8_t *)&AC.PQpower_NUM,4);
    WriteFM24CL64(108, (uint8_t *)&AC.PQpower_NUM,4);
    
    //将负有功电量数据转存到缓冲区
    WriteFM24CL64(112, (uint8_t *)&AC.NQpower_NUM,4);			  //低字节在前，高字节在后
    WriteFM24CL64(116, (uint8_t *)&AC.NQpower_NUM,4);
    WriteFM24CL64(120, (uint8_t *)&AC.NQpower_NUM,4);
}

void PWR_PVD_Init(void)
{  
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);     //使能PWR时钟    
    
    //NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;           //使能PVD所在的外部中断通道
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级1
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //子优先级0
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能外部中断通道
    //NVIC_Init(&NVIC_InitStructure);
            
    /***********************************************
    * 描述： 外部中断0 ~ 4向量
    */ 
    BSP_IntVectSet(BSP_INT_ID_PVD, PVD_IRQHandler);
    BSP_IntEn(BSP_INT_ID_PVD);    
    
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;             //PVD连接到中断线16上
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //使用中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //电压低于阀值时产生中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //使能中断线
    EXTI_Init(&EXTI_InitStructure);                         //初始   
    
    PWR_PVDLevelConfig(PWR_PVDLevel_2V9);                   //设定监控阀值
    PWR_PVDCmd(ENABLE);                                     //使能PVD    
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif