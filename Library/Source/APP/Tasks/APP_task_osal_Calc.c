/*******************************************************************************
 *   Filename:       app_task_store.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 disp 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Disp 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 DISP 按 Ctrl + H, 钩选 Match the case, Replace with
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
//
/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>
#include <global.h>
#include <app_ctrl.h>
#include <CardDataTypeDef.h>
#include <cpu.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_disp__c = "$Id: $";
#endif

#define APP_TASK_CALC_EN     DEF_ENABLED
#if APP_TASK_CALC_EN == DEF_ENABLED				//
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define     FREQ_DOWN_STATE     1
#define     FREQ_WAVE_STATE     2
#define     FREQ_LAG_WAVE_STATE 3
#define     FREQ_UP_STATE       4           		//频率变化状态
#define     FREQ_STOP_STATE     5

//油箱模型编号                                        
#define			HXN3_MODLE		31
#define			HXN5_MODLE		30

#define         MY_MODLE        40                  //自设油箱模型
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
static  OS_TCB   AppTaskCalcTCB;

/***********************************************
* 描述： 任务堆栈（STACKS）
*/
//static  CPU_STK  AppTaskCalcStk[ APP_TASK_CALC_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskCalc           (void *p_arg);
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
void    GetHxn5OilBoxModel(stcCalcModel * sComplexModel);
void    GetHxn3OilBoxModel(stcCalcModel * sComplexModel);
uint8   FRAM_ReadCalcModel(stcCalcModel  *sCalcModel);
void    stcModelTostcModelComplex(stcCalcModel * sComplexModel,stcModel	* sModel);
uint8	GetOilBoxModelsModelSemple(stcModelSemple * sModelSemple);
uint8   FRAM_StoreCalcModel(stcCalcModel  *sCalcModel);
void    stcModelSempleTostcModelComplex(stcCalcModel * sComplexModel,stcModelSemple	* sModelSemple);
uint8   GetDf4dOilBoxModel(stcModel * sModel);
void	InitFlashIO(void);
void	GPIO_Fram_Init(void);
uint8   FRAM_ReadOilPara(StrOilPara  *SOilPara);
uint8   FRAM_StoreCalcModel(stcCalcModel  *sCalcModel);
uint8   FRAM_StoreOilPara(StrOilPara  *SOilPara);


/*******************************************************************************
 * EXTERN VARIABLES
 */

     
     
extern	stcSysCtrl       sCtrl;       	//	全局结构控制体
/*******************************************************************************/

stcModel 		  	l_sModel;
stcModelSemple	  	l_sModelSemple;
stcCalcModel        l_sCalcModel;

/*******************************************************************************
 * 名    称：          GetOilBoxModel()
 * 功    能：          取运算用的油箱模型.执行该函数前，先读 sCtrl.SOilPara，先或的
                      油箱模型编号。
 * 入口参数：          无
 * 出口参数： 	      无
 * 作　 　者： 	      redmornigcn
 * 创建日期： 	      2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
uint8   GetOilBoxModel(void)
{
    //先取预设油箱模型，不成功，再按油箱模型编号，进行计算。（建议现场使用20170905）
    FRAM_ReadCalcModel(&l_sCalcModel);      //取模型
    
    if(l_sCalcModel.valid == 1)             //模型有效
    {
        uint16  crc16 = GetCrc16Check((uint8 *)&l_sCalcModel,sizeof(l_sCalcModel)-2);
        
        if(crc16 == l_sCalcModel.CrcCheck) //，且校验通过
        {
            memcpy((uint8 *)&sCtrl.sCalcModel,(uint8 *)&l_sCalcModel,sizeof(l_sCalcModel));
            
            return 1;                       
        }
    }
    
//////////////////////////////////////////////////////////////////    
    switch (sCtrl.SOilPara.ModelNum)                                //根据模型编号，确定油箱模型，其中自设模型
    {
        case HXN5_MODLE:
            GetHxn5OilBoxModel((stcCalcModel *)&sCtrl.sCalcModel);  //HXN5
            break;
        case HXN3_MODLE:
            GetHxn3OilBoxModel((stcCalcModel *)&sCtrl.sCalcModel);  //HXN3
            break;   
            
        case MY_MODLE:                                              //自设模型
            FRAM_ReadCalcModel((stcCalcModel *)&sCtrl.sCalcModel);  //取自设模型
            break;      
            
        default:
        if(sCtrl.SOilPara.ModelNum < 30)
        {
            if(GetOilBoxModelsModel((stcModel *)&l_sModel))
            {
                stcModelTostcModelComplex((stcCalcModel *)&sCtrl.sCalcModel,(stcModel *)&l_sModel);
                break;
            } 		
            
            if(GetOilBoxModelsModelSemple((stcModelSemple *)&l_sModelSemple))
            {
                stcModelSempleTostcModelComplex((stcCalcModel *)&sCtrl.sCalcModel,(stcModelSemple *)&l_sModelSemple);			//100829
                break;
            }
        }                        
    }
    
    uint8   modelerrflg= 0;                                                     //模型错误标示
    
    //检查模型是否正确,模型异常用DF4D模型
    if(sCtrl.sCalcModel.PotNum < 101 && sCtrl.sCalcModel.PotNum  > 1)           //模型点数2~100，否者模型异常
    {
        for(uint8 i = 0 ;i <  sCtrl.sCalcModel.PotNum-1; i++)
        {
            if(sCtrl.sCalcModel.sModPot[i+1].DipVal >  sCtrl.sCalcModel.sModPot[i].DipVal 
               &&  sCtrl.sCalcModel.sModPot[i+1].HigVal >  sCtrl.sCalcModel.sModPot[i].HigVal)
            { continue; }     //模型的点数后点值大于前点值，任务模型正常，否者异常
            else
            {      
                modelerrflg = 1;
            }                        
        }
    }
    else
    {
        modelerrflg = 1;
    }
    
    if(modelerrflg)    
    {
        //模型错误，采用默认模型DF4D
        GetDf4dOilBoxModel((stcModel *)&l_sModel);    
        
        stcModelTostcModelComplex((stcCalcModel *)&sCtrl.sCalcModel,(stcModel *)&l_sModel); //100829	
        return  0; 
	}
    
    return 1;
}


int16	l_Dip1Frq10SecChg;
int16	l_Dip2Frq10SecChg;
/*******************************************************************************
 * 名    称：        GetFrqChgState
 * 功    能：        取油量变化状态
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-17
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void  GetDip10SecChg(int16	CurDip1Prs,int16	CurDip2Prs )
{
	static	uint32	Times = 0;
	static	int16	Dip1PrsBuf[12];
	static	int16	Dip2PrsBuf[12];	
	
	Dip1PrsBuf[Times %12] = CurDip1Prs;
	Dip2PrsBuf[Times %12] = CurDip2Prs;
    

	if(Times > 12)
	{
		l_Dip1Frq10SecChg = Dip1PrsBuf[Times %12] - Dip1PrsBuf[(Times - 11)%12];		//2??μ			
		l_Dip2Frq10SecChg = Dip2PrsBuf[Times %12] - Dip2PrsBuf[(Times - 11)%12];	
	}
	else
	{
		l_Dip1Frq10SecChg  = 0;
		l_Dip2Frq10SecChg  = 0;				
	}
	
	Times++;  	
}
#define		JUDGE_STATE_TIMES	15
/*******************************************************************************
 * 名    称：        GetFrqChgState
 * 功    能：        取油量变化状态
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-17
 * 修    改：
 * 修改日期：
 *******************************************************************************/
uint8  GetFrqChgState(int16	CurDip1Prs,int16	CurDip2Prs)
{
    static  int32  UpTimes = 0,DownTimes = 0,StopTimes = 0,WaveTimes = 0;
    static	uint8	FreqChgState = FREQ_WAVE_STATE;
    
    GetDip10SecChg(CurDip1Prs,CurDip2Prs);							//è?10??±??ˉ?μ
      
    if(((l_Dip1Frq10SecChg > 30) && (l_Dip2Frq10SecChg > 30)))		//í?éy
    {
        UpTimes++;
        DownTimes = WaveTimes = 0;
        //100815
      	StopTimes	= 0;

        if((UpTimes > JUDGE_STATE_TIMES))
        {        
	        StopTimes	= 0;
            UpTimes 	= 0;
            
            FreqChgState = FREQ_UP_STATE;
        }
    }
    else  if(((l_Dip1Frq10SecChg < -1) && (l_Dip2Frq10SecChg < -1)))		//í??μ	
    {
    	DownTimes++;
    	UpTimes = WaveTimes = 0;
		//100815
		if(abs(l_Dip1Frq10SecChg) > 30 || abs(l_Dip1Frq10SecChg) > 30)
		{
			StopTimes	= 0;
//			FreqChgState = FREQ_WAVE_STATE;
		}
		
        if((DownTimes > JUDGE_STATE_TIMES) )
        {        
	        StopTimes	= 0;
            DownTimes 	= 0;
            
            FreqChgState = FREQ_DOWN_STATE;
        }
    }
	else  if(	(abs(l_Dip1Frq10SecChg) < 15 ) && 
			 	(abs(l_Dip2Frq10SecChg) < 15 ))						//?è?¨
    {
        StopTimes++;
        UpTimes = DownTimes =  WaveTimes = 0;
                
        if(StopTimes > JUDGE_STATE_TIMES * 2)
        {
            StopTimes = 0;
            FreqChgState = FREQ_STOP_STATE;
        }
    }
    else
    {
        UpTimes = DownTimes = StopTimes = 0;						

        if( abs(l_Dip1Frq10SecChg - l_Dip2Frq10SecChg) > 300 )		//′ó2¨?ˉ
        {
	        WaveTimes 	= JUDGE_STATE_TIMES;
	        FreqChgState = FREQ_LAG_WAVE_STATE;
	    }
        else
        {
        	if(WaveTimes > 0)
            {
                WaveTimes--;
               	FreqChgState = FREQ_LAG_WAVE_STATE;	
            }
            else													//2¨?ˉ
            {
                FreqChgState = FREQ_WAVE_STATE;
            }
        }	    
    }
    
    return	FreqChgState;
}
 
/*******************************************************************************
 * 名    称：        GetDipPrs30SecVal
 * 功    能：        计算油量。根据高度值，油箱容积表，修正系数等计算油量.
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
int16    GetDipPrs30SecVal(int16   PrsAvg)
{
	static	uint32	Times	= 0;
	static	int32	Sum30 = 0;
	static	int32	Sum5 = 0;
	int16	PrsAvg30 = 0;
	int16	PrsAvg10 = 0;
	int16	PrsAvg5  = 0;
	int16 	MinutePrsAvg;
	uint8	PrsChgState;
	int32	AvgTmp;
		
	PrsChgState = GetFrqChgState(sCtrl.sRec.Myhig1,sCtrl.sRec.Myhig2);	//波动状态	

	if(Times < 30)
	{
        if(PrsAvg == 0)                                                //非零才计算
            return 0;
                
		Sum30 += PrsAvg;	
		if(Times < 5)
		{
			Sum5 += PrsAvg;
		}
		else
		{
			AvgTmp = Sum5 / 5; 
			Sum5  = Sum5 + PrsAvg - AvgTmp;			
		}

        Times++;

		return	PrsAvg;
	}
	else
	{
		AvgTmp = Sum30 / 30;
		Sum30  = Sum30 + PrsAvg - AvgTmp;		

		AvgTmp = Sum5 / 5; 
		Sum5  = Sum5 + PrsAvg - AvgTmp;	
        
        Times++;
	}

	PrsAvg30 	= Sum30 / 30;
	PrsAvg5 	= Sum5 / 5;
	PrsAvg10 	=(PrsAvg5*2 + PrsAvg30) / 3;

	if( (PrsChgState != FREQ_WAVE_STATE) && (PrsChgState != FREQ_LAG_WAVE_STATE))
	{
		if(PrsChgState == FREQ_STOP_STATE)
		{
			MinutePrsAvg = PrsAvg10;
		}
		else 
		{
			MinutePrsAvg = PrsAvg5;
		}
	}
	else
	{
		MinutePrsAvg = PrsAvg30;
	}
	
	return	MinutePrsAvg;
}

/*******************************************************************************
 * 名    称：        GetMathOilVal
 * 功    能：        计算油量。根据高度值，油箱容积表，修正系数等计算油量.
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
uint16    GetMathOilVal(int16   hig)
{
  	float 		DipVal = 0;                  		
  	float 		Hight;                  			
	float		MinHig,MaxHig;	
	float		MinDip,MaxDip;					
    
	uint8		ModelPotNum;
	uint8		i;
    
	ModelPotNum = sCtrl.sCalcModel.PotNum;              //模型点数

	Hight = hig;                                        //高度值

	Hight += ((float)sCtrl.SOilPara.Hig );              //加入补偿高度	
  	
  	for(i = 1; i <  ModelPotNum; i++)                   //计算点的个数
  	{
		MinHig 	= sCtrl.sCalcModel.sModPot[i-1].HigVal; //第1个点的高度值
		MaxHig 	= sCtrl.sCalcModel.sModPot[i].HigVal;   //第2个点的高度值

		MinDip 	= sCtrl.sCalcModel.sModPot[i-1].DipVal; //第1个点的油量值
		MaxDip 	= sCtrl.sCalcModel.sModPot[i].DipVal;   //第2个点的油量值
						
		if(MinHig == MaxHig  || MinDip == MaxDip)       //计算模型中出现两个相同点
		{
			//DisplaySet(60,1,"E-OL");                    //显示油量值
			return	0;                                  //油箱模型错误，显示0
		}

		if(MinHig <= Hight && Hight < MaxHig)		    //计算油量,在模型值中，按两点值计算
		{	
			DipVal = MinDip + ((Hight - MinHig)  * (MaxDip - MinDip)) / (MaxHig - MinHig);

			break;
		}
	
		if(i == 1)									//小于最小高度，按最低点斜率
		{			
			if(Hight < MinHig)
			{
				DipVal = MinDip + ((Hight - MinHig)  * ((MaxDip - MinDip)) / (MaxHig - MinHig));
				
				break;
			}
		}
		
		if(i == (ModelPotNum - 1))					//高度值大于最后一点，按最后点计算
		{
			
			if(Hight >= MaxHig)
			{
				DipVal = MinDip + ((Hight - MinHig)  * ((MaxDip - MinDip)) / (MaxHig - MinHig));
			}
		}
	}
        
    DipVal *= sCtrl.SOilPara.Slope;                 //计算修正斜率
    DipVal /= 10000;
	DipVal += sCtrl.sCalcModel.StartOil;            //计算值 加 模型起始油量值
    
	
    return  (uint16)DipVal;
}

/*******************************************************************************
 * 名    称：        GetDipPrsAvg
 * 功    能：        取平均值
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
int16	GetDipPrsAvg(int16	CurDip1Prs,int16	CurDip2Prs)
{
    int32   PrsDef;
    int16   CurDipAvgPrs   = 0;
	static  uint32 Times   = 0;
//    static  uint32 hig1chgtimes =0;
//    static  uint32 hig2chgtimes =0;
//    static  int16  lsthig1 = 0;
//    static  int16  lsthig2 = 0;
    
    //无通讯时，高度值会直接赋0值。
    /*****说明
    *   压力值超限，数值直接为零
    */
	if(CurDip1Prs > 10000 || CurDip1Prs < -500 )  
		CurDip1Prs = 0;
	
	if(CurDip2Prs > 10000 || CurDip2Prs < -500 )
		CurDip2Prs = 0;

    
    /*****说明
    *   如果1端传感器值有变化，另外一端传感器值无变化.
    *   变换传感器次次数大于120次，另一端小于10次。认为无变化端异常
    *   传感器值置零
    */
//    if(fabs(CurDip1Prs - lsthig1) > 5){
//        hig1chgtimes++; 
//    }else {   
//        hig1chgtimes = 0;
//    }
//    lsthig1 = CurDip1Prs;
//    
//    if(fabs(CurDip2Prs - lsthig2) > 5){
//        hig2chgtimes++; 
//    }else {   
//        hig2chgtimes = 0;
//    }
//    lsthig2 = CurDip2Prs;    
//    
//    if(hig1chgtimes > 120 && )
//    
    
    /*****说明
    *   如果任意传感器有值，且两传感器值偏差大于2000.
    *   连续2分钟，如果有传感器值小于500，任务传感器异常。
    *   传感器值置零
    */
	PrsDef = CurDip1Prs - CurDip2Prs;

	if(CurDip1Prs > 1000 || CurDip2Prs >1000)
	{
		if(fabs(PrsDef) > 2000 )
		{
			Times++;
			
			if(Times > 120)
			{
				if(CurDip1Prs < 500)
				{
					CurDip1Prs = 0;
				}
				else if(CurDip2Prs < 500)
				{
					CurDip2Prs = 0;
				}
			}
		}
		else
		{
			Times = 0;
		}
	}


    /*****说明
    *   如过高度1，高度2值都有效，平均值为两者均值。
    */
	if( CurDip1Prs && CurDip2Prs )							
	{
		CurDipAvgPrs  =	(CurDip1Prs + CurDip2Prs) / 2;
	}
	else if( CurDip1Prs )
	{
		CurDipAvgPrs  = CurDip1Prs;
	}
	else if( CurDip2Prs )
	{
		CurDipAvgPrs  = CurDip2Prs;
	}else
    {
        CurDipAvgPrs = 0;                                   //输入值均为0 
    }
	//printfcom0("\r\n11 %d,%d,%d",CurDip1Prs,CurDip2Prs,CurDipAvgPrs);
	return	CurDipAvgPrs;
}

/*******************************************************************************
 * 名    称：        CalcOil
 * 功    能：        计算油量。根据高度值，油箱容积表，修正系数等计算油量
 * 入口参数：        无
 * 出口参数： 	    无
 * 作　 　者： 	    redmornigcn
 * 创建日期： 	    2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void	CalcOil(void)							
{
	int16	Avg30SecHig;
    int16   avghig;
    static  char    times = 0;
    
//    if(times < 15)//避免开机运算，等待稳定
//    {
//        times++;
//        return;
//    }
    avghig 	= GetDipPrsAvg(sCtrl.sRec.Myhig1,sCtrl.sRec.Myhig2);    //平均值
        
    Avg30SecHig = GetDipPrs30SecVal(avghig);		                //计算30S的平均值
					
	sCtrl.sRec.MstDip = GetMathOilVal(Avg30SecHig);                 //计算油量值
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
void    BSP_CalcInit(void)
{
    //初始化flash
	InitFlashIO();
    //初始化fram
    GPIO_Fram_Init();
    //读sCtrl
//    FRAM_ReadRecNumMgr((StrRecNumMgr  *)&sCtrl.sRecNumMgr);         //读记录号
//    FRAM_ReadProductInfo((StrProductInfo  *)&sCtrl.sProductInfo);   //读产品编号
//    FRAM_ReadCurRecord((stcFlshRec  *)&sCtrl.sRec);                 //读当前记录
////    FRAM_ReadOilPara((StrOilPara  *)&sCtrl.SOilPara);               //读计算参数
//    FRAM_ReadRunPara((stcRunPara  *)&sCtrl.sRunPara);               //读运行参数
////    FRAM_ReadCalcModel((stcCalcModel  *)&sCtrl.sCalcModel);         //读计算模型
}

void    GetOilPara(void)
{
//	关调度及中断，数据不允许更新
    CPU_SR  cpu_sr;
CPU_CRITICAL_ENTER() ;

    FRAM_ReadOilPara((StrOilPara  *)&sCtrl.SOilPara);       //读计算参数。高度，修正系数，模型编号

    /***********************************************
    * 描述： 参数合理性判断
    */
    if(sCtrl.SOilPara.Slope < 1000 || sCtrl.SOilPara.Slope > 20000 )
        sCtrl.SOilPara.Slope = 10000;
    
    if(sCtrl.SOilPara.Density < 800 || sCtrl.SOilPara.Density > 900)
        sCtrl.SOilPara.Density = 820;
    
    if(sCtrl.SOilPara.ModelNum > 64)
        sCtrl.SOilPara.ModelNum = 1;
    
    if(sCtrl.SOilPara.Hig > 500 || sCtrl.SOilPara.Hig < -500)
        sCtrl.SOilPara.Hig = 0;
    
    FRAM_StoreOilPara((StrOilPara  *)&sCtrl.SOilPara);      //写入算参数。高度，修正系数，模型编号
    
    GetOilBoxModel();                                       //取油箱模型

    FRAM_StoreCalcModel((stcCalcModel  *)&sCtrl.sCalcModel);//取出的油箱模型再保存

CPU_CRITICAL_EXIT() ;

}

/*******************************************************************************
 * 名    称：  TaskInitCalc
 * 功    能：  任务初始化
 * 入口参数： 	无
 * 出口参数： 	无
 * 作　 　者： 	redmornigcn
 * 创建日期： 	2017-05-16
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void TaskInitCalc(void)
{    
//    /***********************************************
//    * 描述： 初始化本任务用到的相关硬件
//    */
    BSP_CalcInit();			    //初始化和计算相关的参数

    GetOilPara();
    
    //GetMathModelPara(stcModelComplex * sComplexModel,uint8 	ReCalFlg)
    //初始化定时器  1s 执1次
    osal_start_timerEx( OS_TASK_ID_CALC,
                      OS_EVT_CALC_TICKS,
                      10*1000); //10秒后再运算
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * 名    称： 	AppTask
 * 功    能： 	控制任务
 * 入口参数： 	p_arg - 由任务创建函数传入
 * 出口参数： 	无
 * 作　 　者： 	redmorningcn.
 * 创建日期： 	2017-05-15
 * 修    改：
 * 修改日期：
 *******************************************************************************/
osalEvt  TaskCalcEvtProcess(INT8U task_id, osalEvt task_event)
{
//    OS_ERR      err;
//    INT32U      ticks;
//    INT32S      dly;
//    CPU_SR_ALLOC();

    if( task_event & OS_EVT_CALC_TICKS ) {

            CalcOil();            //计算油量, 1S执行1次
            
            osal_start_timerEx( OS_TASK_ID_CALC,
                                OS_EVT_CALC_TICKS,
                                1000);
            
            return ( task_event ^ OS_EVT_CALC_TICKS );
    }
    
    return  task_event;
}



/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif
