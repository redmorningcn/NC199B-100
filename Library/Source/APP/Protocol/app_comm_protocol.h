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
#define         FRAM_HERD0     		0x10         	//����ͷ
#define         FRAM_HERD1     		0x28       		//����ͷ				
#define     	HOST_ID      		10				//��λ����ַ	  		   
#define   		DEVICE_ID0      	15				//������ַ0	
#define   		DEVICE_ID1      	15				//������ַ1	
	   
#define			RS485_CHK_CRC		0x01			//b0001��CRC��ʽ��b0010���ۼӺͷ�ʽ�� b0011;�ۼӺͶ����Ʋ��뷽ʽ 
#define			RS485_CHK_SUM		0x02			//b0001��CRC��ʽ��b0010���ۼӺͷ�ʽ�� b0011;�ۼӺͶ����Ʋ��뷽ʽ 
#define			RS485_CHK_RESUM		0x03			//b0001��CRC��ʽ��b0010���ۼӺͷ�ʽ�� b0011;�ۼӺͶ����Ʋ��뷽ʽ 

#define         FRAM_END0     		0x10         	//����β
#define         FRAM_END1     		0x2C       		//����β	

#define         PROTOCOL_VER_HIG_00 0X00            //ͨ��Э��߰汾
#define         PROTOCOL_VER_HIG_01 0X01            //ͨ��Э��߰汾
#define         PROTOCOL_VER_HIG_02 0X02            //ͨ��Э��߰汾
#define         PROTOCOL_VER_HIG_04 0X04            //ͨ��Э��߰汾

#define         PROTOCOL_VER_LOW_00 0X00            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_01 0X01            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_02 0X02            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_03 0X03            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_04 0X04            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_05 0X05            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_06 0X06            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_07 0X07            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_08 0X08            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_09 0X09            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_LOW_10 0X0A            //ͨ��Э��Ͱ汾 ���س���

#define         PROTOCOL_VER_HIG    PROTOCOL_VER_HIG_00            //ͨ��Э��߰汾
#define         PROTOCOL_VER_LOW    PROTOCOL_VER_LOW_02            //ͨ��Э��Ͱ汾
#define         PROTOCOL_VER_PRG    PROTOCOL_VER_LOW_10            //ͨ��Э��Ͱ汾 ���س���
    
/*******************************************************************************
 * MACROS
 */
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
/***************************************************
* ������ ֡ͷ����ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/
__packed
typedef struct {    
    //INT16U                  Head;                               // ��ͷ
    INT08U                  SrcAddr;                            // Դ��ַ
    INT08U                  DstAddr;                            // Ŀ�ĵ�ַ   
    INT08U                  SendSn;                             // �������
    INT08U                  RecvSn;
    union {
    struct {
    INT08U                  VerLow          : 4;                // ���Ŀ����֣� Э��汾b3~b0
    INT08U                  ChkType         : 2;                // ���Ŀ����֣� У׼��ʽb6~b5
    INT08U                  VerHigh         : 2;                // ���Ŀ����֣� Э��汾b7~b6
    } Udat;
    INT08U                  Dat;
    } Pack;
    INT08U                  Len;                                // ���������ȣ� 12
    INT08U                 *Data;                               // ��������
   // INT08U                  PacketIdxChk;                     // ��������У��
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
 * ��������         ��������                ��ڲ���
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