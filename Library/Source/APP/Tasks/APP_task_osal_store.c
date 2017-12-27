/*******************************************************************************
 *   Filename:       app_task_store.c
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
#include <global.h>
#include <app_ctrl.h>
#include <cpu.h>
//#include <bsp_mx25.h>
#include <bsp_FRAM.h>
#include <bsp_DS3231.h>
#include <MX25L1602Drv.h>

     

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_store__c = "$Id: $";
#endif

#define APP_TASK_STORE_EN     DEF_ENABLED
#if APP_TASK_STORE_EN == DEF_ENABLED				//
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define		START_EVT		31		

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
//static  OS_TCB   AppTaskStoreTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
//static  CPU_STK  AppTaskStoreStk[ APP_TASK_STORE_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
extern	void    BSP_StoreInit(void);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� 		GetRecNumAddr
 * ��    �ܣ�      ȡ���ݼ�¼��ַ��
                flash��ַ;���¼�¼�ŵ���Ϣ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
uint32  GetRecNumAddr(uint32 FlshRecNum)
{				
    return  (uint32)(((FlshRecNum * sizeof(stcFlshRec)) % FLSH_MAX_SIZE)); 			
}


      
/*******************************************************************************
 * ��    �ƣ�RoadNum;        		StoreData
 * ��    �ܣ�RelRoadNum;     		���ݴ洢���������ݼ�¼�Ž����ݴ洢��ָ��
 			StationNum;    		flash��ַ;���¼�¼�ŵ���Ϣ���󲿷����ݼ�¼��
 			E_StationNum;  		��������������ֱ�Ӷ�Ctrl.sRec �и��¡��ٲ���
 					���������ڸú����и��¡�
 * ��ڲ�����SignalTyp;      	��
 * ���ڲ�����LocoSign;       	��
 * ���� ����                 �� 	redmornigcn
 * �������ڣ�LocoWorkState;  	2017-05-15
 * ��    �ģ�LocoState;     
 * �޸����ڣ�
 *******************************************************************************/
void  StoreData(void)
{	
    stcTime             sTime;
    stcFlshRec          *sFlsh;

    // 	�������ݼ�¼��ַ
    uint32  FlshAddr = GetRecNumAddr( Ctrl.Para.dat.sRecNumMgr.Current );	
	
    sFlsh = &Ctrl.sRec;

    sFlsh->CmdTpye		 = 0;                                       //��������       1    
 
 //������ʾ
    sFlsh->EvtType       =  0xff   ;                                //�¼�����       1    
    if(Ctrl.Para.dat.sRunPara.StartFlg)
    {
        sFlsh->EvtType  = START_EVT;                                //�_�C          
        Ctrl.Para.dat.sRunPara.StartFlg = 0;
    }   
    
    sFlsh->LocoType     =   Ctrl.Para.dat.sProductInfo.sLocoId.Type  ;//�����ͺ�       2                  
    sFlsh->LocoNum      =   Ctrl.Para.dat.sProductInfo.sLocoId.Num   ;//�� �� ��       2                     
    sFlsh->RecID        =   Ctrl.Para.dat.sRecNumMgr.Current;         //��¼��ˮ��     4                   
    //sFlsh->RunSys       =   Ctrl.Para.dat.sRunPara.SysSta;            //����״̬       1
    
    memcpy((uint8 *)&sFlsh->RunSys,(uint8 *)&Ctrl.Para.dat.sRunPara.SysSta,sizeof(sFlsh->RunSys));
    
    ReadTime((stcTime *)&sTime);
	sFlsh->SysTime.Year =   sTime.Year;			
	sFlsh->SysTime.Mon  =   sTime.Month;		
	sFlsh->SysTime.Day  =   sTime.Date;			
	sFlsh->SysTime.Hour =   sTime.Hour;			
	sFlsh->SysTime.Min  =   sTime.Min;			
	sFlsh->SysTime.Sec  =   sTime.Sec;	        
////  ���ݼ�¼���ݸ�����Ӧ���н��С�
////	�������ݴ洢��Ctrl.sRec �С�
//         
    
//  �洢ʱ���µ�����
    
//  װ��ʱ��:��-��-�գ�ʱ-��-��
	
    memset((uint8_t *)&sFlsh->Tax.buf1[0],0,sizeof(sFlsh->Tax.buf1));//clear
    
    ReadTime((stcTime *)&sTime);

	sFlsh->Tax.Tax2.LKJTime.Year    = sTime.Year;			
	sFlsh->Tax.Tax2.LKJTime.Mon	    = sTime.Month;		
	sFlsh->Tax.Tax2.LKJTime.Day     = sTime.Date;			
	sFlsh->Tax.Tax2.LKJTime.Hour    = sTime.Hour;			
	sFlsh->Tax.Tax2.LKJTime.Min     = sTime.Min;			
	sFlsh->Tax.Tax2.LKJTime.Sec     = sTime.Sec;			

//    if(Ctrl.Tax.ConnectFlag == 1)                                  //�յ�tax����Ϣ�������ݸ��Ƶ����ݼ�¼
//    {
//    // 	����TAX����Ϣ 
//        Ctrl.Tax.ConnectFlag = 0;                                  //����ȡ��Ϣ
//
////        sFlsh->TaxType   = Ctrl.Tax.Dat.Tax2A.Record.Addr;
////        sFlsh->TaxFlg    = Ctrl.Tax.Dat.Tax2A.CheZhanHaoKuoChong;
////        memcpy((uint8_t *)&sFlsh->Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
//    }
//    
// 	�����¼У���
	sFlsh->CrcCheck = GetCrc16Check((uint8 *)sFlsh,sizeof(stcFlshRec) - 2); 
    
//	���ݴ洢��flash    
    if(!WriteFlsh(FlshAddr, (uint8 *)&Ctrl.sRec, sizeof(Ctrl.sRec)))
    {}
    
    // 	�������ݼ�¼��
    Ctrl.Para.dat.sRecNumMgr.Current++;
	FRAM_StoreRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);    //���ݼ�¼�ż�1��������
    
    //����
    FRAM_ReadRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);         //����¼��

    ReadFlsh(FlshAddr, (uint8 *)&Ctrl.sRec, sizeof(Ctrl.sRec));
}

/*******************************************************************************
 * ��    �ƣ� 		ReadFlshRec
 * ��    �ܣ� 		���ݼ�¼�ţ�ȡ���ݼ�¼
 * ��ڲ����� 	    ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
uint8	ReadFlshRec(stcFlshRec * sFlshRec,uint32	FlshRecNum)
{
	uint32		FlshAddr;
	
	FlshAddr = GetRecNumAddr( FlshRecNum );						
	
	if(ReadFlsh(FlshAddr,(uint8 *)sFlshRec,sizeof(stcFlshRec)))
	{
		return 1;
	}
	
	return	0;
}

/*******************************************************************************
 * ��    �ƣ�  TaskInitStore
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void    BSP_StoreInit(void)
{
    uint16 crc16;
    
	InitFlashIO();              //��ʼ��flash
    //SPI_FLASH_Init();         //��ʼ��flash
    JudgeFlashIDErrFlg();       //flash����
    
    GPIO_Fram_Init();           //��ʼ��fram
    
    InitDS3231();               //��ʼ��ʱ��
    
    OSRegWdtFlag( WDT_FLAG_STORE );

    //��Ctrl 
    FRAM_ReadRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);         //����¼��
    FRAM_ReadProductInfo((stcProductInfo  *)&Ctrl.Para.dat.sProductInfo);   //����Ʒ���
    FRAM_ReadCurRecord((stcFlshRec  *)&Ctrl.sRec);                          //����ǰ��¼
    
    FRAM_ReadAirPara((stcAirPara  *)&Ctrl.Para.dat.sAirPara);               //���������
    
    crc16 = GetCrc16Check((uint8 *)&Ctrl.Para.dat.sAirPara,sizeof(stcAirPara) - 2); 
    if(    Ctrl.Para.dat.sAirPara.crc16 != crc16
       ||  (Ctrl.Para.dat.sAirPara.crc16  == 0 && Ctrl.Para.dat.sAirPara.sStandard_Hum.NormalLimit == 0)
      )
    {
        
        Ctrl.Para.dat.sAirPara.sStandard_Dust.GoodLimit     = 200;
        Ctrl.Para.dat.sAirPara.sStandard_Dust.NormalLimit   = 500;
        
        Ctrl.Para.dat.sAirPara.sStandard_VOC.GoodLimit      = 500;
        Ctrl.Para.dat.sAirPara.sStandard_VOC.NormalLimit    = 2000;
        
        Ctrl.Para.dat.sAirPara.sStandard_Hum.GoodLimit      = 60;
        Ctrl.Para.dat.sAirPara.sStandard_Hum.NormalLimit    = 85;
        
        Ctrl.Para.dat.sAirPara.Dust_modefy  = 0;
        Ctrl.Para.dat.sAirPara.Hum_modefy   = 0;
        Ctrl.Para.dat.sAirPara.Voc_modefy   = 0;
    }
    
    
    FRAM_ReadRunPara((stcRunPara  *)&Ctrl.Para.dat.sRunPara);               //�����в���
    
}

/*******************************************************************************
 * ��    �ƣ�  TaskInitStore
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitStore(void)
{    
//    /***********************************************
//    * ������ ��ʼ���������õ������Ӳ��
//    */
    
    BSP_StoreInit();			                                //��ʼ��Flash�ײ���غ���

//��ʼ����ʱ��
    osal_start_timerEx( OS_TASK_ID_STORE,
                        OS_EVT_STORE_TICKS,
                        1000);                                 //60����ٴ洢
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * ��    �ƣ� 		AppTaskStore
 * ��    �ܣ� 		��������
 * ��ڲ����� 	p_arg - �����񴴽���������
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmorningcn.
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
osalEvt  TaskStoreEvtProcess(osalTid task_id, osalEvt task_event)
{
    static  uint16  times = 0;    
    
    OSSetWdtFlag( WDT_FLAG_STORE );
    

    if( task_event & OS_EVT_STORE_TICKS ) {
        
        osal_start_timerEx( OS_TASK_ID_STORE,
                   OS_EVT_STORE_TICKS,
                   1000);   
        
        if(Ctrl.Para.dat.sRunPara.StoreCycle > 60 || Ctrl.Para.dat.sRunPara.StoreCycle < 1)
            Ctrl.Para.dat.sRunPara.StoreCycle = 1;
        
        times++;
        if(times > Ctrl.Para.dat.sRunPara.StoreCycle*60)
        {
            StoreData();            //�������ݼ�¼
            times = 0;
        }
        
        return ( task_event ^ OS_EVT_STORE_TICKS );
    }
    
    return  0;
}



/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif
