/*******************************************************************************
 *   Filename:      app_tax.c
 *   Revised:       $Date: 2017-07-25$
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
#include <app_tax.h>
#include <app_chk.h>
#include <app_ctrl.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_tax__c = "$Id: $";
#endif

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
//UnionTAX  Tax;

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

/*******************************************************************************
 * 名    称： TAX_FCxx_Handler
 * 功    能： TAX_数据接收处理
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈.
 * 创建日期： 2017-07-25
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void  TAX_StartRecv (MODBUS_CH  *pch)
{
    USART_ITConfig(pch->USARTx, USART_IT_RXNE, ENABLE);
}

/*******************************************************************************
 * 名    称： TAX_FCxx_Handler
 * 功    能： TAX_数据接收处理
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈.
 * 创建日期： 2017-07-25
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void  TAX_StopRecv (MODBUS_CH  *pch, osalTime time)
{
    USART_ITConfig(pch->USARTx, USART_IT_RXNE, DISABLE);
    osal_start_timerEx( OS_TASK_ID_TMR, OS_EVT_TMR_TAX ,time);
}

/*******************************************************************************
 * 名    称： TAX_FCxx_Handler
 * 功    能： TAX_数据接收处理
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈.
 * 创建日期： 2017-07-25
 * 修    改：
 * 修改日期：
 *******************************************************************************/
CPU_BOOLEAN  TAX_FCxx_Handler (MODBUS_CH  *pch)
{
    /***********************************************
    * 描述： 先停止接收
    */
    TAX_StopRecv (pch,1000);
//    
//    /***********************************************
//    * 描述： 获取指针及数量
//    */
//    CPU_INT08U  *pBuf   = pch->RxBuf;    
//    CPU_INT16U  len     = pch->RxBufByteCnt;    
//    CPU_INT16U i;
//    
//    /***********************************************
//    * 描述： 如果数量大于72字节
//    */
//    if ( len > 72 ) {
//        /***********************************************
//        * 描述： 查找是否符合TAX2协议
//        */
//        for ( i = 0; i < len-72; i++ ) {
//            if ( ( pBuf[i] == 0x38 ) && ( pBuf[i+32] == 0x39) ) {
//                pBuf    = &pBuf[i];
//                len     = 72;
//                if ( ( pBuf[71] == GetSumXorChk((uint8_t *)&pBuf[0], (uint16_t)71) ) ||
//                   ( ( pBuf[31] == GetSumXorChk((uint8_t *)&pBuf[ 0], (uint16_t)31) ) && 
//                     ( pBuf[71] == GetSumXorChk((uint8_t *)&pBuf[32], (uint16_t)39) ) ) ) {
//                    memcpy((uint8_t *)&Ctrl.Tax.Dat.buf1[0],(uint8_t *)pBuf,len);   // 老TAX2协议
//                    goto exit;
//                }
//            }
//        }
//        /***********************************************
//        * 描述： 查的是否符合TAX3协议
//        */
//        for ( i = 0; i < len-72; i++ ) {
//            if ( ( pBuf[i] == 0xF0 ) || ( pBuf[i] == 0xFA) ) {
//                pBuf    = &pBuf[i];
//                len     = 72;
//                if ( pBuf[71] == GetSumXorChk((uint8_t *)pBuf, (uint16_t)71) ) {
//                    memcpy((uint8_t *)&Ctrl.Tax.Dat.buf1[0],(uint8_t *)pBuf,len);
//                    goto exit;
//                }
//            }
//        }
//    } else if ( len == 72 ) {
//        goto next;
//    }
//    return FALSE;
//next: 
//    CPU_INT08U  head    = pBuf[0];
//    /***********************************************
//    * 描述：
//    */
//    switch ( head ) {
//    /***********************************************
//    * 描述： TAX2015协议：=0XF0：TAX2设备；
//    */
//    case 0xFA:
//    /***********************************************
//    * 描述： TAX2015协议：=0XFA：TAX3设备
//    */
//    case 0xF0:
//        if( len == 72 ) {
//            if ( pBuf[71] == GetSumXorChk((uint8_t *)pBuf, (uint16_t)71) ) {
//                memcpy((uint8_t *)&Ctrl.Tax.Dat.buf1[0],(uint8_t *)pBuf,len);
//                goto exit;
//            }
//        }
//        break;
//    /***********************************************
//    * 描述： 老TAX2协议
//    */
//    case 0x38:
//        if ( len == 72 ) {
//            if ( ( pBuf[71] == GetSumXorChk((uint8_t *)&pBuf[0], (uint16_t)71) ) ||
//                 ( ( pBuf[31] == GetSumXorChk((uint8_t *)&pBuf[ 0], (uint16_t)31) ) && 
//                   ( pBuf[71] == GetSumXorChk((uint8_t *)&pBuf[32], (uint16_t)39) ) ) 
//                  ) {
//                memcpy((uint8_t *)&Ctrl.Tax.Dat.buf1[0],(uint8_t *)pBuf,len);   // 老TAX2协议
//                goto exit;
//            }
//        }
//        break;
//    /***********************************************
//    * 描述： 老TAX2协议
//    */
//    case 0x39:
//        if ( len == 40 ) { 
//            if ( pBuf[39] == GetSumXorChk((uint8_t *)&pBuf[ 0], (uint16_t)39) ) {
//                memcpy((uint8_t *)&Ctrl.Tax.Dat.buf1[32],(uint8_t *)pBuf,len);   // 老TAX2协议
//                goto exit;
//            }
//        }
//        break;
//    }
//
//    /***********************************************
//    * 描述： 不符合TAX协议，返回失败，把数据给后面的函数处理
//    */
//    return FALSE;
//exit:
//    /***********************************************
//    * 描述： 符合TAX协议，且处理结束，关闭接收串口中断1秒钟
//    *        当1秒定时到后，在对应的事件中开启TAX接收中断，继续接收数据
//    */
//    //TAX_StopRecv (pch);
//    
//    Ctrl.Tax.ConnectTimeOut = 0;                    // 重置接收超时计数器
//    Ctrl.Tax.ConnectFlag    = TRUE;                 // 置接收成功标志
//    
    return TRUE;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/