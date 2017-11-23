/*******************************************************************************
 *   Filename:       app_task_dtu.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ˫��ѡ�� dtu �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Dtu �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� DTU �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_DTU_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_DTU_STK_SIZE ����С
 *
 *   Notes:
 *     				E-mail: shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
#include <app_comm_protocol.h>
#include <bsp_flash.h>
#include <iap.h>
#include <xprintf.h>
#include <power_macro.h>
#include <mx25.h>
#include <FM24CL64.h>
#include <crccheck.h>
#include <ds3231.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_dtu__c = "$Id: $";
#endif

#define APP_TASK_DTU_EN     DEF_ENABLED
#if APP_TASK_DTU_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
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
static  OS_TCB      AppTaskDtuTCB;
OS_Q                DTU_RxQ;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskDtuStk[ APP_TASK_DTU_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void    AppTaskDtu                 (void *p_arg);
static void    APP_DtuInit                (void);
       void     App_McuStatusInit          (void);
       void     ReportDevStatusHandle      (void);
       void     InformDtuConfigMode        (u8 mode);
       
       void     App_SendDataFromHistory     (void);

INT08U          APP_DtuRxDataDealCB        (MODBUS_CH  *pch);
INT08U          IAP_DtuRxDataDealCB        (MODBUS_CH  *pch);
/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern void     uartprintf              (MODBUS_CH  *pch,const char *fmt, ...);

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskDtuCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskDtuCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskDtuTCB,                      // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Dtu",                      // ��������
                 (OS_TASK_PTR ) AppTaskDtu,                         // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                                  // ����������
                 (OS_PRIO     ) APP_TASK_DTU_PRIO,                 // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskDtuStk[0],                   // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_DTU_STK_SIZE / 10,         // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_DTU_STK_SIZE,              // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 0u,                                 // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                                 // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                                  // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // �����ջ��������
                                OS_OPT_TASK_STK_CLR),               // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                               // ָ���������ָ�룬���ڴ����������

}

/*******************************************************************************
 * ��    �ƣ� AppTaskDtu
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskDtu (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly     = CYCLE_TIME_TICKS;
    OS_TICK     ticks;
        
    APP_DtuInit();
    
    BSP_OS_TimeDly(OS_TICKS_PER_SEC / 2);
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_DTU,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);
        
        /***********************************************
        * ������ �ȴ�DTU���ݽ�����Ϣ����
        *
        OS_MSG_SIZE p_msg_size;
        
        MODBUS_CH *pch = 
       (MODBUS_CH *)OSQPend ((OS_Q*)&DTU_RxQ,
                    (OS_TICK       )dly,
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,//OS_OPT_PEND_NON_BLOCKING,
                    (OS_MSG_SIZE  *)&p_msg_size,
                    (CPU_TS       *)0,
                    (OS_ERR       *)&err);
        // DTU�յ���Ϣ
        if ( OS_ERR_NONE == err ) {
            // ��Ϣ����
            APP_DtuRxDataDealCB(pch);
        }
        *//***********************************************
        * ������ �ȴ�DTU������־λ
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.DtuEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.DtuEvtFlag,
                    ( OS_TICK      ) dly,
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks   = OSTimeGet(&terr);                        // ��ȡ��ǰOSTick
        
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
            
            /***********************************************
            * ������ DTU��λ
            */
            } 
            if ( flags & COMM_EVT_FLAG_RESET ) {
                flagClr |= COMM_EVT_FLAG_RESET;
                
            /***********************************************
            * ������ DTU����
            */
            } 
            if ( flags & COMM_EVT_FLAG_CONNECT ) {
                flagClr |= COMM_EVT_FLAG_CONNECT;
                
            /***********************************************
            * ������ DTU���ڽ�������
            */
            } 
            if ( flags & COMM_EVT_FLAG_RECV ) {
                flagClr |= COMM_EVT_FLAG_RECV;
                APP_DtuRxDataDealCB(Ctrl.Dtu.pch);
            /***********************************************
            * ������ �ϱ���Ϣ
            */
            } 
            if ( flags & COMM_EVT_FLAG_REPORT ) {
                /***********************************************
                * ������ ������ʷ���ݵ�������
                */
                App_SendDataFromHistory();
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
            } else if ( flags & COMM_EVT_FLAG_CONFIG ) {
                flagClr |= COMM_EVT_FLAG_CONFIG;
            
            /***********************************************
            * ������ IAP����
            */
            } 
            if ( flags & COMM_EVT_FLAG_IAP_END ) {
                flagClr |= COMM_EVT_FLAG_IAP_END;
                
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
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.DtuEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
            
        /***********************************************
        * ������ �����ʱ������һ��������
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            
        }
//next:
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
 * ��    �ƣ� APP_DtuInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void APP_DtuInit(void)
{
    OS_ERR err;
    
    /***********************************************
    * ������ �����¼���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.DtuEvtFlagGrp,
                 ( CPU_CHAR     *)"App_DtuFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.DtuEvtFlag = COMM_EVT_FLAG_HEART       // ����������
                        + COMM_EVT_FLAG_RESET       // DTU��λ
                        + COMM_EVT_FLAG_CONNECT     // DTU����
                        + COMM_EVT_FLAG_RECV        // ���ڽ���
                        + COMM_EVT_FLAG_REPORT      // ���ڷ���
                        + COMM_EVT_FLAG_CLOSE       // �Ͽ�
                        + COMM_EVT_FLAG_TIMEOUT     // ��ʱ
                        + COMM_EVT_FLAG_CONFIG      // ����
                        + COMM_EVT_FLAG_IAP_START   // IAP��ʼ
                        + COMM_EVT_FLAG_IAP_END;    // IAP����
            
    OSQCreate ( (OS_Q        *)&DTU_RxQ,
                (CPU_CHAR    *)"RxQ",
                (OS_MSG_QTY   ) OS_CFG_INT_Q_SIZE,
                (OS_ERR      *)&err);
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_DTU;
}

/*******************************************************************************
 * ��    �ƣ� APP_DtuRxDataDealCB
 * ��    �ܣ� �������ݴ���ص���������MB_DATA.C����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��    �ߣ� ������
 * �������ڣ� 2016-01-04
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� 
 *******************************************************************************/
INT08U APP_DtuRxDataDealCB(MODBUS_CH  *pch)
{
   
    return TRUE;
}

/*******************************************************************************
* ��    �ƣ� UpdateRecordPoint
* ��    �ܣ� 
* ��ڲ����� ��
* ���ڲ����� ��
* ��  ���ߣ� wumingshen.
* �������ڣ� 2017-02-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void UpdateRecordPoint(uint8_t storeflag)
{
    uint32_t    size    = sizeof(StrMater) - sizeof(StrMaterRecord);             // ��ȡ��¼����
    Mater.Record.Tail  += size;
    
    if ( Mater.Record.Tail >= MAX_ADDR ) {
        Mater.Record.Tail = 0; 
    } else if ( Mater.Record.Tail >= Mater.Record.Head ) {
        Mater.Record.Tail  = Mater.Record.Head; 
    }
    if ( storeflag ) {
        //WriteFM24CL64(72, (uint8_t *)&Mater.Head, 4);    
        WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4); 
        //WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    }
}

/*******************************************************************************
 * ��    �ƣ� App_SaveDataToHistory
 * ��    �ܣ�
 * ��ڲ�����
 * ���ڲ����� ��
 * �� �� �ߣ� ������
 * �������ڣ� 2017-02-7
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void  App_SaveDataToHistory( void )
{
    /**************************************************
    * ������ ���浱ǰ����
    */
    uint32_t    size = sizeof(StrMater) - sizeof(StrMaterRecord);             // ���ݳ���
    CPU_SR      cpu_sr;
    
    uint8_t time[6];
    GetTime((TIME *)&time[0]);
     
    OS_CRITICAL_ENTER(); 
    
    if ( Ctrl.Tax.ConnectFlag == FALSE ) { 
        /***************************************************
        * ������ ��ȡʱ��
        */
        Mater.Tax.Tax2.LKJTime.Year  = time[0];
        Mater.Tax.Tax2.LKJTime.Mon   = time[1];
        Mater.Tax.Tax2.LKJTime.Day   = time[2];
        Mater.Tax.Tax2.LKJTime.Hour  = time[3];
        Mater.Tax.Tax2.LKJTime.Min   = time[4];
        Mater.Tax.Tax2.LKJTime.Sec   = time[5];
            
        Mater.TaxType   = 0;
        Mater.TeZhengMa = 0;
        
        memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
    } else {
        /**************************************************
        * ������ ����TAX�����ݣ�
        */
        switch ( Ctrl.Tax.Dat.buf1[0] ) {
        /***********************************************
        * ������ TAX2015Э�飺=0XF0��TAX2�豸��
        */
        case 0xFA:
        /***********************************************
        * ������ TAX2015Э�飺=0XFA��TAX3�豸
        */
        case 0xF0:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax3.Head.SheBeiLeiXing;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax3.Head.TeZhengMa.Dat;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        /***********************************************
        * ������ ��TAX2Э��
        */
        case 0x38:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax2.Record.Addr;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax2.CheZhanHaoKuoChong;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        /***********************************************
        * ������ ��TAX2Э��
        */
        case 0x39:
            Mater.TaxType   = Ctrl.Tax.Dat.Tax2.Record.Addr;
            Mater.TeZhengMa = Ctrl.Tax.Dat.Tax2.CheZhanHaoKuoChong;
            memcpy((uint8_t *)&Mater.Tax.buf1[24],(uint8_t *)&Ctrl.Tax.Dat.buf1[32],40);
            break;
        default:
            Mater.TaxType   = 0;
            Mater.TeZhengMa = 0;
            memcpy((uint8_t *)&Mater.Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
            break;
        }
    }
    
    Mater.ErrTax    = Ctrl.Tax.ConnectFlag;
	Mater.chk       = GetCrc16Chk1((uint8_t *)&Mater,size-2);       // ��ȡ�洢У����
    
    OS_CRITICAL_EXIT();
    
    SaveOneREcord(Mater.Record.Head,(uint8_t *)&Mater,size);        // ��������
	Mater.Record.Head += size;                                      // ����ͷָ��
    
    if ( Mater.Record.Head >= MAX_ADDR ) {                          // ͷָ�����
        Mater.Record.Head = 0;                                      // ���ָ�����
        if ( Mater.Record.Tail == 0 )                               // ���βָ���Ƿ�Ϊ��
            Mater.Record.Tail  += size;                             // Ϊ�����βָ��
    } else {                                                        // ͷָ��û�����
        if ( Mater.Record.Tail >= Mater.Record.Head ) {             // βָ����ڵ���ͷָ��
            Mater.Record.Tail  += size;                             // ����βָ��
            if ( Mater.Record.Tail >= MAX_ADDR )                    // βָ�����
                Mater.Record.Tail = 0;                              // βָ�����
        }
    }
    
    Mater.RecordNbr++;                                              // ��¼��+1
    
    /**************************************************
    * ������ ���浱ǰ��¼�ź�����ָ��
    */
	WriteFM24CL64(72, (uint8_t *)&Mater.Record.Head, 4); 
	WriteFM24CL64(76, (uint8_t *)&Mater.Record.Tail, 4);
	WriteFM24CL64(80, (uint8_t *)&Mater.RecordNbr, 4);
    
    osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
}

/*******************************************************************************
 * ��    �ƣ� App_SendDataFromHistory
 * ��    �ܣ� ����ת��,���ݵ��ֽ���ǰ�����ֽ��ں� 
 * ��ڲ�����
 * ���ڲ����� ��
 * �� �� �ߣ� ������
 * �������ڣ� 2017-02-7
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void App_SendDataFromHistory(void)
{	    
    /***********************************************
    * ������ �������������ģʽ
    */
    if ( ( Iap.Status != IAP_STS_DEF ) && 
         ( Iap.Status != IAP_STS_SUCCEED ) &&
         ( Iap.Status != IAP_STS_FAILED ) ) {
        return;
    }
    
    
    Ctrl.Dtu.pack->SrcAddr  = 0x81;
    Ctrl.Dtu.pack->DstAddr  = 0xCB;
    Ctrl.Dtu.pack->Pack.Udat.VerHigh  = PROTOCOL_VER_HIG;
    Ctrl.Dtu.pack->Pack.Udat.VerLow   = PROTOCOL_VER_LOW;
    Ctrl.Dtu.pack->Pack.Udat.ChkType  = RS485_CHK_CRC;
    Ctrl.Dtu.pack->Data     = (CPU_INT08U     *)&History;
    Ctrl.Dtu.pack->Len      = (CPU_INT08U      ) 128;
    
    CSNC_SendData( (MODBUS_CH      *) Ctrl.Dtu.pch,                     // DistAddr,
                   (StrNcProtocol  *) Ctrl.Dtu.pack);                    // DataLen 
     

    
    /***********************************************
    * ������ ͷָ����ǰ�棨������δ����
    */
    if ( Mater.Record.Head == Mater.Record.Tail ) {
        osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN ,  OS_TICKS_PER_SEC * 10);
    /***********************************************
    * ������ ͷָ���ں��棨������������
    */
    } else {
        uint32_t    size    = sizeof(StrMater) - sizeof(StrMaterRecord);        // ��ȡ��¼����
		MX25L3206_RD(Mater.Record.Tail, size,(uint8_t*)&History);  // ��ȡһ����¼
        
        /**************************************************
        * ������ ����У��
        */
		uint16_t    CRC_sum1 = GetCrc16Chk1((uint8_t*)&History,size-2);		
		uint16_t    CRC_sum2 = History.chk;
        uint32_t    timeout  = OS_TICKS_PER_SEC * 5;
        /**************************************************
        * ������ ���ͼ�¼��������
        */
		if(CRC_sum1 == CRC_sum2) {						    //�����ۼӺͼ���
            if (Ctrl.Dtu.pch->RxBufByteCtr == 0) {
                Ctrl.Dtu.pack->SrcAddr  = 0x81;
                Ctrl.Dtu.pack->DstAddr  = 0xCB;
                Ctrl.Dtu.pack->Pack.Udat.VerHigh  = PROTOCOL_VER_HIG;
                Ctrl.Dtu.pack->Pack.Udat.VerLow   = PROTOCOL_VER_LOW;
                Ctrl.Dtu.pack->Pack.Udat.ChkType  = RS485_CHK_CRC;
                Ctrl.Dtu.pack->Data     = (CPU_INT08U     *)&History;
                Ctrl.Dtu.pack->Len      = (CPU_INT08U      ) size;
                
                CSNC_SendData( (MODBUS_CH      *) Ctrl.Dtu.pch,                     // DistAddr,
                               (StrNcProtocol  *) Ctrl.Dtu.pack);                    // DataLen 
                 
                Ctrl.Dtu.pch->StatNoRespCtr++;
                if ( Ctrl.Dtu.pch->StatNoRespCtr < 10 ) {
                    timeout     = OS_TICKS_PER_SEC * 10; 
                } else if ( Ctrl.Dtu.pch->StatNoRespCtr < 50 ) {
                    timeout     = OS_TICKS_PER_SEC * 20; 
                } else if ( Ctrl.Dtu.pch->StatNoRespCtr < 100 ) {
                    timeout     = OS_TICKS_PER_SEC * 30; 
                } else {                
                    timeout     = OS_TICKS_PER_SEC * 60; 
                }
                
                //uartprintf(Ctrl.Com.pch,"\r\n���ͼ�¼���ݣ�%d",Mater.RecordNbr);
            }
            osal_start_timerRl( OS_TASK_ID_TMR, OS_EVT_TMR_MIN, timeout );
        } else {
            UpdateRecordPoint(0);
            
            osal_set_event( OS_TASK_ID_TMR, OS_EVT_TMR_MIN);
            //timeout = OS_TICKS_PER_SEC * 1;
        }
        
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif