/*******************************************************************************
*   Filename:       app_task_comm.c
*   Revised:        All copyrights reserved to wumingshen.
*   Revision:       v1.0
*   Writer:	     wumingshen.
*
*   Description:    双击选中 comm 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   双击选中 Comm 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   双击选中 COMM 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_COMM_PRIO     ）
*                                            和 任务堆栈（ APP_TASK_COMM_STK_SIZE ）大小
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
#include <app_ctrl.h>
#include <os_cfg_app.h>
#include <bsp_timer.h>
#include "csnc_package_deal.h"
#include "app_chk.h"

//#include <bsp_flash.h>
//#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_comm__c = "$Id: $";
#endif

#define APP_TASK_COMM_EN     DEF_ENABLED
#if APP_TASK_COMM_EN == DEF_ENABLED
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
static  OS_TCB   AppTaskCommTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
static  CPU_STK  AppTaskCommStk[ APP_TASK_COMM_STK_SIZE ];

//StrCtrlDCB       Ctrl;

OS_Q             MTR_RxQ;
/*******************************************************************************
* LOCAL VARIABLES
*/

/*******************************************************************************
* GLOBAL VARIABLES
*/

/***********************************************
* 描述： 软定时器声明
*/
OS_TMR          OSTmr1;                         //定时器1

/***********************************************
* 描述： 软定时器回调函数
*/
void            OSTmr1_callback             (OS_TMR *ptmr,void *p_arg);
CPU_BOOLEAN     APP_CSNC_CommHandler        (MODBUS_CH  *pch);
CPU_BOOLEAN     APP_MaterCommHandler        (MODBUS_CH  *pch);

/*******************************************************************************
* LOCAL FUNCTIONS
*/
static void    AppTaskComm                 (void *p_arg);
static void    APP_CommInit                (void);
INT08U          APP_CommRxDataDealCB        (MODBUS_CH  *pch);
INT08U          IAP_CommRxDataDealCB        (MODBUS_CH  *pch);
/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/

/*******************************************************************************
* EXTERN FUNCTIONS
*/
extern  void    app_comm_mtr(void);
extern  void    app_comm_dtu(void);
extern  void    app_comm_otr(void);


/*******************************************************************************
* 入口参数： 无
* 出口参数： 无
* 作　 　者： redmoringcn.
* 创建日期： 2017-11-17
* 修    改：
* 修改日期：
* 备    注： 任务创建函数需要在app.h文件中声明
*******************************************************************************/
void  App_TaskCommCreate(void)
{
    OS_ERR  err;
    
    /***********************************************
    * 描述： 任务创建
    */
    OSTaskCreate((OS_TCB     *)&AppTaskCommTCB,                     // 任务控制块  （当前文件中定义）
                 (CPU_CHAR   *)"App Task Comm",                     // 任务名称
                 (OS_TASK_PTR ) AppTaskComm,                        // 任务函数指针（当前文件中定义）
                 (void       *) 0,                                  // 任务函数参数
                 (OS_PRIO     ) APP_TASK_COMM_PRIO,                 // 任务优先级，不同任务优先级可以相同，0 < 优先级 < OS_CFG_PRIO_MAX - 2（app_cfg.h中定义）
                 (CPU_STK    *)&AppTaskCommStk[0],                  // 任务栈顶
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE / 10,        // 任务栈溢出报警值
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE,             // 任务栈大小（CPU数据宽度 * 8 * size = 4 * 8 * size(字节)）（app_cfg.h中定义）
                 (OS_MSG_QTY  ) 0u,                                 // 可以发送给任务的最大消息队列数量
                 (OS_TICK     ) 0u,                                 // 相同优先级任务的轮循时间（ms），0为默认
                 (void       *) 0,                                  // 是一个指向它被用作一个TCB扩展用户提供的存储器位置
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // 允许堆栈检查该任务
                                OS_OPT_TASK_STK_CLR),               // 创建任务时堆栈清零
                 (OS_ERR     *)&err);                               // 指向错误代码的指针，用于创建结果处理
    
}

/*******************************************************************************
* 名    称： AppTaskComm
* 功    能： 控制任务
* 入口参数： p_arg - 由任务创建函数传入
* 出口参数： 无
* 作　 　者： wumingshen.
* 创建日期： 2015-02-05
* 修    改：
* 修改日期：
*******************************************************************************/
static  void  AppTaskComm (void *p_arg)
{
    OS_ERR      err;
    
    OS_TICK     dly     = CYCLE_TIME_TICKS;
    //OS_TICK     ticks;
    /***********************************************
    * 描述： 任务初始化
    */
    APP_CommInit();         
    
    /***********************************************
    * 描述： Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 
        /***********************************************
        * 描述： 本任务看门狗标志置位
        */
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_COMM,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);
        
        /***********************************************
        * 描述： 等待COMM的标识组
        */
        OS_FLAGS    flags = 
            OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS     ) Ctrl.Os.CommEvtFlag,
                       ( OS_TICK      ) dly,
                       ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,
                       ( CPU_TS      *) NULL,
                       ( OS_ERR      *)&err);
        
        //OS_ERR      terr;
        //ticks   = OSTimeGet(&terr);                        // 获取当前OSTick
        
        /***********************************************
        * 描述： 没有错误,有事件发生
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
            /***********************************************
            * 和测量某块通讯有关事件发生，调用MTR通讯处理函数
            */           
            if(     flags & COMM_EVT_FLAG_MTR_RX 
               ||   flags & COMM_EVT_FLAG_MTR_TIMEOUT ) {
                   
                   //调用mtr通讯处理函数,和测量模块进行通讯
                   app_comm_mtr();                             
                   
                   if(flags & COMM_EVT_FLAG_MTR_RX) {      
                       flagClr |= COMM_EVT_FLAG_MTR_RX;         //接收到数据，清接收数据标示
                   }else{
                       flagClr |= COMM_EVT_FLAG_MTR_TIMEOUT;   //超时，清接收数据标示
                   }
               }
            
            /***********************************************
            * 和无线发送模块事件发生，调用DTU通讯处理函数
            */           
            else  if(   flags & COMM_EVT_FLAG_DTU_RX 
                     ||  flags & COMM_EVT_FLAG_DTU_TIMEOUT ) {
                         
                         //调用DTU通讯处理函数
                         app_comm_dtu();                            
                         
                         if( flags &    COMM_EVT_FLAG_DTU_RX) {      
                             flagClr |= COMM_EVT_FLAG_DTU_RX;        //接收到数据，清接收数据标示
                         }else{
                             flagClr |= COMM_EVT_FLAG_DTU_TIMEOUT;   //接收到数据，清接收数据标示
                         }
                     }
            
            /***********************************************
            * 和IC模块事件发生，调用OTR通讯处理函数
            */           
            else if(    flags & COMM_EVT_FLAG_OTR_RX 
                    ||  flags & COMM_EVT_FLAG_OTR_TIMEOUT ) {
                        
                        //调用DTU通讯处理函数
                        //app_comm_otr();                             
                        
                        if(flags &      COMM_EVT_FLAG_OTR_RX) {      
                            flagClr |=  COMM_EVT_FLAG_OTR_RX;        //接收到数据，清接收数据标示
                        }else{
                            flagClr |=  COMM_EVT_FLAG_OTR_TIMEOUT;   //接收到数据，清接收数据标示
                        }
                    }
            else if(    flags & COMM_EVT_FLAG_TAX_RX 
                    ||  flags & COMM_EVT_FLAG_TAX_TIMEOUT ) {
            
                        //调用DTU通讯处理函数
                        //app_comm_tax();                             
                        
                        if(flags &      COMM_EVT_FLAG_TAX_RX) {      
                            flagClr |=  COMM_EVT_FLAG_TAX_RX;        //接收到数据，清接收数据标示
                        }else{
                            flagClr |=  COMM_EVT_FLAG_TAX_TIMEOUT;   //接收到数据，清接收数据标示
                        }
                    }
            /***********************************************
            * 描述： 清除标志
            */
            if ( !flagClr ) {
                flagClr = flags;
            }
            
            /***********************************************
            * 描述： 清除标志位
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                       ( OS_FLAGS      )flagClr,
                       ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                       ( OS_ERR       *)&err);
        }
    }
}

/*******************************************************************************
* 名    称： APP_ModbusInit
* 功    能： MODBUS初始化
* 入口参数： 无
* 出口参数： 无
* 作　 　者： 无名沈
* 创建日期： 2015-03-28
* 修    改：
* 修改日期：
* 备    注： 该初始化会创建Modbus任务。
*******************************************************************************/
void App_ModbusInit(void)
{
    MODBUS_CH   *pch;
    /***********************************************
    * 描述： uCModBus初始化，RTU时钟频率为1000HZ
    *        使用了定时器2（TIM2）
    */
    MB_Init(1000);
    
    // UART1
    /***********************************************
    * 描述：  UART1串口和测量装置连接。master
    *        用作主机。
    */
//#if MODBUS_CFG_MASTER_EN == DEF_TRUE
//    pch         = MB_CfgCh( ModbusNode,             // ... Modbus Node # for this slave channel
//                           MODBUS_SLAVE,          // ... This is a MASTER
//                           500,                    // ... 0 when a slave
//                           MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
//                           1,                      // ... Specify UART #1
//                           9600,                   // ... Baud Rate
//                           USART_WordLength_8b,         // ... Number of data bits 7 or 8
//                           USART_Parity_No,       // ... Parity: _NONE, _ODD or _EVEN
//                           USART_StopBits_1,         // ... Number of stop bits 1 or 2
//                           MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
//    pch->AesEn          = DEF_DISABLED;             // ... AES加密禁止
//    pch->NonModbusEn    = DEF_ENABLED;              // ... 支持非MODBUS通信
//    pch->IapModbusEn    = DEF_ENABLED;              // ... 支持IAP MODBUS通信
//    
//    pch->RxFrameHead    = 0x1028;                   // ... 添加匹配帧头
//    pch->RxFrameTail    = 0x102C;                   // ... 添加匹配帧尾
//    
//    Ctrl.Mtr.pch       = pch;                      // ... modbus控制块和全局结构体建立连接
//    
//#endif
    // UART2
    /***********************************************
    * 描述:   UART2串口和测量装置通讯。master
    */
    
#if MODBUS_CFG_MASTER_EN == DEF_TRUE
    pch         = MB_CfgCh( ModbusNode,             // ... Modbus Node # for this slave channel
                           MODBUS_MASTER,           // ... This is a MASTER
                           500,                    // ... 0 when a slave
                           MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
                           1,                      // ... Specify UART #3
                           9600,                  // ... Baud Rate
                           USART_WordLength_8b,         // ... Number of data bits 7 or 8
                           USART_Parity_No,       // ... Parity: _NONE, _ODD or _EVEN
                           USART_StopBits_1,         // ... Number of stop bits 1 or 2
                           MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
    pch->AesEn          =   DEF_DISABLED;           // ... AES加密禁止
    pch->NonModbusEn    =   DEF_ENABLED;            // ... 支持非MODBUS通信
    pch->IapModbusEn    =   DEF_ENABLED;            // ... 支持IAP MODBUS通信
    
    pch->RxFrameHead    = 0x1028;                   // ... 添加匹配帧头
    pch->RxFrameTail    = 0x102C;                   // ... 添加匹配帧尾
    
    
    Ctrl.Mtr.pch       = pch;                      // ... modbus控制块和全局结构体建立连接
#endif
    
    // UART3
    /***********************************************
    * 描述：  UART3串口和无线发送模块连接。master
    *        
    */
#if MODBUS_CFG_MASTER_EN == DEF_TRUE
    
    pch         = MB_CfgCh( ModbusNode,             // ... Modbus Node # for this slave channel
                           MODBUS_SLAVE,           // ... This is a SLAVE
                           500,                    // ... 0 when a slave
                           MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
                           2,                      // ... Specify UART #3
                           57600,                   // ... Baud Rate
                           USART_WordLength_8b,         // ... Number of data bits 7 or 8
                           USART_Parity_No,       // ... Parity: _NONE, _ODD or _EVEN
                           USART_StopBits_1,         // ... Number of stop bits 1 or 2
                           MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
    pch->AesEn          = DEF_DISABLED;             // ... AES加密禁止
    pch->NonModbusEn    = DEF_ENABLED;              // ... 支持非MODBUS通信
    pch->IapModbusEn    = DEF_ENABLED;              // ... 支持IAP MODBUS通信
    
    pch->RxFrameHead    = 0x1028;                   // ... 添加匹配帧头
    pch->RxFrameTail    = 0x102C;                   // ... 添加匹配帧尾
    
    Ctrl.Dtu.pch       = pch;                      // ... modbus控制块和全局结构体建立连接
#endif
    
    // UART3
    /***********************************************
    * 描述：  UART0串口和TAX连接。slave
    *        
    */
//#if MODBUS_CFG_MASTER_EN == DEF_TRUE
//    
//    pch         = MB_CfgCh( ModbusNode,             // ... Modbus Node # for this slave channel
//                           MODBUS_SLAVE,           // ... This is a SLAVE
//                           500,                    // ... 0 when a slave
//                           MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
//                           0,                      // ... Specify UART #0
//                           28800,                   // ... Baud Rate
//                           USART_WordLength_8b,         // ... Number of data bits 7 or 8
//                           //UART_PARITY_NONE,       // ... Parity: _NONE, _ODD or _EVEN
//                           USART_Parity_No,
//                           USART_StopBits_1,         // ... Number of stop bits 1 or 2
//                           MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
//    pch->AesEn          = DEF_DISABLED;             // ... AES加密禁止
//    pch->NonModbusEn    = DEF_ENABLED;              // ... 支持非MODBUS通信
//    pch->IapModbusEn    = DEF_ENABLED;              // ... 支持IAP MODBUS通信
//    
//    pch->RxFrameHead    = 0x1028;                   // ... 添加匹配帧头
//    pch->RxFrameTail    = 0x102C;                   // ... 添加匹配帧尾
//    
//    pch->RTU_TimeoutCnts = 50;
//    pch->RTU_TimeoutCtr  = 50;
//    
//    Ctrl.DevTax.pch    = pch;                      // ... modbus控制块和全局结构体建立连接
//#endif
    
}


/*******************************************************************************
* 名    称： APP_CommInit
* 功    能： 任务初始化
* 入口参数： 无
* 出口参数： 无
* 作　 　者： wumingshen.
* 创建日期： 2015-03-28
* 修    改：
* 修改日期：
*******************************************************************************/
void APP_CommInit(void)
{
    OS_ERR err;
    
    /***********************************************
    * 描述： 创建事件标志组,协调comm收发
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.CommEvtFlag= COMM_EVT_FLAG_MTR_RX        // MTR 接收事件
        + COMM_EVT_FLAG_DTU_RX          // DTU 接收事件
            + COMM_EVT_FLAG_OTR_RX        // OTR 接收事件
                + COMM_EVT_FLAG_MTR_TIMEOUT   // MTR 操作超时，定时发送使用
                    + COMM_EVT_FLAG_DTU_TIMEOUT   // DTU 操作超时，定时发送使用
                        + COMM_EVT_FLAG_OTR_TIMEOUT  // OTR 操作超时，定时发送使用   
                            + COMM_EVT_FLAG_TAX_RX   // TAX 操作超时，定时发送使用
                                + COMM_EVT_FLAG_TAX_TIMEOUT;  // TAX 操作超时，定时发送使用  
    
    /***********************************************
    * 描述： 初始化MODBUS通信
    */        
    App_ModbusInit();
    
    
    /***********************************************
    * 描述： 在看门狗标志组注册本任务的看门狗标志
    */
    WdtFlags |= WDT_FLAG_COMM;
}

/*******************************************************************************
* 名    称： APP_CommRxDataDealCB
* 功    能： 串口数据处理回调函数，由MB_DATA.C调用
* 入口参数： 无
* 出口参数： 无
* 作    者： 无名沈
* 创建日期： 2016-01-04
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
INT08U APP_CommRxDataDealCB(MODBUS_CH  *pch)
{
    /***********************************************
    * 描述： 2016/01/08增加，用于非MODBBUS IAP升级通信
    */
#if MB_IAPMODBUS_EN == DEF_ENABLED
    //    /***********************************************
    //    * 描述： 如果程序处在升级模式
    //    */
    //    if ( ( Iap.Status != IAP_STS_DEF ) && 
    //         ( Iap.Status != IAP_STS_SUCCEED ) &&
    //         ( Iap.Status != IAP_STS_FAILED ) ) {
    //        return IAP_CommRxDataDealCB(pch);
    //    }
#endif
    /***********************************************
    * 描述： 获取帧头
    */
    //CPU_SR_ALLOC();
    
    //redmorningcn 数据复制功能由MBS_RTU_Task 已经执行。此处无需重复拷贝。
    
    //    CPU_CRITICAL_ENTER();
    //    //u8  Len     = pch->RxBufByteCtr;
    //u8  Len     = pch->RxBufByteCnt;
    //    memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    //    CPU_CRITICAL_EXIT();
    
    OS_ERR      err;
    
    //DataBuf = pch->RxFrameData;
    
    //INT08U  *DataBuf = 0 ;
    /***********************************************
    * 描述： 接收数据处理,
    * 根据协议，返回数据是            源地址
    *           目标地址由           GetRecSlaveAddr （）获得。
    *           数据区长度在          Len
    *           数据区                 DataBuf
    *           接收序号            GetRecvFrameNbr（）
    */  
    
     //在此增加新的V2.0   
    stcCsncProtocolPara sCsncPara;      //csns結構
    sCsncPara.destaddr = 0;             //地址赋值
    
    uint8       buf[256];
    uint8       flg; 
    sCsncPara.databuf = buf;
    sCsncPara.rxtxbuf = (uint8 *)pch->RxFrameData;
    sCsncPara.rxtxlen = pch->RxBufByteCnt;
    flg = DataUnpack_CSNC(&sCsncPara);   
//   
//    //協議V1.0
//    uint8   DataPos;
//    u8  SourceAddr = 
//        CSNC_GetData(	(uint8 *)pch->RxFrameData,      //RecBuf,
//                     (uint16 )Len,                      //RecLen, 
//                     (uint8 *)&DataPos,                 //DataBuf,
//                     (uint16 *)&Len);                   //InfoLen)
    /***********************************************
    * 描述： 如果接收地址和装置地址不符，则不处理，接收数据错误。
    */
    
//    if ( MASTE_ADDR_HOST != GetRecSlaveAddr() ) 
//        return FALSE;
    //協議轉發考慮
    if ( MASTE_ADDR_HOST != sCsncPara.destaddr || flg == 0 ) //解析错误、地址错误，退出。
        return FALSE;
    
    /***********************************************
    * 描述： 根据 地址  pch来源  判断数据是 否 合理 ，
    *       并赋值。
    */
    //switch  (pch->Ch )
    switch  (pch->PortNbr )     //根据端口号定义
    {
        /***********************************************
        * 描述： UART0接收到数据
        */
    case 0:
        break;
        
        /***********************************************
        * 描述： UART1接收到数据。数据发送地址：A1、A2
        *                         数据接收地址：80
        */        
    case 1:
////        if(     SLAVE_ADDR_DIP1 == SourceAddr 
////           ||   SLAVE_ADDR_DIP2 == SourceAddr    
//        if(     SLAVE_ADDR_DIP1 == sCsncPara.sourceaddr 
//           ||   SLAVE_ADDR_DIP2 == sCsncPara.sourceaddr                   
//               
//               )   
//        {
////            Ctrl.Mtr.RxCtrl.SourceAddr = SourceAddr;           //源地址
////            Ctrl.Mtr.RxCtrl.DestAddr   = GetRecSlaveAddr();    //目标地址
////            Ctrl.Mtr.RxCtrl.FramNum    = GetRecvFrameNbr();    //接收到的帧号
////            Ctrl.Mtr.RxCtrl.Code       = 0;                    //指令码 ，测量装置未定义 
////            Ctrl.Mtr.RxCtrl.Len        = Len;                  //数据区长度
//            
//            Ctrl.Mtr.RxCtrl.SourceAddr = sCsncPara.sourceaddr;   	//源地址
//            Ctrl.Mtr.RxCtrl.DestAddr   = sCsncPara.destaddr;    	//目标地址
//            Ctrl.Mtr.RxCtrl.FramNum    = sCsncPara.framnum;    	//接收到的帧号
//            Ctrl.Mtr.RxCtrl.FrameCode  = (uint8)sCsncPara.framcode & (0x0f);//指令码 ，测量装置未定义 
//            Ctrl.Mtr.RxCtrl.Len        = sCsncPara.datalen;       	//数据区长度    
//            
//            //取数据记录，将数据记录保存到接收区
//            //OS_CRITICAL_ENTER();
////            memcpy( (INT08U *)&Ctrl.Mtr.Rd.Buf[0], (INT08U *)&pch->RxFrameData[DataPos], Len );
//            //memcpy( (INT08U *)&Ctrl.Mtr.Rd.Buf[0], buf, Len );
//            memcpy( (INT08U *)&Ctrl.Mtr.Rd.Buf[0], buf, sCsncPara.datalen );
//            //OS_CRITICAL_EXIT();
//            
//            /***********************************************
//            * 描述： UART1连接状态描述
//            */       
//            for(uint8 i = 0;i< COMM_DEV_MTR_CONN_NUM;i++)
//            {
////                if( Ctrl.Mtr.ConnCtrl[i].SlaveAddr == SourceAddr)
//                if( Ctrl.Mtr.ConnCtrl[i].SlaveAddr == sCsncPara.sourceaddr )
//                {
//                    Ctrl.Mtr.ConnCtrl[i].RecvEndFlg   = 1;      //接收到数据，置1。数据处理后置0
//                    Ctrl.Mtr.ConnCtrl[i].TimeOut      = 0;      //超时计数器清零。
//                }
//            }
//            
//            OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
//                        ( OS_FLAGS     ) COMM_EVT_FLAG_MTR_RX,
//                        ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                        ( CPU_TS       ) 0,
//                        ( OS_ERR      *) &err);       
//        }
//        
//        Ctrl.Mtr.ConnectFlag       = 1;
//        Ctrl.Mtr.ConnectTimeOut    = 0;   
        
        break;
        
        /***********************************************
        * 描述： UART3接收到数据。数据发送地址：CA、C2
        *                         数据接收地址:80
        */   
    case 2:
        if(        SLAVE_ADDR_DTU   == sCsncPara.sourceaddr 
           ||      SLAVE_ADDR_SET   == sCsncPara.sourceaddr    
               )   
        {
            Ctrl.Dtu.RxCtrl.SourceAddr = sCsncPara.sourceaddr;   	//源地址
            Ctrl.Dtu.RxCtrl.DestAddr   = sCsncPara.destaddr;    	//目标地址
            Ctrl.Dtu.RxCtrl.FramNum    = sCsncPara.framnum;    	//接收到的帧号
            Ctrl.Dtu.RxCtrl.FrameCode  = (uint8)sCsncPara.framcode & (0x0f);//指令码 ，测量装置未定义 
            Ctrl.Dtu.RxCtrl.Len        = sCsncPara.datalen;       	//数据区长度               

			/***********************************************
            * 描述：无线通讯及IC卡通讯，定义数据区的前 4 个字节为命令字，   
            *                           ，4字节后才是真正的数据。                        
            */                   
            //if(Len >=4)     
            if(sCsncPara.datalen >=4 && Ctrl.Dtu.RxCtrl.FrameCode == 0) 
            {
                //Ctrl.Dtu.RxCtrl.Len        = Len - 4;                  //数据区长度
                Ctrl.Dtu.RxCtrl.Len        = sCsncPara.datalen - 4;                  //数据区长度
                //取数据记录，将数据记录保存到接收区
                //OS_CRITICAL_ENTER();
                //memcpy( (INT08U *)&Ctrl.Dtu.RxCtrl.Code, (INT08U *)&pch->RxFrameData[DataPos], 4);        //功能码
                //memcpy( (INT08U *)&Ctrl.Dtu.Rd.Buf[0],   (INT08U *)&pch->RxFrameData[DataPos+4], Len-4 );   //数据区
                memcpy( (INT08U *)&Ctrl.Dtu.RxCtrl.DataCode, buf, 4);        //功能码
                memcpy( (INT08U *)&Ctrl.Dtu.Rd.Buf[0],   (INT08U *)&buf[4], sCsncPara.datalen-4 );   //数据区

                //OS_CRITICAL_EXIT();                    
            }
            else        //V2.0协议操作  //将数据拷贝数据区，在根据协议解析   
            {
                memcpy( (INT08U *)&Ctrl.Dtu.Rd.Buf[0],   (INT08U *)&buf[0], sCsncPara.datalen );
            }
            /***********************************************
            * 描述： UART3连接状态描述
            */       
            for(uint8 i = 0;i< COMM_DEV_DTU_CONN_NUM;i++)
            {
                //if( Ctrl.Dtu.ConnCtrl[i].SlaveAddr == SourceAddr)
                if( Ctrl.Dtu.ConnCtrl[i].SlaveAddr == sCsncPara.sourceaddr)
                {
                    Ctrl.Dtu.ConnCtrl[i].RecvEndFlg  = 1;              //接收到数据，置1。数据处理后置0
                    Ctrl.Dtu.ConnCtrl[i].TimeOut     = 0;              //超时计数器清零。
                }
            }
            
            OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                        ( OS_FLAGS     ) COMM_EVT_FLAG_DTU_RX,
                        ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                        ( CPU_TS       ) 0,
                        ( OS_ERR      *) &err);       
        }
        
        Ctrl.Dtu.ConnectFlag       = 1;
        Ctrl.Dtu.ConnectTimeOut    = 0;   
        
        break;
        /***********************************************
        * 描述： UART4接收到数据
        */   
    case 3:
////        if(         SLAVE_ADDR_OTR == SourceAddr 
////           ||      SLAVE_ADDR_SET  == SourceAddr    
////               
//        if(         SLAVE_ADDR_OTR == sCsncPara.sourceaddr 
//           ||      SLAVE_ADDR_SET  == sCsncPara.sourceaddr    
//               )   
//        {
////            Ctrl.Otr.RxCtrl.SourceAddr = SourceAddr;           //源地址
////            Ctrl.Otr.RxCtrl.DestAddr   = GetRecSlaveAddr();    //目标地址
////            Ctrl.Otr.RxCtrl.FramNum    = GetRecvFrameNbr();    //接收到的帧号
//
//            Ctrl.Otr.RxCtrl.SourceAddr = sCsncPara.sourceaddr;   	//源地址
//            Ctrl.Otr.RxCtrl.DestAddr   = sCsncPara.destaddr;    	//目标地址
//            Ctrl.Otr.RxCtrl.FramNum    = sCsncPara.framnum;    	//接收到的帧号
//            Ctrl.Otr.RxCtrl.FrameCode  = (uint8)sCsncPara.framcode & (0x0f);//指令码 ，测量装置未定义 
//            Ctrl.Otr.RxCtrl.Len        = sCsncPara.datalen;       	//数据区长度               
//
//            /***********************************************
//            * 描述：无线通讯及IC卡通讯，定义数据区的前 4 个字节为命令字，   
//            *                           ，4字节后才是真正的数据。                        
//            */                   
//            //if(Len >=4)  //V1.0协议操作
//            if(sCsncPara.datalen >=4 && Ctrl.Otr.RxCtrl.FrameCode == 0) 
//            {
//                //Ctrl.Otr.RxCtrl.Len        = Len - 4;                  //数据区长度
//                Ctrl.Otr.RxCtrl.Len        = Ctrl.Otr.RxCtrl.Len  - 4;                  //数据区长度 - 4;                  //数据区长度
//                //取数据记录，将数据记录保存到接收区
//                //OS_CRITICAL_ENTER();
//                //memcpy( (INT08U *)&Ctrl.Otr.RxCtrl.Code, (INT08U *)&pch->RxFrameData[DataPos], 4);        //功能码
//                //memcpy( (INT08U *)&Ctrl.Otr.Rd.Buf[0],   (INT08U *)&pch->RxFrameData[DataPos+4], Len-4 );   //数据区
//
//                memcpy( (INT08U *)&Ctrl.Otr.RxCtrl.DataCode, buf, 4);        //功能码
//                memcpy( (INT08U *)&Ctrl.Otr.Rd.Buf[0],   (INT08U *)&buf[4], sCsncPara.datalen-4 );   //数据区                //OS_CRITICAL_EXIT();                    
//            }
//            else        //V2.0协议操作    //将数据拷贝数据区，在根据协议解析 
//            {
//                memcpy( (INT08U *)&Ctrl.Otr.Rd.Buf[0],   (INT08U *)&buf[0], sCsncPara.datalen );
//            }
//            /***********************************************
//            * 描述： UART2连接状态描述
//            */       
//            for(uint8 i = 0;i< COMM_DEV_OTR_CONN_NUM;i++)
//            {
//                //if( Ctrl.Otr.ConnCtrl[i].SlaveAddr == SourceAddr)
//                if( Ctrl.Otr.ConnCtrl[i].SlaveAddr == sCsncPara.sourceaddr)
//                {
//                    Ctrl.Otr.ConnCtrl[i].RecvEndFlg  = 1;      //接收到数据，置1。数据处理后置0
//                    Ctrl.Otr.ConnCtrl[i].TimeOut     = 0;      //超时计数器清零。
//                }
//            } 
//            
//            OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
//                        ( OS_FLAGS     ) COMM_EVT_FLAG_OTR_RX,
//                        ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                        ( CPU_TS       ) 0,
//                        ( OS_ERR      *) &err);       
//        }
//        
//        Ctrl.Otr.ConnectFlag       = 1;
//        Ctrl.Otr.ConnectTimeOut    = 0;   
//        
        break;
    }
    
    return TRUE;
}

/***********************************************
* 描述： 2016/01/08增加，用于非MODBBUS IAP升级通信
*/
#if MB_IAPMODBUS_EN == DEF_ENABLED
/*******************************************************************************
* 名    称： APP_CommRxDataDealCB
* 功    能： 串口数据处理回调函数，由MB_DATA.C调用
* 入口参数： 无
* 出口参数： 无
* 作    者： 无名沈
* 创建日期： 2016-01-04
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
INT08U IAP_CommRxDataDealCB(MODBUS_CH  *pch)
{
    //    CPU_SR_ALLOC();
    //    CPU_CRITICAL_ENTER();
    //    u8  Len     = pch->RxBufByteCtr;
    //    memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    //    CPU_CRITICAL_EXIT();
    //    
    //    /***********************************************
    //    * 描述： 获取帧头
    //    */    
    //    INT08U  *DataBuf    = pch->RxFrameData;
    //    
    //    /***********************************************
    //    * 描述： 接收数据处理
    //    */
    //    u8  DstAddr = 
    //    CSNC_GetData(	(unsigned char *)pch->RxFrameData,          //RecBuf,
    //                    (unsigned char	 )Len,                       //RecLen, 
    //                    (unsigned char *)DataBuf,                   //DataBuf,
    //                    (unsigned short *)&Len);                    //InfoLen)
    //    /***********************************************
    //    * 描述： 检测接收结束
    //    */
    //    if ( ( Len == 16) && 
    //         ( 0 >= memcmp((const char *)"IAP_pragram end!",(const char *)&pch->RxFrameData[8], 16) ) ) {
    //        /***********************************************
    //        * 描述： 发送数据处理
    //        */
    //        CSNC_SendData(	(MODBUS_CH      *)Ctrl.Com.pch,
    //                        (unsigned char  ) Ctrl.Com.SlaveAddr,          // SourceAddr,
    //                        (unsigned char  ) Ctrl.Com.Rd.Head.SrcAddr,    // DistAddr,
    //                        (unsigned char *)&pch->RxFrameData[8],          // DataBuf,
    //                        (unsigned short ) Len); 
    //        /***********************************************
    //        * 描述： 置位IAP结束标志位
    //        */
    //        OS_ERR err;
    //        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
    //                    ( OS_FLAGS     ) COMM_EVT_FLAG_IAP_END,
    //                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
    //                    ( CPU_TS       ) 0,
    //                    ( OS_ERR      *) &err);
    //        
    //        return TRUE;
    //    }
    //    
    //    if ( ( Len == 18) && 
    //         ( 0 >= memcmp((const char *)"IAP_pragram start!",(const char *)&pch->RxFrameData[8], 18) ) ) {
    //        
    //#if defined     (IMAGE_A) || defined   (IMAGE_B)
    //#else
    //        /***********************************************
    //        * 描述： 发送数据处理
    //        */
    //        CSNC_SendData(	(MODBUS_CH      *)Ctrl.Com.pch,
    //                        (unsigned char  ) Ctrl.Com.SlaveAddr,          // SourceAddr,
    //                        (unsigned char  ) Ctrl.Com.Rd.Head.SrcAddr,    // DistAddr,
    //                        (unsigned char *)&pch->RxFrameData[8],         // DataBuf,
    //                        (unsigned int	 ) Len);                        // DataLen 
    //#endif
    //        IAP_Restart();
    //        Iap.FrameIdx    = 0;
    //        return TRUE;
    //    }
    //    
    //    if ( Iap.Status < IAP_STS_START )
    //        return TRUE;
    //    /***********************************************
    //    * 描述： 写数据
    //    */
    //    /***********************************************
    //    * 描述： 将数据打包给发送结构
    //    */
    //    Ctrl.Com.Wr.Head.DataLen       = 0;
    //    /***********************************************
    //    * 描述： 编辑应答内容
    //    */
    //    char str[20];
    //    usprintf(str,"\n%d",Iap.FrameIdx);
    //    str[19]  = 0;
    //    /***********************************************
    //    * 描述： 写数据到Flash
    //    */
    //    IAP_Program((StrIapState *)&Iap, (INT16U *)&pch->RxFrameData[8], Len, (INT16U )GetRecvFrameNbr() );
    //    /***********************************************
    //    * 描述： 发送数据处理
    //    */
    //    CSNC_SendData(	(MODBUS_CH     *) Ctrl.Com.pch,
    //                    (unsigned char  ) Ctrl.Com.SlaveAddr,                    // SourceAddr,
    //                    (unsigned char  ) Ctrl.Com.Rd.Head.SrcAddr,              // DistAddr,
    //                    (unsigned char *) str,                                    // DataBuf,
    //                    (unsigned short ) strlen(str));                           // DataLen 
    //    if ( Len < 128 ) {
    //        /***********************************************
    //        * 描述： 置位IAP结束标志位
    //        */
    //        OS_ERR err;
    //        OS_FlagPost(( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
    //                    ( OS_FLAGS     ) COMM_EVT_FLAG_IAP_END,
    //                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
    //                    ( CPU_TS       ) 0,
    //                    ( OS_ERR      *) &err);
    //    } else {
    //        IAP_Programing();                               // 复位超时计数器
    //    }
    //        
    //    /***********************************************
    //    * 描述： 串口接受COMM模组的消息并处理
    //    */
    //    Ctrl.Com.ConnectTimeOut    = 0;                // 超时计数器清零
    //    Ctrl.Com.ConnectFlag       = TRUE;             // 转连接成功标志
    //    
        return TRUE;
}
#endif

/*******************************************************************************
* 名    称： NMBS_FCxx_Handler
* 功    能： 非MODBUS串口数据处理回调函数，由mbs_core.d调用。处理接收中断应答。
* 入口参数： 无
* 出口参数： 无
* 作    者： 无名沈
* 创建日期： 2017-02-03
* 修    改：
* 修改日期：
* 备    注： 
*******************************************************************************/
extern OS_Q                MTR_RxQ;
extern OS_Q                COM_RxQ;
extern OS_Q                DTU_RxQ;
CPU_BOOLEAN  NMBS_FCxx_Handler (MODBUS_CH  *pch)
{
    //OS_ERR      err;
    //   CPU_INT16U  head    = BUILD_INT16U(pch->RxBuf[1], pch->RxBuf[0]);
    //    CPU_INT16U  tail    = BUILD_INT16U(pch->RxBuf[pch->RxBufByteCtr-1],
    //                                       pch->RxBuf[pch->RxBufByteCtr-2]);
    CPU_INT16U  head    = BUILD_INT16U( pch->RxFrameData[1], pch->RxFrameData[0]);
    CPU_INT16U  tail    = BUILD_INT16U( pch->RxFrameData[pch->RxBufByteCnt-1],
                                       pch->RxFrameData[pch->RxBufByteCnt-2]);           //20170526 RxBufByteCtr 中断函数中动态更新。
    
    /***********************************************
    * 描述： 长沙南车DTU协议处理
    */
    
    if ( ( pch->RxFrameHead == head ) &&              
        ( pch->RxFrameTail == tail ) ) {
            APP_CommRxDataDealCB(pch);
            /***********************************************
            * 描述： 电表调试协议协议,电表协议协议处理处理
            */
        } else  if ( ( 0xAA55 == head ) || ( 0xAAAA == head ) ) {
            OS_ERR  err;
            
            CPU_SR_ALLOC();
            OS_CRITICAL_ENTER();
            memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, pch->RxBufByteCnt );
            OS_CRITICAL_EXIT();
            
            (void)OSQPost((OS_Q         *)&MTR_RxQ,
                          (void         *) pch,
                          //(OS_MSG_SIZE   ) pch->RxBufByteCtr,
                          (OS_MSG_SIZE   ) pch->RxBufByteCnt,
                          (OS_OPT        ) OS_OPT_POST_FIFO,
                          (OS_ERR       *)&err);
             //BSP_OS_TimeDly(5);
             /***********************************************
             * 描述： tax通讯协议
             */  
        } else {
//            if(pch->PortNbr == 0)                       //串口0，做 
//                if(TAX_FCxx_Handler(pch) == FALSE)     //tax通讯处理
                    return DEF_FALSE;
        }
    return DEF_TRUE;
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
//    uint8_t     fram_clr[4]     = {0};
//	uint8_t     IDBuf[4]        = {0};
//	uint8_t     clear_buf[24]   = {0};
//	uint32_t    ID              = 0;
//	uint32_t    utemp;
    
	//TIME        system; 
    
    /***********************************************
    * 描述： 拷贝数据到数据区
    */
    //CPU_SR_ALLOC();
    //CPU_CRITICAL_ENTER();
    //uint32_t  Len     = pch->RxBufByteCnt;
    //memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    //CPU_CRITICAL_EXIT();
    uint32_t    Len       = pch->RxBufByteCnt;
    INT08U      *DataBuf  = pch->RxFrameData;
    
    
    CPU_INT16U  head    = BUILD_INT16U(pch->RxBuf[1], pch->RxBuf[0]);
    CPU_INT16U  tail    = BUILD_INT16U(pch->RxBuf[pch->RxBufByteCnt-1],
                                       pch->RxBuf[pch->RxBufByteCnt-2]);
    /***********************************************
    * 描述： 长沙南车DTU协议处理
    */
    if ( ( pch->RxFrameHead == head ) &&
        ( pch->RxFrameTail == tail ) ) {
            DataBuf = &pch->RxFrameData[8];
            //        Len     = Ctrl.Com.Rd.Head.DataLen;        
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
        int8_t chkSum   = GetCheckSum(DataBuf,Len);
        
        if ( chkSum ==  DataBuf[Len] ) {
            //            pch->TxFrameData[0]     = 0xAA;
            //            pch->TxFrameData[1]     = 0xAA;
            //            pch->TxFrameData[2]     = 0x30;
            //            pch->TxFrameData[3]     = 0x00;
            //            pch->TxFrameData[4]     = 0x71;
            //            
            //            CPU_SR  cpu_sr;
            //            OS_CRITICAL_ENTER();
            //            memcpy(&pch->TxFrameData[5],(uint8_t *)&Mater.Energy, sizeof(StrEnergy));
            //            OS_CRITICAL_EXIT();
            //            
            //            pch->TxFrameData[45]    = 0x00;
            //            pch->TxFrameData[46]    = 0x00;
            //            
            //            pch->TxFrameData[47]    = GetCheckSum(pch->TxFrameData,47);
            //            
            //            NMB_Tx((MODBUS_CH   *)pch,
            //                   (CPU_INT08U  *)pch->TxFrameData,
            //                   (CPU_INT16U   )48);
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
            //uint16_t crc16_Cal = crc16((uint8_t *)&DataBuf[0],9);
            //uint16_t crc16_Rec = BUILD_INT16U(DataBuf[9],DataBuf[10]);
            //if(crc16_Cal != crc16_Rec) {                
            //    return TRUE;
            //}
        }
        switch(DataBuf[2])
        {
            /***********************************************
            * 描述： 设置序列号
            */
        case 0xA1:
            //ID = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            //WriteFM24CL64(64 , (uint8_t *)&ID , 4);
            //ReadFM24CL64(64 , (uint8_t *)&ID , 4);
            //uartprintf(pch,"\r\n 本装置ID设置为：%d\r\n" , ID);
            break;            
            /***********************************************
            * 描述： 设置机车型号/机车号
            */
        case 0xA2:	
            //Mater.LocoTyp   = BUILD_INT16U(DataBuf[3],DataBuf[4]);
            //Mater.LocoNbr   = BUILD_INT16U(DataBuf[5],DataBuf[6]);
            //WriteFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            //WriteFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            //ReadFM24CL64(68 ,  (uint8_t *)&Mater.LocoTyp, 2);
            //ReadFM24CL64(70 ,  (uint8_t *)&Mater.LocoNbr, 2);
            //uartprintf(pch,"\r\n 当前车型：%d；当前车号：%d\r\n", Mater.LocoTyp,Mater.LocoNbr);
            break; 
            /***********************************************
            * 描述： 设置时间
            */
        case 0xA3:
            //            system.Year     =DataBuf[3];
            //            system.Month    =DataBuf[4];
            //            system.Day      =DataBuf[5];
            //            system.Hour     =DataBuf[6];
            //            system.Min      =DataBuf[7];
            //            system.Sec      =DataBuf[8];
            //            WriteTime(system);
            //            GetTime((TIME *)&recordsfr.Time[0]);
            //            uartprintf(pch,"\r\n 当前时间为：20%02d-%02d-%02d  %02d:%02d:%02d", 
            //                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
            //                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);
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
    //    if ( pch->PortNbr == Ctrl.Dtu.pch->PortNbr ) {
    //        if ( GetRecvFrameNbr() == Ctrl.Com.Rd.Head.PacketSn ) {
    //            /***********************************************
    //            * 描述： 清除定时上传标志
    //            */
    //            //SetSendFrameNbr();
    //            //extern void UpdateRecordPoint(uint8_t storeflag);
    //            //UpdateRecordPoint(1);
    //            //pch->StatNoRespCtr  = 0;
    //            
    //            //osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
    //        }
    //    } else {
    //        /***********************************************
    //        * 描述： 将数据打包给发送结构
    //        */ 
    //        APP_MaterCommHandler(pch);
    //    }
    //    
    //    /***********************************************
    //    * 描述： 串口接受COMM模组的消息并处理
    //    */
    //    Ctrl.Com.ConnectTimeOut    = 0;                // 超时计数器清零
    //    Ctrl.Com.ConnectFlag       = TRUE;             // 转连接成功标志
    //    
    return TRUE;
}
#endif
/*******************************************************************************
* 				end of file
*******************************************************************************/
#endif
