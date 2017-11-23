/*******************************************************************************
 *   Filename:       app_task_mater.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ˫��ѡ�� mater �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Mater �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� MATER �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_MATER_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_MATER_STK_SIZE ����С
 *
 *   Notes:
 *     				 E-mail: shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
//#include <app_mater_protocol.h>
#include <bsp_flash.h>
#include <iap.h>
     
#include "stm32f10x.h"
#include <stdio.h>
#include "SZ_STM32F107VC_LIB.h"
#include "DS3231.h"
#include "I2C_CLK.h"
#include "Display.h"
#include "DATA_STORAGE.h"
#include "DELAY.h"
#include "RS485.h"
#include "POWER_MACRO.h"
#include "MX25.h"
#include "FM24CL64.h"
#include "string.h"
#include "xprintf.h"
#include "WatchDog.h"
#include <app_comm_protocol.h>
#include <crccheck.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_mater__c = "$Id: $";
#endif

#define APP_TASK_MATER_EN     DEF_ENABLED
#if APP_TASK_MATER_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
/***********************************************
* ������ ���ݼ�¼��ַ
*
��ַ	����	����	����
AC.PPpower_NUM
0
4
8
AC.NPpower_NUM
12
16
20
��������
32	4		ֱ����ѹƫ��
36	4		ֱ������ƫ��
40	4		������ѹƫ��
44	4		��������ƫ��
48	4	u32	������ѹ����
52	4	u32	������������
56	4	u32	��������
//60	4	u32	���ݵ�ַ
64	4	u32	�豸ID
68	4	U8	���ͳ���
72	4	u32	��¼��ʼ��ַ
76	4	u32	��¼������ַ
80	4	u32	��¼��ˮ��
84	4	u32	��¼ʱ����
88	4	f32	��ѹ����
92	4	f32	��������
96  4   u32 ��ѹ����ֵ
AC.PQpower_NUM
100
104
108
AC.NQpower_NUM
112	4		���й���������
116	4
120	4

//
160 4   f32 ��ѹ����ϵ��
164 4   f32 ��������ϵ��
*/
/*******************************************************************************
 * MACROS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
 * TYPEDEFS
 */

/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB      AppTaskMaterTCB;
OS_Q                MTR_RxQ;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK     AppTaskMaterStk[ APP_TASK_MATER_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */
ENERGY              AC;
SDAT 	            recordsfr;	
StrMater            Mater;
StrMater            History;

static volatile uint32_t            SysTime = 0;

uint8_t             g_DipDisVal[40];
uint32_t            g_Flash_Adr;

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void     AppTaskMater                (void *p_arg);
static void     APP_MaterInit               (void);

void            DC_COUNTInit                (void);
void            AC_COUNTInit                (void);
INT08U          APP_SevrRxDataDealCB        (MODBUS_CH  *pch);
void            APP_MaterSendMsgToServer    (void);
void            EXTI0_ISRHandler            (void);
void            EXTI9_5_ISRHandler          (void);

CPU_BOOLEAN     APP_CSNC_CommHandler        (MODBUS_CH  *pch);
CPU_BOOLEAN     APP_MaterCommHandler        (MODBUS_CH  *pch);

void            SavePowerData               (void);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern void     CS5463_AC_Adjust        (MODBUS_CH  *pch,uint8_t mode)	;
extern void     uartprintf              (MODBUS_CH  *pch,const char *fmt, ...);
/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskMaterCreate
 * ��    �ܣ� ���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskMaterCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskMaterTCB,                    // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Mater",                    // ��������
                 (OS_TASK_PTR ) AppTaskMater,                       // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                                  // ����������
                 (OS_PRIO     ) APP_TASK_MATER_PRIO,                // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskMaterStk[0],                 // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_MATER_STK_SIZE / 10,       // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_MATER_STK_SIZE,            // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 0u,                                 // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                                 // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                                  // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // �����ջ��������
                                OS_OPT_TASK_STK_CLR),               // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                               // ָ���������ָ�룬���ڴ����������
}

/*******************************************************************************
 * ��    �ƣ� AppTaskMater
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskMater (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly     = CYCLE_TIME_TICKS;
    OS_TICK     ticks;
        
    /***********************************************
    * ������ �����¼���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.MaterEvtFlag= COMM_EVT_FLAG_HEART       // ����������
                        + COMM_EVT_FLAG_RESET       // COMM��λ
                        + COMM_EVT_FLAG_CONNECT     // COMM����
                        + COMM_EVT_FLAG_RECV        // ���ڽ���
                        + COMM_EVT_FLAG_REPORT      // ���ڷ���
                        + COMM_EVT_FLAG_CLOSE       // �Ͽ�
                        + COMM_EVT_FLAG_TIMEOUT     // ��ʱ
                        + COMM_EVT_FLAG_CONFIG;     // ����
    
    OSQCreate ( (OS_Q        *)&MTR_RxQ,
                (CPU_CHAR    *)"RxQ",
                (OS_MSG_QTY   ) OS_CFG_INT_Q_SIZE,
                (OS_ERR      *)&err);
    /***********************************************
    * ������ �����ʼ��    
    */ 
  	float   Vac_RMS_F           = 0;
  	float   Iac_RMS_F           = 0;
        
 	uint32_t PP_Power_temp1     = 0;
	uint32_t PP_Power_temp2     = 0;
	uint32_t PP_Power_temp3     = 0;
    
	uint32_t NP_Power_temp1     = 0;
	uint32_t NP_Power_temp2     = 0;
	uint32_t NP_Power_temp3     = 0;
    
	uint8_t  PP_fram_temp[16]   = {0};
	uint8_t  NP_fram_temp[16]   = {0};
    
	int32_t  PQ_Power_temp1     = 0;
	uint32_t PQ_Power_temp2     = 0;
	uint32_t PQ_Power_temp3     = 0;
    
	uint32_t NQ_Power_temp1     = 0;
	uint32_t NQ_Power_temp2     = 0;
	uint32_t NQ_Power_temp3     = 0;
    
	uint8_t  PQ_fram_temp[16]   = {0};
	uint8_t  NQ_fram_temp[16]   = {0};
    
	uint32_t i                  = 0;
        
    /***********************************************
    * ������ ����ʼ��
    */   
    APP_MaterInit();
    
    /***********************************************
    * ������ ��ȡ�����ͺţ�������
    */     
    ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
    ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
    ReadFM24CL64(64 , (uint8_t *)&Ctrl.ID, 4);
    /***********************************************
    * ������ ��¼ʱ����
    */  
    ReadFM24CL64(84 , (uint8_t *)&Mater.Record.RecordTime, 4);
    if ( Mater.Record.RecordTime < OS_TICKS_PER_SEC * 10 )
        Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
    else if ( Mater.Record.RecordTime > OS_TICKS_PER_SEC * 300 )
        Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
        
    ReadFM24CL64(88 , (uint8_t *)&AC.U_SCL, 4);
    if ( AC.U_SCL < 1 )
        AC.U_SCL = 1.0;
    else if ( AC.U_SCL > 50000 )
        AC.U_SCL    = 50000.0;
    
    ReadFM24CL64(92 , (uint8_t *)&AC.I_SCL, 4);
    if ( AC.I_SCL < 1.0 )
        AC.I_SCL = 1.0;
    else if ( AC.I_SCL > 1000.0 )
        AC.I_SCL    = 1000.0;
    //*
    float fdat;
    ReadFM24CL64(96 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 5000.0;    
    else if ( fdat > 20000.0 )
        fdat    = 20000.0;
    AC.U_O      = fdat;
    
    ReadFM24CL64(160 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 1.0;    
    else if ( fdat< 0.5 )
        fdat = 0.5;
    else if ( fdat > 2.0 )
        fdat    = 2.0;
    AC.U_K      = fdat;
    
    ReadFM24CL64(164 , (uint8_t *)&fdat, 4);
    if ( fdat == 0.0 )
        fdat    = 1.0;    
    else if ( fdat < 0.5 )
        fdat = 0.5;
    else if ( fdat > 2.0 )
        fdat    = 2.0;
    AC.I_K      = fdat;
    /***********************************************
    * ������ ��ȡ����
    */ 
	ReadFM24CL64(  0, PP_fram_temp, 12);
	ReadFM24CL64( 12, NP_fram_temp, 12);
    
	ReadFM24CL64(100, PQ_fram_temp, 12);
	ReadFM24CL64(112, NQ_fram_temp, 12);
    /***********************************************
    * ������ ���й�����
    */ 
	PP_Power_temp1 =  PP_fram_temp[3]<<24 |  PP_fram_temp[2]<<16 | PP_fram_temp[1]<<8 | PP_fram_temp[0];
	PP_Power_temp2 =  PP_fram_temp[7]<<24 |  PP_fram_temp[6]<<16 | PP_fram_temp[5]<<8 | PP_fram_temp[4];
	PP_Power_temp3 = PP_fram_temp[11]<<24 | PP_fram_temp[10]<<16 | PP_fram_temp[9]<<8 | PP_fram_temp[8];
	
    if(PP_Power_temp1 == PP_Power_temp2)
		AC.PPpower_base = PP_Power_temp1;
	else if(PP_Power_temp2 == PP_Power_temp3)
		AC.PPpower_base = PP_Power_temp2;
	else
		AC.PPpower_base = PP_Power_temp1;
   	
    /***********************************************
    * ������ ���й�����
    */ 
	NP_Power_temp1 =  NP_fram_temp[3]<<24 |  NP_fram_temp[2]<<16 | NP_fram_temp[1]<<8 | NP_fram_temp[0];
	NP_Power_temp2 =  NP_fram_temp[7]<<24 |  NP_fram_temp[6]<<16 | NP_fram_temp[5]<<8 | NP_fram_temp[4];
	NP_Power_temp3 = NP_fram_temp[11]<<24 | NP_fram_temp[10]<<16 | NP_fram_temp[9]<<8 | NP_fram_temp[8];
    
	if(NP_Power_temp1 == NP_Power_temp2)
		AC.NPpower_base = NP_Power_temp1;
	else if(NP_Power_temp2 == NP_Power_temp3)
		AC.NPpower_base = NP_Power_temp2;
	else
		AC.NPpower_base = NP_Power_temp1;
    
    /***********************************************
    * ������ ���޹�����
    */
	PQ_Power_temp1 =  PQ_fram_temp[3]<<24 |  PQ_fram_temp[2]<<16 | PQ_fram_temp[1]<<8 | PQ_fram_temp[0];
	PQ_Power_temp2 =  PQ_fram_temp[7]<<24 |  PQ_fram_temp[6]<<16 | PQ_fram_temp[5]<<8 | PQ_fram_temp[4];
	PQ_Power_temp3 = PQ_fram_temp[11]<<24 | PQ_fram_temp[10]<<16 | PQ_fram_temp[9]<<8 | PQ_fram_temp[8];
    
	if(PQ_Power_temp1 == PQ_Power_temp2)
		AC.PQpower_base = PQ_Power_temp1;
	else if(PQ_Power_temp2 == PQ_Power_temp3)
		AC.PQpower_base = PQ_Power_temp2;
	else
		AC.PQpower_base = PQ_Power_temp1;
    
    /***********************************************
    * ������ ���޹�����
    */ 
	NQ_Power_temp1 =  NQ_fram_temp[3]<<24 |  NQ_fram_temp[2]<<16  | NQ_fram_temp[1]<<8 | NQ_fram_temp[0];
	NQ_Power_temp2 =  NQ_fram_temp[7]<<24 |  NQ_fram_temp[6]<<16  | NQ_fram_temp[5]<<8 | NQ_fram_temp[4];
	NQ_Power_temp3 = NQ_fram_temp[11]<<24 | NQ_fram_temp[10]<<16  | NQ_fram_temp[9]<<8 | NQ_fram_temp[8];
    
	if(NQ_Power_temp1 == NQ_Power_temp2)
		AC.NQpower_base = NQ_Power_temp1;
	else if(NQ_Power_temp2 == NQ_Power_temp3)
		AC.NQpower_base = NQ_Power_temp2;
	else
		AC.NQpower_base = NQ_Power_temp1;
    
	ReadFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
	ReadFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
	ReadFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
    if ( Mater.Record.Head > MAX_ADDR ) {
        Mater.Record.Head  = 0;
    } else {
        Mater.Record.Head  = Mater.Record.Head / sizeof(Mater);
        Mater.Record.Head  = Mater.Record.Head * sizeof(Mater);
    }
    if ( Mater.Record.Tail > MAX_ADDR ) {
        Mater.Record.Tail  = 0;
    } else {
        Mater.Record.Tail  = Mater.Record.Tail / sizeof(Mater);
        Mater.Record.Tail  = Mater.Record.Tail * sizeof(Mater);
    }
    if ( Mater.RecordNbr == 0xffffffff )
        Mater.RecordNbr  = 0;
    
    BSP_OS_TimeDly(OS_TICKS_PER_SEC / 2);
    /***********************************************
    * ������ 
    */ 
	uartprintf(Ctrl.Com.pch,"\r\n��ǰͷ��ַ��0x%08X\r\n��ǰβ��ַ��0x%08X\r\n��ǰ��¼�ţ�%d",
               Mater.Record.Head,
               Mater.Record.Tail,
               Mater.RecordNbr);
                
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_ERR      terr;
        ticks       = OSTimeGet(&terr);                 // ��ȡ��ǰOSTick
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_MATER,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);
        
        /***********************************************
        * ������ �ȴ�DTU���ݽ�����Ϣ����
        */
        OS_MSG_SIZE p_msg_size;
        
        MODBUS_CH *pch = 
       (MODBUS_CH *)OSQPend ((OS_Q*)&MTR_RxQ,
                    (OS_TICK       )dly,
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,//OS_OPT_PEND_NON_BLOCKING,
                    (OS_MSG_SIZE  *)&p_msg_size,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
        /***********************************************
        * ������ 2017/8/19���յ����������
        */
        if ( OS_ERR_NONE == err ) {
            /***********************************************
            * ������ 2017/8/19��������յ���������������������
            */
            pch->RxBufByteCnt   = p_msg_size;
            APP_MaterCommHandler(pch);
        }
        
        /***********************************************
        * ������ �ȴ�COMM������־λ
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.MaterEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.MaterEvtFlag,
                    ( OS_TICK      ) dly,
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING,
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        /***********************************************
        * ������ û�д���,���¼�����
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
            /***********************************************
            * ������ ����������
            */
            if       ( flags & COMM_EVT_FLAG_HEART  ) {
                flagClr |= COMM_EVT_FLAG_HEART;
                // �������
                SavePowerData();
            /***********************************************
            * ������ COMM��λ
            */
            } 
            if ( flags & COMM_EVT_FLAG_RESET ) {
                flagClr |= COMM_EVT_FLAG_RESET;
                
            /***********************************************
            * ������ COMM����
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONNECT ) {
                flagClr |= COMM_EVT_FLAG_CONNECT;
                
            /***********************************************
            * ������ ���ڽ���
            */
            } 
            if ( flags & COMM_EVT_FLAG_RECV ) {
                flagClr |= COMM_EVT_FLAG_RECV;
                
            /***********************************************
            * ������ �ϱ���Ϣ
            */
            } 
            if ( flags & COMM_EVT_FLAG_REPORT ) {
                flagClr |= COMM_EVT_FLAG_REPORT; 
                
            /***********************************************
            * ������ �Ͽ�
            */
            } 
            if ( flags & COMM_EVT_FLAG_CLOSE ) {
                flagClr |= COMM_EVT_FLAG_CLOSE;
                
            /***********************************************
            * ������ ��ʱ
            */
            } 
            if ( flags & COMM_EVT_FLAG_TIMEOUT ) {
                flagClr |= COMM_EVT_FLAG_TIMEOUT;
                
            /***********************************************
            * ������ ����
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONFIG ) {
                flagClr |= COMM_EVT_FLAG_CONFIG;
                /***********************************************
                * ������ ��������
                */                 
                SavePowerData();
                //SaveRecord();
                void  App_SaveDataToHistory( void );
                App_SaveDataToHistory();
                
                uartprintf(Ctrl.Com.pch,"\r\n��ǰʱ�䣺20%02d-%02d-%02d  %02d:%02d:%02d", 
                           Mater.Tax.Tax3.Vari.LKJTime.Year, 
                           Mater.Tax.Tax3.Vari.LKJTime.Mon, 
                           Mater.Tax.Tax3.Vari.LKJTime.Day,
                           Mater.Tax.Tax3.Vari.LKJTime.Hour, 
                           Mater.Tax.Tax3.Vari.LKJTime.Min, 
                           Mater.Tax.Tax3.Vari.LKJTime.Sec);
                uartprintf(Ctrl.Com.pch,
                           "\r\n�洢���ݳɹ� ID = %d\r\n���й� = %d;\r\n���й� = %d;\r\n���޹� = %d;\r\n���޹� = %d;\r\n��  ѹ = %d;\r\n��  �� = %d;\r\nƵ  �� = %d;\r\n�������� = %d;\r\n�й����� = %d;\r\n�޹����� = %d\r\n",
                            Mater.RecordNbr,
                            Mater.Energy.PPPower,      
                            Mater.Energy.NPPower,      
                            Mater.Energy.PQPower,      
                            Mater.Energy.NQPower,      
                            Mater.Energy.PrimVolt/1000,
                            Mater.Energy.PrimCurr/1000,
                            Mater.Energy.PowerFreq/1000,
                            Mater.Energy.PowerFactor,
                            Mater.Energy.ActivePower,  
                            Mater.Energy.ReactivePower);
                /***********************************************
                * ������ �������ݴ���
                */
            } 
            //exit:
            /***********************************************
            * ������ �����־
            */
            if ( !flagClr ) {
                flagClr = flags;
            }
            
            /***********************************************
            * ������ �����־λ
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
        /***********************************************
        * ������ �����ʱ������һ��������
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.MaterEvtFlagGrp,
                        ( OS_FLAGS      )Ctrl.Os.MaterEvtFlag,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err );
        }
        /***********************************************
        * ������ ���й������й������޹������޹�����
        */
        i++;
		AC.PPpower_NUM = (uint32_t)(AC.PPpower_temp + AC.PPpower_base);
		AC.NPpower_NUM = (uint32_t)(AC.NPpower_temp + AC.NPpower_base);
		AC.PQpower_NUM = (uint32_t)(AC.PQpower_temp + AC.PQpower_base);
		AC.NQpower_NUM = (uint32_t)(AC.NQpower_temp + AC.NQpower_base);
		
        /***********************************************
        * ������ ѭ����ʾ�����ֵΪ99999999
        */ 
        if(AC.PPpower_NUM >= 100000000)	{
		
			AC.PPpulse = 0;
			AC.PPpower_temp = 0;
			AC.PPpower_base = 1; 
		}
        /***********************************************
        * ������ ѭ����ʾ�����ֵΪ99999999
        */ 
		if(AC.NPpower_NUM >= 100000000)	{
		
			AC.NPpulse = 0;
			AC.NPpower_temp = 0;
			AC.NPpower_base = 1; 
		}
        /***********************************************
        * ������ ѭ����ʾ�����ֵΪ99999999
        */ 
		if(AC.PQpower_NUM >= 100000000)	{
		
			AC.PQpulse = 0;
			AC.PQpower_temp = 0;
			AC.PQpower_base = 1; 
		}
        /***********************************************
        * ������ ѭ����ʾ�����ֵΪ99999999
        */ 
		if(AC.NQpower_NUM >= 100000000)	{
		
			AC.NQpulse = 0;
			AC.NQpower_temp = 0;
			AC.NQpower_base = 1; 
		}
        /***********************************************
        * ������ �й����ʡ��޹����ʡ��������ء�Ƶ��
        */ 
//		AC.ACTIVE_POWER     = (int32_t)(SPI_CS5463_AC_Read_Else_FLOAT(PA) * 20000);
//		AC.REACTIVE_POWER   = (int32_t)(SPI_CS5463_AC_Read_Else_FLOAT(PQ) * 20000);
//		AC.Power_Factor     = SPI_CS5463_AC_Read_Else_FLOAT(PF);
//		AC.Power_Freq       = SPI_CS5463_AC_Read_Else_FLOAT(FREQUENCY) * 4000;
//        
        /***********************************************
        * ������ ��������
        */
        if ( fabs( AC.Power_Factor ) < 0.0005 )
            AC.Power_Factor     = 0;
         
        /**************** *******************************
        * ������ ��ѹ������
        */ 
//		Vac_RMS_F = SPI_CS5463_AC_Read_VIrms_FLOAT(VRMS);
//		Iac_RMS_F = SPI_CS5463_AC_Read_VIrms_FLOAT(IRMS);
		
        /***********************************************
        * ������ ˲ʱ��ѹ��˲ʱ����:17020003
        *
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//����ϵ����25000V/0.6(SS4����)
        //AC.I_RMS = Iac_RMS_F * 500;				    //����ϵ����300A/0.6(SS4����)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * 0.99960016;		    //����ϵ����25000V/0.6(HXD1D����)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6);       //1000;	    //����ϵ����600A/0.6(HXD1D����)
		
		//if(Vac_RMS_F < 0.0012)							    //��ѹ����ϵ���ϴ�ȥ������ѹ
		//	AC.U_RMS = 0;
        
        if ( AC.I_RMS  > 80 ) {
             AC.I_RMS = AC.I_RMS * 0.975609756;
        }
        
        /***********************************************
        * ������ ˲ʱ��ѹ��˲ʱ����:17020001
        */
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//����ϵ����25000V/0.6(SS4����)
        //AC.I_RMS = Iac_RMS_F * 500;				    //����ϵ����300A/0.6(SS4����)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * AC.U_K;  //0.995421063;		//����ϵ����25000V/0.6(HXD1D����)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6) * AC.I_K;       //1000;	    //����ϵ����600A/0.6(HXD1D����)
		
		//if(Vac_RMS_F < 0.0012)							    //��ѹ����ϵ���ϴ�ȥ������ѹ
		//	AC.U_RMS = 0;
        
        //if ( AC.I_RMS  > 80 ) {
        //     AC.I_RMS = AC.I_RMS * 0.975609756;
        //}
        /***********************************************
        * ������ ˲ʱ��ѹ��˲ʱ����:17020002
        * 
        //AC.U_RMS = Vac_RMS_F * 41666.7;				//����ϵ����25000V/0.6(SS4����)
        //AC.I_RMS = Iac_RMS_F * 500;				    //����ϵ����300A/0.6(SS4����)
        
		AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6);		    //����ϵ����25000V/0.6(HXD1D����)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6);       //1000;	    //����ϵ����600A/0.6(HXD1D����)
		
		//if(Vac_RMS_F < 0.0012)							    //��ѹ����ϵ���ϴ�ȥ������ѹ
		//	AC.U_RMS = 0;
        
        //if ( AC.I_RMS  > 80 ) {
        //     AC.I_RMS = AC.I_RMS * 0.975609756;
        //}
        */
        AC.U_RMS = Vac_RMS_F * ((float)AC.U_SCL/0.6) * AC.U_K;		    //����ϵ����25000V/0.6(HXD1D����)        
		AC.I_RMS = Iac_RMS_F * ((float)AC.I_SCL/0.6) * AC.I_K;       //1000;	    //����ϵ����600A/0.6(HXD1D����)
		
        /***********************************************
        * ������ 
        */
        if ( AC.U_RMS  < AC.U_O ) {
             AC.U_RMS = 0;
        }
        
        Mater.Energy.PPPower        = (uint32_t)((float)AC.PPpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //���й�����    1kvarh      99999999 kvarh
        Mater.Energy.NPPower        = (uint32_t)((float)AC.NPpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //���й�����    1kvarh      99999999 kvarh 
        Mater.Energy.PQPower        = (uint32_t)((float)AC.PQpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //���޹�����    1kvarh      99999999 kvarh
        Mater.Energy.NQPower        = (uint32_t)((float)AC.NQpower_NUM / (float)(( 100000.0 * 100.0 * 5.0 / (float)AC.U_SCL / (float)AC.I_SCL) ) );			                //���޹�����    1kvarh      99999999 kvarh
        Mater.Energy.PrimVolt       = (uint32_t)(AC.U_RMS * 1000);				//ԭ�ߵ�ѹ      0.001V      0��35000.000V
        Mater.Energy.PrimCurr       = (uint32_t)(AC.I_RMS * 1000);				//ԭ�ߵ���      0.001A      0��600.000A
        Mater.Energy.PowerFreq      = (uint32_t)(AC.Power_Freq * 1000);			//Ƶ��          0.001Hz    
        Mater.Energy.PowerFactor    = (int32_t)(AC.Power_Factor * 1000);		//��������      0.001       -1.000��1.000
        Mater.Energy.ActivePower    = AC.ACTIVE_POWER;			                //�й�����      0.001kW     -12000.000  kW��12000.000  kW
        Mater.Energy.ReactivePower  = AC.REACTIVE_POWER;		                //�޹�����      0.001kvar   -12000.000  kvar��12000.000 
        
//        SPI_CS5463_AC_GetDrdy();
//        SPI_CS5463_AC_ClearDrdy();
        /***********************************************
        * ������ ����ʣ��ʱ��
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        }
    }
}

/*******************************************************************************
 * ��    �ƣ� APP_MaterInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2016-11-11
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void APP_MaterInit(void)
{
    /***********************************************
    * ������ �����¼���־��
    */    
	//SZ_USART1_DATA.uart = SZ_STM32_COM1;
    
	AC.U_RMS = 0;
	AC.I_RMS = 0;
    
	AC.Power_Factor = 0;
	AC.Power_Freq = 0;
    
	AC.ACTIVE_POWER = 0;
	AC.REACTIVE_POWER = 0;
    
	AC.PPpower_NUM = 0;								
	AC.NPpower_NUM = 0;			
	AC.PPpulse = 0;				
	AC.NPpulse = 0;				
	AC.PPpower_base = 0;		
	AC.NPpower_base = 0;		
	AC.PPpower_temp = 0;		
	AC.NPpower_temp = 0;
    
	AC.PQpower_NUM = 0;								
	AC.NQpower_NUM = 0;			
	AC.PQpulse = 0;				
	AC.NQpulse = 0;				
	AC.PQpower_base = 0;		
	AC.NQpower_base = 0;		
	AC.PQpower_temp = 0;		
	AC.NQpower_temp = 0;
        
    BSP_OS_TimeDly(OS_TICKS_PER_SEC);
    /***********************************************
    * ������ 
    */
	//SZ_STM32_COMInit(COM1, 115200);
	//RS485_SET_RX_Mode();      /* Ĭ������RS485�ķ���Ϊ���գ������������߳�ͻ */
	//xPrintf_Init(19200);           //COM2��Ӧ����RS485
	//LED_DIS_Config();
	I2C_GPIO_Config();
    void EEP_OS_Init(void);
    EEP_OS_Init();
	InitDS3231();
//	SPI_AC_INIT();
//	CS5463_AC_INIT();
	SPI_FLASH_Init();
    
    /***********************************************
    * ������ 
    */
    //DC_COUNTInit();
	AC_COUNTInit();
    /***********************************************
    * ������ 
    */
	//NVIC_GroupConfig();
	//NVIC_COMConfiguration();
    
    /***********************************************
    * ������ 
    */
	//Dis_Test();							 //������ϵ����
   	//SZ_STM32_SysTickInit(100);
    void PWR_PVD_Init(void);
	PWR_PVD_Init();
    //WDG_Init();
        
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_MATER;
}

/*******************************************************************************
 * ��    �ƣ� DC_COUNTInit
 * ��    �ܣ� ��ʼ��PE9���ⲿ�жϹ��ܣ������������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2016-11-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void DC_COUNTInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    
    /* ʹ��GPIOB��Clockʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* ��ʼ��GPIOB8�ܽţ�����Ϊ������������ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* ��ʼ��GPIOA0Ϊ�ж�ģʽ */
    /* ��GPIOA0��Ӧ�Ĺܽ����ӵ��ڲ��ж��� */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource9);
    
    /* Configure Button EXTI line */
    /* ��GPIOA0����Ϊ�ж�ģʽ���½��ش����ж� */    
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line9); 
    
    /***********************************************
    * ������ �ⲿ�ж�9 ~ 5����
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI9_5, EXTI9_5_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI9_5);
}

/*******************************************************************************
 * ��    �ƣ� AC_COUNTInit
 * ��    �ܣ� ��ʼ��PE0���ⲿ�жϹ��ܣ������������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2016-11-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void AC_COUNTInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    /***********************************************
    * ������ ������������������ų�ʼ��
    */   
    /* ��ʼ��GPIOD2ΪGPIO */   
	/* ʹ��GPIOD��Clockʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    /* Configure Button pin as input floating */
    /* ��ʼ��GPIOD2�ܽţ�����Ϊ������������ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    /***********************************************
    * ������ �й�����������ų�ʼ��
    */  
    /* ʹ��GPIOE��Clockʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* ��ʼ��GPIOE0�ܽţ�����Ϊ������������ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
	/* ��ʼ��GPIOE0Ϊ�ж�ģʽ */
    /* ��GPIOE0��Ӧ�Ĺܽ����ӵ��ڲ��ж��� */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line0); 
    
    /***********************************************
    * ������ �����ⲿ�ж������������ж�
    */    
    /***********************************************
    * ������ �ⲿ�ж�0 ~ 4����
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI0, EXTI0_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI0);
    
    /***********************************************
    * ������ �޹�����������ų�ʼ��
    */  
    /* ʹ��GPIOB��Clockʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Configure Button pin as input floating */
    /* ��ʼ��GPIOB8�ܽţ�����Ϊ������������ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* ��ʼ��GPIOA0Ϊ�ж�ģʽ */
    /* ��GPIOA0��Ӧ�Ĺܽ����ӵ��ڲ��ж��� */    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE , GPIO_PinSource9);
    
    /* Configure Button EXTI line */
    /* ��GPIOA0����Ϊ�ж�ģʽ���½��ش����ж� */    
    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    //NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line9); 
    
    /***********************************************
    * ������ �ⲿ�ж�9 ~ 5����
    */ 
    BSP_IntVectSet(BSP_INT_ID_EXTI9_5, EXTI9_5_ISRHandler);
    BSP_IntEn(BSP_INT_ID_EXTI9_5);
}

/*******************************************************************************
 * ��    �ƣ� EXTI0_IRQHandler
 * ��    �ܣ� �ⲿ�ж�0�������������������й�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2016-11-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void EXTI0_ISRHandler(void) 
{   
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /***********************************************
        * ������ 
        */ 
        if ( ( Mater.Energy.PrimVolt        == 0 ) ||
             ( Mater.Energy.PrimCurr        == 0 ) ||
             ( Mater.Energy.PowerFreq       == 0 ) ||
             ( Mater.Energy.PowerFactor     == 0 ) ) {
        } else {
            /***********************************************
            * ������ ���й�
            */ 
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)) {
                AC.PPpulse++;
                AC.PPpower_temp = AC.PPpulse;                                   //���峣��100000��25kV/150V---600A/1A(HXD1D����)
            /***********************************************
            * ������ ���й�
            */ 
            } else {
                AC.NPpulse++;
                AC.NPpower_temp = AC.NPpulse;                                   //���峣��100000��25kV/150V---600A/1A(HXD1D����)
            }
        }
        /* Clear the EXTI Line 0 */  
        EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*******************************************************************************
 * ��    �ƣ� EXTI1_IRQHandler
 * ��    �ܣ� �ⲿ�ж�1�������������������޹�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2016-11-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void EXTI9_5_ISRHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /***********************************************
        * ������ 
        */ 
        if ( ( Mater.Energy.PrimVolt        == 0 ) ||
             ( Mater.Energy.PrimCurr        == 0 ) ||
             ( Mater.Energy.PowerFreq       == 0 ) ||
             ( Mater.Energy.PowerFactor     == 0 ) ) {
        } else {
            /***********************************************
            * ������ ���޹�
            */ 
            if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2)) {
                AC.PQpulse++;
                AC.PQpower_temp = AC.PQpulse;                                   //���峣��100000��25kV/150V---600A/1A(HXD1D����)
            /***********************************************
            * ������ ���޹�
            */ 
            } else {
                AC.NQpulse++;
                AC.NQpower_temp = AC.NQpulse;                                   //���峣��100000��25kV/150V---600A/1A(HXD1D����)
            }
        }
        /* Clear the EXTI Line 9 */  
        EXTI_ClearITPendingBit(EXTI_Line9);
	}
}

/*******************************************************************************
 * ��    �ƣ� APP_MaterDispHandler
 * ��    �ܣ� �����ʾ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void APP_MaterDispHandler(INT08U *step)
{
	//static uint8_t step = 0;
    
    BSP_DispClrAll();
    switch ( *step ) {
    /***********************************************
    * ������ 
    */
    case 0:     uprintf("E1      "); 	            break;                      // 
    case 1:     uprintf("%8d",Mater.Energy.PPPower);		break;              //��ʾ���й�����
    case 2:     uprintf("E2      ");		        break;
    case 3:     uprintf("%8d",Mater.Energy.NPPower);*step = 7;		break;	    //��ʾ���й�����
    case 4:     uprintf("E3      ");		        break;
    case 5:     uprintf("%8d",Mater.Energy.PQPower);break;	                    //��ʾ���޹�����
    case 6:     uprintf("E4      ");		        break;
    case 7:     uprintf("%8d",Mater.Energy.NQPower);break;                     //��ʾ���޹�����
    case 8:     uprintf("U %6d",(uint32_t)AC.U_RMS);break;                     //��ʾ��ѹ
    case 9:     uprintf("A %6d",(uint32_t)AC.I_RMS);break;                     //��ʾ����
    case 10:    
        if ( AC.Power_Factor == 1.0 )
            uprintf("P   .1000",AC.Power_Factor);	                            //��ʾ����
        else if ( AC.Power_Factor == -1.0 )
            uprintf("P  -.1000",AC.Power_Factor);	                            //��ʾ����
        else if(AC.Power_Factor < 0 )
            uprintf("P  -.0%03d",abs(AC.Power_Factor * 1000.0));	            //��ʾ����
        else
            uprintf("P   .0%03d",(int)(AC.Power_Factor * 1000.0));	            //��ʾ����
        break;                                                                  //��ʾ����
            
    default:
        *step   = 0;
        break;
    }
    *step  += 1;
    if ( *step > 10 )
        *step   = 0;    
}

/*******************************************************************************
 * ��    �ƣ� APP_MaterCommHandler
 * ��    �ܣ� ��������ݴ�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
/***********************************************
* ������ 2015/12/07���ӣ����ڷ�MODBBUSͨ��
*        ��MODBUSͨ�ţ���֡ͷ֡β��ͨ�����ݴ���
*/
#if MB_NONMODBUS_EN == DEF_ENABLED
CPU_BOOLEAN APP_MaterCommHandler (MODBUS_CH  *pch)
{    
    uint8_t     fram_clr[4]     = {0};
	uint8_t     IDBuf[4]        = {0};
	uint8_t     clear_buf[24]   = {0};
	uint32_t    ID              = 0;
	uint32_t    utemp;
    	
	TIME        system; 
    
    /***********************************************
    * ������ �������ݵ�������
    */
    //CPU_SR_ALLOC();
    //CPU_CRITICAL_ENTER();
    //uint32_t  Len     = pch->RxBufByteCnt;
    //memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    //CPU_CRITICAL_EXIT();
    uint32_t  Len       = pch->RxBufByteCnt;
    INT08U  *DataBuf    = pch->RxFrameData;
    
    
    CPU_INT16U  head    = BUILD_INT16U(pch->RxBuf[1], pch->RxBuf[0]);
    CPU_INT16U  tail    = BUILD_INT16U(pch->RxBuf[pch->RxBufByteCnt-1],
                                       pch->RxBuf[pch->RxBufByteCnt-2]);
    /***********************************************
    * ������ ��ɳ�ϳ�DTUЭ�鴦��
    */
    if ( ( pch->RxFrameHead == head ) &&
         ( pch->RxFrameTail == tail ) ) {
        DataBuf = &pch->RxFrameData[8];
        //Len     = Ctrl.Com.Rd.Head.DataLen;        
        head    = BUILD_INT16U(DataBuf[1], DataBuf[0]);
    }
    
    /***********************************************
    * ������ 
    ���    ����       �ֽ��� ����˵��
    1       ֡ͷ       2      0xAA 0xAA
    2       ֡����     1      0x08 ֡���ȹ̶�Ϊ8�ֽ�
    3       ��������   2      0x00 0x01
    4       Ԥ��       2      0x00 0x00
    5       �ۼӺ�     1      ǰ7���ֽڵ��ۼӺ�
    */ 
    if ( 0xAAAA == head ) {
        Len            -= 1;
        int8_t chkSum   = GetSumChk(DataBuf,Len);
        
        if ( chkSum ==  DataBuf[Len] ) {
            pch->TxFrameData[0]     = 0xAA;
            pch->TxFrameData[1]     = 0xAA;
            pch->TxFrameData[2]     = 0x30;
            pch->TxFrameData[3]     = 0x00;
            pch->TxFrameData[4]     = 0x71;
            
            CPU_SR  cpu_sr;
            OS_CRITICAL_ENTER();
            memcpy(&pch->TxFrameData[5],(uint8_t *)&Mater.Energy, sizeof(StrEnergy));
            OS_CRITICAL_EXIT();
            
            pch->TxFrameData[45]    = 0x00;
            pch->TxFrameData[46]    = 0x00;
            
            pch->TxFrameData[47]    = GetSumChk(pch->TxFrameData,47);
            
            NMB_Tx((MODBUS_CH   *)pch,
                   (CPU_INT08U  *)pch->TxFrameData,
                   (CPU_INT16U   )48);
            return TRUE;
        }
    /***********************************************
    * ������ ���Բ���
    */
    } else if ( ( 0xAA55 == head ) && ( Len > 10 )) {
        /***********************************************
        * ������ ����Ƿ���֡β��û��֡β�����CRCУ��
        */
        if ( tail != 0x55AA  ) {
            uint16_t crc16_Cal = GetCrc16Chk1((uint8_t *)&DataBuf[0],9);
            uint16_t crc16_Rec = BUILD_INT16U(DataBuf[9],DataBuf[10]);
            if(crc16_Cal != crc16_Rec) {                
                return TRUE;
            }
        }
        switch(DataBuf[2])
        {
        /***********************************************
        * ������ �������к�
        */
        case 0xA1:
            ID = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(64 , (uint8_t *)&ID , 4);
            ReadFM24CL64(64 , (uint8_t *)&ID , 4);
            uartprintf(pch,"\r\n��װ��ID����Ϊ��%d\r\n" , ID);
            break;            
        /***********************************************
        * ������ ���û����ͺ�/������
        */
        case 0xA2:	
            Mater.LocoTyp   = BUILD_INT16U(DataBuf[3],DataBuf[4]);
            Mater.LocoNbr   = BUILD_INT16U(DataBuf[5],DataBuf[6]);
            WriteFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            WriteFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            ReadFM24CL64(68 ,  (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 ,  (uint8_t *)&Mater.LocoNbr, 2);
            uartprintf(pch,"\r\n��ǰ���ͣ�%d����ǰ���ţ�%d\r\n", Mater.LocoTyp,Mater.LocoNbr);
            break; 
        /***********************************************
        * ������ ����ʱ��
        */
        case 0xA3:
            system.Year     =DataBuf[3];
            system.Month    =DataBuf[4];
            system.Day      =DataBuf[5];
            system.Hour     =DataBuf[6];
            system.Min      =DataBuf[7];
            system.Sec      =DataBuf[8];
            WriteTime(system);
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n��ǰʱ��Ϊ��20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);
            break;
        // �������У׼˳��
        /***********************************************
        * ������ 1����ѹֱ�����У׼
        */
//        case 0xA4:
//            CS5463_AC_Adjust(pch,VDCOFFC);												
//            break;
//        /***********************************************
//        * ������ 2������ֱ�����У׼
//        */
//        case 0xA5:
//            CS5463_AC_Adjust(pch,IDCOFFC);										
//            break;
//        /***********************************************
//        * ������ 4����ѹ�������У׼
//        */
//        case 0xA6:
//            CS5463_AC_Adjust(pch,VACOFFC);												
//            break;
//        /***********************************************
//        * ������ 5�������������У׼
//        */
//        case 0xA7:
//            CS5463_AC_Adjust(pch,IACOFFC);											
//            break;
//        /***********************************************
//        * ������ 3����ѹֱ������У׼��������У׼��
//        */
//        case 0xA8:
//            CS5463_AC_Adjust(pch,VACGAINC);									
//            break;
//        /***********************************************
//        * ������ 3������ֱ������У׼��ֱ����У׼��
//        */
//        case 0xA9:
//            CS5463_AC_Adjust(pch,IACGAINC);									
//            break;            
        /***********************************************
        * ������ �����������
        */
        case 0xAB:
            WriteFM24CL64(32, clear_buf, 24);
            uartprintf(pch,"\r\n�������������\n");
            break;
        /***********************************************
        * ������ ���й�����
        */
        case 0xAC:
            AC.PPpulse = 0;
            AC.PPpower_base = 0;
            AC.PPpower_temp = 0;
            uartprintf(pch,"\r\n���й�����������\n");
            break;
        /***********************************************
        * ������ ���й�����
        */
        case 0xAD:
            AC.NPpulse = 0;
            AC.NPpower_base = 0;
            AC.NPpower_temp = 0;
            uartprintf(pch,"\r\n���й�����������\n");
            break;
        /***********************************************
        * ������ ���޹�����
        */
        case 0xAE:
            AC.PQpulse = 0;
            AC.PQpower_base = 0;
            AC.PQpower_temp = 0;
            uartprintf(pch,"\r\n���޹�����������\n");
            break;
        /***********************************************
        * ������ ���޹�����
        */
        case 0xAF:
            AC.NQpulse = 0;
            AC.NQpower_base = 0;
            AC.NQpower_temp = 0;
            uartprintf(pch,"\r\n���޹�����������\n");
            break;            
//        case 0x5A:
//            CS5463_AC_Adjust(pch,P_rate_ADD);
//            break;
//        case 0x5B:
//            CS5463_AC_Adjust(pch,P_rate_SUB);
//            break;
//        case 0x5C:
//            CS5463_AC_Adjust(pch,P_rate_DEL);
//            break;
//        case 0x5d:
//            CS5463_AC_Adjust(pch,P_rate_SET);
//            break;
        /***********************************************
        * ������ ����ԭ�ߵ�ѹ
        */
        case 0x5E:
            AC.U_SCL = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(88 , (uint8_t *)&AC.U_SCL , 4);
            ReadFM24CL64(88 , (uint8_t *)&AC.U_SCL , 4);
            uartprintf(pch,"\r\nԭ�߶��ѹ����Ϊ��%dV\r\n" , AC.U_SCL);
            break;
        /***********************************************
        * ������ ����ԭ�ߵ���
        */
        case 0x5F:
            AC.I_SCL = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(92 , (uint8_t *)&AC.I_SCL , 4);
            ReadFM24CL64(92 , (uint8_t *)&AC.I_SCL , 4);
            uartprintf(pch,"\r\nԭ�߶��������Ϊ��%dA\r\n" , AC.I_SCL);
            break;
        /***********************************************
        * ������ ��ѹΪ��ķ�ֵ������ѹС�ڸ�ֵʱ��ѹ����Ϊ��
        */
        case 0x50:
            AC.U_O = BUILD_INT32U(DataBuf[3], DataBuf[4], DataBuf[5], DataBuf[6]);
            WriteFM24CL64(96 , (uint8_t *)&AC.U_O , 4);
            ReadFM24CL64(96 , (uint8_t *)&AC.U_O , 4);
            uartprintf(pch,"\r\n��ѹ��ֵ����Ϊ��%fV\r\n" , AC.U_O);
            break;
        /***********************************************
        * ������ ��ȡID
        */
        case 0x51:				
            ReadFM24CL64(64 , (uint8_t *)&ID , 4);
            uartprintf(pch,"\r\n��װ��IDΪ��%d\r\n" , ID);
            break;
        /***********************************************
        * ������ 
        */
        case 0x52:
            ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            if( ( Mater.LocoTyp > 0 ) &&
                ( Mater.LocoTyp < 10000 ) &&
                ( Mater.LocoNbr > 0 ) && 
                ( Mater.LocoNbr < 10000 ) ) {
                uartprintf(pch,"\r\n��ǰ���ͣ�%d�� ��ǰ���ţ�%04d\r\n" , Mater.LocoTyp,Mater.LocoNbr);
            } else {
                uartprintf(pch,"\r\nδ����װ�����ͣ�\r\n");
            }
            break;            
        /***********************************************
        * ������ ��ȡʱ��
        */
        case 0x53:
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n��ǰʱ��Ϊ��20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);				
            break;            
        /***********************************************
        * ������ ͨ������ת��FLASH�е�������Ч����
        */
        case 0x54:
            GetTime((TIME *)&recordsfr.Time[0]);
            uartprintf(pch,"\r\n��ǰʱ��Ϊ��20%02d-%02d-%02d  %02d:%02d:%02d", 
                       recordsfr.Time[0], recordsfr.Time[1], recordsfr.Time[2],
                       recordsfr.Time[3], recordsfr.Time[4], recordsfr.Time[5]);
            
            ReadFM24CL64(68 , (uint8_t *)&Mater.LocoTyp, 2);
            ReadFM24CL64(70 , (uint8_t *)&Mater.LocoNbr, 2);
            if( ( Mater.LocoTyp > 0 ) &&
                ( Mater.LocoTyp < 10000 ) &&
                ( Mater.LocoNbr > 0 ) && 
                ( Mater.LocoNbr < 10000 ) ) {
                uartprintf(pch,"\r\n��ǰ���ͣ�%d�� ��ǰ���ţ�%04d\r\n" , Mater.LocoTyp,Mater.LocoNbr);
            } else {
                uartprintf(pch,"\r\nδ����װ�����ͣ�\r\n");
            }
            
            //DOWNLOAD(g_Flash_Adr/sizeof(Mater));
            if(g_Flash_Adr == 0)
                uartprintf(pch,"\r\nFLASHоƬ������Ч���ݣ�\r\n");
            break;
            
        case 0x55:
            uartprintf(pch,"\r\nFLASHоƬ������............\r\n");
            uartprintf(pch,"\r\n��Լ��Ҫ30�룬�����ĵȴ���\r\n");
            MX25L3206_Erase(0, 4096);
            uartprintf(pch,"\r\nFLASHоƬ�Ѳ���\r\n");
            break;
            
        case 0x56:
            uartprintf(pch,"\r\nFRAMоƬ������...\r\n");
            WriteFM24CL64(60, fram_clr, 4);
            g_Flash_Adr = 0;
            Mater.Record.Head  = 0;
            Mater.Record.Tail  = 0;
            Mater.RecordNbr  = 0;
            WriteFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
            WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
            WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
            uartprintf(pch,"\r\nFRAMоƬ�Ѳ���\r\n");
            break;
        case 0x57:{
            float x =(float)((pch->RxFrameData[4] >> 4) & 0x0f) * 1
                    + (float)((pch->RxFrameData[4] >> 0) & 0x0f) * 0.1
                    + (float)((pch->RxFrameData[5] >> 4) & 0x0f) * 0.01
                    + (float)((pch->RxFrameData[5] >> 0) & 0x0f) * 0.001
                    + (float)((pch->RxFrameData[6] >> 4) & 0x0f) * 0.0001
                    + (float)((pch->RxFrameData[6] >> 0) & 0x0f) * 0.00001
                    + (float)((pch->RxFrameData[7] >> 4) & 0x0f) * 0.000001
                    + (float)((pch->RxFrameData[7] >> 0) & 0x0f) * 0.0000001
                    + (float)((pch->RxFrameData[8] >> 4) & 0x0f) * 0.00000001
                    + (float)((pch->RxFrameData[8] >> 0) & 0x0f) * 0.000000001;
            
            AC.U_K = x; 
            WriteFM24CL64(160, (uint8_t *)&AC.U_K, 4);
            uartprintf(pch,"\r\n��ѹ����б�� = %f\n", AC.U_K);
            } break;
        case 0x58:{
            float x =(float)((pch->RxFrameData[4] >> 4) & 0x0f) * 1
                    + (float)((pch->RxFrameData[4] >> 0) & 0x0f) * 0.1
                    + (float)((pch->RxFrameData[5] >> 4) & 0x0f) * 0.01
                    + (float)((pch->RxFrameData[5] >> 0) & 0x0f) * 0.001
                    + (float)((pch->RxFrameData[6] >> 4) & 0x0f) * 0.0001
                    + (float)((pch->RxFrameData[6] >> 0) & 0x0f) * 0.00001
                    + (float)((pch->RxFrameData[7] >> 4) & 0x0f) * 0.000001
                    + (float)((pch->RxFrameData[7] >> 0) & 0x0f) * 0.0000001
                    + (float)((pch->RxFrameData[8] >> 4) & 0x0f) * 0.00000001
                    + (float)((pch->RxFrameData[8] >> 0) & 0x0f) * 0.000000001;
            
            AC.I_K = x; 
            WriteFM24CL64(164, (uint8_t *)&AC.I_K, 4);
            uartprintf(pch,"\r\n��������б�� = %f\n", AC.I_K);
            } break;
        case 0x00: 
            utemp   = BUILD_INT32U(DataBuf[3],DataBuf[4],DataBuf[5],DataBuf[6]); 
            if ( utemp < 10 )
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
            else if ( utemp > 300 )
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * 60;
            else
                Mater.Record.RecordTime    = OS_TICKS_PER_SEC * utemp;
            
            WriteFM24CL64(84, (uint8_t *)&Mater.Record.RecordTime, 4);
            ReadFM24CL64(84 , (uint8_t *)&utemp, 4);
            osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN ,Mater.Record.RecordTime);
            uartprintf(pch,"\r\n�������ݼ�¼/���ͼ��ʱ��Ϊ��%d��\r\n",utemp / OS_TICKS_PER_SEC);
            break;
        case 0x01:    
            ReadFM24CL64(84 , (uint8_t *)&utemp, 4);
            uartprintf(pch,"\r\n��ǰ���ݼ�¼/���ͼ��ʱ��Ϊ��%d��\r\n",utemp / OS_TICKS_PER_SEC);
            break;
        case 0xf0:
            SystemReset();							    //ϵͳ����
            break;
        default:
            break;
        }
    }
    /***********************************************
    * ������ ���ô������ݴ���ص�����
    */ 
    return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� APP_CSNC_CommHandler
 * ��    �ܣ� �������ݴ�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
CPU_BOOLEAN APP_CSNC_CommHandler (MODBUS_CH  *pch)
{   
    if ( pch->PortNbr == Ctrl.Dtu.pch->PortNbr ) {
//        if ( GetRecvFrameNbr() == Ctrl.Com.Rd.Head.PacketSn ) {
//            /***********************************************
//            * ������ �����ʱ�ϴ���־
//            */
//            SetSendFrameNbr();
//            extern void UpdateRecordPoint(uint8_t storeflag);
//            UpdateRecordPoint(1);
//            pch->StatNoRespCtr  = 0;
//            
//            osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
//        }
    } else {
        /***********************************************
        * ������ �����ݴ�������ͽṹ
        */ 
        APP_MaterCommHandler(pch);
    }
    
    /***********************************************
    * ������ ���ڽ���COMMģ�����Ϣ������
    */
    Ctrl.Com.ConnectTimeOut    = 0;                // ��ʱ����������
    Ctrl.Com.ConnectFlag       = TRUE;             // ת���ӳɹ���־
    
    return TRUE;
}
#endif

void SavePowerData(void)
{
    /***********************************************
    * ������ �������й��������ݵ�FRAM
    *       ���й�����    1kvarh      99999999 kvarh
    */
    if ( recordsfr.PPpower_NUM != AC.PPpower_NUM ) {	
        WriteFM24CL64(0, (uint8_t *)&AC.PPpower_NUM,4);
        WriteFM24CL64(4, (uint8_t *)&AC.PPpower_NUM,4);
        WriteFM24CL64(8, (uint8_t *)&AC.PPpower_NUM,4);
    }
    /***********************************************
    * ������ ���渺�й��������ݵ�FRAM
    *        ���й�����    1kvarh      99999999 kvarh 
    */ 
    if ( recordsfr.NPpower_NUM != AC.NPpower_NUM ) { 
        WriteFM24CL64(12, (uint8_t *)&AC.NPpower_NUM,4);
        WriteFM24CL64(16, (uint8_t *)&AC.NPpower_NUM,4);
        WriteFM24CL64(20, (uint8_t *)&AC.NPpower_NUM,4);
    }
    /***********************************************
    * ������ ���渺�й��������ݵ�FRAM
    *        ���޹�����    1kvarh      99999999 kvarh 
    */
    if ( recordsfr.PQpower_NUM != AC.PQpower_NUM ) {
        WriteFM24CL64(100, (uint8_t *)&AC.PQpower_NUM,4);
        WriteFM24CL64(104, (uint8_t *)&AC.PQpower_NUM,4);
        WriteFM24CL64(108, (uint8_t *)&AC.PQpower_NUM,4);
    }
    /***********************************************
    * ������ ���渺�й��������ݵ�FRAM
    *       ���޹�����    1kvarh      99999999 kvarh 
    */ 
    if ( recordsfr.NQpower_NUM != AC.NQpower_NUM ) {
        WriteFM24CL64(112, (uint8_t *)&AC.NQpower_NUM,4);			  
        WriteFM24CL64(116, (uint8_t *)&AC.NQpower_NUM,4);
        WriteFM24CL64(120, (uint8_t *)&AC.NQpower_NUM,4);
    }    
    
    recordsfr.PPpower_NUM       = AC.PPpower_NUM;
    recordsfr.NPpower_NUM       = AC.NPpower_NUM;
    recordsfr.PQpower_NUM       = AC.PQpower_NUM;
    recordsfr.NQpower_NUM       = AC.NQpower_NUM;
}

void PVD_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line16);            //���ж�
    
    //num = BKP_ReadBackupRegister(BKP_DR10);
    //num++;   
    
    //�û���ӽ���������봦    
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//ʹ��PWR��BKP����ʱ��
    //PWR_BackupAccessCmd(ENABLE);//ʹ�ܺ󱸼Ĵ�������
    
    //BKP_WriteBackupRegister(BKP_DR10, (u8)num);//��������
    
    WriteFM24CL64(0, (uint8_t *)&AC.PPpower_NUM,4);
    WriteFM24CL64(4, (uint8_t *)&AC.PPpower_NUM,4);
    WriteFM24CL64(8, (uint8_t *)&AC.PPpower_NUM,4);
    
    //�����й���������ת�浽������
    WriteFM24CL64(12, (uint8_t *)&AC.NPpower_NUM,4);			  //���ֽ���ǰ�����ֽ��ں�
    WriteFM24CL64(16, (uint8_t *)&AC.NPpower_NUM,4);
    WriteFM24CL64(20, (uint8_t *)&AC.NPpower_NUM,4);                
    
    WriteFM24CL64(100, (uint8_t *)&AC.PQpower_NUM,4);
    WriteFM24CL64(104, (uint8_t *)&AC.PQpower_NUM,4);
    WriteFM24CL64(108, (uint8_t *)&AC.PQpower_NUM,4);
    
    //�����й���������ת�浽������
    WriteFM24CL64(112, (uint8_t *)&AC.NQpower_NUM,4);			  //���ֽ���ǰ�����ֽ��ں�
    WriteFM24CL64(116, (uint8_t *)&AC.NQpower_NUM,4);
    WriteFM24CL64(120, (uint8_t *)&AC.NQpower_NUM,4);
}

void PWR_PVD_Init(void)
{  
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;    
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);     //ʹ��PWRʱ��    
    
    //NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;           //ʹ��PVD���ڵ��ⲿ�ж�ͨ��
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�1
    //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //�����ȼ�0
    //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //ʹ���ⲿ�ж�ͨ��
    //NVIC_Init(&NVIC_InitStructure);
            
    /***********************************************
    * ������ �ⲿ�ж�0 ~ 4����
    */ 
    BSP_IntVectSet(BSP_INT_ID_PVD, PVD_IRQHandler);
    BSP_IntEn(BSP_INT_ID_PVD);    
    
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;             //PVD���ӵ��ж���16��
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //ʹ���ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //��ѹ���ڷ�ֵʱ�����ж�
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;               //ʹ���ж���
    EXTI_Init(&EXTI_InitStructure);                         //��ʼ   
    
    PWR_PVDLevelConfig(PWR_PVDLevel_2V9);                   //�趨��ط�ֵ
    PWR_PVDCmd(ENABLE);                                     //ʹ��PVD    
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif