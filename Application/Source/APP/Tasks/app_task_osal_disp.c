/*******************************************************************************
 *   Filename:       app_task_disp.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ˫��ѡ�� disp �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Disp �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� DISP �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_DISP_PRIO ��
 *                                            �� �����ջ�� APP_TASK_DISP_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskDispCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Disp ��
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
#include <global.h>
#include <bsp_adc7682.h>
#include <bsp_max7219.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_disp__c = "$Id: $";
#endif

#define APP_TASK_DISP_EN     DEF_ENABLED
#if APP_TASK_DISP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 2u)

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
static  OS_TCB   AppTaskDispTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskDispStk[ APP_TASK_DISP_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskDisp           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskDispCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskDispCreate(void)
{

#if ( OSAL_EN == DEF_ENABLED )
#else
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskDispTCB,                 // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Disp",                 // ��������
                 (OS_TASK_PTR ) AppTaskDisp,                    // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_DISP_PRIO,             // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskDispStk[0],              // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_DISP_STK_SIZE / 10,    // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_DISP_STK_SIZE,         // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
#endif
}

/*******************************************************************************
 * ��    �ƣ� AppTaskDisp
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/

#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskDispEvtProcess(INT8U task_id, osalEvt task_event)
#else
static  void  AppTaskDisp (void *p_arg)
#endif
{
    OS_ERR      err;
    INT32U      ticks;
    INT32S      dly;
    CPU_SR_ALLOC();
    
    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
#if ( OSAL_EN == DEF_ENABLED )
#else
    TaskInitDisp();
    
    while (DEF_TRUE) {
#endif
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                     (OS_FLAGS     ) WDT_FLAG_DISP,
                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                     (CPU_TS       ) 0,
                     (OS_ERR      *) &err);
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        ticks = OSTimeGet(&err);
        
#if ( OSAL_EN == DEF_ENABLED )
        if( task_event & OS_EVT_DISP_TICKS ) {
#else
#endif
            static INT8U       step    = 0;
            static INT16S      *pValue;
                    int         DispValue ;
            
            /***********************************************
            * ������ ����ʾ
            */
            BSP_DispClrAll();
                
            /***********************************************
            * ������ ������ʾģʽ��ʾ
            */
            switch(Ctrl.Disp.Mode) {
            /*******************************************************************
            * ������ ��ʾ��������
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            *        ���ݣ���SF-X����
            *        ���壺X=S����������X=A��Ӧ�ó���A��X=B��Ӧ�ó���B
            */
            case 0: 
                Ctrl.Disp.Mode++;
                BSP_DispClrAll(); 
#if defined     (IMAGE_A)
                /***********************************************
                * ������ ����A
                */
                uprintf("SF-A");
#elif defined   (IMAGE_B)
                /***********************************************
                * ������ ����B
                */
                uprintf("SF-B");
#else
                /***********************************************
                * ������ ��׼����
                */
                uprintf("SF-S");
#endif
                break;
            /*******************************************************************
            * ������ ������Լ�
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            *        ���ݣ���8.8.8.8.����
            *        ���壺8.8.8.8.Ϊȫ��.
            */
            case 1: 
                Ctrl.Disp.Mode++;                  
                uprintf("ID-%d", Ctrl.Comm.pch->NodeAddr);
                break;
            /*******************************************************************
            * ������ ��ʾӲ���汾
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            *        ���ݣ����ݣ���HV21����
            *        ���壺HVΪӲ��21Ϊ�汾�š�
            */
            case 2: 
                Ctrl.Disp.Mode++;
                uprintf("HV22");
                
                break;
            /*******************************************************************
            * ������ ��ʾ����汾
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            *        ���ݣ����ݣ���SV25�� 
            *        ���壺SVΪ�����25Ϊ�汾�š�
            */
            case 3: 
                Ctrl.Disp.Mode++;
                uprintf("SV22");                
                
                break;
            /*******************************************************************
            * ������ ��ʾ�߶Ȳ�
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            *        ���ݣ���XX.Xd����
            *        ���壺XX.XΪ�߶�ֵ����λ.1mm��
            */
            case 4: 
                Ctrl.Disp.Mode++;
                DispValue   = (INT16U)Ctrl.Sen.Para.dat.ChkedDiffHight;// 800;
                /***********************************************
                * ������ 
                */
                uprintf("d%d",DispValue);
                //BSP_DispWrite((int)DispValue,"%D",RIGHT,0,(1<<2),1,20,0);
                
                break;
            /*******************************************************************
            * ������ ���ϴ�����ʾ
            *        ʱ�䣺����ʱ��ʾ���й���ʱ����ʾģ��Ϊѭ����ʾʱ4sˢ��һ�Σ�
            *        ���ݣ���E-XX����
            *        ���壺XX���ϴ��롣
            */
            case 5:  
                Ctrl.Disp.Mode++;
                DispValue   = Ctrl.Para.dat.SenSts.Dat[1];
                /***********************************************
                * ������ 
                */
                uprintf("E-%02X",DispValue);                
                break;
            /*******************************************************************
            * ������ �������߶ȡ����ϴ�����ʾ
            */
            case 6: {
                INT08U  err = 0;
                INT08U  dot = 0;
                /***********************************************
                * ������ �����ͳ��װ�����ӳɹ�������ʾͳ��װ�÷�������ֵ
                */
                switch ( Ctrl.Para.dat.Sel.udat.DispSel ) {
                    
                /***********************************************
                * ������ ��ʾģʽ��0��������ʾ�������Ͽ���ʾ�߶�
                */
                case 0:
                    /***********************************************
                    * ������ ��ͳ��ģ�����ӳɹ���ʾ����
                    */
                    if ( Ctrl.Comm.ConnectFlag ) {
                        pValue  = (INT16S *)&Ctrl.Comm.Rd.Data.Oil;
                    /***********************************************
                    * ������ �����ͳ��װ������ʧ�ܣ�����ʾ��ǰ�����߶�
                    */
                    } else {
                        pValue  = (INT16S *)&Ctrl.Sen.C.Hight;
                        dot     = 1;
                    }
                    break;
                /***********************************************
                * ������ ��ʾģʽ��1����ʾ�߶�
                */
                case 1:
                    pValue  = (INT16S *)&Ctrl.Sen.C.Hight;
                    dot     = 1;
                    break;
                /***********************************************
                * ������ ��ʾģʽ��2��ѭ����ʾ
                */
                case 2: 
                    /***********************************************
                    * ������ ��ʾ����
                    */
                    if ( step < 2 ) {
                        pValue  = (INT16S *)&Ctrl.Comm.Rd.Data.Oil;
                    /***********************************************
                    * ������ ��ʾ�߶ȣ�����������ʱ��ʾ���ϴ���
                    */
                    } else if ( step < 4 ) {
                        dot     = 1;
                        /***********************************************
                        * ������ ��λ�������й���
                        */
                        if (Ctrl.Para.dat.SenSts.Udat.Err.HSenBrokenErr || 
                            Ctrl.Para.dat.SenSts.Udat.Err.HSenShortErr ||
                        /***********************************************
                        * ������ ��λ�������й���
                        */
                            Ctrl.Para.dat.SenSts.Udat.Err.LSenBrokenErr || 
                            Ctrl.Para.dat.SenSts.Udat.Err.LSenShortErr ) {
                            err = 1;
                            DispValue   = Ctrl.Para.dat.SenSts.Dat[1];
                        } else {                        
                            pValue  = (INT16S *)&Ctrl.Sen.C.Hight;
                        }
                    }
                    break;
                /***********************************************
                * ������ ��ʾģʽ��3���ر���ʾ
                */
                case 3:
                    BSP_DispOff();
                    goto exit;
                    break;
                /***********************************************
                * ������ ��ʾģʽ������ֵ�ָ�Ĭ��ֵ
                */
                default:
                    Ctrl.Para.dat.Sel.udat.DispSel  = 0;
                }
                
                BSP_DispClrAll();
                /***********************************************
                * ������ 
                */
                if ( err ) {
                    uprintf("E-%02X",DispValue);
                } else {
                    CPU_CRITICAL_ENTER();
                    DispValue  = *pValue;
                    CPU_CRITICAL_EXIT();
                    
                    if ( DispValue > 9999 ) {
                        DispValue   /= 10;
                    }
                    BSP_DispWrite((int)DispValue,"%",RIGHT,0,(dot<<1),1,20,0);
                }
                
                /***********************************************
                * ������ 
                */  
                if (++step >= 4)
                    step    = 0;
            } break;
            /*******************************************************************
            * ������ ������ʾ
            */
            case 7:
                /***********************************************
                * ������ 
                */
                if ( step < 3 ) {
                    pValue  = (INT16S *)&Ctrl.Sen.H.HightSec;
                 } else if ( step < 6 ) {
                    pValue  = (INT16S *)&Ctrl.Sen.L.HightSec;
                } else if ( step < 9 ) {
                    pValue  = (INT16S *)&Ctrl.Sen.C.CalcDiffHightSec;
                }
            
                BSP_DispClrAll();
                    
                DispValue  = *pValue;
                /***********************************************
                * ������ 
                */
                //BSP_DispWrite((u16)*pValue,"%",RIGHT,0x00,1,20,0); 
                if ( DispValue > 9999 ) {
                    DispValue   /= 10;
                    BSP_DispWrite((int)DispValue,"%",RIGHT,0,0x01,1,20,0);
                } else {
                    if ( ( step < 3 ) ) {
                        DispValue   += Ctrl.Sen.C.ChkedDiffHight;
                        //DispValue   /= 10;
                        BSP_DispWrite((int)DispValue,"%",RIGHT,0,(0<<0),1,20,0);
                    } else if ( step < 6 ) {
                        //DispValue   /= 10;
                        BSP_DispWrite((int)DispValue,"%",RIGHT,0,(1<<0),1,20,0);                
                    } else {
                        BSP_DispWrite((int)DispValue,"%d",RIGHT,0,(1<<2),1,20,0);                 
                    }
                }
                if (++step >= 9)
                    step    = 0;
                break;
            default:
                Ctrl.Disp.Mode  = 0;
                break;
            }
            /*******************************************************************
            * ������ ������ʾ����
            */
            BSP_DispEvtProcess();
        exit:
            /***********************************************
            * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
            */
            dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
            if ( dly  <= 0 ) {
                dly   = 1;
            }
#if ( OSAL_EN == DEF_ENABLED )
            osal_start_timerEx( OS_TASK_ID_DISP,
                                OS_EVT_DISP_TICKS,
                                dly);
            
            return ( task_event ^ OS_EVT_SEN_SAMPLE );
        }
#endif
        
#if ( OSAL_EN == DEF_ENABLED )
        return 0;
#else
        BSP_OS_TimeDly(dly);
    }
#endif
}

/*******************************************************************************
 * ��    �ƣ� APP_DispInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-12-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitDisp(void)
{
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    BSP_DispInit();
    Ctrl.Disp.Led   = (StrLedDisp  *)&LedDispCtrl;
    Ctrl.Disp.Mode  = 0;
    BSP_DispOff();
    BSP_DispSetBrightness(10);
    uprintf("8888");
    BSP_DispClrAll();
    BSP_DispEvtProcess();
    
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_DISP;
    /*************************************************
    * �����������¼���ѯ
    */
#if ( OSAL_EN == DEF_ENABLED )
#if defined     (IMAGE_A)
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        1);
#elif defined   (IMAGE_B)
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        1);
#else
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        CYCLE_TIME_TICKS);
#endif
#else
#endif
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif