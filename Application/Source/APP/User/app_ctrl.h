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
#include <bsp_key.h>
#include <bsp_max7219.h>
#include <protocol.h>
#include <os.h>
#include <osal.h>
#include <mb.h>

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
* ������ RFӦ������֡���л���
*/
#define AppMsgQ                            10
#define AppMsgQSize                        10
 
/***************************************************
* ������ ����洢�豸����
*/
#define INTNER_FLASH                        0
#define INTNER_EEPROM                       1
#define EXTERN_EEPROM                       2
#define STORAGE_TYPE                        EXTERN_EEPROM

/***************************************************
* ������ WIFI�¼���־��
*/    
#define WIFI_EVT_FLAG_HEART               (1 << 0)// ����������
#define WIFI_EVT_FLAG_RESET               (1 << 1)// WIFI��λ
#define WIFI_EVT_FLAG_CONNECT             (1 << 2)// WIFI����
#define WIFI_EVT_FLAG_RECV                (1 << 3)// ���ڽ���
#define WIFI_EVT_FLAG_REPORT              (1 << 4)// �ϱ���Ϣ
#define WIFI_EVT_FLAG_CLOSE               (1 << 5)// �Ͽ�
#define WIFI_EVT_FLAG_TIMEOUT             (1 << 6)// ��ʱ
#define WIFI_EVT_FLAG_CONFIG              (1 << 7)// ����

#define COMM_EVT_FLAG_HEART               (1 << 0)// ����������
#define COMM_EVT_FLAG_RESET               (1 << 1)// WIFI��λ
#define COMM_EVT_FLAG_CONNECT             (1 << 2)// WIFI����
#define COMM_EVT_FLAG_RECV                (1 << 3)// ���ڽ���
#define COMM_EVT_FLAG_REPORT              (1 << 4)// �ϱ���Ϣ
#define COMM_EVT_FLAG_CLOSE               (1 << 5)// �Ͽ�
#define COMM_EVT_FLAG_TIMEOUT             (1 << 6)// ��ʱ
#define COMM_EVT_FLAG_CONFIG              (1 << 7)// ����
 
/*******************************************************************************
 * 				            OSAL��ض���                                       *
 *******************************************************************************/
/*******************************************************************************
* ������ OSAL�������ȼ�����
*/
#define	OS_TASK_PRO_BEEP                OS_TASK_PRIO_HIGHEST - 0
#define	OS_TASK_PRO_KEY                 OS_TASK_PRIO_HIGHEST - 1
#define	OS_TASK_PRO_CTRL                OS_TASK_PRIO_HIGHEST - 0

#define	OS_TASK_PRO_SEN                 OS_TASK_PRIO_HIGHEST - 0
#define	OS_TASK_PRO_DISP                OS_TASK_PRIO_HIGHEST - 1
/***************************************************
* ������ OSAL����ID����
*/
#define OS_TASK_ID_BEEP                 0x00
#define OS_TASK_ID_KEY                  0x01
#define OS_TASK_ID_CTRL                 0x00

#define OS_TASK_ID_SEN                  0x00
#define OS_TASK_ID_DISP                 0x01
/*******************************************************************************
* ������ OSAL�¼�����
*/
/***************************************************
* ������ OSAL�¼����壺��������
*/
#define OS_EVT_CTRL_TICK                0X00000001
#define OS_EVT_CTRL_FAN                 0X00000002
#define OS_EVT_CTRL_LED                 0X00000004
#define OS_EVT_CTRL_WIFI_CFG            0X00000008
#define OS_EVT_CTRL_WIFI_TMO            0X00000010
#define OS_EVT_CTRL_FILTER              0X00000020
#define OS_EVT_CTRL_TIMING_ON_OFF       0X00000040
#define OS_EVT_CTRL_AIR_VOLUME          0X00000080
/***************************************************
* ������ OSAL�¼����壺SENSOR�����¼�
*/
#define OS_EVT_SEN_TICKS                0X00000001
#define OS_EVT_SEN_MSEC                 0X00000002
#define OS_EVT_SEN_SEC                  0X00000004
#define OS_EVT_SEN_MIN                  0X00000008
#define OS_EVT_SEN_DEAL                 0X00000010
#define OS_EVT_SEN_UPDATE_DENSITY       0X00000020
#define OS_EVT_SEN_6                    0X00000040
#define OS_EVT_SEN_ERR_CHK              0X00000080
     
#define OS_EVT_SEN_SAMPLE               0X00000001
#define OS_EVT_SEN_PM2D5_CHK            0X00000002
#define OS_EVT_SEN_CHK_TIMEOUT          0X00000004
#define OS_EVT_SEN_UART_TIMEOUT         0X00000008
#define OS_EVT_SEN_4                    0X00000010
#define OS_EVT_SEN_5                    0X00000020
#define OS_EVT_SEN_6                    0X00000040
#define OS_EVT_SEN_7                    0X00000080
/***************************************************
* ������ OSAL�¼����壺LED�����¼�
*/
#define OS_EVT_LED_0                    0X00000001
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
#define OS_EVT_DISP_1                    0X00000002
#define OS_EVT_DISP_2                    0X00000004
#define OS_EVT_DISP_3                    0X00000008
#define OS_EVT_DISP_4                    0X00000010
#define OS_EVT_DISP_5                    0X00000020
#define OS_EVT_DISP_6                    0X00000040
#define OS_EVT_DISP_7                    0X00000080
/*******************************************************************************
 * 				            ��������                                           *
 *******************************************************************************/
/***************************************************
* ������ ʱ�䶨��
*/
#define FILTER_LIFE_SAVE_TIME           30          // ����
#define FILTER_LIFE_TIME               (3*30*24*60/1)// ����(3����)
//#define FILTER_LIFE_SAVE_TIME           1          // ����
//#define FILTER_LIFE_TIME               (60/FILTER_LIFE_SAVE_TIME)        // ����(24Сʱ��������)

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
typedef struct  {    
    CPU_FP32            I1;                         // ����1  uAΪ��λ
    CPU_FP32            I2;                         // ����2
    CPU_FP32            Adc1;                       // ADC1
    CPU_FP32            Adc2;                       // ADC2
    CPU_FP32            K;                          // б��   �Ŵ�10000��
    CPU_FP32            B;                          // ƫ��
} StrAdcToIBoardChk;

__packed
typedef struct  {
    CPU_FP32              P1;                       // ѹ��1  PaΪ��λ
    CPU_FP32              P2;                       // ѹ��2
    CPU_FP32              I1;                       // ����1  uAΪ��λ
    CPU_FP32              I2;                       // ����2
    CPU_FP32              K;                        // б��
    CPU_FP32              B;                        // ƫ��
} StrIToPSenChk;
/***************************************************
* ������ ����������
*/
__packed
typedef struct {                                   // ״̬	        1	
    INT08U                  HSenBrokenErr           : 1;        //              Bit1����λ�������쳣
    INT08U                  HSenExitDetection       : 1;        //              Bit3��Һλ���ͣ���λ�������˳����
    INT08U                  HSenShortErr            : 1;        //              Bit7��Һλ���ߣ��򴫸�����·
    INT08U                  PwrDownEvt              : 1;        //              Bit4���е����¼�����ȷ���պ󣬸�0��
    INT08U                  LSenBrokenErr           : 1;        //              Bit0����λ�������쳣
    INT08U                  LSenExitDetection       : 1;        //              Bit2��Һλ���ͣ���λ�������˳����
    INT08U                  LSenShortErr            : 1;        //              Bit6��Һλ���ߣ��򴫸�����·
    INT08U                  WorkStsReq              : 1;        //              Bit5�����������״̬��Ϣ���ٶȡ����ٶȡ�ǣ�����ƶ�����λ����
} StrSenErr;

/*************************************************
* ������ ����ģ�ͱ�
*/
__packed
 typedef struct {
    INT16U  Hight;                                  // �߶�
    INT16U  Volume;                                 // ����
 } _StrOilPoint;

__packed
 typedef struct {
    INT16S                  Offset;                 // �߶�ƫ��
    INT16U                  Cnts;                   // ģ�͵���,14
    _StrOilPoint            Point[10];              // �߶�,����
 } StrOilVolumeTab;
 
__packed
 typedef union {
    StrOilVolumeTab         Tab;                    // ����ģ��    
    INT08U                  buf1[44];
    INT16U                  buf2[22];
    INT32U                  buf3[11];
 } StrOilTab;
 
/***************************************************
* ������ ��������������256 + 4 = 260�ֽ�
*/
__packed
typedef struct _StrCtrlPara {
    /***************************************************
    * ������ ϵͳ��������ʼ��ַ = 000
    */    
    INT16U              Password;                       // ����MODEBUSͨѶȷ����Կ��Ĭ��Ϊ6237����ֹ�Ƿ�������
    /***************************************************
    * ������ �ܶȣ���ʼ��ַ = 002
    */
    CPU_FP32            LastDensity;                    // �ϴβ����ܶ�
    
    /***************************************************
    * ������ ƫ�ã���ʼ��ַ = 006
    */
    INT16S              Offset;
    /***************************************************
    * ������ Ԥ������ʼ��ַ = 008
    */     
    CPU_FP32            LastGravity;                    // �ϴβ����������ٶ�
    
    CPU_FP32            SetDensity;                     // �����ܶ�
    CPU_FP32            SetGravity;                     // �����������ٶ�   
    
    //INT16U              Rsv1[3];
    /***************************************************
    * ������ У׼��������ʼ��ַ = 020
    */
    StrAdcToIBoardChk   Check[2]; 
    /***************************************************
    * ������ У׼��������ʼ��ַ = 068
    */
    StrIToPSenChk       SenChk[2];    
    /***************************************************
    * ������ У׼��������ʼ��ַ = 116
    */
    union __uSENSOR_CHECK_TYPE {
        struct __sSENSOR_CHECK_TYPE {
            INT08U      RstFlag             : 1;        // ��λ��־,��־λλ�úͼĴ���λ�ò������������MB_DATA.C�еĸ�λҲҪ���ŵ���
            INT08U      Test                : 1;        // ���Ա�־
            INT08U      Rsv                 : 6;        //
            StrSenErr   Err;                            // �����־
        }Udat;
        INT08U Dat[2];
    } SenSts;
    /***************************************************
    * ������ У׼��������ʼ��ַ = 116
    */
    union __uSEL_TYPE {
        struct __sSEL_TYPE {
            INT16U      FilterSel           : 4;        // �������˲�����ѡ��0~4
            INT16U      SensorSel           : 2;        // ������ѡ��:0�͡�1�ߡ�2ƽ����3����
            INT16U      DensitySel          : 2;        // �ܶ�ѡ��0�趨��1������2�趨����ƽ��ֵ��3����
            INT16U      GravitySel          : 2;        // �������ٶ�ѡ��0�趨��1������2�趨����ƽ����3����
            INT16U      CalcSel             : 2;        // ���㷽��ѡ��0A-I-P��1A-P
            INT16U      DispSel             : 2;        // ��ʾģʽ��0������ʾ�������Ͽ���ʾ�߶ȣ�1��ʾ�߶ȣ�2��������ʾ 3���ر���ʾ
            INT16U      Rsv                 : 2;        // Ԥ��
        }udat;
        INT16U dat;
    } Sel;
    
    /***************************************************
    * ������ �û���������ʼ��ַ = 118
    */
    StrOilTab   Vtab;
    /***************************************************
    * ������ �û���������ʼ��ַ = 118
    */
    INT16S              RSV[44];                        // ����13������
    /***********************************************
    * ������״̬�� ����ʼ��ַ = 252
    */
    union __sSYSTEM_STATUS {
        struct __uSYSTEM_STATUS {
            uint16_t  AbnormalPowerReset    : 1;        // �쳣�����־
            uint16_t  App_Fault_ISR         : 1;        // �쳣�ж�
            uint16_t  ResetFlag             : 1;        // ������־λ
            uint16_t  rsv1                  : 1;        // 
            
            uint16_t  ModuleType            : 2;        // ģ������
            uint16_t  Channels              : 2;        // ͨ����
            
            INT16U    FirstPwrFlag          : 1;        // �״��ϵ��־λ
            INT16U    SenChkFlag            : 1;        // PM2D5У׼��־λ
            uint16_t  RSV2                  : 6;        //
        }udat;
        INT16S dat;
    } SysSts;

    /***************************************************
    * ������ �ָ��������ã���ʼ��ַ = 254
    */
    INT16S          RestoreSetting;                     // �ָ���������

    const struct _StrCtrlPara *idx;                    // ���ָ�룬ռ4�ֽ�

} StrCtrlPara;

/***************************************************
* ������
*/
__packed
typedef struct {
    INT08U                 *pEdit;                      // �༭����ָ��
    INT08U                 *pMax;                       // �༭���������ֵָ��
    INT08U                 *pMin;                       // �༭��������Сֵָ��
    INT08U                 *pStart;                     // �༭����ʼ����
    INT08U                 *pEnd;                       // �༭�Ľ�������
    INT08U                  EditIdx;                    // �༭��Ŀ���
    INT08U                  Edit;                       // �༭����
} StrEdit;

__packed
typedef struct {
    INT08U                  Mode;                       // �����޸ı�־
    StrLedDisp             *Led;
} StrDisp;

__packed
typedef struct {
    INT08U                 *p08;
    INT16U                 *p16;
    INT32U                 *p32;
    INT16U                  len;
} NOTIFY_EEP_SAVE_TYPE;

__packed
typedef struct {
    OS_FLAG_GRP             WifiEvtFlagGrp;
    OS_FLAGS                WifiEvtFlag;
    OS_FLAG_GRP             CommEvtFlagGrp;
    OS_FLAGS                CommEvtFlag;
} StrCtrlOS;


__packed
typedef struct {    
    INT08U                  SysSts                  : 4;        // ϵͳ״̬
    INT08U                  WifiSts                 : 4;        // WIFI״̬
        
    INT16U                  FirstPwrFlag            : 1;        // �״��ϵ��־λ
    INT16U                  PM2D5ChkFlag            : 1;        // PM2D5У׼��־λ
    INT16U                  RsvFlag                 :14;        // ����    
} StrSystem;

/***************************************************
* ������ ֡ͷ����ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/
__packed
typedef struct {    
    INT16U                  Head;                               // ��ͷ
    INT08U                  SrcAddr;                            // Դ��ַ
    INT08U                  DstAddr;                            // Ŀ�ĵ�ַ   
    INT08U                  PacketSn;                           // �������
    INT08U                  PacketCtrl;                         // ���Ŀ����֣� b0000
    INT08U                  DataLen;                            // ���������ȣ� 12
    INT08U                  PacketIdx;                          // ��������
    INT08U                  PacketIdxChk;                       // ��������У��
} StrCommHead;

/***************************************************
* ������ �������ݣ���ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/
// �������ݽṹ������-->�ͳ�
__packed
typedef struct {
    INT32U                  Oil;                                // ����	        4	��ֵ��Χ0-9999L 
    INT16S                  Accelerations;                      // ���ٶ�	    2	-9.000-9.000��m/s2��(1000��)
    INT16U                  Speed;                              // �ٶ�	        2	��ֵ0-600��Km/H��
    struct __recv_work__ {                                     // ��������	    1
    INT08U                  Null                    : 1;        //              0x01:��λ
    INT08U                  Tow                     : 1;        //              0x02:ǣ��
    INT08U                  Brake                   : 1;        //              0x04:�ƶ�
    INT08U                  Rsv                     : 5;        // ������δ����
    } Work;
    INT08U                  Rsv[3+10];                          // ����	        3
} StrCommRecvData;

/***************************************************
* ������ �������ݣ���ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/
// Ӧ�����ݽṹ���ͳ�-->����
__packed
typedef struct {
    INT16U                  Hight;                              // Һλ�߶�	    2	xxx.x���ף�10����
    INT16U                  Density;                            // �߶ȼ����ܶ�	2	0.xxxx��/�������ף�10000����
    union __usend_status {
    StrSenErr               Udat;
    INT08U                  Dat;
    } Sts;
    INT08U                  RefLiqTemp;                         // Һ��ο��¶�	1	0.25~63.50�ȣ��ֱ���1-254��ʾ������-49.5~77.0�ȣ��ֱ���1-254��ʾ��100��ʾ0�ȣ�0�����ͣ�255������
    INT16U                  AdcValue[2];                        // Adcֵ
    INT16U                  HightDiff;                          // �߶Ȳ�	    2	0~1000x0.1mm(���������߶Ȳ�)
    INT16U                  Volume;                             // ���  	    2	0~10000L(����)
    INT16U                  Weight;                             // ����	        2	0~10000kg(����)
    INT16S                  Pressure[2];                        // ѹǿ	        2	0~10000pa
    INT16S                  CalcDensity;                        // �����ܶ�     x10000
    INT16S                  Gravity;                            // ������ٶ�   x10000
    INT16S                  CalcGravity;                        // �������ٶ�   x10000
    //INT08U                  Rsv[2+10];                        // ����	        2	
} StrCommSendData;

/***************************************************
* ������ ���ݣ���ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
*/
__packed
typedef struct { 
    INT16U                  Check;                              // У���룬����ǰ�����ں�
    INT16U                  Tail;                               // ��β
} StrCommTail;

__packed
typedef struct { 
    /***************************************************
    * ������ ��ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
    */
    StrCommHead             Head;                               // ͷ
    StrCommRecvData         Data;                               // ����������
    StrCommTail             Tail;                               // β
} StrCommRecvPack;

__packed
typedef struct { 
    /***************************************************
    * ������ ��ɳ������ͨ����װ���첽����ͨ��ͨ��ͨ��Э��
    */
    StrCommHead             Head;                               // ͷ
    StrCommSendData	        Data;                               // ����������
    StrCommTail             Tail;                               // β
} StrCommSendPack;

__packed
typedef struct {     
    StrCommRecvPack         Rd;                                 // ����������
    StrCommSendPack	        Wr;                                 // ����������
    /***************************************************
    * ������ ���ڿ�����
    */
    INT08U                  SlaveAddr;                          // �ӻ���ַ    
    MODBUS_CH               *pch;                               // MODBUS���
    
    INT08U                  ConnectTimeOut;                     // ���ӳ�ʱ����������Ϊ��λ
    INT08U                  ConnectFlag;                        // ���ӱ�־
} StrDevComm;

#define ADC_SAMPLE_CNTS_PER_SEC     10                          // ÿ��ͨ��һ���Ӳ�������
#define ADC_SAMPLE_SECS             5                           // �뼶�˲�����
#define ADC_SAMPLE_CNTS_MAX         18                          // 
#define ADC_SAMPLE_TIME             (OS_CFG_TICK_RATE_HZ / ADC_SAMPLE_CNTS_PER_SEC / 2)                          // 

#define ADC_SIGNAL_CYCLE            1500                        // ��ͨ�˲�����С�ź�����
#define ADC_SIGNAL_CYCLE_CNT        10                          // �ɼ����ڸ���
#define ADC_SAMPLE_CYCLE            100                         // ������ݴ����������
#define ADC_SAMPLE_CNT              (1)                         // ������ݴ����������
#define ADC_SAMPLE_CNTS_DEEPTH      (ADC_SIGNAL_CYCLE * ADC_SIGNAL_CYCLE_CNT / ADC_SAMPLE_CYCLE) // ���ݴ������

__packed
typedef struct { 
    INT16U                  AdcValue;                           // ����ADCֵ
    INT16U                  AdcValueSec;                        // ÿ����ADCֵ
    INT16U                  AdcValueMin;                        // ÿ����ADCֵ
    INT16U                  AdcValueAll;                        // ������ADCֵ
    INT16U                  AdcValueSample;                     // ���е�ѹ
    CPU_FP32                Pressure;                           // ѹ��
    CPU_FP32                PressureSec;                        // ÿ����ѹ��
    CPU_FP32                PressureMin;                        // ÿ����ѹ��
    CPU_FP32                PressureAll;                        // ����ѹ��
    CPU_FP32                PressureSample;                         // ���е�ѹ
    INT16S                  Hight;                              // �߶�
    INT16S                  HightSec;                           // ÿ���Ӹ߶�
    INT16S                  HightMin;                           // ÿ���Ӹ߶�
    INT16S                  HightAll;                           // ���и߶�
    INT16S                  HightSample;                         // ���е�ѹ
    INT16S                  Voltage;                            // ��ѹ0.1MV
    INT16S                  VoltageSec;                         // ÿ���ӵ�ѹ
    INT16S                  VoltageMin;                         // ÿ���ӵ�ѹ
    INT16S                  VoltageAll;                         // ���е�ѹ
    INT16S                  VoltageSample;                      // ���е�ѹ
    
    CPU_FP32                Current;                            // ����
    CPU_FP32                CurrentSec;                         // ÿ���ӵ���
    CPU_FP32                CurrentMin;                         // ÿ���ӵ���
    CPU_FP32                CurrentAll;                         // ���е���
    CPU_FP32                CurrentSample;                      // ���е���
    
    INT08U                  Head;                               // �˲�����ͷָ��
    INT08U                  Tail;                               // �˲�����βָ��
    INT08U                  HeadSec;                            // �˲�����ͷָ��
    INT08U                  TailSec;                            // �˲�����βָ��
    INT08U                  HeadMin;                            // �˲�����ͷָ��
    INT08U                  TailMin;                            // �˲�����βָ��
    
    INT16U                  AdcFilter   [ADC_SAMPLE_CNTS_PER_SEC];// 100������ADCֵ
    INT16U                  AdcFilterSec[ADC_SAMPLE_SECS];      // 1����ADCֵ
    INT16U                  AdcFilterMin[ADC_SAMPLE_CNTS_MAX];  // 1����ADCֵ
    INT16U                  Rsv[100-38-ADC_SAMPLE_CNTS_PER_SEC-ADC_SAMPLE_SECS-ADC_SAMPLE_CNTS_MAX]; 
}__StrSen;

__packed
typedef struct {     
    INT16S                  AverHight;                          // ƽ���߶�
    INT16S                  AverHightSec;                       // ƽ���߶�
    INT16S                  AverHightMin;                       // ƽ���߶�
    INT16S                  AverHightAll;                       // ƽ���߶�
    INT16S                  AverHightSample;                    // ���е�ѹ
    INT16S                  AverPressure;                       // ƽ��ѹ��
    INT16S                  AverPressureSec;                    // ƽ��ѹ��
    INT16S                  AverPressureMin;                    // ƽ��ѹ��
    INT16S                  AverPressureAll;                    // ƽ��ѹ��
    INT16S                  AverPressureSample;                 // ���е�ѹ
    CPU_FP32                DiffPressure;                       // ѹ����
    CPU_FP32                DiffPressureSec;                    // ѹ����
    CPU_FP32                DiffPressureMin;                    // ѹ����
    CPU_FP32                DiffPressureAll;                    // ѹ����
    CPU_FP32                DiffPressureSample;                 // ���е�ѹ
    INT16S                  CalcHight;                          // ����߶Ȳ�
    INT16S                  CalcHightSec;                       // ����߶Ȳ�
    INT16S                  CalcHightMin;                       // ����߶Ȳ�
    INT16S                  CalcHightAll;                       // ����߶Ȳ�
    INT16S                  CalcHightSample;                    // ���е�ѹ
    INT16S                  OffsetHight;                        // �߶�ƫ��
    INT16S                  CalcDiffHight;                      // ����߶Ȳ�
    INT16S                  CalcDiffHightSec;                   // ����߶Ȳ�
    INT16S                  CalcDiffHightMin;                   // ����߶Ȳ�
    INT16S                  CalcDiffHightAll;                   // ����߶Ȳ�
    INT16S                  CalcDiffHightSample;                // У׼�߶Ȳ�
    INT16S                  ChkedDiffHight;                     // У׼�߶Ȳ�
    CPU_FP32                ChkedDensity;                       // �ܶ�
    CPU_FP32                CalcDensity;                        // �ܶ�
    CPU_FP32                CalcDensitySec;                     // �ܶ�
    CPU_FP32                CalcDensityMin;                     // �ܶ�
    CPU_FP32                CalcDensityAll;                     // �ܶ�
    CPU_FP32                CalcDensitySample;                  // ���е�ѹ
    CPU_FP32                Kp;                                 // 
    CPU_FP32                KpSample;                           // 
    CPU_FP32                Volume;                             // ���
    CPU_FP32                Weight;                             // ����
    INT16S                  Hight;                              // �ϴ��߶�   
    CPU_FP32                Density;                            // �����ܶ�
    CPU_FP32                Gravity;                            // ������ٶ�
    CPU_FP32                CalcGravity;                        // �������ٶ�
    INT08U                  Rsv[200-118];//100-57];                         // 

} __StrSenCom;

/***************************************************
* ������ ��������������256 + 4 = 260�ֽ�
*/
__packed
typedef struct _StrSenPara {
    /***************************************************
    * ������ ��2����ϵͳ��������ʼ��ַ = 000
    */    
    CPU_FP32                ChkedDiffHight;             // У׼�߶Ȳ�
    CPU_FP32                DiffPNbr;                   // ѹǿ�����������
    CPU_FP32                PINbr;                      // ѹǿ�������������    
    CPU_FP32                TestDensity;                // �����ܶ�
    CPU_FP32                TestGravity;                // ���Լ��ٶ�
    
    INT32U                  SenId;                      // ��������ţ�����ǰ�����ں�
    CPU_FP32                Offset;                     // ����5������
    /***************************************************
    * ������ У׼��������ʼ��ַ = 28
    */
    union __uSENSOR_CHECK_TYPE2 {
        struct __sSENSOR_CHECK_TYPE2 {
            INT16U      ChkType             : 4;        // ��λ������У׼���ͣ�0��У׼��1ѹǿ��ֶΣ�2��ѹǿ�����ԣ�3��ѹ�������ֶΣ�4��ѹ����������
            INT16U      Test                : 4;        // ��λ������У׼���ͣ�0��У׼��1ѹǿ��ֶΣ�2��ѹǿ�����ԣ�3��ѹ�������ֶΣ�4��ѹ����������
            INT16U      Rst                 : 4;        // ��λ��־
            INT16U      Rsv                 : 4;
        }udat;
        INT32U dat;
    } SenSts;
    CPU_FP32                RSV1[2];                    // ����5������
    /***************************************************
    * ������ ��2����ϵͳ��������ʼ��ַ = 040 
    */
    CPU_FP32                DiffP[10];
    /***************************************************
    * ������ ��3����ϵͳ��������ʼ��ַ = 080
    */ 
    CPU_FP32                PI[2][6][2];
    /***************************************************
    * ������ ��4����ϵͳ��������ʼ��ַ = 176
    */ 
    StrIToPSenChk           Check[2];
    /***************************************************
    * ������ ��1�����û���������ʼ��ַ = 200
    */
    CPU_FP32                RSV2[7];                     // ����5������

    /***************************************************
    * ������ У���룺��ʼ��ַ = 252
    */
    INT16U                  CheckSum;                   // У����
    /***************************************************
    * ������ У���룺��ʼ��ַ = 254
    */
    INT16U                  ReadFlag;                   // �Ѷ�ȡ��־��0X5AΪ�����ݣ�0XA5Ϊ�Ѷ�������Ϊ��Ч
} StrSenPara;

__packed
typedef union {
    StrSenPara             dat;
    INT08U                  buf1[256];
    INT16U                  buf2[128];
    INT32U                  buf3[64];
} UnionSenPara;

__packed
typedef struct { 
    __StrSen                H;                                  // �ߵ㴫����
    __StrSen                L;                                  // �͵㴫����
    __StrSenCom             C;                                  // ��������������
    UnionSenPara            Para;
    INT16U                  Filter[2][ADC_SAMPLE_CNTS_DEEPTH];  //������ݴ���
    INT08U                  DensityStep;                        //�ܶȻ�ȡ״̬��
    INT08U                  DensitySts;                         //�ܶȻ�ȡ״̬��
    INT16U                  DensityCtr;                         //�ܶȻ�ȡ״̬��
} StrSen; 


__packed
typedef union {
    StrCtrlPara             dat;
    INT08U                  buf1[256];
    INT16U                  buf2[128];
    INT32U                  buf3[64];
} UnionCtrlPara;

//APP_EXT StrOilTab       OilTab;

__packed
typedef struct {
    /***************************************************
    * ������ ���ݽṹ��˳�����������
    */
    UnionCtrlPara           Para;                               // ���Ʋ���
    StrSen                  Sen;                                // ���������� 
    StrOilTab               Tab;                                // ����ģ��
    StrDevComm              Comm;                               // COMM���� 
    
    StrOilTab              *VTab;                               // �����ݻ���
    StrParaRange           *Prge;                               // ���Ʋ�����Χ
    
    StrCtrlOS               Os;                                 // OS����        
    StrDisp                 Disp;                               // ��ʾ����        
    StrSystem               Sys;                                // ϵͳ״̬
} StrCtrlDCB;

extern StrCtrlDCB           Ctrl;

/*******************************************************************************
 * 				                  ����                                         *
 *******************************************************************************/
/***************************************************
* ������ ������Ϣ���ݽṹ�����ڷ���
*/
__packed
typedef struct {
  INT08U                tail;                                   // β
  INT08U                head;                                   // ͷ
  INT08U                cnt;                                    // ������
  INT08U                qbuf[AppMsgQ][AppMsgQSize];             // ��Ϣ����
} StrMsgInfo;

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/***************************************************
* ������ �ṹ������
*/
APP_EXT NOTIFY_EEP_SAVE_TYPE    NotifyEepSave;

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
void        TaskInitSen                 ( void );
void        TaskInitDisp                ( void );
/***************************************************
* ������ OSAL�����¼�������������
*/
osalEvt     TaskBeepEvtProcess          (INT8U task_id, osalEvt task_event);
osalEvt     TaskKeyEvtProcess           (INT8U task_id, osalEvt task_event);
osalEvt     TaskCtrlEvtProcess          (INT8U task_id, osalEvt task_event);
osalEvt     TaskSenEvtProcess           (INT8U task_id, osalEvt task_event);
osalEvt     TaskDispEvtProcess          (INT8U task_id, osalEvt task_event);

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
#endif	/* GLOBLES_H */