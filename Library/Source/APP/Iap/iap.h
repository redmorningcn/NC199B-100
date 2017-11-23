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
* ������ 2017/8/19��IAP ʹ��
*/
#define IAP_EN                  DEF_ABLED
     
#define KB						*1024
/***********************************************
* ������ 2017/8/19������FLSAH��С
*/
#define	FLASH_SIZE				(256KB)											// FLASH��С
/***********************************************
* ������ 2017/8/19��FLASH����
*/
#define	FLASH_SIZE_BOOT			(80KB)											// BOOT����С
#define	FLASH_SIZE_EEP			(16KB)											// ��������С

#define	FLASH_SIZE_A			(FLASH_SIZE-FLASH_SIZE_BOOT-FLASH_SIZE_EEP)/2	// ����A��С
#define	FLASH_SIZE_B			(FLASH_SIZE-FLASH_SIZE_BOOT-FLASH_SIZE_EEP)/2	// ����B��С
/***********************************************
* ������ 2017/8/19����ַ����
*/
#define FLASH_ADDR_BOOT			0x8000000  								        // ����������ʼ��ַ(�����FLASH)
#define FLASH_ADDR_APP_A		(0x8000000 + FLASH_SIZE_BOOT)					// ��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
#define FLASH_ADDR_APP_B		(0x8000000 + FLASH_SIZE_BOOT + FLASH_SIZE_A)  	// �ڶ���Ӧ�ó�����ʼ��ַ(�����FLASH)
#define FLASH_ADDR_APP			FLASH_ADDR_BOOT  								// Ӧ�ó�����ʼ��ַ(�����FLASH)
#define FLASH_ADDR_DATA			(FLASH_SIZE-FLASH_SIZE_EEP)  					// Ӧ�ó�����ʼ��ַ(�����FLASH)


#define FLASH_APP_ADDR_SAVE		(FLASH_SIZE_EEP - 4)  	                        // �����´����еĳ����ַ
#define FLASH_APP_STATUS		(FLASH_SIZE_EEP - 5)  	                        // ��������״̬

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
* ������ 2017/8/19��IAP������Ϣ
*/
typedef  enum  iap_err {
    IAP_ERR_NONE                =     0u,
} IAP_ERR;
/***********************************************
* ������ 2017/8/19��IAP��ַ
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
* ������ 2017/8/19��IAP��������
*/
typedef  enum  iap_cmd {
    IAP_CMD_START               = 0X01,
    IAP_CMD_PRG                 = 0X02,
    IAP_CMD_FINISH              = 0X03,
    IAP_CMD_STOP                = 0X04,
} IAP_CMD;
/***********************************************
* ������ 2017/8/19��IAP����״̬
*/
typedef  enum  iap_sr_start {
    IAP_SR_START_START          = 0X01,
    IAP_SR_START_CONTINUE       = 0X02,
} IAP_SR_START;
/***********************************************
* ������ 2017/8/19��IAP��ʼӦ��
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
* ������ 2017/8/19��IAP����״̬
*/
typedef  enum  iap_sr_finish {
    IAP_SR_FINISH_NO_RUN        = 0X00,
    IAP_SR_FINISH_RUN           = 0X01,
    IAP_SR_FINISH_RESTART       = 0X02,
} IAP_SR_FINISH;
/***********************************************
* ������ 2017/8/19��IAP��ֹ״̬
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
    INT32U      AppAddr;                            // Ӧ�ó����ַ    INT08U      Status              : 4;            // // 0δ������1��ʼ��2�����У�3ʧ�ܣ�4�ɹ����������޳���
    INT08U      Status;                             // 0δ������1��ʼ��2�����У�3ʧ�ܣ�4�ɹ����������޳���
    INT08U      Step;
    INT16U      FrameIdx;                           // ���յ�����֡���
    INT16U      SysNbr;                             // ͬ�����к�
    INT16U      TimeOut;                            // ��ʱ�˳�IAP
    
    INT32U      FileSize;                           // �ļ���С
    INT32U      WrittenSize;                        // ��д���ļ���С
    INT32U      SectorAddr;                         // ��ǰд���������ַ
    INT32U      SectorAddrLast;                     // �ϴ�д���������ַ
    INT32U      Sectors;                            // ��������
    INT08U      WriteCtr;                           // 16֡128�ֽ����һ��2K��������
    INT16U     *pBuf;                               // ���ݻ�����ָ��
    
    
    INT16U      HwVer;                              // Ӳ���汾
    INT16U      SwVer;                              // ����汾
    INT32U      PrgSize;                            // �����С
    INT32U      CurAddr;                            // ��ǰ��ַ
    INT16U      CurIdx;                             // ��ǰ֡��
    
} StrIapState;

extern StrIapState Iap;

__packed
typedef struct {
    INT08U              Cmd;                        // 1��ʼ��2�����У�������4��ֹ��������δ����
    INT08U              Sts;                        // ״̬
    INT16U              HwVer;                      // Ӳ���汾
    INT16U              SwVer;                      // ����汾
    INT32U              FileSize;                   // �ļ���С
    INT32U              RecvSize;                   // �����ļ���С
    INT32U              CurAddr;                    // ��ǰ��ַ
    INT16U              CurIdx;                     // ��ǰ֡��
    INT32U              FileChk;                    // �ļ�У����Ϣ
    INT16U*             pBuf;                       // ���ݻ�����ָ��
    INT16U              Len;                        // ���ݻ���������
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
* ������
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