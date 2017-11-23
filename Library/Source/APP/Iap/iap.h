/*******************************************************************************
 *   Filename:       iap.h
 *   Revised:        $Date: 2014-04-23
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:    
 *
 *   Notes:        
 *					   
 *			
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

#ifndef __IAP_H__
#define __IAP_H__
/*******************************************************************************
 * INCLUDES
 */
 #include "stm32f10x.h"

/*******************************************************************************
 * CONSTANTS
 */
/***********************************************
* 描述： 2017/8/19：IAP 使能
*/
#define IAP_EN                  DEF_ABLED
     
#define KB						*1024
/***********************************************
* 描述： 2017/8/19：定义FLSAH大小
*/
#define	FLASH_SIZE				(256KB)											// FLASH大小
/***********************************************
* 描述： 2017/8/19：FLASH分区
*/
#define	FLASH_SIZE_BOOT			(80KB)											// BOOT区大小
#define	FLASH_SIZE_EEP			(16KB)											// 参数区大小

#define	FLASH_SIZE_A			(FLASH_SIZE-FLASH_SIZE_BOOT-FLASH_SIZE_EEP)/2	// 程序A大小
#define	FLASH_SIZE_B			(FLASH_SIZE-FLASH_SIZE_BOOT-FLASH_SIZE_EEP)/2	// 程序B大小
/***********************************************
* 描述： 2017/8/19：地址分配
*/
#define FLASH_ADDR_BOOT			0x8000000  								        // 引导程序起始地址(存放在FLASH)
#define FLASH_ADDR_APP_A		(0x8000000 + FLASH_SIZE_BOOT)					// 第一个应用程序起始地址(存放在FLASH)
#define FLASH_ADDR_APP_B		(0x8000000 + FLASH_SIZE_BOOT + FLASH_SIZE_A)  	// 第二个应用程序起始地址(存放在FLASH)
#define FLASH_ADDR_APP			FLASH_ADDR_BOOT  								// 应用程序起始地址(存放在FLASH)
#define FLASH_ADDR_DATA			(FLASH_SIZE-FLASH_SIZE_EEP)  					// 应用程序起始地址(存放在FLASH)


#define FLASH_APP_ADDR_SAVE		(FLASH_SIZE_EEP - 4)  	                        // 保存下次运行的程序地址
#define FLASH_APP_STATUS		(FLASH_SIZE_EEP - 5)  	                        // 程序升级状态

#if defined     ( IMAGE_A )
#define IAP_SELF_APP_ADDR       FLASH_ADDR_APP_A 
#elif defined   ( IMAGE_B )
#define IAP_SELF_APP_ADDR       FLASH_ADDR_APP_B 
#else
#define IAP_SELF_APP_ADDR       FLASH_ADDR_BOOT
#endif 


#define IAP_STS_DEF             0
#define IAP_STS_RST             1
#define IAP_STS_START           2
#define IAP_STS_PROGRAMING      3
#define IAP_STS_FINISH          4
#define IAP_STS_FAILED          5
#define IAP_STS_SUCCEED         6

/*******************************************************************************
 * MACROS
 */
/***********************************************
* 描述： 2017/8/19：IAP错误信息
*/
typedef  enum  iap_err {
    IAP_ERR_NONE                =     0u,
} IAP_ERR;
/***********************************************
* 描述： 2017/8/19：IAP地址
*/
typedef  enum  iap_addr {
    IAP_ADDR_BOOT               = FLASH_ADDR_BOOT,
    IAP_ADDR_APPA               = FLASH_ADDR_APP_A,
    IAP_ADDR_APPB               = FLASH_ADDR_APP_B,
    IAP_ADDR_APP                = FLASH_ADDR_BOOT,
    IAP_ADDR_BACK               = FLASH_ADDR_APP_B,
    IAP_ADDR_DATA               = FLASH_ADDR_DATA,
} IAP_ADDR;
/***********************************************
* 描述： 2017/8/19：IAP升级命令
*/
typedef  enum  iap_cmd {
    IAP_CMD_START               = 0X01,
    IAP_CMD_PRG                 = 0X02,
    IAP_CMD_FINISH              = 0X03,
    IAP_CMD_STOP                = 0X04,
} IAP_CMD;
/***********************************************
* 描述： 2017/8/19：IAP升级状态
*/
typedef  enum  iap_sr_start {
    IAP_SR_START_START          = 0X01,
    IAP_SR_START_CONTINUE       = 0X02,
} IAP_SR_START;
/***********************************************
* 描述： 2017/8/19：IAP开始应答
*/
typedef  enum  iap_sr_start_rsp {
    IAP_SR_START_OK             = 0X01,
    IAP_SR_START_ERR_HV         = 0X03,
    IAP_SR_START_ERR_SV         = 0X04,
    IAP_SR_START_ERR_SIZE       = 0X05,
    IAP_SR_START_ERR_ADDR       = 0X06,
    IAP_SR_START_ERR_IDX        = 0X07,
    IAP_SR_START_ERR_CHK        = 0X08,
} IAP_SR_START_RSP;
/***********************************************
* 描述： 2017/8/19：IAP结束状态
*/
typedef  enum  iap_sr_finish {
    IAP_SR_FINISH_NO_RUN        = 0X00,
    IAP_SR_FINISH_RUN           = 0X01,
    IAP_SR_FINISH_RESTART       = 0X02,
} IAP_SR_FINISH;
/***********************************************
* 描述： 2017/8/19：IAP中止状态
*/
typedef  enum  iap_sr_stop {
    IAP_SR_STOP_PULSE           = 0X00,
    IAP_SR_STOP_CLR             = 0X01,
} IAP_SR_STOP;
/*******************************************************************************
 * TYPEDEFS
 */
__packed
typedef struct {
    INT32U      AppAddr;                            // 应用程序地址    INT08U      Status              : 4;            // // 0未升级，1开始，2升级中，3失败，4成功，其他：无程序
    INT08U      Status;                             // 0未升级，1开始，2升级中，3失败，4成功，其他：无程序
    INT08U      Step;
    INT16U      FrameIdx;                           // 接收到数据帧序号
    INT16U      SysNbr;                             // 同步序列号
    INT16U      TimeOut;                            // 超时退出IAP
    
    INT32U      FileSize;                           // 文件大小
    INT32U      WrittenSize;                        // 已写入文件大小
    INT32U      SectorAddr;                         // 当前写入的扇区地址
    INT32U      SectorAddrLast;                     // 上次写入的扇区地址
    INT32U      Sectors;                            // 总扇区数
    INT08U      WriteCtr;                           // 16帧128字节组成一个2K扇区扇区
    INT16U     *pBuf;                               // 数据缓冲区指针
    
    
    INT16U      HwVer;                              // 硬件版本
    INT16U      SwVer;                              // 软件版本
    INT32U      PrgSize;                            // 程序大小
    INT32U      CurAddr;                            // 当前地址
    INT16U      CurIdx;                             // 当前帧号
    
} StrIapState;

extern StrIapState Iap;

__packed
typedef struct {
    INT08U              Cmd;                        // 1开始，2升级中，结束，4中止，其他：未定义
    INT08U              Sts;                        // 状态
    INT16U              HwVer;                      // 硬件版本
    INT16U              SwVer;                      // 软件版本
    INT32U              FileSize;                   // 文件大小
    INT32U              RecvSize;                   // 接收文件大小
    INT32U              CurAddr;                    // 当前地址
    INT16U              CurIdx;                     // 当前帧号
    INT32U              FileChk;                    // 文件校验信息
    INT16U*             pBuf;                       // 数据缓冲区指针
    INT16U              Len;                        // 数据缓冲区长度
} IAP_PARA;

/*******************************************************************************
 * LOCAL VARIABLES
 */

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

/*******************************************************************************
* EXTERN FUNCTIONS
*/

/***********************************************
* 描述：
*/
void        IAP_DevDeInit       (void);
u8          IAP_GetAppAStatus   (void);
u8          IAP_GetBppAStatus   (void);
u32     	IAP_GetAppAddr  	(void);
u8    	    IAP_SetAppAddr  	(u32 appAddr);
u8      	IAP_JumpToApp   	(u32 flag);
u8      	IAP_JumpToAppA   	(void);
u8      	IAP_JumpToAppB   	(void);
u8          IAP_JumpToAddr      (u32 appAddr);

u8          IAP_GetStatus       (void);
void        IAP_SetStatus       (u8);

void        IAP_Reset           (void);
void        IAP_Restart         (void);
void        IAP_Finish          (void);
void        IAP_Exit            (u8 timeout);
void        IAP_Programing      (void);
BOOL        IAP_FileInfoInit    (void);
BOOL        IAP_Program        (StrIapState *iap, INT16U *buf, INT16U len, INT16U idx );
/*******************************************************************************
 * 				end of file
 *******************************************************************************/ 
#endif