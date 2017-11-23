#ifndef	BSP_CS5463_H
#define	BSP_CS5463_H
/*******************************************************************************
 *   Filename:       bsp_cs5463.h.h
 *   Revised:        Date: 2017/8/19 $
 *   Revision:       V1.0
 *   Writer:         Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *                   QQ:276193028
 *                   E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <global.h>
#include <os.h>
#include <osal.h>
#include <mb.h>
     
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
/***********************************************
* ������ 2017/8/19���Ĵ���˵��
*/
typedef cs_5463_reg {
    /***********************************************
    * ������ 2017/8/19�����üĴ��� ��ַ=0
    */
    INT32U              K               : 4;    // ʱ�ӷ�Ƶ��
    INT32U              ICPU            : 1;    // 
    INT32U              IHPF            : 1;    // 
    INT32U              VHPF            : 1;    // 
    INT32U              RS              : 1;    // 
    INT32U              DL0             : 1;    // 
    INT32U              DL1             : 1;    // 
    INT32U              EOD             : 1;    // 
    INT32U              SI              : 2;    // 
    INT32U              Res             : 1;    // 
    INT32U              EWA             : 1;    // 
    INT32U              Gi              : 1;    // 
    INT32U              PC              : 7;    // 
    INT32U              Res2            : 8;    //
    INT16U              DC_OFF_I;               // ��ַ=1��������ͨ��DCƫ�����Ĵ�����
    INT16U              K_I;                    // ��ַ�� 2 ������ͨ������Ĵ�����
    INT16U              DC_OFF_U;               // ��ַ=3������ѹͨ��DCƫ�����Ĵ�����
    INT16U              K_U;                    // ��ַ�� 4 ����ѹͨ������Ĵ�����
}
/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
 
/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif	/* BSP_CS5463_H */
