#ifndef	APP_CTRL_H
#define	APP_CTRL_H
/*******************************************************************************
 *   Filename:       app_ctrl.h
 *   Revised:        $Date: 2009-01-08	20:15 (Fri) $
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.  现代虚拟仪器仪表研究所
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <global.h>
#include <app.h>
#include <protocol.h>
#include <app_comm_protocol.h>
#include <os.h>
#include <osal.h>
#include <mb.h>
#include <app_tax.h>
#include <stc_AirDef.h>
#include <stc_ProCommDef .h>
#include <bsp_DS3231.h>

#ifdef __cplusplus
extern "C" {
#endif
     
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
/*******************************************************************************
 * CONSTANTS
 */

 /*******************************************************************************
 * 				            系统参数相关宏定义                                  *
 *******************************************************************************/

    
/***************************************************
* 描述： 定义存储设备类型
*/
#define INTNER_FLASH                        0
#define INTNER_EEPROM                       1
#define EXTERN_EEPROM                       2
#define STORAGE_TYPE                        EXTERN_EEPROM

/***************************************************
* 描述： TAX
*/
#ifndef TAX_EN
#define TAX_EN                              DEF_ENABLED
#if     defined(TAX_EN)
#define TAX_II_EN                           DEF_ENABLED
#define TAX_III_EN                          DEF_ENABLED
#endif
#endif
    
/***************************************************
* 描述： COMM事件标志组
*/    
#define COMM_EVT_FLAG_HEART               (1 << 0)// 心跳包发送
#define COMM_EVT_FLAG_RESET               (1 << 1)// WIFI复位
#define COMM_EVT_FLAG_CONNECT             (1 << 2)// WIFI连接
#define COMM_EVT_FLAG_RECV                (1 << 3)// 串口接收
#define COMM_EVT_FLAG_REPORT              (1 << 4)// 上报消息
#define COMM_EVT_FLAG_CLOSE               (1 << 5)// 断开
#define COMM_EVT_FLAG_TIMEOUT             (1 << 6)// 超时
#define COMM_EVT_FLAG_CONFIG              (1 << 7)// 配置
#define COMM_EVT_FLAG_IAP_START           (1 << 8)// IAP开始
#define COMM_EVT_FLAG_IAP_END             (1 << 9)// IAP结束


/*******************************************************************************
 * 				            OSAL相关定义                                       *
 *******************************************************************************/
/*******************************************************************************
* 描述： OSAL任务优先级定义
*/
#define	OS_TASK_PRO_TMR                 (OS_TASK_PRIO_HIGHEST - 0)
#define	OS_TASK_PRO_DISP                (OS_TASK_PRIO_HIGHEST - 1)
#define	OS_TASK_PRO_LED                 (OS_TASK_PRIO_HIGHEST - 6)
#define	OS_TASK_PRO_STORE               (OS_TASK_PRIO_HIGHEST - 4)

/***************************************************
* 描述： OSAL任务ID定义
*/
#define OS_TASK_ID_TMR                  0x00
#define OS_TASK_ID_DISP                 0x01
#define OS_TASK_ID_LED		            0x06
#define OS_TASK_ID_STORE		        0x04
    
/*******************************************************************************
* 描述： OSAL事件定义
*/
/***************************************************
* 描述： OSAL事件定义：TMR任务事件
*/
#define OS_EVT_TMR_TICKS                0X00000001
#define OS_EVT_TMR_MSEC                 0X00000002
#define OS_EVT_TMR_SEC                  0X00000004
#define OS_EVT_TMR_MIN                  0X00000008
#define OS_EVT_TMR_DEAL                 0X00000010
#define OS_EVT_TMR_UPDATE_DENSITY       0X00000020
#define OS_EVT_TMR_UPDATE_PARA          0X00000040
#define OS_EVT_TMR_ERR_CHK              0X00000080
#define OS_EVT_TMR_IAP_RXTIMEOUT        0X00000100
/*******************************************************************************
* 描述： OSAL事件定义
*/
/***************************************************
* 描述： OSAL事件定义：TMR任务事件(20170930 定时器会不时停止发送，控制数据量8个)
*/
//#define OS_EVT_TMR_TICKS                0X00000001
#define OS_EVT_TMR_MTR                  0X00000001
//#define OS_EVT_TMR_MSEC                 0X00000002
#define OS_EVT_TMR_DTU                  0X00000002
#define OS_EVT_TMR_SEC                  0X00000004
//#define OS_EVT_TMR_MIN                  0X00000008
#define OS_EVT_TMR_OTR                  0X00000008
//#define OS_EVT_TMR_DEAL                 0X00000010
#define OS_EVT_TMR_TAX                  0X00000010

#define OS_EVT_TMR_5                    0X00000020
#define OS_EVT_TMR_6                    0X00000040
#define OS_EVT_TMR_7                    0X00000080

/***************************************************
* 描述： OSAL事件定义：STORE 任务事件
*/
#define OS_EVT_STORE_TICKS              0X00000001
#define OS_EVT_STORE_INIT               0X00000002
#define OS_EVT_STORE_2                  0X00000004
#define OS_EVT_STORE_3                  0X00000008
#define OS_EVT_STORE_4                  0X00000010
#define OS_EVT_STORE_5                  0X00000020
#define OS_EVT_STORE_6                  0X00000040
#define OS_EVT_STORE_7                  0X00000080


//#define OS_EVT_TMR_TAX                  0X00000100


/***************************************************
* 描述： OSAL事件定义：LED任务事件
*/
#define OS_EVT_LED_TICKS                0X00000001
#define OS_EVT_LED_1                    0X00000002
#define OS_EVT_LED_2                    0X00000004
#define OS_EVT_LED_3                    0X00000008
#define OS_EVT_LED_4                    0X00000010
#define OS_EVT_LED_5                    0X00000020
#define OS_EVT_LED_6                    0X00000040
#define OS_EVT_LED_7                    0X00000080

/***************************************************
* 描述： OSAL事件定义：DISP任务事件
*/
#define OS_EVT_DISP_TICKS                0X00000001
#define OS_EVT_DISP_INIT                 0X00000002
#define OS_EVT_DISP_2                    0X00000004
#define OS_EVT_DISP_3                    0X00000008
#define OS_EVT_DISP_4                    0X00000010
#define OS_EVT_DISP_5                    0X00000020
#define OS_EVT_DISP_6                    0X00000040
#define OS_EVT_DISP_7                    0X00000080


#define COMM_EVT_FLAG_MTR_RX                (1 << 0)	// MTR 接收事件
#define COMM_EVT_FLAG_DTU_RX                (1 << 1)	// DTU 接收事件
#define COMM_EVT_FLAG_OTR_RX                (1 << 2)	// OTR 接收事件
#define COMM_EVT_FLAG_MTR_TIMEOUT           (1 << 3)	// MTR 操作超时，定时发送使用
#define COMM_EVT_FLAG_DTU_TIMEOUT           (1 << 4)	// DTU 操作超时，定时发送使用
#define COMM_EVT_FLAG_OTR_TIMEOUT           (1 << 5)	// OTR 操作超时，定时发送使用

#define COMM_EVT_FLAG_TAX_RX                (1 << 6)	// OTR 接收事件
#define COMM_EVT_FLAG_TAX_TIMEOUT           (1 << 7)	// OTR 操作超时，定时发送使用

/*******************************************************************************
 * 				            其他定义                                           *
 *******************************************************************************/
/***************************************************
* 描述： 时间定义
*/

/*******************************************************************************
 * TYPEDEFS
 */
#define PARA_TYPE_INT08S                0
#define PARA_TYPE_INT08U                1
#define PARA_TYPE_INT16S                2
#define PARA_TYPE_INT16U                3

#define PARA_TYPE   PARA_TYPE_INT16S
__packed
typedef struct {
    
#if     ( PARA_TYPE == PARA_TYPE_INT08S )
    INT08S      Min;
    INT08U      Max;
#elif   ( PARA_TYPE == PARA_TYPE_INT08U )
    INT08U      Min;
    INT08U      Max;
#elif   ( PARA_TYPE == PARA_TYPE_INT16S )
    INT16S      Min;
    INT16S      Max;
#elif   ( PARA_TYPE == PARA_TYPE_INT16U )
    INT16U      Min;
    INT16U      Max;
#else
    INT08S      Min;
    INT08U      Max;
#endif
} StrParaRange;

__packed
typedef struct {
    OS_FLAG_GRP             WifiEvtFlagGrp;
    OS_FLAGS                WifiEvtFlag;
    OS_FLAG_GRP             CommEvtFlagGrp;
    OS_FLAGS                CommEvtFlag;
    OS_FLAG_GRP             MaterEvtFlagGrp;
    OS_FLAGS                MaterEvtFlag;
    OS_FLAG_GRP             DtuEvtFlagGrp;
    OS_FLAGS                DtuEvtFlag;
} StrCtrlOS;


#define	MTR_COMM_RECV_BUF_LEN	160
/***************************************************
* 描述： 主板-- 测量装置，MODBUS协议。
主板为主机，直接发送FC03寄存器读取指令。
接收数据区为压缩空气质量检测装置的 测量值。
暂无需要发送的数据。
*/
typedef union {
	stcAirQuality		sAirQuality;                    //接收的空气质量检测装置的内容
	uint8				Buf[MTR_COMM_RECV_BUF_LEN];
}UniMtrCommRecvData;

//发送数据定义
__packed
typedef struct {
    uint8   buf[10];
} StrMtrCommSendData;


//发送联合体，可定义多类型结构互用
typedef union {
	StrMtrCommSendData	StrSend;
	uint8				Buf[MTR_COMM_RECV_BUF_LEN];
}UniMtrCommSendData;

//異步通訊使用
typedef struct {     
    INT08U      DestAddr;       //接收地址      slave  =0xA1\A2	   
    INT08U      SourceAddr;     //源地址       master   =0x80	   
    INT08U      FramNum;        //帧序号
    INT08U      Len;			//接收有效数据长度
    INT08U      FrameCode;       
    INT08U      Tmp[3];
    INT32U      DataCode;       //接收控制字
} StrDevMtrRecvCtrl;

//连接控制字
// 4 Bytes
typedef struct {     
    uint8       ConnFlg;        //连接控制,1，允许连接，0，不允许连接
    uint8       RecvEndFlg;		//接收标示，1，数据接收完成，0，无数据接收。
    uint8       TimeOut;		//超时时间，单位1s
    uint8       ErrFlg;		    //错误标示，连接正常，0；连接错误，1
    uint8       SlaveAddr;       //接收地址         slave  =0xCA	   
    uint8       MasterAddr;     //源地址           master   =0x80	   
    uint8       SendFramNum;    //帧序号   
    uint8       SendFlg;        //发送标示， 有数据发送，1；无数据发送，0
} StrDevMtrConnCtrl;		

//主板和测量装置通讯控制
#define	COMM_DEV_MTR_CONN_NUM	2	        // 设备连接数A1\A2	
__packed
typedef struct {  
	UniMtrCommRecvData      Rd;				// 160 接收数据区
	UniMtrCommSendData      Wr;				// 160 发送数据区

    /***************************************************
    * 描述： 串口控制组
    */
	StrDevMtrRecvCtrl    	RxCtrl;         // 8		接收控制，包含当前接收到的控制信息
	StrDevMtrConnCtrl		ConnCtrl[COMM_DEV_MTR_CONN_NUM];
                                            // 4*2	连接控制，对每个地址作为单独的数据连接。
	
    MODBUS_CH            	*pch;           // 4 	MODBUS句柄
    
    INT08U                  ConnectTimeOut  : 7;             // 连接超时计数器，秒为单位
    INT08U                  ConnectFlag     : 1;                // 连接标志
} StrDevMtr;


/***************************************************
* 描述： 统计模块--无线发送模块，异步串行通信通用通信协议
*/
// 发送数据结构  		统计-->无线
__packed
typedef union {
	stcFlshRec  sRec;						//数据记录     128 	
	uint8		Buf[160];					//
} UniDtuCommSendData;


/***************************************************
* 描述： 统计模块--无线发送模块，长沙南睿轨道交通车辆装置异步串行通信通用通信协议
*/

typedef struct  _stcIAPStart_
{
    uint8   DataCode;       //数据指令
    uint8   Status;         //状态
    uint16  HardVer;        //硬件版本
    uint16  SoftVer;        //软件版本
    uint32  SoftSize;       //软件大小
    uint32  Addr;           //当前地址
    uint16  CurNum;         //当前序号
}stcIapStart;

// 发送数据结构		无线-->统计		
__packed
typedef union {
	stcTime		sTime;      //TIME_CARD     设置时间            8     second,minute,hour,data,month,year,crccheck
	stcLocoId   sLocoId;    //FIX_CARD      设置机车信息	    4     type,num;
    stcIapStart IapStart;   //IAP开始     
	stcRunPara	sRunPara;   //运行参数 
    stcAirPara  sAirPara;
    uint8		Buf[160];   //	
} UniDtuCommRecvData;

//接收控制字
typedef struct {     
    INT08U          DestAddr;       //接收地址      slave  =0xA1\A2	   
    INT08U          SourceAddr;     //源地址       master   =0x80	   
    INT08U          FramNum;        //帧序号
    INT08U			Len;			//接收有效数据长度
    INT08U          FrameCode;       
    INT08U          Tmp[3];
    INT32U			DataCode;			//接收控制字
} StrDevDtuRecvCtrl;

//连接控制字，
typedef struct {     
    INT08U          ConnFlg;        //连接控制,1，允许连接，0，不允许连接
    INT08U			RecvEndFlg;		//接收标示，1，数据接收完成，0，无数据接收。
    INT08U			TimeOut;		//超时时间，单位1s
    INT08U			ErrFlg;		    //错误标示，连接正常，0；连接错误，1

    INT08U          SlaveAddr;       //接收地址      slave  =0xCA	   
    INT08U          MasterAddr;     //源地址           master   =0x80	   
    INT08U          SendFramNum;    //帧序号   
    INT08U          SendFlg;        //发送标示， 有数据发送，1；无数据发送，0
} StrDevDtuConnCtrl;		

#define	COMM_DEV_DTU_CONN_NUM	2	// 设备连接数CA/C1	
__packed
typedef struct {  
	UniDtuCommRecvData	Rd;			//接收数据区
	UniDtuCommSendData	Wr;			//发送数据区

    /***************************************************
    * 描述： 串口控制组
    */
	StrDevDtuRecvCtrl    	RxCtrl;				            //接收控制，包含当前接收到的控制信息
	StrDevDtuConnCtrl		ConnCtrl[COMM_DEV_DTU_CONN_NUM];//连接控制，对每个地址作为单独的数据连接。
	
    MODBUS_CH            	*pch;                   		// MODBUS句柄
    
    INT08U                  ConnectTimeOut  : 7;     	// 连接超时计数器，秒为单位
    INT08U                  ConnectFlag     : 1;         	// 连接标志
} StrDevDtu;

__packed
typedef struct {
    /***************************************************
    * 描述： 数据结构，顺序不能随意调整
    */
    UniCtrlPara             Para;                               // 控制参数
    
    stcFlshRec              sRec;                               //  128  数据记录      包含所有采集数据及运算结果。
    
    StrDevMtr	            Mtr;                                //  和测量模块通讯控制字，接收收、发缓冲，接收控制，发送控制
    
	StrDevDtu	            Dtu;						        //  和无线发送模块通讯控制字，接收收、发缓冲，	接收控制，发送控制

    StrCtrlOS               Os;                                 // OS属性        
  
} stcCtrlDCB;

extern stcCtrlDCB           Ctrl;

/*******************************************************************************
 * 				                  其他                                         *
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/***************************************************
* 描述： 结构体声明
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
void        Beep                        (INT32U flag);
void        BeepExt                     (u8 cnt, u16 freq, u16 dly);
void        Power                       (INT08U onoff);
/***************************************************
* 描述： 应用层函数体声明
*/
BOOL        App_PraInit                 (void);
BOOL        App_GetParaFromEeprom       (void);
BOOL        App_SetParaToEeprom         (void);
void        App_Error                   (void);
void        App_FanHandCtrl             (INT08U step);
void        App_FanAutoCtrl             (INT08U step);
void        App_ParaFilter              (void);

/***************************************************
* 描述： OSAL任务初始化函数体声明
*/
void        TaskInitBeep                ( void );
void        TaskInitKey                 ( void );
void        TaskInitCtrl                ( void );
void        TaskInitTmr                 ( void );
void        TaskInitDisp                ( void );
void        TaskInitLed                 ( void );
void        TaskInitStore               ( void );


/***************************************************
* 描述： OSAL任务事件处理函数体声明
*/
osalEvt     TaskBeepEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskKeyEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskCtrlEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskTmrEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskDispEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskLedEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskStoreEvtProcess         (osalTid task_id, osalEvt task_event);



/***************************************************
* 描述： AES加减密函数体声明
*/
/***************************************************
* 描述： 看门狗函数体声明
*/
void        App_MsgWriteCmd             ( INT16U cmd );
/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif	/* GLOBLES_H */