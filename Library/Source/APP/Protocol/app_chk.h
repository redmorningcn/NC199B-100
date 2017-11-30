/*******************************************************************************
 *   Filename:       app_chk.h
 *   Revised:        $Date: 2017-07-26$
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/
#ifndef	APP_CHK_H
#define	APP_CHK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
    
/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

/*******************************************************************************
 * CONSTANTS
 */     
     
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
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *
 * 返回类型         函数名称                入口参数
 */
 uint16_t           GetCrc16Chk1            (uint8_t *buf, uint16_t len);
 uint16_t           GetCrc16Chk2            (uint8_t *buf, uint16_t len);
 uint16_t           GetMBCrc16Chk           (uint8_t *buf, uint16_t len);
 uint16_t           GetMBCrc16Up            (uint8_t *buf, uint16_t len);
 uint8_t            GetSumChk               (uint8_t *buf, uint16_t len);
 uint8_t            GetSumXorChk            (uint8_t *buf, uint16_t len);
 uint16_t           GetSum16Chk             (uint8_t *buf, uint16_t len);
 uint16_t           GetSum16XorChk          (uint8_t *buf, uint16_t len);
 uint16_t           GetCrc16Check           (uint8_t *buf, uint16_t len);
 INT08U             GetCheckSumNR           (INT08U  Buf[], INT32U Len);
INT08U              GetCheckSum             (INT08U  Buf[], INT32U Len);

 
/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif