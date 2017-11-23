/*******************************************************************************
 *   Filename:       app_task_comm.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ˫��ѡ�� comm �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Comm �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� COMM �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_COMM_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_COMM_STK_SIZE ����С
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

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_comm__c = "$Id: $";
#endif

#define APP_TASK_COMM_EN     DEF_ENABLED
#if APP_TASK_COMM_EN == DEF_ENABLED
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
static  OS_TCB   AppTaskCommTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskCommStk[ APP_TASK_COMM_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/***********************************************
* ������ ��ʱ������
*/
OS_TMR          OSTmr1;                         //��ʱ��1

/***********************************************
* ������ ��ʱ���ص�����
*/
void            OSTmr1_callback             (OS_TMR *ptmr,void *p_arg);

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void    AppTaskComm                 (void *p_arg);
static void    APP_CommInit                (void);
        void    App_McuStatusInit           (void);
        void    ReportDevStatusHandle       (void);
        void    InformCommConfigMode        (u8 mode);
static void    App_ModbusInit              (void);

INT08U          APP_CommRxDataDealCB        (MODBUS_CH  *pch);
INT08U          IAP_CommRxDataDealCB        (MODBUS_CH  *pch);
/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskCommCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskCommCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskCommTCB,                     // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Comm",                     // ��������
                 (OS_TASK_PTR ) AppTaskComm,                        // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                                  // ����������
                 (OS_PRIO     ) APP_TASK_COMM_PRIO,                 // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskCommStk[0],                  // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE / 10,        // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_COMM_STK_SIZE,             // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 0u,                                 // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                                 // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                                  // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // �����ջ��������
                                OS_OPT_TASK_STK_CLR),               // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                               // ָ���������ָ�룬���ڴ����������

}

/*******************************************************************************
 * ��    �ƣ� AppTaskComm
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskComm (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly     = CYCLE_TIME_TICKS;
    OS_TICK     ticks;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_CommInit();
    
    BSP_OS_TimeDly(OS_TICKS_PER_SEC * 2);
    /***********************************************
    * ������ 2016/01/08���ӣ����ڷ�MODBBUS IAP����ͨ��
    */
#if defined     (IMAGE_A)
#elif defined   (IMAGE_B)
#else
    u8 status   = IAP_GetStatus();
    if ( status != IAP_STS_PROGRAMING ) 
        IAP_JumpToApp();
#endif
        
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDT_FLAG_COMM,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);        
        
        /***********************************************
        * ������ �ȴ�COMM������־λ
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.CommEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.CommEvtFlag,
                    ( OS_TICK      ) dly,
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks   = OSTimeGet(&terr);                        // ��ȡ��ǰOSTick
        
        /***********************************************
        * ������ ����ͨѶ��ʱ���������յ�������ʱ�����㣬�˴�ֻ����
        */
        if ( Ctrl.Comm.ConnectTimeOut < 10 ) {
            Ctrl.Comm.ConnectTimeOut++;
        } else {
            
        }
        /***********************************************
        * ������ 2016/01/08���ӣ����ڷ�MODBBUS IAP����ͨ��
        */
#if MB_IAPMODBUS_EN == DEF_ENABLED
        /***********************************************
        * ������ ������������У�3���û�������˳�����
        */
        IAP_Exit(3);
#endif        
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
            * ������ COMM��λ
            */
            } else if ( flags & COMM_EVT_FLAG_RESET ) {
                flagClr |= COMM_EVT_FLAG_RESET;
                
            /***********************************************
            * ������ COMM����
            */
            } else if ( flags & COMM_EVT_FLAG_CONNECT ) {
                flagClr |= COMM_EVT_FLAG_CONNECT;
                
            /***********************************************
            * ������ ���ڽ���
            */
            } else if ( flags & COMM_EVT_FLAG_RECV ) {
                flagClr |= COMM_EVT_FLAG_RECV;
                //APP_CommRxDataDealCB(Ctrl.Comm.pch);
            /***********************************************
            * ������ �ϱ���Ϣ
            */
            } else if ( flags & COMM_EVT_FLAG_REPORT ) {
                flagClr |= COMM_EVT_FLAG_REPORT;
                
            /***********************************************
            * ������ �Ͽ�
            */
            } else if ( flags & COMM_EVT_FLAG_CLOSE ) {
                flagClr |= COMM_EVT_FLAG_CLOSE;
                
            /***********************************************
            * ������ ��ʱ
            */
            } else if ( flags & COMM_EVT_FLAG_TIMEOUT ) {
                flagClr |= COMM_EVT_FLAG_TIMEOUT;
                
            /***********************************************
            * ������ ����
            */
            } else if ( flags & COMM_EVT_FLAG_CONFIG ) {
                flagClr |= COMM_EVT_FLAG_CONFIG;
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
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
        /***********************************************
        * ������ �����ʱ������һ��������
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                        ( OS_FLAGS      )Ctrl.Os.CommEvtFlag,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err );
        }
        
          /*************************************************
          * ������ ���Ͳ�ѯ����
          */
#if MODBUS_CFG_MASTER_EN == DEF_TRUE          
          MBM_FC03_HoldingRegRd( (MODBUS_CH   *)Ctrl.Comm.pch,
                                 (CPU_INT08U   )Ctrl.Comm.pch->NodeAddr,
                                 (CPU_INT16U   )0,
                                 (CPU_INT16U  *)&Ctrl.Para.buf2[0],
                                 (CPU_INT16U   )20 );
#endif
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
 * ��    �ƣ� APP_ModbusInit
 * ��    �ܣ� MODBUS��ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� �ó�ʼ���ᴴ��Modbus����
 *******************************************************************************/
void App_ModbusInit(void)
{
    MODBUS_CH   *pch;
    /***********************************************
    * ������ uCModBus��ʼ����RTUʱ��Ƶ��Ϊ1000HZ
    *        ʹ���˶�ʱ��2��TIM2��
    */
    MB_Init(1000);
    /***********************************************
    * ������ ��ModBus����Ϊ�ӻ�����mb_cfg.c��
    */
#if MODBUS_CFG_SLAVE_EN == DEF_TRUE
    pch         = MB_CfgCh( ModbusNode,             // ... Modbus Node # for this slave channel
                            MODBUS_SLAVE,           // ... This is a SLAVE
                            2,                      // ... 0 when a slave
                            MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
                            0,                      // ... Specify UART #2
                            9600,                   // ... Baud Rate
                            USART_WordLength_8b,    // ... Number of data bits 7 or 8
                            USART_Parity_No,        // ... Parity: _NONE, _ODD or _EVEN
                            USART_StopBits_1,       // ... Number of stop bits 1 or 2
                            MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
#endif

    /***********************************************
    * ������ ��ModBus����Ϊ��������mb_cfg.c��
    */
#if MODBUS_CFG_MASTER_EN == DEF_TRUE
    pch         = MB_CfgCh( 1,                      // ... Modbus Node # for this channel
                            MODBUS_MASTER,          // ... This is a MASTER
                            500,                    // ... One second timeout waiting for slave response
                            MODBUS_MODE_RTU,        // ... Modbus Mode (_ASCII or _RTU)
                            0,                      // ... Specify UART #3
                            9600,                   // ... Baud Rate
                            USART_WordLength_8b,    // ... Number of data bits 7 or 8
                            USART_Parity_No,        // ... Parity: _NONE, _ODD or _EVEN
                            USART_StopBits_1,       // ... Number of stop bits 1 or 2
                            MODBUS_WR_EN);          // ... Enable (_EN) or disable (_DIS) writes
#endif
    pch->AesEn          = DEF_DISABLED;             // ... AES���ܽ�ֹ
    pch->NonModbusEn    = DEF_ENABLED;              // ... ֧�ַ�MODBUSͨ��
    pch->IapModbusEn    = DEF_ENABLED;              // ... ֧��IAP MODBUSͨ��
    
    pch->RxFrameHead    = 0x1028;                   // ... ���ƥ��֡ͷ
    pch->RxFrameTail    = 0x102c;                   // ... ���ƥ��֡β
    
    Ctrl.Comm.pch       = pch;
}

/*******************************************************************************
 * ��    �ƣ� APP_CommInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void APP_CommInit(void)
{
    OS_ERR err;
    
    /***********************************************
    * ������ �����¼���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.CommEvtFlagGrp,
                 ( CPU_CHAR     *)"App_CommFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.CommEvtFlag = COMM_EVT_FLAG_HEART       // ����������
                        + COMM_EVT_FLAG_RESET       // COMM��λ
                        + COMM_EVT_FLAG_CONNECT     // COMM����
                        + COMM_EVT_FLAG_RECV        // ���ڽ���
                        + COMM_EVT_FLAG_REPORT      // ���ڷ���
                        + COMM_EVT_FLAG_CLOSE       // �Ͽ�
                        + COMM_EVT_FLAG_TIMEOUT     // ��ʱ
                        + COMM_EVT_FLAG_CONFIG;     // ����
        
    /***********************************************
    * ������ ��ʼ������Ӳ����ȥ������ע����������
    */
    //BSP_UartCfg( 1, 9600);
    /***********************************************
    * ����������һ����ʱ�� OS_OPT_TMR_ONE_SHOT, OS_OPT_TMR_PERIODIC
    *
    OSTmrCreate ((OS_TMR               *)&OSTmr1,
                 (CPU_CHAR             *)"tmr1",
                 (OS_TICK               )1,
                 (OS_TICK               )1,
                 (OS_OPT                )OS_OPT_TMR_PERIODIC,
                 (OS_TMR_CALLBACK_PTR   )OSTmr1_callback,
                 (void                 *)NULL,
                 (OS_ERR               *)&err);
    OSTmrStart ((OS_TMR  *)&OSTmr1,
                (OS_ERR  *)&err);

    *//***********************************************
    * ������ ��ʼ��MODBUSͨ��
    */        
    App_ModbusInit();
    
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_COMM;
}

/*******************************************************************************
 * ��    �ƣ� APP_CommRxDataDealCB
 * ��    �ܣ� �������ݴ���ص���������MB_DATA.C����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2016-01-04
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� 
 *******************************************************************************/
INT08U APP_CommRxDataDealCB(MODBUS_CH  *pch)
{
    /***********************************************
    * ������ 2016/01/08���ӣ����ڷ�MODBBUS IAP����ͨ��
    */
#if MB_IAPMODBUS_EN == DEF_ENABLED
    /***********************************************
    * ������ �������������ģʽ
    */
    if ( ( Iap.Status != IAP_STS_DEF ) && 
         ( Iap.Status != IAP_STS_SUCCEED ) &&
         ( Iap.Status != IAP_STS_FAILED ) ) {
        return IAP_CommRxDataDealCB(pch);
    }
#endif
    /***********************************************
    * ������ ��ȡ֡ͷ
    */
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    u8  Len     = pch->RxBufByteCtr;
    memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    CPU_CRITICAL_EXIT();
    
    INT08U  *DataBuf    = pch->RxFrameData;
    /***********************************************
    * ������ �������ݴ���
    */
    u8  DstAddr = 
    CSNR_GetData(	(unsigned char *)pch->RxFrameData,          //RecBuf,
                    (unsigned char	 )Len,                       //RecLen, 
                    (unsigned char *)DataBuf,                   //DataBuf,
                    (unsigned short *)&Len);                     //InfoLen)

    /***********************************************
    * ������ ������ݳ��Ȼ��ߵ�ַΪ0���򲻴���
    */
    if ( ( Len == 0 ) || ( DstAddr == 0 ) )
        return FALSE;
    
    /***********************************************
    * ������ ��ȡ֡ͷ
    */
    OS_CRITICAL_ENTER();
    memcpy( (INT08U *)&Ctrl.Comm.Rd.Head, (INT08U *)pch->RxFrameData, 8 );
    OS_CRITICAL_EXIT();

    /***********************************************
    * ������ ��ַ�ȶԣ����Ǳ�����ַ��ֱ�ӷ���
    */
    if ( Ctrl.Comm.Rd.Head.DstAddr != Ctrl.Comm.SlaveAddr ) {
        return FALSE;
    }
    
    /***********************************************
    * ������ �������ָ��
    */
    if ( ( Len == 18) && 
         ( 0 >= memcmp((const char *)"IAP_pragram start!",(const char *)&pch->RxFrameData[8], 18) ) ) {
#if defined     (IMAGE_A) || defined   (IMAGE_B)
             
#else
        /***********************************************
        * ������ �������ݴ���
        */
        DataSend_CSNR(	(unsigned char  ) Ctrl.Comm.SlaveAddr,         // SourceAddr,
                        (unsigned char  ) Ctrl.Comm.Rd.Head.SrcAddr,   // DistAddr,
                        (unsigned char *)&pch->RxFrameData[8],         // DataBuf,
                        (unsigned int	 ) Len);                        // DataLen 
#endif
        IAP_Reset();
        return TRUE;
    }    

    /***********************************************
    * ������ ��ȡ����
    */
    OS_CRITICAL_ENTER();
    memcpy( (INT08U *)&Ctrl.Comm.Rd.Data, (INT08U *)&pch->RxFrameData[8], Len );
    OS_CRITICAL_EXIT();
    
    /***********************************************
    * ������ д����
    */
    if ( ( Ctrl.Comm.Rd.Head.PacketCtrl & 0x0f ) == 0x01 ) {
        
    /***********************************************
    * ������ ������
    */
    } else if ( ( Ctrl.Comm.Rd.Head.PacketCtrl & 0x0f ) == 0x00  ) {
        OS_CRITICAL_ENTER();
        
        Ctrl.Comm.Rd.Data.Oil           = SW_INT16U((Ctrl.Comm.Rd.Data.Oil>>16));
        /***********************************************
        * ������ �����ݴ�������ͽṹ
        */
        Ctrl.Comm.Wr.Data.Hight         = SW_INT16U(Ctrl.Sen.C.Hight);          // �߶�
        Ctrl.Comm.Wr.Data.HightDiff     = (INT16U)Ctrl.Sen.C.CalcDiffHight;     // �߶Ȳ�
        Ctrl.Comm.Wr.Data.Pressure[0]   = (INT16S)Ctrl.Sen.H.PressureSample;    // ��λ������ѹ��
        Ctrl.Comm.Wr.Data.Pressure[1]   = (INT16S)Ctrl.Sen.L.PressureSample;    // ��λ������ѹ��
        Ctrl.Comm.Wr.Data.Density       = (INT16U)(Ctrl.Sen.C.Density * 10000.0);//�ܶ�
        
        Ctrl.Comm.Wr.Data.AdcValue[0]   = (INT16U)(Ctrl.Sen.H.AdcValueSample);  // ��λADCֵ
        Ctrl.Comm.Wr.Data.AdcValue[1]   = (INT16U)(Ctrl.Sen.L.AdcValueSample);  // ��λADCֵ
        Ctrl.Comm.Wr.Data.Volume        = (INT16U)(Ctrl.Sen.C.Volume);          // 2016.04.16����
        Ctrl.Comm.Wr.Data.Weight        = (INT16U)(Ctrl.Sen.C.Weight);          // 2016.04.16����
        
        Ctrl.Comm.Wr.Data.CalcDensity   = (INT16S)(Ctrl.Sen.C.CalcDensity * 10000.0);
        Ctrl.Comm.Wr.Data.Gravity       = (INT16S)(Ctrl.Sen.C.Gravity * 10000.0);
        Ctrl.Comm.Wr.Data.CalcGravity   = (INT16S)(Ctrl.Sen.C.CalcGravity * 10000.0);
        
        Ctrl.Comm.Wr.Data.Sts.Dat       = Ctrl.Para.dat.SenSts.Dat[1];          // ״̬��־
        
        Ctrl.Comm.Wr.Head.DataLen       = 12+14;                                 // 2016.04.16����4�ֽ�
        OS_CRITICAL_EXIT();

        /***********************************************
        * ������ �������ݴ���
        */
        DataSend_CSNR( (INT08U ) Ctrl.Comm.SlaveAddr,                           // SourceAddr,
                       (INT08U ) Ctrl.Comm.Rd.Head.SrcAddr,                     // DistAddr,
                       (INT08U*)&Ctrl.Comm.Wr.Data,                             // DataBuf,
                       (INT16U ) Ctrl.Comm.Wr.Head.DataLen);                    // DataLen 
    }
    
    /***********************************************
    * ������ ���ڽ���COMMģ�����Ϣ������
    */
    Ctrl.Comm.ConnectTimeOut    = 0;                // ��ʱ����������
    Ctrl.Comm.ConnectFlag       = TRUE;             // ת���ӳɹ���־
    
    return TRUE;
}

/***********************************************
* ������ 2016/01/08���ӣ����ڷ�MODBBUS IAP����ͨ��
*/
#if MB_IAPMODBUS_EN == DEF_ENABLED
/*******************************************************************************
 * ��    �ƣ� APP_CommRxDataDealCB
 * ��    �ܣ� �������ݴ���ص���������MB_DATA.C����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2016-01-04
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� 
 *******************************************************************************/
INT08U IAP_CommRxDataDealCB(MODBUS_CH  *pch)
{
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    u8  Len     = pch->RxBufByteCtr;
    memcpy( (INT08U *)&pch->RxFrameData, (INT08U *)pch->RxBuf, Len );
    CPU_CRITICAL_EXIT();
    
    /***********************************************
    * ������ ��ȡ֡ͷ
    */    
    INT08U  *DataBuf    = pch->RxFrameData;
    
    /***********************************************
    * ������ �������ݴ���
    */
    u8  DstAddr = 
    CSNR_GetData(	(unsigned char *)pch->RxFrameData,          //RecBuf,
                    (unsigned char	 )Len,                       //RecLen, 
                    (unsigned char *)DataBuf,                   //DataBuf,
                    (unsigned short *)&Len);                     //InfoLen)
    /***********************************************
    * ������ �����ս���
    */
    if ( ( Len == 16) && 
         ( 0 >= memcmp((const char *)"IAP_pragram end!",(const char *)&pch->RxFrameData[8], 16) ) ) {
        /***********************************************
        * ������ �������ݴ���
        */
        DataSend_CSNR(	(unsigned char  ) Ctrl.Comm.SlaveAddr,          // SourceAddr,
                        (unsigned char  ) Ctrl.Comm.Rd.Head.SrcAddr,    // DistAddr,
                        (unsigned char *)&pch->RxFrameData[8],          // DataBuf,
                        (unsigned short ) Len); 
        
        IAP_Finish();
        return TRUE;
    }
    
    if ( ( Len == 18) && 
         ( 0 >= memcmp((const char *)"IAP_pragram start!",(const char *)&pch->RxFrameData[8], 18) ) ) {
        
#if defined     (IMAGE_A) || defined   (IMAGE_B)
#else
        /***********************************************
        * ������ �������ݴ���
        */
        DataSend_CSNR(	(unsigned char  ) Ctrl.Comm.SlaveAddr,          // SourceAddr,
                        (unsigned char  ) Ctrl.Comm.Rd.Head.SrcAddr,    // DistAddr,
                        (unsigned char *)&pch->RxFrameData[8],            // DataBuf,
                        (unsigned int	 ) Len);   // DataLen 
#endif
        IAP_Reset();
        return TRUE;
    }
    /***********************************************
    * ������ д����
    */
    /***********************************************
    * ������ �����ݴ�������ͽṹ
    */
    Ctrl.Comm.Wr.Head.DataLen       = 0;
    /***********************************************
    * ������ д���ݵ�Flash
    */
    IAP_Program((StrIapState *)&Iap, (INT16U *)&pch->RxFrameData[8], Len );    
    /***********************************************
    * ������ �༭Ӧ������
    */
    char str[20];
    if ( Len < 128 ) {
        usprintf(str,"\n%d\nFinished\n",Iap.FrameIdx);
    } else {
        usprintf(str,"\n%d",Iap.FrameIdx);
    }
    str[19]  = 0;
    /***********************************************
    * ������ �������ݴ���
    */
    DataSend_CSNR(	(unsigned char  ) Ctrl.Comm.SlaveAddr,                    // SourceAddr,
                    (unsigned char  ) Ctrl.Comm.Rd.Head.SrcAddr,              // DistAddr,
                    (unsigned char *) str,                                    // DataBuf,
                    (unsigned short ) strlen(str));                            // DataLen 
    if ( Len < 128 ) {
        BSP_OS_TimeDly(100);
        IAP_Finish();
    } else {
        IAP_Programing();                           // ��λ��ʱ������
    }
    /***********************************************
    * ������ ���ڽ���COMMģ�����Ϣ������
    */
    Ctrl.Comm.ConnectTimeOut    = 0;                // ��ʱ����������
    Ctrl.Comm.ConnectFlag       = FALSE;            // ת���ӳɹ���־
    
    return TRUE;
}
#endif
/*******************************************************************************
 * ��    �ƣ� OSTmr1_callback
 * ��    �ܣ� ��ʱ��1�Ļص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ��ʱ���ص���������ʹ����ʱ����
 *******************************************************************************/
void OSTmr1_callback(OS_TMR *ptmr,void *p_arg)
{

}


/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif