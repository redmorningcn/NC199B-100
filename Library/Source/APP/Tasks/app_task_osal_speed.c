/*******************************************************************************
 *   Filename:       app_task_store.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� speed �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Speed �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� SPEED �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_SPEED_PRIO ��
 *                                            �� �����ջ�� APP_TASK_SPEED_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskSpeedCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Speed ��
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
#include <cpu.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_speed__c = "$Id: $";
#endif

#define APP_TASK_SPEED_EN     DEF_ENABLED
#if APP_TASK_SPEED_EN == DEF_ENABLED				//
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
extern  uint16  l_speed1;
extern  uint16  l_speed2;
extern  uint16  l_speedcs;
extern  uint8   l_locoworksta;


#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskSpeedTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskSpeedStk[ APP_TASK_SPEED_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
extern	void    BSP_SpeedInit(void);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern	stcSysCtrl       sCtrl;       	//	ȫ�ֽṹ������
/*******************************************************************************/



/*******************************************************************************
 * ��    �ƣ�  TaskInitSpeed
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void    BSP_SpeedInit(void)
{
	
    Time0Init();        //��ʼ���ٶȴ�����1
    
	Time1Init();        //��ʼ���ٶȴ�����2
    
	Time2Init();        //��ʼ�����ټ��
    
    InitLocoStaIO();    //��ʼ���������IO��
}

/*******************************************************************************
 * ��    �ƣ�  TaskInitSpeed
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitSpeed(void)
{    
//    /***********************************************
//    * ������ ��ʼ���������õ������Ӳ��
//    */
    BSP_SpeedInit();			//��ʼ��Flash�ײ���غ���

//��ʼ����ʱ��
    osal_start_timerEx( OS_TASK_ID_SPEED,
                      OS_EVT_SPEED_TICKS,
                      1000);
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * ��    �ƣ� 		AppTaskSpeed
 * ��    �ܣ� 		��������
 * ��ڲ����� 	p_arg - �����񴴽���������
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmorningcn.
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
osalEvt  TaskSpeedEvtProcess(INT8U task_id, osalEvt task_event)
{
    OS_ERR      err;
    INT32U      ticks;
    INT32S      dly;
    CPU_SR_ALLOC();

    if( task_event & OS_EVT_SPEED_TICKS ) {

            SpeedAndDistTast();         //�ٶȼ��㣬�ڶ����� l_speed1��l_speed2��l_speedcs��ȫ�ֱ���
            
            CheckLocoStaTask();         //������⣬������l_locoworkstaȫ�ֱ���
            
            //���ٶȡ������ȸ�ֵ
            sCtrl.sRec.Myspeed1         = l_speed1;
            sCtrl.sRec.Myspeed2         = l_speed2;
            sCtrl.sRec.MyEngRotSpd      = l_speedcs;
            
            sCtrl.sRec.MyLocoWorkState  = l_locoworksta;
            
            osal_start_timerEx( OS_TASK_ID_SPEED,
                                OS_EVT_SPEED_TICKS,
                                1000);
            
            return ( task_event ^ OS_EVT_SPEED_TICKS );
        }
}



/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif
