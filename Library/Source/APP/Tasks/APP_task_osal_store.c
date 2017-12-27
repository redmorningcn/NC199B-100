/*******************************************************************************
 *   Filename:       app_task_store.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 store 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Store 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 STORE 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_STORE_PRIO ）
 *                                            和 任务堆栈（ APP_TASK_STORE_STK_SIZE ）大小
 *                   在 app.h 中声明本任务的     创建函数（ void  App_TaskStoreCreate(void) ）
 *                                            和 看门狗标志位 （ WDTFLAG_Store ）
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
* 描述： 任务控制块（TCB）
*/
//static  OS_TCB   AppTaskStoreTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
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
 * 名    称： 		GetRecNumAddr
 * 功    能：      取数据记录地址。
                flash地址;更新记录号等信息。
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
uint32  GetRecNumAddr(uint32 FlshRecNum)
{				
    return  (uint32)(((FlshRecNum * sizeof(stcFlshRec)) % FLSH_MAX_SIZE)); 			
}


      
/*******************************************************************************
 * 名    称：RoadNum;        		StoreData
 * 功    能：RelRoadNum;     		数据存储。根据数据记录号将数据存储到指定
 			StationNum;    		flash地址;更新记录号等信息。大部分数据记录的
 			E_StationNum;  		内容在其他任务直接对Ctrl.sRec 中更新。少部分
 					数据内容在该函数中跟新。
 * 入口参数：SignalTyp;      	无
 * 出口参数：LocoSign;       	无
 * 作　 　者                 ： 	redmornigcn
 * 创建日期：LocoWorkState;  	2017-05-15
 * 修    改：LocoState;     
 * 修改日期：
 *******************************************************************************/
void  StoreData(void)
{	
    stcTime             sTime;
    stcFlshRec          *sFlsh;

    // 	计算数据记录地址
    uint32  FlshAddr = GetRecNumAddr( Ctrl.Para.dat.sRecNumMgr.Current );	
	
    sFlsh = &Ctrl.sRec;

    sFlsh->CmdTpye		 = 0;                                       //命令类型       1    
 
 //开机标示
    sFlsh->EvtType       =  0xff   ;                                //事件类型       1    
    if(Ctrl.Para.dat.sRunPara.StartFlg)
    {
        sFlsh->EvtType  = START_EVT;                                //開機          
        Ctrl.Para.dat.sRunPara.StartFlg = 0;
    }   
    
    sFlsh->LocoType     =   Ctrl.Para.dat.sProductInfo.sLocoId.Type  ;//机车型号       2                  
    sFlsh->LocoNum      =   Ctrl.Para.dat.sProductInfo.sLocoId.Num   ;//机 车 号       2                     
    sFlsh->RecID        =   Ctrl.Para.dat.sRecNumMgr.Current;         //记录流水号     4                   
    //sFlsh->RunSys       =   Ctrl.Para.dat.sRunPara.SysSta;            //运行状态       1
    
    memcpy((uint8 *)&sFlsh->RunSys,(uint8 *)&Ctrl.Para.dat.sRunPara.SysSta,sizeof(sFlsh->RunSys));
    
    ReadTime((stcTime *)&sTime);
	sFlsh->SysTime.Year =   sTime.Year;			
	sFlsh->SysTime.Mon  =   sTime.Month;		
	sFlsh->SysTime.Day  =   sTime.Date;			
	sFlsh->SysTime.Hour =   sTime.Hour;			
	sFlsh->SysTime.Min  =   sTime.Min;			
	sFlsh->SysTime.Sec  =   sTime.Sec;	        
////  数据记录内容更新在应用中进行。
////	数据内容存储在Ctrl.sRec 中。
//         
    
//  存储时更新的内容
    
//  装置时间:年-月-日，时-分-秒
	
    memset((uint8_t *)&sFlsh->Tax.buf1[0],0,sizeof(sFlsh->Tax.buf1));//clear
    
    ReadTime((stcTime *)&sTime);

	sFlsh->Tax.Tax2.LKJTime.Year    = sTime.Year;			
	sFlsh->Tax.Tax2.LKJTime.Mon	    = sTime.Month;		
	sFlsh->Tax.Tax2.LKJTime.Day     = sTime.Date;			
	sFlsh->Tax.Tax2.LKJTime.Hour    = sTime.Hour;			
	sFlsh->Tax.Tax2.LKJTime.Min     = sTime.Min;			
	sFlsh->Tax.Tax2.LKJTime.Sec     = sTime.Sec;			

//    if(Ctrl.Tax.ConnectFlag == 1)                                  //收到tax箱信息，将数据复制到数据记录
//    {
//    // 	保存TAX箱信息 
//        Ctrl.Tax.ConnectFlag = 0;                                  //清已取信息
//
////        sFlsh->TaxType   = Ctrl.Tax.Dat.Tax2A.Record.Addr;
////        sFlsh->TaxFlg    = Ctrl.Tax.Dat.Tax2A.CheZhanHaoKuoChong;
////        memcpy((uint8_t *)&sFlsh->Tax.buf1[0],(uint8_t *)&Ctrl.Tax.Dat.buf1[6],64);
//    }
//    
// 	计算记录校验和
	sFlsh->CrcCheck = GetCrc16Check((uint8 *)sFlsh,sizeof(stcFlshRec) - 2); 
    
//	数据存储到flash    
    if(!WriteFlsh(FlshAddr, (uint8 *)&Ctrl.sRec, sizeof(Ctrl.sRec)))
    {}
    
    // 	保存数据记录号
    Ctrl.Para.dat.sRecNumMgr.Current++;
	FRAM_StoreRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);    //数据记录号加1，并保存
    
    //调试
    FRAM_ReadRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);         //读记录号

    ReadFlsh(FlshAddr, (uint8 *)&Ctrl.sRec, sizeof(Ctrl.sRec));
}

/*******************************************************************************
 * 名    称： 		ReadFlshRec
 * 功    能： 		根据记录号，取数据记录
 * 入口参数： 	    无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-15
 * 修    改：
 * 修改日期：
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
 * 名    称：  TaskInitStore
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void    BSP_StoreInit(void)
{
    uint16 crc16;
    
	InitFlashIO();              //初始化flash
    //SPI_FLASH_Init();         //初始化flash
    JudgeFlashIDErrFlg();       //flash测试
    
    GPIO_Fram_Init();           //初始化fram
    
    InitDS3231();               //初始化时钟
    
    OSRegWdtFlag( WDT_FLAG_STORE );

    //读Ctrl 
    FRAM_ReadRecNumMgr((stcRecNumMgr  *)&Ctrl.Para.dat.sRecNumMgr);         //读记录号
    FRAM_ReadProductInfo((stcProductInfo  *)&Ctrl.Para.dat.sProductInfo);   //读产品编号
    FRAM_ReadCurRecord((stcFlshRec  *)&Ctrl.sRec);                          //读当前记录
    
    FRAM_ReadAirPara((stcAirPara  *)&Ctrl.Para.dat.sAirPara);               //读计算参数
    
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
    
    
    FRAM_ReadRunPara((stcRunPara  *)&Ctrl.Para.dat.sRunPara);               //读运行参数
    
}

/*******************************************************************************
 * 名    称：  TaskInitStore
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitStore(void)
{    
//    /***********************************************
//    * 描述： 初始化本任务用到的相关硬件
//    */
    
    BSP_StoreInit();			                                //初始化Flash底层相关函数

//初始化定时器
    osal_start_timerEx( OS_TASK_ID_STORE,
                        OS_EVT_STORE_TICKS,
                        1000);                                 //60秒后再存储
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * 名    称： 		AppTaskStore
 * 功    能： 		控制任务
 * 入口参数： 	p_arg - 由任务创建函数传入
 * 出口参数： 	无
 * 作　 　者： 	redmorningcn.
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
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
            StoreData();            //保存数据记录
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
