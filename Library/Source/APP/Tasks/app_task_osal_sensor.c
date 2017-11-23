/*******************************************************************************
 *   Filename:       app_task_sensor.c
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ˫��ѡ�� sensor �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Sensor �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� SENSOR �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_SENSOR_PRIO ��
 *                                            �� �����ջ�� APP_TASK_SENSOR_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskSensorCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Sensor ��
 *
 *   Notes:
 *     				E-mail:261313062@qq.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
//#include "bsp_pm25.h"
//#include "bsp_shtxx.h"
#include <bsp_adc7682.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_sensor__c = "$Id: $";
#endif

#define APP_TASK_SENSOR_EN     DEF_ENABLED
#if APP_TASK_SENSOR_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
INT32U   CYCLE_TIME_TICKS     = (OS_CFG_TICK_RATE_HZ * 1u);

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskSensorTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskSensorStk[ APP_TASK_SENSOR_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskSensor           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
/***************************************************
* ������ ��ʱ������
*/
OS_TMR          OSTmr2;                             // ��ʱ��1
/***********************************************
* ������ ��ʱ���ص�����
*/
void            OSTmr2_callback         (OS_TMR *ptmr,void *p_arg);
/*******************************************************************************
 * EXTERN VARIABLES
 */

extern   Dev_StatusTypeDef      Dev_Status;

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskSensorCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskSensorCreate(void)
{

#if ( OSAL_EN == DEF_ENABLED )
#else
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskSensorTCB,               // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Sensor",               // ��������
                 (OS_TASK_PTR ) AppTaskSensor,                  // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_SENSOR_PRIO,           // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskSensorStk[0],            // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_SENSOR_STK_SIZE / 10,  // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_SENSOR_STK_SIZE,       // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
#endif
}

/*******************************************************************************
 * ��    �ƣ� AppTaskSensor
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/

#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskSensorEvtProcess(INT8U task_id, osalEvt task_event)
#else
static  void  AppTaskSensor (void *p_arg)
#endif
{
    OS_ERR      err;
    INT32U      ticks;
    INT32S      dly;
#if defined(PM2D5_UART_CHK)
    static INT08U       UartSendFlag    = 0;
#endif
    
    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
#if ( OSAL_EN == DEF_ENABLED )
#else
    TaskInitSensor();
    
    while (DEF_TRUE) {
#endif
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                     (OS_FLAGS     ) WDT_FLAG_SEN,
                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                     (CPU_TS       ) 0,
                     (OS_ERR      *) &err);
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        ticks = OSTimeGet(&err);
        
#if ( OSAL_EN == DEF_ENABLED )
        if( task_event & OS_EVT_SEN_SAMPLE ) {
#else
#endif
            static INT8U       step    = 0;
            static INT08U      ctr     = 0;
            /***********************************************
            * ������ ���SHT��ʪ�ȴ�������ֵ
            */
            switch(step) {
            case 0:
                if(BSP_ShtMeasureOnce(0,0) == 0) {
                    step = 1;
                } else {
                    step = 0;
                }
                CYCLE_TIME_TICKS = 350;
                break;
            case 1:
                if(BSP_ShtMeasureOnce(0,1) == 0) {
                    step = 2;
                } else{
                    step = 0;
                }
                CYCLE_TIME_TICKS = 10;
                break;
                
            case 2:
                if(BSP_ShtMeasureOnce(1,0) == 0) {
                    step = 3;
                } else {
                    step = 0;
                }
                CYCLE_TIME_TICKS = 350;
                break;
                
            case 3:
                if(BSP_ShtMeasureOnce(1,1) == 0) {
                    step = 4;
                } else {
                    step = 0;
                }
                CYCLE_TIME_TICKS = 10;
                break;
                
            case 4:
                if(BSP_ShtMeasureOnce(2,1) == 0) {
                    step = 0;
                    //Ctrl.Wifi.Rd.Temperature   = (int8_t)SHT_Data.Temp;
                    //Ctrl.Wifi.Rd.Humidity      = (uint8_t)SHT_Data.Humi;
                    
                } else {
                    step = 0;
                }
                
                CYCLE_TIME_TICKS = 500;
                break;
            }
            
            /***********************************************
            * ������ ���PM2.5��ֵ��PM2.5�ĵȼ�
            */
            if( ++ctr >= 4 ) {
                ctr = 0;
                /***********************************************
                * ������ ��ȡʵ��ֵ
                */
                Ctrl.PM2D5.G1Val        = BSP_PM2D5GetValue(0); // ��ȡһ��ֵ
                Ctrl.PM2D5.G2Val        = BSP_PM2D5GetValue(1); // ��ȡ����ֵ

                /***********************************************
                * ������ ѡ�񴫸���
                */
                if ( Ctrl.PM2D5.Type == 0 ) {
                    Ctrl.PM2D5.Val      = Ctrl.PM2D5.G1Val;
                } else {
                    Ctrl.PM2D5.Val      = Ctrl.PM2D5.G2Val;
                }
                
                Ctrl.PM2D5.ChkedVal = Ctrl.PM2D5.Val + Ctrl.Para.dat.Pm2d5Chk.Offset.b[Ctrl.PM2D5.Type]; 
                /***********************************************
                * ������ ����У׼�Ժ��ֵ
                */
                /***********************************************
                * ������ ��������
                */
                Ctrl.PM2D5.ChkedVal     = (Ctrl.PM2D5.ChkedVal >   0) ? Ctrl.PM2D5.ChkedVal : 1;
                Ctrl.PM2D5.ChkedVal     = (Ctrl.PM2D5.ChkedVal < 999) ? Ctrl.PM2D5.ChkedVal : 999;
                
                Ctrl.Wifi.Rd.PM25Val    = Ctrl.PM2D5.ChkedVal;
                /***********************************************
                * ������ ����ȼ�
                */
                Ctrl.Wifi.Rd.PM25Level  = BSP_PM2D5GetLevel(Ctrl.PM2D5.ChkedVal);
                Ctrl.PM2D5.Level        = Ctrl.Wifi.Rd.PM25Level;
#if defined(PM2D5_UART_CHK)
                if ( UartSendFlag ) {
                    u8  Sendbuf[11];
                    Sendbuf[0] = 0xFF;
                    Sendbuf[1] = 0xA5;
                    Sendbuf[2] = 6;
                    /***********************************************
                    * ������ ����PM25��ֵ
                    */
                    Sendbuf[3] = HI_INT16U(Ctrl.Wifi.Rd.PM25Val);
                    Sendbuf[4] = LO_INT16U(Ctrl.Wifi.Rd.PM25Val);
                    /***********************************************
                    * ������ ��ȡ��ʪ�ȵ�ֵ
                    */
                    Sendbuf[5] = HI_INT16U(Ctrl.Wifi.Rd.Temperature);
                    Sendbuf[6] = LO_INT16U(Ctrl.Wifi.Rd.Temperature);
                    
                    Sendbuf[7] = HI_INT16U(Ctrl.Wifi.Rd.Humidity);
                    Sendbuf[8] = LO_INT16U(Ctrl.Wifi.Rd.Humidity);

                    Sendbuf[9] = LO_INT16U(Sendbuf[3] + Sendbuf[4] + Sendbuf[5] + Sendbuf[6] + Sendbuf[7] + Sendbuf[8]);
                    Sendbuf[10] = 0x5a;
                    USART_WriteBytes(USART1, Sendbuf, 11);
                }
#endif
            }
            /***********************************************
            * ������ ���VOC��������ֵ(��ʱδ����)
            */
            Ctrl.Wifi.Rd.Voc = 0;
            /***********************************************
            * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
            */
            dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
            if ( dly  <= 0 ) {
                dly   = 1;
            }
#if ( OSAL_EN == DEF_ENABLED )
            osal_start_timerEx( OS_TASK_ID_SEN,
                               OS_EVT_SEN_SAMPLE,
                                dly);
            
            return ( task_event ^ OS_EVT_SEN_SAMPLE );
        }
        /***********************************************
        * ������ �޸�PM2.5�궨��ֵ
        */
        if( task_event & OS_EVT_SEN_PM2D5_CHK ) {
            BSP_PM2D5Chk( PM2D5_CHK_TYPE_B_OFFSET, Ctrl.PM2D5.ChkVal );
            BeepExt(3, 3000, 500);
            return ( task_event ^ OS_EVT_SEN_PM2D5_CHK );
        }

        /***********************************************
        * ������ PM2.5��ֵУ��
        */
        if( task_event & OS_EVT_SEN_CHK_TIMEOUT ) {  
            Ctrl.PM2D5.ChkCtr   = 0; 
            return ( task_event ^ OS_EVT_SEN_CHK_TIMEOUT );
        }
        
        /***********************************************
        * ������ PM2.5����У��
        */
#if defined(PM2D5_UART_CHK)
        if( task_event & OS_EVT_SEN_UART_TIMEOUT ) { 
            extern u8 Uart1RxBuf[];
            extern u8 Uart1RxCtr;
            if ( ( Uart1RxBuf[0] == 0xFF ) && 
                 ( Uart1RxBuf[1] == 0XA5 ) && 
                 ( Uart1RxBuf[6] == 0x5A ) ) {
                 INT16S     value   = BUILD_INT16U(Uart1RxBuf[4],Uart1RxBuf[3]);
                 switch (Uart1RxBuf[2]) {
                 case 0:
                    BSP_PM2D5Chk( PM2D5_CHK_TYPE_DEF, 0 );
                    BeepExt(1, 3000, 500);
                    break;
                 case 1:
                    PM2D5ChkVal = value;
                    BSP_PM2D5Chk( PM2D5_CHK_TYPE_B_OFFSET, value );
                    BeepExt(1, 3000, 200);
                    break;
                 case 2:
                    BSP_PM2D5Chk( PM2D5_CHK_TYPE_K_Y1, value );
                    BeepExt(2, 3000, 200);
                    break;
                 case 3:
                    BSP_PM2D5Chk( PM2D5_CHK_TYPE_K_Y2, value );
                    BeepExt(3, 3000, 200);
                    break;
                 case 0xff:
                    UartSendFlag    = !UartSendFlag;
                    BeepExt(1, 3000, 200);
                    break;
                }
            }
            Uart1RxCtr  = 0;
            memset(Uart1RxBuf, 0, 11);
            return ( task_event ^ OS_EVT_SEN_UART_TIMEOUT );
        }
#endif 
        return 0;
#else
        BSP_OS_TimeDly(dly);
    }
#endif
}

/*******************************************************************************
 * ��    �ƣ� APP_SensorInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitSensor(void)
{
    OS_ERR err;
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    //BSP_PM2D5Init();
    //BSP_ShtxxInit();
    BSP_ADCInit();
    /***********************************************
    * ����������һ����ʱ�� OS_OPT_TMR_ONE_SHOT, OS_OPT_TMR_PERIODIC
    */
    OSTmrCreate ((OS_TMR               *)&OSTmr2,
                 (CPU_CHAR             *)"tmr2",
                 (OS_TICK               )10,
                 (OS_TICK               )10,
                 (OS_OPT                )OS_OPT_TMR_PERIODIC,
                 (OS_TMR_CALLBACK_PTR   )OSTmr2_callback,
                 (void                 *)NULL,
                 (OS_ERR               *)&err);
    OSTmrStart ((OS_TMR  *)&OSTmr2,
                (OS_ERR  *)&err);
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_SEN;
    Ctrl.PM2D5.ChkVal   = 25;
    /*************************************************
    * �����������¼���ѯ
    */
#if ( OSAL_EN == DEF_ENABLED )
    osal_start_timerEx( OS_TASK_ID_SEN,
                        (1<<0),
                        1);
#else
#endif
}

/*******************************************************************************
 * ��    �ƣ� OSTmr2_callback
 * ��    �ܣ� ��ʱ��1�Ļص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ��ʱ���ص���������ʹ����ʱ����
 *******************************************************************************/
void OSTmr2_callback(OS_TMR *ptmr,void *p_arg)
{
    extern void BSP_PM2D5GetAdcValueCB(void);
    BSP_PM2D5GetAdcValueCB();
}
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif