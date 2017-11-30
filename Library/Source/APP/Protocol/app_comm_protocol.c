/*******************************************************************************
 *   Filename:       app_lib.c
 *   Revised:        All copyrights reserved to Wuming Shen.
 *   Date:           2014-07-05
 *   Revision:       v1.0
 *   Writer:	     Wuming Shen.
 *
 *   Description:    ˫��ѡ�� wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� WDT �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ����������ȼ� �� �����ջ��С
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <app.h>
#include <app_chk.h>
#include <bsp_usart.h>

#define BSP_COMM_PROTOCOL_MODULE_EN 1
#if BSP_COMM_PROTOCOL_MODULE_EN > 0

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_comm_protocol__c = "$Id: $";
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
 
unsigned char   l_recslaveaddr = 0;

/***********************************************
* ������ 
*/
unsigned char GetRecSlaveAddr(void)
{
	return	l_recslaveaddr;
}

static INT08U	RecvFrameNbr = 0;
/***********************************************
* ������ 
*/
INT08U	GetRecvFrameNbr(void)
{
	return 	RecvFrameNbr;
}


/***********************************************
* ������ 
*/
void	SetRecvFrameNbr(void)
{
	RecvFrameNbr++;		
}

/*******************************************************************************
* ��    �ƣ� CSNC_GetData
* ��    �ܣ� 
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//unsigned char  CSNC_GetData(	INT08U *RecvBuf,
//								INT16U	RecLen,
//								INT08U *DataBuf,
//								INT16U *InfoLen)
unsigned char  CSNC_GetData(	INT08U *RecvBuf,
								INT16U	RecvLen,
                                StrNcProtocol   *pack)
{
    unsigned short		j,k;
    unsigned char		SumCheck8;
    unsigned char		Tmp;
    unsigned short		RecCRC16,Check16;
		
	unsigned short		DataLen          = pack->Len;
    unsigned char     *pDat             = RecvBuf;    
    unsigned short    MaxDataLen        = RecvLen - 2;
    
    //pack->Len            = 0;
    /***********************************************
    * ������ ʶ��֡ͷ
    */
//    for(i = 0; i < MaxDataLen; i++)	{		        //ѭ������
//		if(	( (pack->Data[i + 0] ) == FRAM_HERD0 ) && 
//            ( (pack->Data[i + 1] ) == FRAM_HERD1 ) ) {
//            pDat    = &pack->Data[i];
//            goto next;
//        }
//    }
//    return 0;
    /***********************************************
    * ������ ��������ȥ��֡ͷʶ����0X10���ظ�����
    */
	for(j = 0;j < RecvLen ;j++ )	{				    //ȥ�ظ�ͨѶ���ֽ�
        if( ( pDat[j]   == FRAM_HERD0 ) &&
            ( pDat[j+1] == FRAM_HERD0 ) ) {
            for(k = j; k < RecvLen; k++) {
               Tmp	        = pDat[k+1];
               pDat[k]      = Tmp;
            }
        }
	}
    pDat    += 6;
    DataLen  = *pDat;
    // ���ȳ���Χ
    if ( DataLen > MaxDataLen )
        return 0;
    
    pDat    += DataLen + 4;
    /***********************************************
    * ������ ���֡β
    */
//    if( ( pDat[0] 	!= FRAM_END0)	||
//        ( pDat[1] 	!= FRAM_END1) ) {
//        return 0;
//    }
    pDat            = RecvBuf;
    SumCheck8 		= GetCheckSumNR((unsigned char *)&pDat[2],5);	//������У��
    RecCRC16 		= ((unsigned short)pDat[8+DataLen])
                    * 256 + pDat[9+DataLen];	        //У��	
               
    switch((pDat[5]>>4) & 0x03) {							    //У����㷨�ж�
    case	RS485_CHK_SUM:
        break;
    case	RS485_CHK_CRC:
       Check16 = GetCrc16Chk1(&pDat[2],6+DataLen);
        break;
    case	RS485_CHK_RESUM:
        break;
    }
    
    if(	(SumCheck8 == 	pDat[7] ) && 
        (RecCRC16  ==	Check16 ) ) {
        /***********************************************
        * ������ ��ȡ֡ͷ
        */
        pack->SrcAddr 	= pDat[2];
        pack->DstAddr   = pDat[3];
        pack->RecvSn   	= pDat[4];
        pack->Pack.Dat 	= pDat[5];
        pack->Len 		= pDat[6]; 
        pack->Data      =&pDat[8];
        return		pack->DstAddr;
    }
               
    return	0;
}

static INT08U	SendFrameNbr = 0;

/***********************************************
* ������ 
*/
INT08U	GetSendFrameNbr(void)
{
	return SendFrameNbr;		
}

/***********************************************
* ������ 
*/
void	SetSendFrameNbr(void)
{
	SendFrameNbr++;		
}

/*******************************************************************************
* ��    �ƣ� CSNC_SendData
* ��    �ܣ� 
* ��ڲ����� ��
* ���ڲ����� ��
* ��  ���ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//void	CSNC_SendData(	MODBUS_CH *pch,
//                        INT08U  SourceAddr,
//						INT08U  DistAddr,
//						INT08U *DataBuf,
//						INT16U  DataLen)
//void	CSNC_SendData(	MODBUS_CH       *pch,
//                        StrNcProtocol   *pack)
//{
//	unsigned char	SndBuf[250] = {0};
//	unsigned char	SndBufTmp[250] = {0};
//	unsigned char	SumCheck8;
//	unsigned short	Crc16;
//	unsigned short	AddHeadNum;
//	unsigned short	i;
//	unsigned char	ByteStation;
//    
//	ByteStation = 0;
//	SndBuf[ByteStation++] = FRAM_HERD0;
//	SndBuf[ByteStation++] = FRAM_HERD1;
//	SndBuf[ByteStation++] = pack->SrcAddr;  //SourceAddr;	 
//	SndBuf[ByteStation++] = pack->DstAddr;  //DistAddr;				    // ��ַ
//	SndBuf[ByteStation++] = pack->SendSn;       //SendFrameNbr;               // SendFrameNbr++;
//	SndBuf[ByteStation++] = pack->Pack.Dat;//(RS485_CHK_CRC<<4) | (PROTOCOL_VER_HIG << 6) | (PROTOCOL_VER_LOW << 0);
//	SndBuf[ByteStation++] = pack->Len;      //DataLen;
//	
//	SumCheck8 = GetCheckSumNR(&SndBuf[2],5);		   	// ������У��
//
//	SndBuf[ByteStation++] = SumCheck8;
//
//	//memcpy(&SndBuf[8],DataBuf,DataLen);
//	memcpy(&SndBuf[8],pack->Data,pack->Len);
//
//	Crc16 = GetCrc16Chk1(&SndBuf[2],6+ pack->Len);
//
//	SndBuf[2+6+pack->Len+1] = Crc16;
//	SndBuf[2+6+pack->Len]   = Crc16>>8;
//
//	SndBufTmp[0] = SndBuf[0];						    // ׼�����ݷ���
//	SndBufTmp[1] = SndBuf[1];
//	
//	AddHeadNum = 0;
//	for(i = 2; i< 2+6+pack->Len + 2;i++ )	{				// ���ݣ�����λ FRAM_HERD0
//		SndBufTmp[i+AddHeadNum] = SndBuf[i];
//		
//		if(SndBuf[i] == FRAM_HERD0) {
//			SndBufTmp[i+AddHeadNum+1] = FRAM_HERD0;
//			AddHeadNum++;
//		}
//	}
//
//	SndBufTmp[2+6+pack->Len + 2 +AddHeadNum] = FRAM_END0;
//	SndBufTmp[2+6+pack->Len + 2 +AddHeadNum+1] = FRAM_END1;      
//    
//    CPU_INT16U nbr_bytes= (2+6+pack->Len + 2 +AddHeadNum +2);
//    if ( nbr_bytes < MB_DATA_NBR_REGS * 2 ) {  
//        NMB_Tx(pch,
//               (CPU_INT08U  *)SndBufTmp,
//               (CPU_INT16U   )nbr_bytes);
//    }
//}


static  unsigned char	SndBuf[250] = {0};
static  unsigned char	SndBufTmp[250] = {0};
/*******************************************************************************
* ��    �ƣ� CSNC_SendData
* ��    �ܣ� 
* ��ڲ����� ��
* ���ڲ����� ��
* ��  ���ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void	CSNC_SendData(	MODBUS_CH *pch,
                        INT08U  SourceAddr,
						INT08U  DistAddr,
                        INT08U  SendFrameNum,
						INT08U  SendCode,                       
						INT08U *DataBuf,
						INT16U  DataLen)
{
	unsigned char	    SumCheck8;
	unsigned short	Crc16;
	unsigned short	AddHeadNum;
	unsigned short	i;
	unsigned char	    ByteStation;	
    
	ByteStation = 0;
	SndBuf[ByteStation++] = FRAM_HERD0;
	SndBuf[ByteStation++] = FRAM_HERD1;
	SndBuf[ByteStation++] = SourceAddr;	 
	SndBuf[ByteStation++] = DistAddr;				    // ��ַ
	SndBuf[ByteStation++] = SendFrameNum;               // ����֡���

	SndBuf[ByteStation++] =  ( RS485_CHK_CRC<<4 )+ SendCode;// ���Ϳ�����	
	SndBuf[ByteStation++] = DataLen;
	
	SumCheck8 = GetCheckSumNR(&SndBuf[2],5);		   	// ������У��

	SndBuf[ByteStation++] = SumCheck8;

	memcpy(&SndBuf[8],DataBuf,DataLen);

	Crc16 = GetCrc16Check(&SndBuf[2],6+DataLen);

	SndBuf[2+6+DataLen+1] = Crc16;
	SndBuf[2+6+DataLen]   = Crc16>>8;

	SndBufTmp[0] = SndBuf[0];						    // ׼�����ݷ���
	SndBufTmp[1] = SndBuf[1];
	
	AddHeadNum = 0;
	for(i = 2; i< 2+6+DataLen + 2 + AddHeadNum;i++ )	{				// ���ݣ�����λ FRAM_HERD0
	
		SndBufTmp[i+AddHeadNum] = SndBuf[i];
		
		if(SndBuf[i] == FRAM_HERD0) {
			SndBufTmp[i+AddHeadNum+1] = FRAM_HERD0;
			AddHeadNum++;
		}
	}

	SndBufTmp[2+6+DataLen + 2 +AddHeadNum] = FRAM_END0;
	SndBufTmp[2+6+DataLen + 2 +AddHeadNum+1] = FRAM_END1;      
    
    CPU_INT16U nbr_bytes= (2+6+DataLen + 2 +AddHeadNum +2);
    if ( nbr_bytes < MB_DATA_NBR_REGS * 2 ) {  
        //�����ڷǽ���״̬���������ݡ�
        if (pch->RxBufByteCtr == 0) 
        {                
            NMB_Tx(pch,
                   (CPU_INT08U  *)SndBufTmp,
                   (CPU_INT16U   )nbr_bytes);
        }
        else
        {
            return;
        }
    }
}


/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif