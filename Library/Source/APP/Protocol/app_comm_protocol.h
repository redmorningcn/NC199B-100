/*******************************************************************************
 *   Filename:       app_comm_protocol.h
 *   Revised:        $Date: 2015-12-07$
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
#ifndef	APP_COMM_PROTOCOL_H
#define	APP_COMM_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

/*******************************************************************************
 * CONSTANTS
 */
#define         FRAM_HERD0     		0x10         	//报文头
#define         FRAM_HERD1     		0x28       		//报文头				
#define     	HOST_ID      		10				//上位机地址	  		   
#define   		DEVICE_ID0      	15				//本机地址0	
#define   		DEVICE_ID1      	15				//本机地址1	
	   
#define			RS485_CHK_CRC		0x01			//b0001：CRC方式；b0010：累加和方式； b0011;累加和二进制补码方式 
#define			RS485_CHK_SUM		0x02			//b0001：CRC方式；b0010：累加和方式； b0011;累加和二进制补码方式 
#define			RS485_CHK_RESUM		0x03			//b0001：CRC方式；b0010：累加和方式； b0011;累加和二进制补码方式 

#define         FRAM_END0     		0x10         	//报文尾
#define         FRAM_END1     		0x2C       		//报文尾	

#define         PROTOCOL_VER_HIG_00 0X00            //通信协议高版本
#define         PROTOCOL_VER_HIG_01 0X01            //通信协议高版本
#define         PROTOCOL_VER_HIG_02 0X02            //通信协议高版本
#define         PROTOCOL_VER_HIG_04 0X04            //通信协议高版本

#define         PROTOCOL_VER_LOW_00 0X00            //通信协议低版本
#define         PROTOCOL_VER_LOW_01 0X01            //通信协议低版本
#define         PROTOCOL_VER_LOW_02 0X02            //通信协议低版本
#define         PROTOCOL_VER_LOW_03 0X03            //通信协议低版本
#define         PROTOCOL_VER_LOW_04 0X04            //通信协议低版本
#define         PROTOCOL_VER_LOW_05 0X05            //通信协议低版本
#define         PROTOCOL_VER_LOW_06 0X06            //通信协议低版本
#define         PROTOCOL_VER_LOW_07 0X07            //通信协议低版本
#define         PROTOCOL_VER_LOW_08 0X08            //通信协议低版本
#define         PROTOCOL_VER_LOW_09 0X09            //通信协议低版本
#define         PROTOCOL_VER_LOW_10 0X0A            //通信协议低版本 下载程序

#define         PROTOCOL_VER_HIG    PROTOCOL_VER_HIG_00            //通信协议高版本
#define         PROTOCOL_VER_LOW    PROTOCOL_VER_LOW_02            //通信协议低版本
#define         PROTOCOL_VER_PRG    PROTOCOL_VER_LOW_10            //通信协议低版本 下载程序
    
/*******************************************************************************
 * MACROS
 */
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
/***************************************************
* 描述： 帧头：长沙南睿轨道交通车辆装置异步串行通信通用通信协议
*/
__packed
typedef struct {    
    //INT16U                  Head;                               // 帖头
    INT08U                  SrcAddr;                            // 源地址
    INT08U                  DstAddr;                            // 目的地址   
    INT08U                  SendSn;                             // 报文序号
    INT08U                  RecvSn;
    union {
    struct {
    INT08U                  VerLow          : 4;                // 报文控制字： 协议版本b3~b0
    INT08U                  ChkType         : 2;                // 报文控制字： 校准方式b6~b5
    INT08U                  VerHigh         : 2;                // 报文控制字： 协议版本b7~b6
    } Udat;
    INT08U                  Dat;
    } Pack;
    INT08U                  Len;                                // 数据区长度： 12
    INT08U                 *Data;                               // 报文索引
   // INT08U                  PacketIdxChk;                     // 报文索引校验
} StrNcProtocol;

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
INT08U	            GetRecvFrameNbr      (void);
void	            SetRecvFrameNbr      (void);
INT08U	            GetSendFrameNbr      (void);
void	            SetSendFrameNbr      (void);
     
//unsigned char      CSNC_GetData        ( INT08U         *RecBuf,
//                                          INT16U	      RecLen,
//                                          StrNcProtocol   *pack);
//void	            CSNC_SendData       ( MODBUS_CH       *pch,
//                                          StrNcProtocol   *pack);

//unsigned char      CSNC_GetData        ( unsigned char *RecBuf,
//                                          unsigned short  RecLen,
//                                          unsigned char  *DataBuf,
//                                          unsigned short *InfoLen);
//
//void	            CSNC_SendData       ( MODBUS_CH *pch,
//                                          unsigned char   SourceAddr,
//                                          unsigned char   DistAddr,
//                                          unsigned char  *DataBuf,
//                                          unsigned short  DataLen);

char	            CSNC_WrDataRsp      ( MODBUS_CH *pch,
                                          unsigned char   SourceAddr,
                                          unsigned char   DistAddr,
                                          unsigned char  *DataBuf,
                                          unsigned short  DataLen);
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