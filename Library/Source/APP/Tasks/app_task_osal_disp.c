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
#include <app_ctrl.h>
#include <bsp_max7219.h>
#include <osal_timer.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_disp__c = "$Id: $";
#endif

#define APP_TASK_DISP_EN     DEF_ENABLED
#if APP_TASK_DISP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
//#define CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 2u)

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
//static  CPU_STK  AppTaskDispStk[ APP_TASK_DISP_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void    comm_mtr_err_judye(void);

     
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskDisp           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
uint8 JudgeFlashIDErrFlg(void) ;  

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern stcCtrlDCB           Ctrl;


/*******************************************************************************/
static  uint8   dis_mode    = 0;

void    SetDispNow(void)
{
    BSP_DispEvtProcess();
    dis_mode = 13*2;
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
extern  MODBUS_CH   *g_pch; 

//#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskDispEvtProcess(osalTid task_id, osalEvt task_event)
//#else
//static  void  AppTaskDisp (void *p_arg)
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
//    TaskInitDisp();
//    
//    while (DEF_TRUE) {
//#endif
//        /***********************************************
//        * ������ �������Ź���־��λ
//        */
//        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
//                     (OS_FLAGS     ) WDT_FLAG_DISP,
//                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                     (CPU_TS       ) 0,
//                     (OS_ERR      *) &err);
//        /***********************************************
//        * ������ �õ�ϵͳ��ǰʱ��
//        */
//        ticks = OSTimeGet(&err);
//        
//#if ( OSAL_EN == DEF_ENABLED )

        if( task_event & OS_EVT_DISP_TICKS ) {
//#else
//#endif
            static  uint32   times   = 0;
            static  uint8   disflg  = 0;

            stcTime         sTime;

            /***********************************************
            * ������ ������ʾģʽ��ʾ
            */
            switch(dis_mode/2) {
            /*******************************************************************
            * ������ ��ʾװ��ʱ��
            */
            case 0:         //����
                dis_mode++;
                uprintf("%d",Ctrl.Para.dat.sProductInfo.sLocoId.Type);  
                    break;
            case 1:         //����
                dis_mode++;
                uprintf("%d",Ctrl.Para.dat.sProductInfo.sLocoId.Num);  
                    break;

            /*******************************************************************
            * ������ ��ʾ�����汾
            *        ʱ�䣺������ʾһ�Σ���ʾ2s��
            */
            case 2: 
                dis_mode++;
                Ctrl.Para.dat.SoftWareID = 0x0101;
                uprintf("SV.%02X",(uint8)(Ctrl.Para.dat.SoftWareID>>8));
                
                break;
            /*******************************************************************
            * ������ ��ʾӲ���汾
            *        ������Ӳ��    
            */
            case 3: 
                dis_mode++;
                uprintf("HV1.0");
                
                break;
            /*******************************************************************
            * ������ �۳�����ֵ
            */
            case 4: 
                dis_mode++;
                uprintf("%d",Ctrl.Para.dat.sAirPara.Dust_modefy);  
                break;
            /*******************************************************************
            * ������ ʪ������ֵ
            */
            case 5:  
                dis_mode++;
                uprintf("%d",Ctrl.Para.dat.sAirPara.Hum_modefy);  
                break;  
            /*******************************************************************
            * ������ Voc����ֵ
            */
            case 6:  
                dis_mode++;
                uprintf("%d",Ctrl.Para.dat.sAirPara.Voc_modefy);  
                break; 
            /*******************************************************************
            * ������ ���ͳ���
            */
            case 7:  
                dis_mode++;
                uprintf("      ");

                break; 
            /*******************************************************************
            * ������ ȫ��
            */
            case 8:  
                dis_mode++;
                uprintf("88888888");
                
                break;   
            /*******************************************************************
            * ������ �߶�ֵ
            */
            case 9:    
                dis_mode++;
                
            case 13:  
                dis_mode++;
                //����ֵ��ֻ��ʾ
                //disflg = BSP_DispEvtProcess();  //ִ����ʾ��������������ʾ������ֵ������������ֵ

                break;   
//            case 14:  
//               dis_mode++;
//                //����ֵ��ֻ��ʾ
//                //disflg = BSP_DispEvtProcess();  //ִ����ʾ��������������ʾ������ֵ������������ֵ
//                break;   
//            case 15:  
//                dis_mode++;
//                //disflg = BSP_DispEvtProcess();  //ִ����ʾ��������������ʾ������ֵ������������ֵ
//                //����ֵ��ֻ��ʾ
//                times = 0;
//                break;   
            /*******************************************************************
            * ������ ��������ʾ������ͨѶ�����жϼ����ϴ���
            */
            default:
              
                if(disflg != 0)                                 //������������ݣ�����ʾ��������
                    break;
                    
                switch(times++%7)                               //ѭ����ʾ��ˮ
                {
                    case 0:uprintf("H2o");   
                            break;                              //��ʾʪ����ʾ
                    case 1:uprintf("%4d",Ctrl.sRec.sAir.Humidity);   
                            break;                              //��ʾʪ��ֵ
                    
                    case 2:uprintf("o1L");   
                            break;                              //��ʾ�ͺ���
                    case 3:uprintf("%4d",Ctrl.sRec.sAir.VOCs);     
                            break;                              //��ʾʪ��ֵ

                    case 4:uprintf("dust");                            
                            break;                              //��ʾ�۳���ʾ
                    case 5: uprintf("%4d",Ctrl.sRec.sAir.Dusts_2D5);   
                            break;                              //��ʾ�۳�ֵ  
                    
                    case 6:                                 
                            break;                              //�й�����ʾ���ϴ���
                }
                
                break;
            }
            
            disflg = BSP_DispEvtProcess();                      //ִ����ʾ��������������ʾ������ֵ������������ֵ
            
            osal_start_timerEx( OS_TASK_ID_DISP,
                                OS_EVT_DISP_TICKS,
                                500);
            return ( task_event ^ OS_EVT_DISP_TICKS );
        }
    return  task_event;
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
//    /***********************************************
//    * ������ ��ʼ���������õ������Ӳ��
//    */
    BSP_DispInit();

    BSP_DispOff();
    
    BSP_DispSetBrightness(10);
    BSP_DispClrAll();
    uprintf("88888888");
    BSP_DispEvtProcess();
    

    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_DISP;
    /*************************************************
    * �����������¼���ѯ
    */
    osal_start_timerEx( OS_TASK_ID_DISP,
                        OS_EVT_DISP_TICKS,
                        100);
}

void    comm_mtr_err_judye(void)
{
    /*******************************************************************
    * �������ͳߴ���ͨѶ�����ж�
    */
//    static  uint32  comm_mtr_err_times = 0;
//    
//    if(         Ctrl.Mtr.ConnCtrl[0].ErrFlg == 1 
//       &&       Ctrl.Mtr.ConnCtrl[1].ErrFlg == 1){
//
//        comm_mtr_err_times++;                   //�������
//        
//        if(comm_mtr_err_times > 4 && Ctrl.sRunPara.SysSta == 0 ){
//            Ctrl.sRunPara.SysSta   = 83;       //����װ��1������װ��2���й���
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //������븳ֵ
//        }
//    }else if(   Ctrl.Mtr.ConnCtrl[0].ErrFlg == 1){
//        comm_mtr_err_times++;                   //�������
//        
//        if(comm_mtr_err_times > 8 && Ctrl.sRunPara.SysSta == 0 ){
//            Ctrl.sRunPara.SysSta   = 71;       //����װ��1����
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //������븳ֵ
//        }
//        
//    }else if(   Ctrl.Mtr.ConnCtrl[1].ErrFlg == 1){
//        comm_mtr_err_times++;                   //�������
//        
//        if(comm_mtr_err_times > 8 && Ctrl.sRunPara.SysSta == 0 )
//            Ctrl.sRunPara.SysSta   = 81;       //����װ��2����
//        
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta;    //������븳ֵ
//    }else {
//        comm_mtr_err_times = 0;                 //�������
//
//        if(     Ctrl.sRunPara.SysSta == 81
//            ||  Ctrl.sRunPara.SysSta == 71
//            ||  Ctrl.sRunPara.SysSta == 83){
//            uprintf("E-%d",Ctrl.sRunPara.SysSta+100);  //��ʾ���ϴ���+100
//            
//            Ctrl.sRec.ErrorCode    = Ctrl.sRunPara.SysSta+100;//������븳ֵ
//            Ctrl.sRunPara.SysSta   = 0;        //������ϴ���
//        }
//    }
}
/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif