/*******************************************************************************
 *   Filename:      app.c
 *   Revised:       $Date: 2013-06-30$
 *   Revision:      $
 *	 Writer:		Wuming Shen.
 *
 *   Description:
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <app.h>
#include <includes.h>
//#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app__c = "$Id: $";
#endif

/*******************************************************************************
 * CONSTANTS
 */
/****************************************************
* ������ ι���������ִ�����ڣ���λ:MS��
*/
#define CYCLE_TIME_TICKS            (OS_CFG_TICK_RATE_HZ)

//#define DEBUG_SIM                   DEF_ENABLED    // ������濪��
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
/***************************************************
* ������ uC/OS-III ������ƿ飨TCB��
*
*       ����        TCB����
*/
static  OS_TCB      AppTaskStartTCB;

/***************************************************
* ������ uC/OS ������ƿ飨STACKS��
*
*       ����        ��ջ����                ��ջ��С
*/
static  CPU_STK     AppTaskStartStk         [ APP_TASK_START_STK_SIZE ];

/***********************************************
* ������ ������־��
*/
OS_FLAG_GRP         WdtFlagGRP;                     //���Ź���־��
OS_FLAGS            WdtFlags;

/***************************************************
* ������ ��ʱ������
*/
OS_TMR              OSTmr0;                         // ��ʱ��1
/****************************************************
* ������ ����WdtTimeoutSec��ʱ�仹û��ι����ϵͳ��λ��
*        ��λΪ �루S��
*/
#define             WdtTimeoutSec           60

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 *
 * ��������         ��������                ��ڲ���
 */
static  void        AppTaskCreate           (void);
static  void        AppObjCreate            (void);
/***********************************************
* ������ ��������
*/
//static  void        App_Init                (void);
static  void        AppTaskStart            (void *p_arg);
/***********************************************
* ������ ��ʱ���ص�����
*/
void                OSTmr0_callback         (OS_TMR *ptmr,void *p_arg);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
extern  void        APP_TempEventProcess    (void);
extern  void        App_InitStartHook       (void);
extern  void        App_InitEndHook         (void);
extern  void        AppTaskStart            (void *p_arg);
extern  void        App_Main                (void);

/*******************************************************************************
 * @fn      &:main
 * @brief   &:First function called after startup.
 * @return  &:don't care
 ******************************************************************************/
int main (void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ��һ�����ú��������Բ�����
    */
    App_Main();
//    
//    ////20171128
//    BSP_Init();
    
#if DEBUG_SIM != DEF_ENABLED
//#if !defined (LIBRARY)
    /***********************************************
    * ������ ����64KоƬ���ã���������оƬ����ʹ��
    */
    INT16U FlashSize    = *(INT16U*)(0x1FFFF7E0);  // ���������Ĵ���   
    if ( FlashSize != STM32_FLASH_SIZE )           // STM32F103RC  256K����option�ж���
        while(1);
//#endif
#endif
    /***********************************************
    * ������ Init uC/OS-III.
    */
    OSInit(&err);

    //dog
    IWDG_ReloadCounter();

    
    //test_DM412_Led();
    /***********************************************
    * ������ ������ʼ����
    */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                // ������ƿ飨��ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Start",                // ��������
                 (OS_TASK_PTR ) AppTaskStart,                   // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_START_PRIO,            // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskStartStk[0],             // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,   // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,        // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 0u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������

  
    /***********************************************
    * ������ ���� uC/OS-III
    */
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X10000);
    SCB->VTOR = (NVIC_VectTab_FLASH | 0X10000);
        
    OSStart(&err);
}

/*
********************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's 
*               text, you MUST initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning 
*               because 'p_arg' is not used.  The compiler should not generate 
*               any code for this statement.
********************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
    INT32U      ticks;
    INT32S      dly;
//    INT16U  	TimeOutCnt     = 0;                 //���Ź���ʱ������
    uint8_t     ReadRTCTimeCnt = 0;
   (void)p_arg;

   
    //test_DM412_Led();

    /***********************************************
    * ������ ����STM32��ϵͳʱ�ӣ�I/O�����ã���ʱ����
    */
#if DEBUG_SIM != DEF_ENABLED
    BSP_Init();                                                 /* Initialize BSP functions                             */
#endif
    CPU_Init();
    
    /***********************************************
    * ������ ��ʼ���δ�ʱ��������ʼ��ϵͳ����ʱ�ӡ�
    */
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
    
    /***********************************************
    * ������ ����UCOS���Ӻ���
    */
    App_OS_SetAllHooks();
    
    /***********************************************
    * ������ ��ʼ���ڴ����ģ��
    */
#if (LIB_MEM_CFG_ALLOC_EN == DEF_ENABLED)
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
#endif
    
    /***********************************************
    * ������ ���Ź���ʼ��
    */
#if DEBUG_SIM != DEF_ENABLED
#if defined     (RELEASE)
    BSP_WDT_Init(BSP_WDT_MODE_ALL);
#else
    BSP_WDT_Init(BSP_WDT_MODE_NONE);
#endif  
#endif
    
    WdtReset();
    /***********************************************
    * ������ E2PROM��ʼ��
    */
#if DEBUG_SIM != DEF_ENABLED
    //App_Init();
#endif
    extern StrNcProtocol   ComPackCtrl[4];
//    Ctrl.MtrPar     = &Mater;
//    Ctrl.Com.pack   = &ComPackCtrl[0];
//    Ctrl.Dtu.pack   = &ComPackCtrl[1];
    
    
    App_TaskCommCreate();
    
    App_TaskMeasureCreate();
    /***********************************************
    * ������ ι��
    */
    WdtReset();

    /***********************************************
    * ������ ������û����������ʱCPU������
    */
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    /***********************************************
    * ���������ڲ�����ֹ�жϵ�ʱ��
    */
    CPU_IntDisMeasMaxCurReset();

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(115200);                                       /* Enable Serial Interface                              */
#endif
    
    /***********************************************
    * ������ ���������¼���,��־��
    */
    AppObjCreate();                                             /* Create Application Objects                           */

    /***********************************************
    * ������ ������������
    */
    AppTaskCreate();                                            /* Create Application Tasks                             */

//    Ctrl.Com.SlaveAddr     = 0xA3;
    /***************************************************************************
    * ������ÿһ�������ڴ���ʱ������һ��ι����־λ
    *	    ÿ��������ִ�й����У���δ�������Ź���ʱʱ����
    *	    ����Ӧ�ı�־λ��λ��������ÿ���Ӳ�ѯһ��
    *	    �Ѿ�ע�ᵽι����־λ��ı�־λ�������ǰ��־
    *	    λ��ע��ı�־λ��ȣ����ʾÿ������ǰ������
    *	    �״̬����ʱ����ʱ���������㣬���¿�ʼ��ʱ����
    *	    ���ι����־λû��ȫ����λ���򽫳�ʱ������+1����
    *	    ����ֵ�����趨�ļ���ֵʱ��ϵͳ����������
    */
    while (DEF_TRUE) {
        /***********************************************
        * ������ �õ�ϵͳ��ǰʱ��
        */
        ticks = OSTimeGet(&err);
        ReadRTCTimeCnt  ++;

        /***********************************************
        * ������ ι��
        */
        WdtReset();
        IWDG_ReloadCounter();
        
        /***********************************************************************
        * ������ �������Ź���־���飬 �ж��Ƿ�����������ι��
        */
        OSFlagPend(( OS_FLAG_GRP *)&WdtFlagGRP,
                   ( OS_FLAGS     ) WdtFlags,
                   ( OS_TICK      ) 50,
                   ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ALL,   //ȫ����һ
                   ( CPU_TS      *) NULL,
                   ( OS_ERR      *)&err);
        
        if(err == OS_ERR_NONE) {                                //����������ι��
            //TimeOutCnt = 0;                                     //��ʱ����������
            BSP_LED_Flash( 1, 1, 40, 40);
            OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,            //�������б�־
                         (OS_FLAGS     ) WdtFlags,
                         (OS_OPT       ) OS_OPT_POST_FLAG_CLR,
                         (CPU_TS       ) 0,
                         (OS_ERR      *) &err);
        } else {                                                //������������ι��
//            TimeOutCnt++;                                       //��ʱ��������1
//            if(TimeOutCnt > WdtTimeoutSec) {                    //ι����ʱ
//                /***********************************************
//                * ������ �������������ģʽ
//                */
//                if ( ( Iap.Status != IAP_STS_DEF ) && ( Iap.Status != IAP_STS_SUCCEED ) ) {
//                } else {
//                    SystemReset();							    //ϵͳ����
//                    while(1){
//                        /*******************************************************
//                        * ������ ��ʱ��Ȳ���ĳ����Ŀ��Ź���־λע�ᣬ˵�����������ˣ�
//                        *        ָʾ�ƿ�����˸��������ѭ���ȴ�ϵͳ��λ
//                        */
//                        BSP_LED_Toggle(1);
//                        Delay_Nms(100);
//                    };
//                }
//            } else {
//                BSP_LED_Flash( 1, 1, 500, 500); 
//            }
        }
        /***********************************************
        * ������ ȥ���������е�ʱ�䣬�ȵ�һ������������ʣ����Ҫ��ʱ��ʱ��
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  <= 0 ) {
            dly   = 0;
        }else if(dly > CYCLE_TIME_TICKS)
        {
            dly   =  CYCLE_TIME_TICKS;
        }
        OSTimeDly(dly, OS_OPT_TIME_DLY, &err);
    }
}

//
///*******************************************************************************
// * ��    �ƣ� App_Init
// * ��    �ܣ� �û�Ӧ�ó�ʼ��
// * ��ڲ����� ��
// * ���ڲ����� ��
// * ���� ���ߣ� ������.
// * �������ڣ� 2015-03-19
// * ��    �ģ�
// * �޸����ڣ�
// *******************************************************************************/
//static void App_Init(void)
//{
//    /********************************************************************
//    * ������ ��ʼ��EEPROM
//    */
//    App_PraInit();
//    WdtReset();
//    Ctrl.Para.dat.Password  = 0; 
//    /********************************************************************
//    * ������ �ڷ���RELEASE�汾ʱ����FLASH���϶�����
//    */
//#ifndef DEBUG
//    /***********************************************
//    * ������Flashд�����Ͷ����������ӱ�����
//    *       ��ʹ��JFLASH����оƬ�ſ��Խ����´���������档
//    */
//    if( FLASH_GetReadOutProtectionStatus() != SET ){
//        FLASH_Unlock();
//        FLASH_ReadOutProtection(ENABLE);
//        FLASH_Lock();
//    }
//    /***********************************************
//    * ������Flashд�����Ͷ����������ӱ�����
//    *       ��ʹ��JFLASH����оƬ�ſ��Խ����´���������档
//    */
//#endif
//}
//
/*
********************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
********************************************************************************
*/

static  void  AppObjCreate (void)
{
    OS_ERR    err;

    /***********************************************
    * �������������Ź���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&WdtFlagGRP,
                 ( CPU_CHAR     *)"Wdt Flag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);

    /***********************************************
    * ����������һ����ʱ�� OS_OPT_TMR_ONE_SHOT, OS_OPT_TMR_PERIODIC
    */
    //OSTmrCreate ((OS_TMR               *)&OSTmr0,
    //             (CPU_CHAR             *)"tmr0",
    //             (OS_TICK               )60 * OS_CFG_TMR_TASK_RATE_HZ,
    //             (OS_TICK               )60 * OS_CFG_TMR_TASK_RATE_HZ,
    //             (OS_OPT                )OS_OPT_TMR_ONE_SHOT,
    //             (OS_TMR_CALLBACK_PTR   )OSTmr0_callback,
    //             (void                 *)NULL,
    //             (OS_ERR               *)&err);
}


/*******************************************************************************
 * ��    �ƣ� OSTmr0_callback
 * ��    �ܣ� ��ʱ��0�Ļص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ����  �ߣ� ������
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ��ʱ���ص���������ʹ����ʱ����
 *******************************************************************************/
void OSTmr0_callback(OS_TMR *ptmr,void *p_arg)
{
    /***********************************************
    * ������ע�⣺�ص�������ִ�е�ʱ�䲻�˹���
    */

}

/*
********************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
********************************************************************************
*/
static  void  AppTaskCreate (void)
{
    /***********************************************
    * ������ �ڴ˴���������
    */
//    extern  void OS_TaskCreateHook(void);
//    OS_TaskCreateHook();
    
    /***********************************************
    * ������ �ڴ˴�����OSAL����
    */
//#if ( OSAL_EN == DEF_ENABLED )
    App_TaskOsalCreate();
    /***********************************************
    * ������ �ڴ˴���UCOS������
    */
//#else
//    App_TaskSensorCreate();
//    //App_TaskControlCreate();
//#endif
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/