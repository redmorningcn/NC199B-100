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
 *   All copyrights reserved to Wuming Shen.  �ִ����������Ǳ��о���
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
 * 				            ϵͳ������غ궨��                                  *
 *******************************************************************************/

    
/***************************************************
* ������ ����洢�豸����
*/
#define INTNER_FLASH                        0
#define INTNER_EEPROM                       1
#define EXTERN_EEPROM                       2
#define STORAGE_TYPE                        EXTERN_EEPROM

/***************************************************
* ������ TAX
*/
#ifndef TAX_EN
#define TAX_EN                              DEF_ENABLED
#if     defined(TAX_EN)
#define TAX_II_EN                           DEF_ENABLED
#define TAX_III_EN                          DEF_ENABLED
#endif
#endif
    
/***************************************************
* ������ COMM�¼���־��
*/    
#define COMM_EVT_FLAG_HEART               (1 << 0)// ����������
#define COMM_EVT_FLAG_RESET               (1 << 1)// WIFI��λ
#define COMM_EVT_FLAG_CONNECT             (1 << 2)// WIFI����
#define COMM_EVT_FLAG_RECV                (1 << 3)// ���ڽ���
#define COMM_EVT_FLAG_REPORT              (1 << 4)// �ϱ���Ϣ
#define COMM_EVT_FLAG_CLOSE               (1 << 5)// �Ͽ�
#define COMM_EVT_FLAG_TIMEOUT             (1 << 6)// ��ʱ
#define COMM_EVT_FLAG_CONFIG              (1 << 7)// ����
#define COMM_EVT_FLAG_IAP_START           (1 << 8)// IAP��ʼ
#define COMM_EVT_FLAG_IAP_END             (1 << 9)// IAP����


/*******************************************************************************
 * 				            OSAL��ض���                                       *
 *******************************************************************************/
/*******************************************************************************
* ������ OSAL�������ȼ�����
*/
#define	OS_TASK_PRO_TMR                 (OS_TASK_PRIO_HIGHEST - 0)
#define	OS_TASK_PRO_DISP                (OS_TASK_PRIO_HIGHEST - 1)
#define	OS_TASK_PRO_LED                 (OS_TASK_PRIO_HIGHEST - 6)
#define	OS_TASK_PRO_STORE               (OS_TASK_PRIO_HIGHEST - 4)

/***************************************************
* ������ OSAL����ID����
*/
#define OS_TASK_ID_TMR                  0x00
#define OS_TASK_ID_DISP                 0x01
#define OS_TASK_ID_LED		            0x06
#define OS_TASK_ID_STORE		        0x04
    
/*******************************************************************************
* ������ OSAL�¼�����
*/
/***************************************************
* ������ OSAL�¼����壺TMR�����¼�
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
* ������ OSAL�¼�����
*/
/***************************************************
* ������ OSAL�¼����壺TMR�����¼�(20170930 ��ʱ���᲻ʱֹͣ���ͣ�����������8��)
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
* ������ OSAL�¼����壺STORE �����¼�
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
* ������ OSAL�¼����壺LED�����¼�
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
* ������ OSAL�¼����壺DISP�����¼�
*/
#define OS_EVT_DISP_TICKS                0X00000001
#define OS_EVT_DISP_INIT                 0X00000002
#define OS_EVT_DISP_2                    0X00000004
#define OS_EVT_DISP_3                    0X00000008
#define OS_EVT_DISP_4                    0X00000010
#define OS_EVT_DISP_5                    0X00000020
#define OS_EVT_DISP_6                    0X00000040
#define OS_EVT_DISP_7                    0X00000080


#define COMM_EVT_FLAG_MTR_RX                (1 << 0)	// MTR �����¼�
#define COMM_EVT_FLAG_DTU_RX                (1 << 1)	// DTU �����¼�
#define COMM_EVT_FLAG_OTR_RX                (1 << 2)	// OTR �����¼�
#define COMM_EVT_FLAG_MTR_TIMEOUT           (1 << 3)	// MTR ������ʱ����ʱ����ʹ��
#define COMM_EVT_FLAG_DTU_TIMEOUT           (1 << 4)	// DTU ������ʱ����ʱ����ʹ��
#define COMM_EVT_FLAG_OTR_TIMEOUT           (1 << 5)	// OTR ������ʱ����ʱ����ʹ��

#define COMM_EVT_FLAG_TAX_RX                (1 << 6)	// OTR �����¼�
#define COMM_EVT_FLAG_TAX_TIMEOUT           (1 << 7)	// OTR ������ʱ����ʱ����ʹ��

/*******************************************************************************
 * 				            ��������                                           *
 *******************************************************************************/
/***************************************************
* ������ ʱ�䶨��
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
* ������ ����-- ����װ�ã�MODBUSЭ�顣
����Ϊ������ֱ�ӷ���FC03�Ĵ�����ȡָ�
����������Ϊѹ�������������װ�õ� ����ֵ��
������Ҫ���͵����ݡ�
*/
typedef union {
	stcAirQuality		sAirQuality;                    //���յĿ����������װ�õ�����
	uint8				Buf[MTR_COMM_RECV_BUF_LEN];
}UniMtrCommRecvData;

//�������ݶ���
__packed
typedef struct {
    uint8   buf[10];
} StrMtrCommSendData;


//���������壬�ɶ�������ͽṹ����
typedef union {
	StrMtrCommSendData	StrSend;
	uint8				Buf[MTR_COMM_RECV_BUF_LEN];
}UniMtrCommSendData;

//����ͨӍʹ��
typedef struct {     
    INT08U      DestAddr;       //���յ�ַ      slave  =0xA1\A2	   
    INT08U      SourceAddr;     //Դ��ַ       master   =0x80	   
    INT08U      FramNum;        //֡���
    INT08U      Len;			//������Ч���ݳ���
    INT08U      FrameCode;       
    INT08U      Tmp[3];
    INT32U      DataCode;       //���տ�����
} StrDevMtrRecvCtrl;

//���ӿ�����
// 4 Bytes
typedef struct {     
    uint8       ConnFlg;        //���ӿ���,1���������ӣ�0������������
    uint8       RecvEndFlg;		//���ձ�ʾ��1�����ݽ�����ɣ�0�������ݽ��ա�
    uint8       TimeOut;		//��ʱʱ�䣬��λ1s
    uint8       ErrFlg;		    //�����ʾ������������0�����Ӵ���1
    uint8       SlaveAddr;       //���յ�ַ         slave  =0xCA	   
    uint8       MasterAddr;     //Դ��ַ           master   =0x80	   
    uint8       SendFramNum;    //֡���   
    uint8       SendFlg;        //���ͱ�ʾ�� �����ݷ��ͣ�1�������ݷ��ͣ�0
} StrDevMtrConnCtrl;		

//����Ͳ���װ��ͨѶ����
#define	COMM_DEV_MTR_CONN_NUM	2	        // �豸������A1\A2	
__packed
typedef struct {  
	UniMtrCommRecvData      Rd;				// 160 ����������
	UniMtrCommSendData      Wr;				// 160 ����������

    /***************************************************
    * ������ ���ڿ�����
    */
	StrDevMtrRecvCtrl    	RxCtrl;         // 8		���տ��ƣ�������ǰ���յ��Ŀ�����Ϣ
	StrDevMtrConnCtrl		ConnCtrl[COMM_DEV_MTR_CONN_NUM];
                                            // 4*2	���ӿ��ƣ���ÿ����ַ��Ϊ�������������ӡ�
	
    MODBUS_CH            	*pch;           // 4 	MODBUS���
    
    INT08U                  ConnectTimeOut  : 7;             // ���ӳ�ʱ����������Ϊ��λ
    INT08U                  ConnectFlag     : 1;                // ���ӱ�־
} StrDevMtr;


/***************************************************
* ������ ͳ��ģ��--���߷���ģ�飬�첽����ͨ��ͨ��ͨ��Э��
*/
// �������ݽṹ  		ͳ��-->����
__packed
typedef union {
	stcFlshRec  sRec;						//���ݼ�¼     128 	
	uint8		Buf[160];					//
} UniDtuCommSendData;


/***************************************************
* ������ ͳ��ģ��--���߷���ģ�飬��ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/

typedef struct  _stcIAPStart_
{
    uint8   DataCode;       //����ָ��
    uint8   Status;         //״̬
    uint16  HardVer;        //Ӳ���汾
    uint16  SoftVer;        //����汾
    uint32  SoftSize;       //�����С
    uint32  Addr;           //��ǰ��ַ
    uint16  CurNum;         //��ǰ���
}stcIapStart;

// �������ݽṹ		����-->ͳ��		
__packed
typedef union {
	stcTime		sTime;      //TIME_CARD     ����ʱ��            8     second,minute,hour,data,month,year,crccheck
	stcLocoId   sLocoId;    //FIX_CARD      ���û�����Ϣ	    4     type,num;
    stcIapStart IapStart;   //IAP��ʼ     
	stcRunPara	sRunPara;   //���в��� 
    stcAirPara  sAirPara;
    uint8		Buf[160];   //	
} UniDtuCommRecvData;

//���տ�����
typedef struct {     
    INT08U          DestAddr;       //���յ�ַ      slave  =0xA1\A2	   
    INT08U          SourceAddr;     //Դ��ַ       master   =0x80	   
    INT08U          FramNum;        //֡���
    INT08U			Len;			//������Ч���ݳ���
    INT08U          FrameCode;       
    INT08U          Tmp[3];
    INT32U			DataCode;			//���տ�����
} StrDevDtuRecvCtrl;

//���ӿ����֣�
typedef struct {     
    INT08U          ConnFlg;        //���ӿ���,1���������ӣ�0������������
    INT08U			RecvEndFlg;		//���ձ�ʾ��1�����ݽ�����ɣ�0�������ݽ��ա�
    INT08U			TimeOut;		//��ʱʱ�䣬��λ1s
    INT08U			ErrFlg;		    //�����ʾ������������0�����Ӵ���1

    INT08U          SlaveAddr;       //���յ�ַ      slave  =0xCA	   
    INT08U          MasterAddr;     //Դ��ַ           master   =0x80	   
    INT08U          SendFramNum;    //֡���   
    INT08U          SendFlg;        //���ͱ�ʾ�� �����ݷ��ͣ�1�������ݷ��ͣ�0
} StrDevDtuConnCtrl;		

#define	COMM_DEV_DTU_CONN_NUM	2	// �豸������CA/C1	
__packed
typedef struct {  
	UniDtuCommRecvData	Rd;			//����������
	UniDtuCommSendData	Wr;			//����������

    /***************************************************
    * ������ ���ڿ�����
    */
	StrDevDtuRecvCtrl    	RxCtrl;				            //���տ��ƣ�������ǰ���յ��Ŀ�����Ϣ
	StrDevDtuConnCtrl		ConnCtrl[COMM_DEV_DTU_CONN_NUM];//���ӿ��ƣ���ÿ����ַ��Ϊ�������������ӡ�
	
    MODBUS_CH            	*pch;                   		// MODBUS���
    
    INT08U                  ConnectTimeOut  : 7;     	// ���ӳ�ʱ����������Ϊ��λ
    INT08U                  ConnectFlag     : 1;         	// ���ӱ�־
} StrDevDtu;

__packed
typedef struct {
    /***************************************************
    * ������ ���ݽṹ��˳�����������
    */
    UniCtrlPara             Para;                               // ���Ʋ���
    
    stcFlshRec              sRec;                               //  128  ���ݼ�¼      �������вɼ����ݼ���������
    
    StrDevMtr	            Mtr;                                //  �Ͳ���ģ��ͨѶ�����֣������ա������壬���տ��ƣ����Ϳ���
    
	StrDevDtu	            Dtu;						        //  �����߷���ģ��ͨѶ�����֣������ա������壬	���տ��ƣ����Ϳ���

    StrCtrlOS               Os;                                 // OS����        
  
} stcCtrlDCB;

extern stcCtrlDCB           Ctrl;

/*******************************************************************************
 * 				                  ����                                         *
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/***************************************************
* ������ �ṹ������
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
* ������ Ӧ�ò㺯��������
*/
BOOL        App_PraInit                 (void);
BOOL        App_GetParaFromEeprom       (void);
BOOL        App_SetParaToEeprom         (void);
void        App_Error                   (void);
void        App_FanHandCtrl             (INT08U step);
void        App_FanAutoCtrl             (INT08U step);
void        App_ParaFilter              (void);

/***************************************************
* ������ OSAL�����ʼ������������
*/
void        TaskInitBeep                ( void );
void        TaskInitKey                 ( void );
void        TaskInitCtrl                ( void );
void        TaskInitTmr                 ( void );
void        TaskInitDisp                ( void );
void        TaskInitLed                 ( void );
void        TaskInitStore               ( void );


/***************************************************
* ������ OSAL�����¼�������������
*/
osalEvt     TaskBeepEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskKeyEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskCtrlEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskTmrEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskDispEvtProcess          (osalTid task_id, osalEvt task_event);
osalEvt     TaskLedEvtProcess           (osalTid task_id, osalEvt task_event);
osalEvt     TaskStoreEvtProcess         (osalTid task_id, osalEvt task_event);



/***************************************************
* ������ AES�Ӽ��ܺ���������
*/
/***************************************************
* ������ ���Ź�����������
*/
void        App_MsgWriteCmd             ( INT16U cmd );
/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif	/* GLOBLES_H */