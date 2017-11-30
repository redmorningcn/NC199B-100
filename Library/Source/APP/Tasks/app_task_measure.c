/*******************************************************************************
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� store �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Store �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� STORE �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_STORE_PRIO ��
 *                                            �� �����ջ�� APP_TASK_STORE_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskStoreCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Store ��
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


//#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
//const  CPU_CHAR  *app_task_key__c = "$Id: $";
//#endif

#define APP_TASK_KEY_EN     DEF_ENABLED
#if APP_TASK_KEY_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
//������Ŀ�������   (OS_CFG_TICK_RATE_HZ)Ϊ1S
//#define  KEY_CYCLE_TIME_TICKS     (OS_CFG_TICK_RATE_HZ * 1u)

//�����ڵ�ŷ�  pd.3
#define     DCF_GPIO_AIR_IN      303  
//�����ڵ�ŷ� pd.2
#define     DCF_GPIO_AIR_OUT     302   
     
     
/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskMeasureTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskMeasureStk[ APP_TASK_MEASURE_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskMeasure          (void *p_arg);
static  void    APP_MeasureInit         (void);
void            StartAirMeasure         (void);

//static  void    MeasureValueHandle      (StrCtrlDCB *dcb);


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern OS_SEM   App_BeepSem;

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskMeasureCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskMeasureCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskMeasureTCB,                  // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Measure",                  // ��������
                 (OS_TASK_PTR ) AppTaskMeasure,                     // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_KEY_PRIO,              // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskMeasureStk[0],               // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE / 10,     // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_KEY_STK_SIZE,          // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������

}

/*******************************************************************************
 * ��    �ƣ� AppTaskMeasure
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskMeasure (void *p_arg)
{
//    OS_ERR      err;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_MeasureInit();

    /***********************************************
    * ������Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) {

        /***********************************************
        * ������ �������Ź���־��λ
        */
//        OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
//                     (OS_FLAGS     ) WDT_FLAG_KEY,
//                     (OS_OPT       ) OS_OPT_POST_FLAG_SET,
//                     (CPU_TS       ) 0,
//                     (OS_ERR      *) &err);
        /***********************************************
        * ������ ���а����¼�ʱ��ȡ��ֵ�Ͱ����¼�
        */
        StartAirMeasure();
        
        BSP_OS_TimeDlyHMSM(0,10,0,0);       //10min
    }
}
/*******************************************************************************
 * �������ߣ� redmorningcn.
 * �������ڣ� 2017-11-22
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void        Measure_GPIO_Init(void)
{
    //�˿ڳ�ʼ��ΪI/O��
    GPIO_PinselConfig(DCF_GPIO_AIR_IN,0);
    GPIO_PinselConfig(DCF_GPIO_AIR_OUT,0);
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN, 0);   //�رյ�ŷ�
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,0);   //�رյ�ŷ�
    
    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 0;  //��·�򿪱�ʶ��1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 1;         
}

/*******************************************************************************
 * �������ߣ� redmorningcn.
 * �������ڣ� 2017-11-23
 * ��    �ģ� �� ��ŷ����ȴ�������ٴ����롣Ȼ���ò�����ʶ
              �ر� ��ŷ����ȹ����룬�ڹ������Ȼ���ò��Ա�ʶ
 * �޸����ڣ�
 *******************************************************************************/
void    CloseAir(void)
{
    OS_ERR  err;

    ////////�رղ���
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN,0);    //�ȹ�����
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //��ʱ0.1s
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,0);   //�ٹ����
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //��ʱ0.5s    
}

/*******************************************************************************
 * �������ߣ� redmorningcn.
 * �������ڣ� 2017-11-22
 * ��    �ģ� �� ��ŷ����ȴ�������ٴ����롣Ȼ���ò�����ʶ
              �ر� ��ŷ����ȹ����룬�ڹ������Ȼ���ò��Ա�ʶ
 * �޸����ڣ�
 *******************************************************************************/
void    StartAirMeasure(void)
{
    OS_ERR  err;

    ////////��������
    GPIO_SetOrClearValue(DCF_GPIO_AIR_OUT,1);   //�����
    OSTimeDly(500, OS_OPT_TIME_DLY, &err);      //��ʱ0.5s
    
    GPIO_SetOrClearValue(DCF_GPIO_AIR_IN,1);    //�����
    OSTimeDly(100, OS_OPT_TIME_DLY, &err);      //��ʱ0.1s

    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 1;  //��·�򿪱�ʶ��1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 0; 
    
    OSTimeDly(10000, OS_OPT_TIME_DLY, &err);    //��ʱ10s
    Ctrl.Para.dat.sRunPara.SysSta.StartMeasure = 1;
    
    OSTimeDly(2000, OS_OPT_TIME_DLY, &err);     //��ʱ2s    
    //��ʼ����ʱ��
    osal_start_timerEx( OS_TASK_ID_STORE,
                      OS_EVT_STORE_TICKS,
                      1);                       //�ô洢��ʱ��1�����������洢
    Ctrl.Para.dat.sRunPara.SysSta.Store   = 1;  //���Խ������ݴ��棨�������̵����ݣ�
    
    OSTimeDly(2000, OS_OPT_TIME_DLY, &err);     //��ʱ2s   
    
    CloseAir();                                 //�ر���·
      
    Ctrl.Para.dat.sRunPara.SysSta.OpenAir = 0;  //��·�򿪱�ʶ��1
    Ctrl.Para.dat.sRunPara.SysSta.CloseAir= 1;     
    
    Ctrl.Para.dat.sRunPara.SysSta.StartMeasure = 0;
}


/*******************************************************************************
 * ��    �ƣ� APP_MeasureInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static void APP_MeasureInit(void)
{
    /***********************************************
    * ������ ��ʼ���������õ������Ӳ��
    */
    Measure_GPIO_Init();            //��ʼ���˿�

    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    //WdtFlags |= WDT_FLAG_KEY;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif