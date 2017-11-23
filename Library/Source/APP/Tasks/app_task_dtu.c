/*******************************************************************************
 *   Filename:       app_task_dtu.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    双击选中 dtu 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Dtu 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 DTU 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_DTU_PRIO     ）
 *                                            和 任务堆栈（ APP_TASK_DTU_STK_SIZE ）大小
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
#include <app_comm_protocol.h>
#include <bsp_flash.h>
#include <iap.h>
#include <xprintf.h>
#include <power_macro.h>
#include <mx25.h>
#include <FM24CL64.h>
#include <crccheck.h>
#include <ds3231.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_dtu__c = "$Id: $";
#endif

#define APP_TASK_DTU_EN     DEF_ENABLED
#if APP_TASK_DTU_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
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
static  OS_TCB      AppTaskDtuTCB;
OS_Q                DTU_RxQ;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK  AppTaskDtuStk[ APP_TASK_DTU_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void    AppTaskDtu                 (void *p_arg);
static void    APP_DtuInit                (void);
       void     App_McuStatusInit          (void);
       void     ReportDevStatusHandle      (void);
       void     InformDtuConfigMode        (u8 mode);
       
       void     App_SendDataFromHistory     (void);

INT08U          APP_DtuRxDataDealCB        (MODBUS_CH  *pch);
INT08U          IAP_DtuRxDataDealCB        (MODBUS_CH  *pch);
/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern void     uartprintf              (MODBUS_CH  *pch,const char *fmt, ...);

/*******************************************************************************/

/*******************************************************************************
 * 名    称： App_TaskDtuCreate
 * 功    能： **任务创建
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
void  App_TaskDtuCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * 描述： 任务创建
    */
    OSTaskCreate((OS_TCB     *)&AppTaskDtuTCB,                      // 任务控制块  （当前文件中定义）
                 (CPU_CHAR   *)"App Task Dtu",                      // 任务名称
                 (OS_TASK_PTR ) AppTaskDtu,                         // 任务函数指针（当前文件中定义）
                 (void       *) 0,                                  // 任务函数参数
                 (OS_PRIO     ) APP_TASK_DTU_PRIO,                 // 任务优先级，不同任务优先级可以相同，0 < 优先级 < OS_CFG_PRIO_MAX - 2（app_cfg.h中定义）
                 (CPU_STK    *)&AppTaskDtuStk[0],                   // 任务栈顶
                 (CPU_STK_SIZE) APP_TASK_DTU_STK_SIZE / 10,         // 任务栈溢出报警值
                 (CPU_STK_SIZE) APP_TASK_DTU_STK_SIZE,              // 任务栈大小（CPU数据宽度 * 8 * size = 4 * 8 * size(字节)）（app_cfg.h中定义）
                 (OS_MSG_QTY  ) 0u,                                 // 可以发送给任务的最大消息队列数量
                 (OS_TICK     ) 0u,                                 // 相同优先级任务的轮循时间（ms），0为默认
                 (void       *) 0,                                  // 是一个指向它被用作一个TCB扩展用户提供的存储器位置
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // 允许堆栈检查该任务
                                OS_OPT_TASK_STK_CLR),               // 创建任务时堆栈清零
                 (OS_ERR     *)&err);                               // 指向错误代码的指针，用于创建结果处理

}

/*******************************************************************************
 * 名    称： AppTaskDtu
 * 功    能： 控制任务
 * 入口参数： p_arg - 由任务创建函数传入
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2015-02-05
 * 修    改：
 * 修改日期：
 *******************************************************************************/
static  void  AppTaskDtu (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly     = CYCLE_TIME_TICKS;
    OS_TICK     ticks;
        
    APP_DtuInit();
    
    BSP_OS_TimeDly(OS_TICKS_PER_SEC / 2);
    /***********************************************
    * 描述： Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 
        /***********************************************
        * 描述： 本任务看门狗标志置位
        */
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_DTU,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);
        
        /***********************************************
        * 描述： 等待DTU数据接收消息队列
        *
        OS_MSG_SIZE p_msg_size;
        
        MODBUS_CH *pch = 
       (MODBUS_CH *)OSQPend ((OS_Q*)&DTU_RxQ,
                    (OS_TICK       )dly,
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,//OS_OPT_PEND_NON_BLOCKING,
                    (OS_MSG_SIZE  *)&p_msg_size,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
        // DTU收到消息
        if ( OS_ERR_NONE == err ) {
            // 消息处理
            APP_DtuRxDataDealCB(pch);
        }
        *//***********************************************
        * 描述： 等待DTU操作标志位
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.DtuEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.DtuEvtFlag,
                    ( OS_TICK      ) dly,
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks   = OSTimeGet(&terr);                        // 获取当前OSTick
        
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
            
            /***********************************************
            * 描述： DTU复位
            */
            } 
            if ( flags & COMM_EVT_FLAG_RESET ) {
                flagClr |= COMM_EVT_FLAG_RESET;
                
            /***********************************************
            * 描述： DTU连接
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONNECT ) {
                flagClr |= COMM_EVT_FLAG_CONNECT;
                
            /***********************************************
            * 描述： DTU串口接收数据
            */
            } 
            if ( flags & COMM_EVT_FLAG_RECV ) {
                flagClr |= COMM_EVT_FLAG_RECV;
                APP_DtuRxDataDealCB(Ctrl.Dtu.pch);
            /***********************************************
            * 描述： 上报消息
            */
            } 
            if ( flags & COMM_EVT_FLAG_REPORT ) {
                /***********************************************
                * 描述： 发送历史数据到服务器
                */
                App_SendDataFromHistory();
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
            } else if ( flags & COMM_EVT_FLAG_CONFIG ) {
                flagClr |= COMM_EVT_FLAG_CONFIG;
            
            /***********************************************
            * 描述： IAP结束
            */
            } 
            if ( flags & COMM_EVT_FLAG_IAP_END ) {
                flagClr |= COMM_EVT_FLAG_IAP_END;
                
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
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.DtuEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
            
        /***********************************************
        * 描述： 如果超时，则发送一次心跳包
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            
        }
//next:
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
 * 名    称： APP_DtuInit
 * 功    能： 任务初始化
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： wumingshen.
 * 创建日期： 2015-03-28
 * 修    改：
 * 修改日期：
 *******************************************************************************/
 void APP_DtuInit(void)
{
    OS_ERR err;
    
    /***********************************************
    * 描述： 创建事件标志组
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.DtuEvtFlagGrp,
                 ( CPU_CHAR     *)"App_DtuFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.DtuEvtFlag = COMM_EVT_FLAG_HEART       // 心跳包发送
                        + COMM_EVT_FLAG_RESET       // DTU复位
                        + COMM_EVT_FLAG_CONNECT     // DTU连接
                        + COMM_EVT_FLAG_RECV        // 串口接收
                        + COMM_EVT_FLAG_REPORT      // 串口发送
                        + COMM_EVT_FLAG_CLOSE       // 断开
                        + COMM_EVT_FLAG_TIMEOUT     // 超时
                        + COMM_EVT_FLAG_CONFIG      // 配置
                        + COMM_EVT_FLAG_IAP_START   // IAP开始
                        + COMM_EVT_FLAG_IAP_END;    // IAP结束
            
    OSQCreate ( (OS_Q        *)&DTU_RxQ,
                (CPU_CHAR    *)"RxQ",
                (OS_MSG_QTY   ) OS_CFG_INT_Q_SIZE,
                (OS_ERR      *)&err);
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    WdtFlags |= WDT_FLAG_DTU;
}

/*******************************************************************************
 * 名    称： APP_DtuRxDataDealCB
 * 功    能： 串口数据处理回调函数，由MB_DATA.C调用
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2016-01-04
 * 修    改：
 * 修改日期：
 * 备    注： 
 *******************************************************************************/
INT08U APP_DtuRxDataDealCB(MODBUS_CH  *pch)
{
   
    return TRUE;
}

/*******************************************************************************
* 名    称： UpdateRecordPoint
* 功    能： 
* 入口参数： 无
* 出口参数： 无
* 作  　者： wumingshen.
* 创建日期： 2017-02-07
* 修    改：
* 修改日期：
* 备    注：
*******************************************************************************/
void UpdateRecordPoint(uint8_t storeflag)
{
    uint32_t    size    = sizeof(StrMater) - sizeof(StrMaterRecord);             // 获取记录长度
    Mater.Record.Tail  += size;
    
    if ( Mater.Record.Tail >= MAX_ADDR ) {
        Mater.Record.Tail = 0; 
    } else if ( Mater.Record.Tail >= Mater.Record.Head ) {
        Mater.Record.Tail  = Mater.Record.Head; 
    }
    if ( storeflag ) {
        //WriteFM24CL64(72, (uint8_t *)&Mater.Head, 4);    
        WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4); 
        //WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    }
}

/*******************************************************************************
 * 名    称： App_SaveDataToHistory
 * 功    能：
 * 入口参数：
 * 出口参数： 无
 * 作 　 者： 无名沈
 * 创建日期： 2017-02-7
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void  App_SaveDataToHistory( void )
{
    /**************************************************
    * 描述： 保存当前数据
    */
    uint32_t    size = sizeof(StrMater) - sizeof(StrMaterRecord);             // 数据长度
    CPU_SR      cpu_sr;
    
    uint8_t time[6];
    GetTime((TIME *)&time[0]);
     
    OS_CRITICAL_ENTER(); 
    
    if ( Ctrl.Tax.ConnectFlag == FALSE ) { 
        /***************************************************
        * 描述： 读取时间
        */
        Mater.Tax.Tax2.LKJTime.Year  = time[0];
        Mater.Tax.Tax2.LKJTime.Mon   = time[1];
        Mater.Tax.Tax2.LKJTime.Day   = time[2];
        Mater.Tax.Tax2.LKJTime.Hour  = time[3];
        Mater.Tax.Tax2.LKJTime.Min   = time[4];
        Mater.Tax.Tax2.LKJTime.Sec   = time[5];
            
        Mater.TaxType   = 0;
        Mater.TeZhengMa = 0;
        
        memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
    } else {
        /**************************************************
        * 描述： 复制TAX箱数据；
        */
        switch ( Ctrl.Tax.Dat.buf1[0] ) {
        /***********************************************
        * 描述： TAX2015协议：=0XF0：TAX2设备；
        */
        case 0xFA:
        /***********************************************
        * 描述： TAX2015协议：=0XFA：TAX3设备
        */
        case 0xF0:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax3.Head.SheBeiLeiXing;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax3.Head.TeZhengMa.Dat;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        /***********************************************
        * 描述： 老TAX2协议
        */
        case 0x38:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax2.Record.Addr;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax2.CheZhanHaoKuoChong;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        /***********************************************
        * 描述： 老TAX2协议
        */
        case 0x39:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax2.Record.Addr;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax2.CheZhanHaoKuoChong;
            memcpy((uint8_t *)&Mater.Tax.buf1[24],(uint8_t *)&Ctrl.Tax.Dat.buf1[32],40);
            break;
        default:
            Mater.TaxType   = 0;
            Mater.TeZhengMa = 0;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        }
    }
    
    Mater.ErrTax    = Ctrl.Tax.ConnectFlag;
	Mater.chk       = GetCrc16Chk1((uint8_t *)&Mater,size-2);       // 获取存储校验码
    
    OS_CRITICAL_EXIT();
    
    SaveOneREcord(Mater.Record.Head,(uint8_t *)&Mater,size);        // 保存数据
	Mater.Record.Head += size;                                      // 更改头指针
    
    if ( Mater.Record.Head >= MAX_ADDR ) {                          // 头指针溢出
        Mater.Record.Head = 0;                                      // 溢出指针归零
        if ( Mater.Record.Tail == 0 )                               // 检查尾指针是否为零
            Mater.Record.Tail  += size;                             // 为零更改尾指针
    } else {                                                        // 头指针没有溢出
        if ( Mater.Record.Tail >= Mater.Record.Head ) {             // 尾指针大于等于头指针
            Mater.Record.Tail  += size;                             // 更改尾指针
            if ( Mater.Record.Tail >= MAX_ADDR )                    // 尾指针溢出
                Mater.Record.Tail = 0;                              // 尾指针归零
        }
    }
    
    Mater.RecordNbr++;                                              // 记录号+1
    
    /**************************************************
    * 描述： 保存当前记录号和数据指针
    */
	WriteFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
	WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
	WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
    osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
}

/*******************************************************************************
 * 名    称： App_SendDataFromHistory
 * 功    能： 数据转储,数据低字节在前，高字节在后 
 * 入口参数：
 * 出口参数： 无
 * 作 　 者： 无名沈
 * 创建日期： 2017-02-7
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void App_SendDataFromHistory(void)
{	    
    /***********************************************
    * 描述： 如果程序处在升级模式
    */
    if ( ( Iap.Status != IAP_STS_DEF ) && 
         ( Iap.Status != IAP_STS_SUCCEED ) &&
         ( Iap.Status != IAP_STS_FAILED ) ) {
        return;
    }
    
    
    Ctrl.Dtu.pack->SrcAddr  = 0x81;
    Ctrl.Dtu.pack->DstAddr  = 0xCB;
    Ctrl.Dtu.pack->Pack.Udat.VerHigh  = PROTOCOL_VER_HIG;
    Ctrl.Dtu.pack->Pack.Udat.VerLow   = PROTOCOL_VER_LOW;
    Ctrl.Dtu.pack->Pack.Udat.ChkType  = RS485_CHK_CRC;
    Ctrl.Dtu.pack->Data     = (CPU_INT08U     *)&History;
    Ctrl.Dtu.pack->Len      = (CPU_INT08U      ) 128;
    
    CSNC_SendData( (MODBUS_CH      *) Ctrl.Dtu.pch,                     // DistAddr,
                   (StrNcProtocol  *) Ctrl.Dtu.pack);                    // DataLen 
     

    
    /***********************************************
    * 描述： 头指针在前面（缓冲区未满）
    */
    if ( Mater.Record.Head == Mater.Record.Tail ) {
        osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN ,  OS_TICKS_PER_SEC * 10);
    /***********************************************
    * 描述： 头指针在后面（缓冲区已满）
    */
    } else {
        uint32_t    size    = sizeof(StrMater) - sizeof(StrMaterRecord);        // 获取记录长度
		MX25L3206_RD(Mater.Record.Tail, size,(uint8_t*)&History);  // 读取一条记录
        
        /**************************************************
        * 描述： 数据校验
        */
		uint16_t    CRC_sum1 = GetCrc16Chk1((uint8_t*)&History,size-2);		
		uint16_t    CRC_sum2 = History.chk;
        uint32_t    timeout  = OS_TICKS_PER_SEC * 5;
        /**************************************************
        * 描述： 发送记录至服务器
        */
		if(CRC_sum1 == CRC_sum2) {						    //进行累加和计算
            if (Ctrl.Dtu.pch->RxBufByteCtr == 0) {
                Ctrl.Dtu.pack->SrcAddr  = 0x81;
                Ctrl.Dtu.pack->DstAddr  = 0xCB;
                Ctrl.Dtu.pack->Pack.Udat.VerHigh  = PROTOCOL_VER_HIG;
                Ctrl.Dtu.pack->Pack.Udat.VerLow   = PROTOCOL_VER_LOW;
                Ctrl.Dtu.pack->Pack.Udat.ChkType  = RS485_CHK_CRC;
                Ctrl.Dtu.pack->Data     = (CPU_INT08U     *)&History;
                Ctrl.Dtu.pack->Len      = (CPU_INT08U      ) size;
                
                CSNC_SendData( (MODBUS_CH      *) Ctrl.Dtu.pch,                     // DistAddr,
                               (StrNcProtocol  *) Ctrl.Dtu.pack);                    // DataLen 
                 
                Ctrl.Dtu.pch->StatNoRespCtr++;
                if ( Ctrl.Dtu.pch->StatNoRespCtr < 10 ) {
                    timeout     = OS_TICKS_PER_SEC * 10; 
                } else if ( Ctrl.Dtu.pch->StatNoRespCtr < 50 ) {
                    timeout     = OS_TICKS_PER_SEC * 20; 
                } else if ( Ctrl.Dtu.pch->StatNoRespCtr < 100 ) {
                    timeout     = OS_TICKS_PER_SEC * 30; 
                } else {                
                    timeout     = OS_TICKS_PER_SEC * 60; 
                }
                
                //uartprintf(Ctrl.Com.pch,"\r\n发送记录数据：%d",Mater.RecordNbr);
            }
            osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN, timeout );
        } else {
            UpdateRecordPoint(0);
            
            osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
            //timeout = OS_TICKS_PER_SEC * 1;
        }
        
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif