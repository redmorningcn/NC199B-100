/*******************************************************************************
 *   Filename:       app_task_control.c
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ˫��ѡ�� control �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Control �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� CONTROL �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_CONTROL_PRIO ��
 *                                            �� �����ջ�� APP_TASK_CONTROL_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskControlCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Control ��
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
#include "bsp_control.h"
#include <app.h>
#include <Protocol.h>
#include <bsp_adc.h>
#include <math.h>
     
#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_ctrl__c = "$Id: $";
#endif

#define APP_TASK_CONTROL_EN     DEF_ENABLED
#if APP_TASK_CONTROL_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define CONTROL_CYCLE_TIME_TICKS    (OS_CFG_TICK_RATE_HZ / 50)
//static INT32U CYCLE_TIME_TICKS      = (OS_CFG_TICK_RATE_HZ / 50);

#define BREATHE_CYCEL               4000.0                                      // ��������(3-5��)
#define BREATHE_TICK                10.0                                        // 
#define BREATHE_DUTY_MIN            5.0
#define BREATHE_TICK_NBR           (BREATHE_CYCEL / BREATHE_TICK)               // 
#define BREATHE_STEP               (2.0 * ( 100.0 - BREATHE_DUTY_MIN) / BREATHE_TICK_NBR)
/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* ������ ������ƿ飨TCB��
*/
#if ( OSAL_EN == DEF_ENABLED )
#else
static  OS_TCB   AppTaskControlTCB;
#endif

/***********************************************
* ������ �����ջ��STACKS��
*/
#if ( OSAL_EN == DEF_ENABLED )
#else
static  CPU_STK  AppTaskControlStk[ APP_TASK_CONTROL_STK_SIZE ];
#endif

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskControl          (void *p_arg);
#endif
void            CalcFilterLife          (u8 level, u8 min);
void            CalcAirVolume           (u8 level, u8 min);
static  void    App_PwrOff              (StrCtrlDCB *dcb);
static  void    App_PwrOn               (StrCtrlDCB *dcb);
static  void    App_WifiConfig          (StrCtrlDCB *dcb);

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
 * ��    �ƣ� App_TaskControlCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskControlCreate(void)
{
#if ( OSAL_EN == DEF_ENABLED )
#else
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskControlTCB,              // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Control",              // ��������
                 (OS_TASK_PTR ) AppTaskControl,                 // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_CONTROL_PRIO,          // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskControlStk[0],           // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_CONTROL_STK_SIZE / 10, // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_CONTROL_STK_SIZE,      // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
#endif
}

/*******************************************************************************
 * ��    �ƣ� AppTaskControl
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskCtrlEvtProcess(INT8U task_id, osalEvt task_event)
#else
static  void  AppTaskControl (void *p_arg)
#endif
{
    OS_ERR      err;
    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
#if ( OSAL_EN == DEF_ENABLED )
#else
    TaskInitCtrl();
    
    while (DEF_TRUE) {
#endif
    /***********************************************
    * ������ �������Ź���־��λ
    */
    OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                 (OS_FLAGS     ) WDT_FLAG_CTRL,
                 (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                 (CPU_TS       ) 0,
                 (OS_ERR      *) &err);
    
    /*************************************************
    * ������ ����LED��FAN���
    */
    if( task_event & (OS_EVT_CTRL_TICK) ) {
        /*******************************************************************
        * ������ ѡ���������
        */
        switch(Ctrl.Sys.SysSts) {
            /***********************************************
            * ������ �ػ�״̬
            */
        case SYS_PWR_OFF:
            App_PwrOff(&Ctrl);
            break;
            /***********************************************
            * ������ ����״̬
            */
        case SYS_PWR_ON:
            App_PwrOn(&Ctrl);
            break;
            /***********************************************
            * ������ 
            */
        default:
            Ctrl.Sys.SysSts = SYS_PWR_OFF;
            break;
        }
        
        /***********************************************
        * ������ WIFI����
        */
        App_WifiConfig(&Ctrl);
        
        return ( task_event ^ (OS_EVT_CTRL_TICK) );
    }
    
    /***********************************************
    * ������ �������
    */
    if( task_event & (OS_EVT_CTRL_FAN) ) {             
        switch(Ctrl.Sys.SysSts) {
            /***********************************************
            * ������ �ػ�״̬
            */
        case SYS_PWR_OFF:
            App_PwrOff(&Ctrl);
            break;
            /***********************************************
            * ������ ����״̬
            */
        case SYS_PWR_ON:
            /***********************************************
            * ������ ���PWM���
            */
#if defined(TEST_RB)
            if ( Ctrl.Fan.Mode == FAN_MODE_SMART ) {
                if ( FAN_DUTY_SCALE_MAX <= Ctrl.Fan.CurScale ) {
                    Ctrl.Fan.SetScale   = FAN_DUTY_SCALE_MIN;
                } else if ( FAN_DUTY_SCALE_MIN >= Ctrl.Fan.CurScale ) {
                    Ctrl.Fan.SetScale   = FAN_DUTY_SCALE_MAX;
                }
                
                if ( Ctrl.Fan.SetScale > Ctrl.Fan.CurScale ) {
                    Ctrl.Fan.CurScale   += 10;
                    PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                } else if ( Ctrl.Fan.SetScale < Ctrl.Fan.CurScale ) {
                    PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                    Ctrl.Fan.CurScale   -= 10;
                } else {
                }
            } else {
                if ( Ctrl.Fan.SetScale > Ctrl.Fan.CurScale ) {
                    PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                    Ctrl.Fan.CurScale++;
                } else if ( Ctrl.Fan.SetScale < Ctrl.Fan.CurScale ) {
                    PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                    Ctrl.Fan.CurScale--;
                }            
            }
#else
            if ( Ctrl.Fan.SetScale > Ctrl.Fan.CurScale ) {
                PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                Ctrl.Fan.CurScale++;
            } else if ( Ctrl.Fan.SetScale < Ctrl.Fan.CurScale ) {
                PWM_SetDuty(&FanPwm, PWM_CH_FAN,(float)Ctrl.Fan.CurScale/10.0);
                Ctrl.Fan.CurScale--;
            } 
            if ( Ctrl.Fan.CurScale < FAN_DUTY_SCALE_0 )
                Ctrl.Fan.CurScale = FAN_DUTY_SCALE_0;
#endif 
            break;
        }
        
        return ( task_event ^ (OS_EVT_CTRL_FAN) );            
    }
    
    /***********************************************
    * ������ ˯�ߣ�����ģʽ�������¼�
    */
    if( task_event & (OS_EVT_CTRL_LED) ) {
        if ( Ctrl.Sys.SysSts == SYS_PWR_ON ) {
            if ( Ctrl.Fan.Level == FAN_LEVEL_SILENT ) {
                /***********************************************
                * ������ ������ʽ��һ�κ���
                *
                    static float  step    = BREATHE_STEP; 
                    if( Ctrl.Led.Brightness > 60 ) {
                    Ctrl.Led.Brightness  += step * Ctrl.Led.Brightness / 60.0;
                } else {
                    Ctrl.Led.Brightness  += step;
                }
                    if( Ctrl.Led.Brightness > 100 ) {
                    Ctrl.Led.Brightness    = 100;
                    step    = -BREATHE_STEP;
                } else if( Ctrl.Led.Brightness < BREATHE_DUTY_MIN ) {
                    Ctrl.Led.Brightness    = BREATHE_DUTY_MIN;
                    step    = BREATHE_STEP;
                } 
                *//***********************************************
                * ������ ������ʽ�����κ���
                */  
                static int      step    = 1;
                static int      ctr     = 0;
                
                static double   x       = 0.0;
                static double   a       = 100.0 / (BREATHE_TICK_NBR/2) / (BREATHE_TICK_NBR/2);
                static double   b       = 0.0;
                static double   c       = BREATHE_DUTY_MIN;
                static double   y       = 0.0;
                
                ctr += step;
                
                if( ctr > BREATHE_TICK_NBR / 2 ) {
                    step    = -1;
                } else if( ctr < ( 0 - BREATHE_TICK_NBR / 2 ) ) {
                    step    = 1;
                }            
                
                x   = ctr;            
                y   = a * pow(x,2);
                y   = y + b*x +c;
                
                Ctrl.Led.Breathe    = y; 
                
                if( Ctrl.Led.Breathe > 100 ) {
                    Ctrl.Led.Breathe    = 100;
                } else if( Ctrl.Led.Breathe < BREATHE_DUTY_MIN ) {
                    Ctrl.Led.Breathe    = BREATHE_DUTY_MIN;
                }
                
                Ctrl.Led.SetColor   = BUILD_RGB(0,0,(INT08U)(Ctrl.Led.Breathe * Ctrl.Led.ColorScale / 100));    // R,G,B
                /***********************************************
                * ������ 
                */  
            }
            
            /***********************************************
            * ������ LED���
            */
            if ( ( Ctrl.Sys.WifiSts == WIFI_STS_OK ) ) {
                LED_SetColor(Ctrl.Led.SetColor);
            }
        }
        return ( task_event ^ (OS_EVT_CTRL_LED) );
    }
    
    /***********************************************
    * ������ WIFI��������״ָ̬ʾ
    */  
    if( task_event & (OS_EVT_CTRL_WIFI_CFG) ) {
        static INT08U   flag    = 0;
        flag    = ~flag;
        /***********************************************
        * ������ 
        */
        if ( flag == 0) {
            Ctrl.Led.SetColor   = BUILD_RGB(0,0,0);    // R,G,B
            LED_SetColor(Ctrl.Led.SetColor);
        /***********************************************
        * ������ 
        */
        } else {
            if ( Ctrl.Sys.SysSts != SYS_PWR_ON )
                LED_SetColor(0x00ffffff);               // �ػ���ɫ
            else
                LED_SetColor(Ctrl.Led.SetColor);        // ������ɫ
        }
        if ( Ctrl.Sys.WifiSts != WIFI_STS_OK )
            osal_start_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_CFG), 200);
        
        return ( task_event ^ (OS_EVT_CTRL_WIFI_CFG) );
    }
    
    /***********************************************
    * ������ WIFI�������ó�ʱ
    */  
    if( task_event & (OS_EVT_CTRL_WIFI_TMO) ) {
        osal_stop_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_TMO) );
        return ( task_event ^ (OS_EVT_CTRL_WIFI_TMO) );
    }
    
    /***********************************************
    * ������ ������������ʱ
    */  
    if( task_event & (OS_EVT_CTRL_FILTER) ) {
        // �����´�������������ʱ��ʱ��
        osal_start_timerEx( OS_TASK_ID_CTRL, 
                           OS_EVT_CTRL_FILTER, 
                           OS_TICKS_PER_SEC * 60 * FILTER_LIFE_SAVE_TIME  );
        if ( Ctrl.Sys.SysSts == SYS_PWR_ON ) {
            // ��������
            CalcFilterLife(Ctrl.Fan.Level, 1);
            // ���������
            CalcAirVolume(Ctrl.Fan.Level, 1);
            // ���浱ǰ������
            BSP_FlashWriteBytes((u32) 0*2, (u8 *)&Ctrl.Para.buf1[0], (u16) 10*2);
        }
        return ( task_event ^ (OS_EVT_CTRL_FILTER) );
    }
     
    /***********************************************
    * ������ ������ͳ��
    */  
    if( task_event & (OS_EVT_CTRL_AIR_VOLUME) ) {
        osal_start_timerEx( OS_TASK_ID_CTRL, 
                           OS_EVT_CTRL_AIR_VOLUME, 
                           OS_TICKS_PER_SEC * 60 );
        if ( Ctrl.Sys.SysSts == SYS_PWR_ON ) {
            // ��������
            CalcFilterLife(Ctrl.Fan.Level, 1);
            // ���������
            CalcAirVolume(Ctrl.Fan.Level, 1);
        }
        return ( task_event ^ (OS_EVT_CTRL_AIR_VOLUME) );
    }
    /***********************************************
    * ������ ��ʱ���ػ�
    */  
    if( task_event & (OS_EVT_CTRL_TIMING_ON_OFF) ) {
        static INT08U   ctr = 0;
        /***********************************************
        * ������ ��ʱ����
        */ 
        if ( Ctrl.Wifi.Wr.CountDownOnMin ) {
            if ( ++ctr >= 60 ) {
                ctr = 0;
                if ( --Ctrl.Wifi.Wr.CountDownOnMin == 0 ) {
                    Power(ON);
                }
            }
        /***********************************************
        * ������ ��ʱ�ػ�
        */ 
        } else if (Ctrl.Wifi.Wr.CountDownOffMin) {
            if ( ++ctr >= 60 ) {
                ctr = 0;
                if ( --Ctrl.Wifi.Wr.CountDownOffMin == 0 ) {
                    Power(OFF);
                }
            }
        } else {
            ctr = 0;
        }
        
        /***********************************************
        * ������������bety29��
        *
        union __alert {
            struct __ualert {
                uint8_t  FilterLife :1;//��о��������
                uint8_t  AirQuality :1;//������������
                uint8_t  RSV        :6;//Ԥ��
            }udat;
            uint8_t  uAlert;
        }Alert;
        *//***********************************************
        * ���������� ��bety30��
        *
        union __fault {
            struct __ufault {
                uint8_t  Motor       :1;  //�������
                uint8_t  AirSensor   :1;  //������������VOC������
                uint8_t  DustSensor  :1;  //�ҳ�����������
                uint8_t  RSV         :5;  //Ԥ��
            }udat;
            uint8_t  uFault;
        }Fault;
    
        */
        return ( task_event ^ (OS_EVT_CTRL_TIMING_ON_OFF) );
    }
        
    return 0;
}

/*******************************************************************************
 * ��    �ƣ� APP_ControlInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitCtrl(void)
{
    Ctrl.Fan.SetScale   = (INT16S)FAN_DUTY_SCALE_2;
    Ctrl.Fan.CurScale   = (INT16S)FAN_DUTY_SCALE_1;
    
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    ControlInit();
    
#if (STORAGE_TYPE == INTNER_FLASH ) 
    if ( (INT16U) Ctrl.Para.dat.SysSts.dat != 0 ) {
        if ( Ctrl.Para.dat.SysSts.udat.App_Fault_ISR == 1 ) {
            Ctrl.Fan.Mode    = Ctrl.Para.dat.SysSts.udat.FanMode;
            Ctrl.Fan.Level   = Ctrl.Para.dat.SysSts.udat.FanLevel;
        }
        Ctrl.Para.dat.SysSts.dat = 0;
        BSP_FlashWriteBytes((u32) 126*2, (u8 *)&Ctrl.Para.dat.SysSts.udat, (u16) 2);
    }
    
#elif (STORAGE_TYPE == INTNER_EEPROM )
#elif (STORAGE_TYPE == EXTERN_EEPROM )
#endif
    
    App_PwrOff(&Ctrl);
  
    // ��������
    CalcFilterLife(0, 0);
    // ���������
    CalcAirVolume(0, 0);
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_CTRL;
    /*************************************************
    * �����������¼���ѯ
    */
#if ( OSAL_EN == DEF_ENABLED )
    osal_start_timerRl( OS_TASK_ID_CTRL, OS_EVT_CTRL_TICK,  OS_TICKS_PER_SEC / 5);
    osal_start_timerRl( OS_TASK_ID_CTRL, OS_EVT_CTRL_FAN,   OS_TICKS_PER_SEC / 10);
    osal_start_timerRl( OS_TASK_ID_CTRL, OS_EVT_CTRL_LED,   BREATHE_TICK);
    osal_start_timerRl( OS_TASK_ID_CTRL, OS_EVT_CTRL_TIMING_ON_OFF, OS_TICKS_PER_SEC);
#else
#endif
        
    /*************************************************
    * ������ ����ǲ��ԣ���ֱ�ӿ���
    */
#if defined(TEST_RB)
    Power(ON);
#endif
}

/*******************************************************************************
 * ��    �ƣ� App_PwrOff
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-09-02
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void    App_PwrOff              (StrCtrlDCB *dcb)
{
    PWM_SetDuty(&FanPwm, PWM_CH_FAN, FAN_DUTY_STOP);
    
    if ( dcb->Sys.WifiSts == WIFI_STS_OK ) {
        LED_SetColor(0);
    }
}

/*******************************************************************************
 * ��    �ƣ� App_PwrOff
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-09-02
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void    App_WifiConfig         (StrCtrlDCB *dcb)
{  
    /***********************************************
    * ������ 
    */
    switch ( dcb->Sys.WifiSts ) {
    /***********************************************
    * ������ 
    */
    case WIFI_STS_OK:
        return;
        break;
    /***********************************************
    * ������ 
    */
    case WIFI_STS_CONFIG: 
        if ( Ctrl.Wifi.W2dWifiSts.WifiStatus.ustatus.ConnRouter ) {
            dcb->Sys.WifiSts    = WIFI_STS_OK;
            //CYCLE_TIME_TICKS    = CONTROL_CYCLE_TIME_TICKS;
            osal_stop_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_CFG) );
            LED_SetColor(0);
        } else if ( !Ctrl.Wifi.W2dWifiSts.WifiStatus.ustatus.SoftApMode ) {
            dcb->Sys.WifiSts    = WIFI_STS_OK;
            //CYCLE_TIME_TICKS    = CONTROL_CYCLE_TIME_TICKS;
            osal_stop_timerEx( OS_TASK_ID_CTRL, (OS_EVT_CTRL_WIFI_CFG) );
            LED_SetColor(0);
            return;
        } else {
            
        }
        break;
    /***********************************************
    * ������ 
    */
    case WIFI_STS_CONFIG_OK:
        break;
    /***********************************************
    * ������ 
    */
    case WIFI_STS_CONFIG_ERR:
        break;
    }
}

/*******************************************************************************
 * ��    �ƣ� App_Pwron
 * ��    �ܣ�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-09-02
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void    App_PwrOn               (StrCtrlDCB *dcb)
{
    extern Device_ReadTypeDef               Device_ReadStruct;
    /***********************************************
    * ������ ����ģʽѡ�����ȼ�
    */
    switch(dcb->Fan.Mode) {
    case FAN_MODE_PWR_OFF:                          // ����	�ϵ��Զ����У������λΪ0��������������
        break;
    case FAN_MODE_SILENT:                           // ����ģʽ	�����λ1��LED����ɫ������Ч��
        dcb->Fan.Level  = FAN_LEVEL_SILENT;
        break;
    case FAN_MODE_SMART:                            // ����ģʽ	�����λ���ݴ����������Ŀ���������̬�仯2-4��LED����������ı�
        dcb->Fan.Level  = FAN_LEVEL_AQL1_LOW + Ctrl.PM2D5.Level;
        break;
    case FAN_MODE_MAX_SPEED:                        // ����ģʽ	�����λ5��LED����ɫ
        dcb->Fan.Level  = FAN_LEVEL_MAX;
        break;
    case FAN_MODE_HAND:                             // �ֶ�ģʽ	�ֶ��л�ģʽ2-4��LED������������ı�
        break;
    default:
        dcb->Fan.Level  = FAN_LEVEL_SILENT;
        break;
    }
    
    /***********************************************
    * ������ ���ݹ���ǿ�ȼ���LED�ȼ�
    */    
#if PWM_OUT_NEGTIVE == DEF_ENABLED
    dcb->Led.Brightness     = 1.0;
#else
    static float   Brightness   = 1.0;
    
    if ( ADC_AverageValue[1] < 120 ) {
        Brightness  = 0.05;
    } else if ( ADC_AverageValue[1] > 130 ) {
        Brightness  = 1.0;
    } else if ( Brightness  < 0.05 ) {
        Brightness  = 1.0;
    }
    //Brightness      = 0.02
    //                + (float)ADC_AverageValue[1] 
    //                / 1000.0;
    //
    //if( Brightness > 1.0 )
    //    Brightness = 1.0; 
    //
    //if ( dcb->Led.Brightness == 0.0 )
    //    dcb->Led.Brightness = Brightness;
    dcb->Led.Brightness     = dcb->Led.Brightness   * 0.9 
                            + Brightness            * 0.1;
    dcb->Led.ColorScale     = (INT08U)(dcb->Led.Brightness   * 255);
#endif
    float   SetScale;
    /***********************************************
    * ������ �ֱ������ȼ���LEDָʾ��ɫ
    *        ���������200mm*73mm
    */    
    switch(dcb->Fan.Level) {
        /***********************************************
        * ������ �ػ�/����ģʽ��Ϩ��
        */
    case FAN_LEVEL_OFF:{
        SetScale            = FAN_DUTY_STOP;
        PWM_SetDuty(&FanPwm, PWM_CH_FAN, FAN_DUTY_STOP);
        LED_SetColor(0);
    } break;
    /***********************************************
    * ������ ����ģʽ����ɫ���� 1.2m/s
    */
    case FAN_LEVEL_SILENT:{
        SetScale            = FAN_DUTY_SCALE_1;
        //dcb->Led.SetColor   = BUILD_RGB(0,0,(INT08U)(dcb->Led.Breathe * LedScale / 100));    // R,G,B
    } break;
    /***********************************************
    * ������ ����ģʽ����ɫ   2.3m/s
    */
    case FAN_LEVEL_AQL1_LOW:{
        SetScale            = FAN_DUTY_SCALE_2;
        dcb->Led.SetColor   = BUILD_RGB(0,0,dcb->Led.ColorScale);          // R,G,B
    } break;
    /***********************************************
    * ������ ����ģʽ����ɫ   3.5m/s
    */
    case FAN_LEVEL_AQL2_MID:{
        SetScale            = FAN_DUTY_SCALE_3;
        dcb->Led.SetColor   = BUILD_RGB(dcb->Led.ColorScale,dcb->Led.ColorScale,0);    // R,G,B
    } break;
    /***********************************************
    * ������ ����ģʽ����ɫ   4.7m/s
    */
    case FAN_LEVEL_AQL3_HIG:{
        SetScale            = FAN_DUTY_SCALE_4;
        dcb->Led.SetColor   = BUILD_RGB(dcb->Led.ColorScale,0,0);          // R,G,B
    } break;
    /***********************************************
    * ������ ����ģʽ����ɫ   5.3m/s
    */
    case FAN_LEVEL_MAX:{
        SetScale            = FAN_DUTY_SCALE_5;
        dcb->Led.SetColor   = BUILD_RGB(0,dcb->Led.ColorScale,0);          // R,G,B
    } break;
    default:
        SetScale            = dcb->Fan.SetScale;
        break;
    }
    
#if defined(TEST_RB)
#else
    dcb->Fan.SetScale   = (INT16S)SetScale;
#endif
}

/*******************************************************************************
 * ��    �ƣ� CalcFilterLife
 * ��    �ܣ� ������������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen
 * �������ڣ� 2015-10-20
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void CalcFilterLife(u8 level, u8 min)
{
    if (Ctrl.Para.dat.FilterLife.Life) {
        // ������FILTER_LIFE_SAVE_TIME����
        Ctrl.Para.dat.FilterLife.Life   -= min;
    }
            
    if ( ( Ctrl.Para.dat.FilterLife.Life < 0 ) ||
         ( Ctrl.Para.dat.FilterLife.Life > FILTER_LIFE_TIME ) )
        Ctrl.Para.dat.FilterLife.Life    = FILTER_LIFE_TIME;
    
    Ctrl.Wifi.Rd.FilterLife         = ((INT32U)Ctrl.Para.dat.FilterLife.Life
                                    * 100)
                                    / (FILTER_LIFE_TIME);
    Ctrl.Wifi.Wr.FilterLife         = Ctrl.Wifi.Rd.FilterLife;
}

/*******************************************************************************
 * ��    �ƣ� CalcFilterLife
 * ��    �ܣ� ������������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen
 * �������ڣ� 2015-10-20
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void CalcAirVolume(u8 level, u8 min)
{
    u32     volume  = Ctrl.Para.dat.AirVolume.Volume;
    float   v;
    
    if ( volume  == 0xffff )
        volume      = 0;    
    
    /***********************************************
    * ������ �ֱ������ȼ���LEDָʾ��ɫ
    *        ���������200mm*73mm
    */    
    switch(level) {
        /***********************************************
        * ������ �ػ�/����ģʽ��Ϩ��
        */
    case FAN_LEVEL_OFF:         v   = 0.0;  break;
    /***********************************************
    * ������ ����ģʽ����ɫ���� 1.2m/s
    */
    case FAN_LEVEL_SILENT:      v   = 1.8;  break;
    /***********************************************
    * ������ ����ģʽ����ɫ   2.3m/s
    */
    case FAN_LEVEL_AQL1_LOW:    v   = 3.0;  break;
    /***********************************************
    * ������ ����ģʽ����ɫ   3.5m/s
    */
    case FAN_LEVEL_AQL2_MID:    v   = 4.0;  break;
    /***********************************************
    * ������ ����ģʽ����ɫ   4.7m/s
    */
    case FAN_LEVEL_AQL3_HIG:    v   = 5.0;  break;
    /***********************************************
    * ������ ����ģʽ����ɫ   5.3m/s
    */
    case FAN_LEVEL_MAX:         v   = 6.0;  break;
    default:                    v   = 0.0;  break;
    }    
    
    //1�������λ������֮����ڷ������㣺�����ߵ�λ��   
#define     s   (0.22 * 0.074)                      // ������=�����M^2��
                                                    // ��������ƽ������=m/s��m/s��
    float   qs  = s * v;                            // �����m^2����ƽ������=m^3/s��m^3/S��
    float   qm  = qs * 60;                          // m^3/s��60=m^3/minute(m^3/min)
    volume                          = (u32)(volume
                                    + qm * min);
        
    Ctrl.Para.dat.AirVolume.Volume  = volume;
    //Ctrl.Wifi.Wr.Ext.dat.AirVolume  = volume;
    memcpy(&Ctrl.Wifi.Wr.Ext.buf1[0], (INT08U *)&volume, 4);
}

/*******************************************************************************
 * ��    �ƣ� Power
 * ��    �ܣ� ���ػ�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen
 * �������ڣ� 2015-10-20
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void Power(INT08U onoff)
{
    OS_ERR  err;
    
    if ( onoff ) {
        /***********************************************
        * ������ ����״̬��ʼ��
        */  
        Ctrl.Sys.SysSts             = SYS_PWR_ON;
        Ctrl.Fan.CurScale           = FAN_DUTY_SCALE_0;
        Ctrl.Fan.Mode               = FAN_MODE_SMART;
        Ctrl.Wifi.Wr.CountDownOnMin = 0;
#if defined(TEST_RB)
        Ctrl.Fan.SetScale           = FAN_DUTY_SCALE_MAX;
#endif
        /***********************************************
        * ������ ������
        */
        Beep( BEEP_FLAG_PWR_ON );
    } else {
        /***********************************************
        * ������ �ػ�״̬��ʼ��
        */  
        Ctrl.Sys.SysSts             = SYS_PWR_OFF;
        Ctrl.Fan.Mode               = FAN_MODE_PWR_OFF;
        Ctrl.Fan.CurScale           = FAN_DUTY_SCALE_0;
        Ctrl.Wifi.Wr.CountDownOffMin= 0;
        /***********************************************
        * ������ �ػ���
        */
        Beep( BEEP_FLAG_PWR_OFF );
    }
    /***********************************************
    * ������ ���ػ�״̬�ϱ�
    */    
    OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
               ( OS_FLAGS      )WIFI_EVT_FLAG_REPORT,
               ( OS_OPT        )OS_OPT_POST_FLAG_SET,
               ( OS_ERR       *)&err);
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif