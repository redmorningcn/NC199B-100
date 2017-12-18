/*******************************************************************************
 *   Filename:       app_task_led.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmoringcn.
 *
 *   Description:    ˫��ѡ�� led �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Led �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� LED �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_LED_PRIO ��
 *                                            �� �����ջ�� APP_TASK_LED_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskLedCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Led ��
 *
 *   Notes:
 *     				E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
#include <global.h>
#include <app_ctrl.h>
#include <bsp_DM412.h>

//#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_led__c = "$Id: $";
#endif

#define APP_TASK_LED_EN     DEF_ENABLED
#if APP_TASK_LED_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
//#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 2u)

/*******************************************************************************
 * MACROS
 */
//#define     BSP_LED_NUM         8
////LED�������� 
//#define     LED_TOGGLE_CNT      15


/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
//
//uint8   LedWorkTimes[BSP_LED_NUM + 1] = 0;

#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskLedTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
//static  CPU_STK  AppTaskLedStk[ APP_TASK_LED_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskLed           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************/
//red
#define     RGB_LED_COLOR_RED       0xffff,0x0000,0x0000
#define     RGB_LED_COLOR_GREEN     0x0000,0xffff,0x0000
#define     RGB_LED_COLOR_BLUE      0x0000,0x0000,0xffff
#define     RGB_LED_COLOR_WHITE     0xffff,0xffff,0xffff

//led
#define     HUM_LED         0
#define     DUST_LED        1
#define     OIL_LED         2

#define     GOOD_GRADE      0
#define     NORMAL_GRADE    1
#define     BAD_GRADE       2

/*******************************************************************************
 * ��    �ƣ� AppTaskLed
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/

//#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskLedEvtProcess(osalTid task_id, osalEvt task_event)
//#else
//static  void  AppTaskLed (void *p_arg)
//#endif
{
//    OS_ERR      err;
//    INT32U      ticks;
//    INT32S      dly;
//    CPU_SR_ALLOC();
//    
//    /***********************************************
//    * ������Task body, always written as an infinite loop.
//    */
//#if ( OSAL_EN == DEF_ENABLED )
//#else
//    TaskInitLed();
//    
//    while (DEF_TRUE) {
//#endif
//        /***********************************************
//        * ������ �������Ź���־��λ
//        */
//        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
//                     (OS_FLAGS     ) WDT_FLAG_LED,
//                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                     (CPU_TS       ) 0,
//                     (OS_ERR      *) &err);
//        /***********************************************
//        * ������ �õ�ϵͳ��ǰʱ��
//        */
//        ticks = OSTimeGet(&err);
//        
//#if ( OSAL_EN == DEF_ENABLED )
        if( task_event & OS_EVT_LED_TICKS ) {
//#else
//#endif
            static  uint8   mode = 0;
            /***********************************************
            * ������ ������ʾ.red /green /blue
            */
            while(mode < 12)
            {
                mode++;
                switch(mode %4)
                {
                    case 0:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_RED);   break;
                    case 1:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_GREEN); break;
                    case 2:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_BLUE);  break;
                    case 3:
                            for(uint8 i= 0;i < 3;i++)
                                SetLedColor(i,RGB_LED_COLOR_WHITE); break;
                }
                    
                osal_start_timerEx( OS_TASK_ID_LED,
                                    OS_EVT_LED_TICKS,
                                    600);
                return ( task_event ^ OS_EVT_LED_TICKS );
            }

            /***********************************************
            * ������ ʪ����ʾ
            */
            switch(Ctrl.sRec.Air_Hum_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(HUM_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(HUM_LED,RGB_LED_COLOR_BLUE);    break;
            }
            
            /***********************************************
            * ������ ����ʾ
            */
            switch(Ctrl.sRec.Air_Voc_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(OIL_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(OIL_LED,RGB_LED_COLOR_BLUE);    break;
            }
               
            /***********************************************
            * ������ �۳���ʾ
            */
            switch(Ctrl.sRec.Air_Dust_Grade)
            {
                case GOOD_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_BLUE);    break;
                case NORMAL_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_GREEN);   break;
                case BAD_GRADE:
                    SetLedColor(DUST_LED,RGB_LED_COLOR_RED);     break;
                default:       
                    SetLedColor(DUST_LED,RGB_LED_COLOR_BLUE);    break;
            }            
            
            /***********************************************
            * ������ ��ʱ������
            */
            osal_start_timerEx( OS_TASK_ID_LED,
                                OS_EVT_LED_TICKS,
                                100);
            return ( task_event ^ OS_EVT_LED_TICKS );
        }
        
        
        
        return  task_event;
}


/*******************************************************************************
 * ��    �ƣ�APP_LedInit
 * ��    �ܣ������ʼ��
 * ��ڲ�������
 * ���ڲ�������
 * ���� ���ߣ�redmorningcn.
 * �������ڣ�2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitLed(void)
{    
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    bsp_DM412Init();            //DM412��ʼ��
        
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    //WdtFlags |= WDT_FLAG_LED;
    /*************************************************
    * �����������¼���ѯ
    */

    osal_start_timerEx( OS_TASK_ID_LED,
                        OS_EVT_LED_TICKS,
                        1000);
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif