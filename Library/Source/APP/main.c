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
#include <iap.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *main__c = "$Id: $";
#endif

/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 2)

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 *
 * ��������         ��������                ��ڲ���
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */



void    app_init_sctrl(void)
{
    Ctrl.Para.dat.Password      =   6237;
//    Ctrl.Para.dat.SoftWareID    =   1705;   //���£��汾
    

//   	FRAM_StoreRunPara((stcRunPara *) &Ctrl.sRunPara);
    FRAM_ReadRunPara((stcRunPara *) &Ctrl.Para.dat.sRunPara);   //��ȡ���в���
 
//��ʼ��ʾ    //�洢ʱ����
    Ctrl.Para.dat.sRunPara.StartFlg = 1;  
    //Ctrl.Para.dat.sRunPara.SysSta   = 0;        
        
    Ctrl.Para.dat.sRunPara.RecClear = 0;
    Ctrl.Para.dat.sRunPara.StoreTime= 60;       
    Ctrl.Para.dat.sRunPara.SysReset = 0;
    
    Ctrl.Para.dat.sAirPara.sStandard_Dust.GoodLimit     = 150;
    Ctrl.Para.dat.sAirPara.sStandard_Dust.NormalLimit   = 500;
    
    Ctrl.Para.dat.sAirPara.sStandard_VOC.GoodLimit      = 300;
    Ctrl.Para.dat.sAirPara.sStandard_VOC.NormalLimit    = 1000;
    
    Ctrl.Para.dat.sAirPara.sStandard_Hum.GoodLimit      = 50;
    Ctrl.Para.dat.sAirPara.sStandard_Hum.NormalLimit    = 70;
    
    Ctrl.Para.dat.sAirPara.Dust_modefy  = 0;
    Ctrl.Para.dat.sAirPara.Hum_modefy   = 0;
    Ctrl.Para.dat.sAirPara.Voc_modefy   = 0;
    
    FRAM_StoreRunPara((stcRunPara *) &Ctrl.Para.dat.sRunPara);   //��ȡ���в���
    
    
//����װ��1 ��ʼ��    
    Ctrl.Mtr.ConnCtrl[0].ConnFlg   = 1;
    Ctrl.Mtr.ConnCtrl[0].ErrFlg    = 0;
    Ctrl.Mtr.ConnCtrl[0].MasterAddr= MASTE_ADDR_HOST;
//    Ctrl.Mtr.ConnCtrl[0].SlaveAddr = MASTE_ADDR_HOST+0x10;
    Ctrl.Mtr.ConnCtrl[0].SlaveAddr = SLAVE_ADDR_DIP1;
    Ctrl.Mtr.ConnCtrl[0].SendFlg   = 0;
    Ctrl.Mtr.ConnCtrl[0].SendFramNum=1;
    Ctrl.Mtr.ConnCtrl[0].TimeOut   = 10;
    
//����װ��2 ��ʼ��    
    Ctrl.Mtr.ConnCtrl[1].ConnFlg   = 1;
    Ctrl.Mtr.ConnCtrl[1].ErrFlg    = 0;
    Ctrl.Mtr.ConnCtrl[1].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Mtr.ConnCtrl[1].SlaveAddr = SLAVE_ADDR_DIP2;
    Ctrl.Mtr.ConnCtrl[1].RecvEndFlg= 0;
    Ctrl.Mtr.ConnCtrl[1].SendFlg   = 0;
    Ctrl.Mtr.ConnCtrl[1].SendFramNum=1;
    Ctrl.Mtr.ConnCtrl[1].TimeOut   = 10;    
    
//����ģ�� ���� ��ʼ��    
    Ctrl.Dtu.ConnCtrl[0].ConnFlg   = 1;
    Ctrl.Dtu.ConnCtrl[0].ErrFlg    = 0;
    Ctrl.Dtu.ConnCtrl[0].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Dtu.ConnCtrl[0].SlaveAddr = SLAVE_ADDR_DTU;
    Ctrl.Dtu.ConnCtrl[0].RecvEndFlg= 0;
    Ctrl.Dtu.ConnCtrl[0].SendFlg   = 0;
    Ctrl.Dtu.ConnCtrl[0].SendFramNum=1;
    Ctrl.Dtu.ConnCtrl[0].TimeOut   = 10;    
    
//����ģ�� ���� ��ʼ��    
    Ctrl.Dtu.ConnCtrl[1].ConnFlg   = 1;
    Ctrl.Dtu.ConnCtrl[1].ErrFlg    = 0;
    Ctrl.Dtu.ConnCtrl[1].MasterAddr= MASTE_ADDR_HOST;
    Ctrl.Dtu.ConnCtrl[1].SlaveAddr = SLAVE_ADDR_SET;
    Ctrl.Dtu.ConnCtrl[1].RecvEndFlg= 0;
    Ctrl.Dtu.ConnCtrl[1].SendFlg   = 0;
    Ctrl.Dtu.ConnCtrl[1].SendFramNum=1;
    Ctrl.Dtu.ConnCtrl[1].TimeOut   = 10;   
}


/*******************************************************************************
 * ��    �ƣ� App_Main
 * ��    �ܣ� ϵͳ����ʼǰ���ô˺���
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-08-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void App_Main(void)
 {
    /***********************************************
    * ������ Disable all interrupts.
    */
    BSP_IntDisAll();
    /***********************************************
    * ������ ��������Ӧ�ó����ж����������û��BOOTLOADER
    *        NVIC_VectTab_FLASH_OFFSET = 0
    */
#if defined     (IMAGE_A)
    /***********************************************
    * ������ ����A
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR_A-FLASH_BOOT_ADDR);
    IAP_SetStatus(IAP_STS_SUCCEED); 
#elif defined   (IMAGE_B)
    /***********************************************
    * ������ ����B
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_APP_ADDR_B-FLASH_BOOT_ADDR);
    IAP_SetStatus(IAP_STS_SUCCEED); 
#else
    /***********************************************
    * ������ ����
    */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X0000);
#endif
 
    
    //iap  
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0X10000);

    //init  ctrl
    app_init_sctrl();
 }

/*******************************************************************************
 * ��    �ƣ� App_InitStartHook
 * ��    �ܣ� ϵͳ��ʼ����ʼǰ���ô˺���
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-08-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void App_InitStartHook(void)
 {
 
 }
 
/*******************************************************************************
 * ��    �ƣ� App_InitEndHook
 * ��    �ܣ� ϵͳ��ʼ�����ٺ���ô˺���
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-08-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void App_InitEndHook(void)
 {
 
 }
 
 /*******************************************************************************
 * ��    �ƣ� OS_TaskCreateHook
 * ��    �ܣ� OS��������ص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ��  ���ߣ� ������
 * �������ڣ� 2014-08-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void OS_TaskCreateHook(void)
 {     
//    App_TaskMaterCreate();
//    App_TaskDtuCreate();
    //App_TaskKeyCreate();
    //App_TaskBeepCreate();
 }
 
 /*******************************************************************************
 * ��    �ƣ� OSAL_TaskCreateHook
 * ��    �ܣ� OSAL��������ص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-08-08
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
 void OSAL_TaskCreateHook(void)
 {
    osal_add_Task(TaskInitTmr,                      // �����ʼ������ָ��
                  TaskTmrEvtProcess,                // �����¼�������ָ��
                  OS_TASK_PRO_TMR,                  // �������ȼ�
                  OS_TASK_ID_TMR);                  // ����ID
    
    osal_add_Task(TaskInitDisp,                     // �����ʼ������ָ��
                  TaskDispEvtProcess,               // �����¼�������ָ��
                  OS_TASK_PRO_DISP,                 // �������ȼ�
                  OS_TASK_ID_DISP);                 // ����ID 
    
    osal_add_Task(TaskInitLed,                      // �����ʼ������ָ��
                  TaskLedEvtProcess,                // �����¼�������ָ��
                  OS_TASK_PRO_LED,                  // �������ȼ�
                  OS_TASK_ID_LED);                  // ����ID    
    
    osal_add_Task(TaskInitStore,                    // �����ʼ������ָ��
                  TaskStoreEvtProcess,              // �����¼�������ָ��
                  OS_TASK_PRO_STORE,                // �������ȼ�
                  OS_TASK_ID_STORE);                // ����ID
 }
/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
