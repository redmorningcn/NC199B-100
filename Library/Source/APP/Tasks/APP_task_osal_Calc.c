/*******************************************************************************
 *   Filename:       app_task_store.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� disp �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Disp �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� DISP �� Ctrl + H, ��ѡ Match the case, Replace with
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
#define     FREQ_UP_STATE       4           		//Ƶ�ʱ仯״̬
#define     FREQ_STOP_STATE     5

//����ģ�ͱ��                                        
#define			HXN3_MODLE		31
#define			HXN5_MODLE		30

#define         MY_MODLE        40                  //��������ģ��
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
static  OS_TCB   AppTaskCalcTCB;

/***********************************************
* ������ �����ջ��STACKS��
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

     
     
extern	stcSysCtrl       sCtrl;       	//	ȫ�ֽṹ������
/*******************************************************************************/

stcModel 		  	l_sModel;
stcModelSemple	  	l_sModelSemple;
stcCalcModel        l_sCalcModel;

/*******************************************************************************
 * ��    �ƣ�          GetOilBoxModel()
 * ��    �ܣ�          ȡ�����õ�����ģ��.ִ�иú���ǰ���ȶ� sCtrl.SOilPara���Ȼ��
                      ����ģ�ͱ�š�
 * ��ڲ�����          ��
 * ���ڲ����� 	      ��
 * ���� ���ߣ� 	      redmornigcn
 * �������ڣ� 	      2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
uint8   GetOilBoxModel(void)
{
    //��ȡԤ������ģ�ͣ����ɹ����ٰ�����ģ�ͱ�ţ����м��㡣�������ֳ�ʹ��20170905��
    FRAM_ReadCalcModel(&l_sCalcModel);      //ȡģ��
    
    if(l_sCalcModel.valid == 1)             //ģ����Ч
    {
        uint16  crc16 = GetCrc16Check((uint8 *)&l_sCalcModel,sizeof(l_sCalcModel)-2);
        
        if(crc16 == l_sCalcModel.CrcCheck) //����У��ͨ��
        {
            memcpy((uint8 *)&sCtrl.sCalcModel,(uint8 *)&l_sCalcModel,sizeof(l_sCalcModel));
            
            return 1;                       
        }
    }
    
//////////////////////////////////////////////////////////////////    
    switch (sCtrl.SOilPara.ModelNum)                                //����ģ�ͱ�ţ�ȷ������ģ�ͣ���������ģ��
    {
        case HXN5_MODLE:
            GetHxn5OilBoxModel((stcCalcModel *)&sCtrl.sCalcModel);  //HXN5
            break;
        case HXN3_MODLE:
            GetHxn3OilBoxModel((stcCalcModel *)&sCtrl.sCalcModel);  //HXN3
            break;   
            
        case MY_MODLE:                                              //����ģ��
            FRAM_ReadCalcModel((stcCalcModel *)&sCtrl.sCalcModel);  //ȡ����ģ��
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
    
    uint8   modelerrflg= 0;                                                     //ģ�ʹ����ʾ
    
    //���ģ���Ƿ���ȷ,ģ���쳣��DF4Dģ��
    if(sCtrl.sCalcModel.PotNum < 101 && sCtrl.sCalcModel.PotNum  > 1)           //ģ�͵���2~100������ģ���쳣
    {
        for(uint8 i = 0 ;i <  sCtrl.sCalcModel.PotNum-1; i++)
        {
            if(sCtrl.sCalcModel.sModPot[i+1].DipVal >  sCtrl.sCalcModel.sModPot[i].DipVal 
               &&  sCtrl.sCalcModel.sModPot[i+1].HigVal >  sCtrl.sCalcModel.sModPot[i].HigVal)
            { continue; }     //ģ�͵ĵ������ֵ����ǰ��ֵ������ģ�������������쳣
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
        //ģ�ʹ��󣬲���Ĭ��ģ��DF4D
        GetDf4dOilBoxModel((stcModel *)&l_sModel);    
        
        stcModelTostcModelComplex((stcCalcModel *)&sCtrl.sCalcModel,(stcModel *)&l_sModel); //100829	
        return  0; 
	}
    
    return 1;
}


int16	l_Dip1Frq10SecChg;
int16	l_Dip2Frq10SecChg;
/*******************************************************************************
 * ��    �ƣ�        GetFrqChgState
 * ��    �ܣ�        ȡ�����仯״̬
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-17
 * ��    �ģ�
 * �޸����ڣ�
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
		l_Dip1Frq10SecChg = Dip1PrsBuf[Times %12] - Dip1PrsBuf[(Times - 11)%12];		//2??��			
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
 * ��    �ƣ�        GetFrqChgState
 * ��    �ܣ�        ȡ�����仯״̬
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-17
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
uint8  GetFrqChgState(int16	CurDip1Prs,int16	CurDip2Prs)
{
    static  int32  UpTimes = 0,DownTimes = 0,StopTimes = 0,WaveTimes = 0;
    static	uint8	FreqChgState = FREQ_WAVE_STATE;
    
    GetDip10SecChg(CurDip1Prs,CurDip2Prs);							//��?10??��??��?��
      
    if(((l_Dip1Frq10SecChg > 30) && (l_Dip2Frq10SecChg > 30)))		//��?��y
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
    else  if(((l_Dip1Frq10SecChg < -1) && (l_Dip2Frq10SecChg < -1)))		//��??��	
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
			 	(abs(l_Dip2Frq10SecChg) < 15 ))						//?��?��
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

        if( abs(l_Dip1Frq10SecChg - l_Dip2Frq10SecChg) > 300 )		//�䨮2��?��
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
            else													//2��?��
            {
                FreqChgState = FREQ_WAVE_STATE;
            }
        }	    
    }
    
    return	FreqChgState;
}
 
/*******************************************************************************
 * ��    �ƣ�        GetDipPrs30SecVal
 * ��    �ܣ�        �������������ݸ߶�ֵ�������ݻ�������ϵ���ȼ�������.
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
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
		
	PrsChgState = GetFrqChgState(sCtrl.sRec.Myhig1,sCtrl.sRec.Myhig2);	//����״̬	

	if(Times < 30)
	{
        if(PrsAvg == 0)                                                //����ż���
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
 * ��    �ƣ�        GetMathOilVal
 * ��    �ܣ�        �������������ݸ߶�ֵ�������ݻ�������ϵ���ȼ�������.
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
uint16    GetMathOilVal(int16   hig)
{
  	float 		DipVal = 0;                  		
  	float 		Hight;                  			
	float		MinHig,MaxHig;	
	float		MinDip,MaxDip;					
    
	uint8		ModelPotNum;
	uint8		i;
    
	ModelPotNum = sCtrl.sCalcModel.PotNum;              //ģ�͵���

	Hight = hig;                                        //�߶�ֵ

	Hight += ((float)sCtrl.SOilPara.Hig );              //���벹���߶�	
  	
  	for(i = 1; i <  ModelPotNum; i++)                   //�����ĸ���
  	{
		MinHig 	= sCtrl.sCalcModel.sModPot[i-1].HigVal; //��1����ĸ߶�ֵ
		MaxHig 	= sCtrl.sCalcModel.sModPot[i].HigVal;   //��2����ĸ߶�ֵ

		MinDip 	= sCtrl.sCalcModel.sModPot[i-1].DipVal; //��1���������ֵ
		MaxDip 	= sCtrl.sCalcModel.sModPot[i].DipVal;   //��2���������ֵ
						
		if(MinHig == MaxHig  || MinDip == MaxDip)       //����ģ���г���������ͬ��
		{
			//DisplaySet(60,1,"E-OL");                    //��ʾ����ֵ
			return	0;                                  //����ģ�ʹ�����ʾ0
		}

		if(MinHig <= Hight && Hight < MaxHig)		    //��������,��ģ��ֵ�У�������ֵ����
		{	
			DipVal = MinDip + ((Hight - MinHig)  * (MaxDip - MinDip)) / (MaxHig - MinHig);

			break;
		}
	
		if(i == 1)									//С����С�߶ȣ�����͵�б��
		{			
			if(Hight < MinHig)
			{
				DipVal = MinDip + ((Hight - MinHig)  * ((MaxDip - MinDip)) / (MaxHig - MinHig));
				
				break;
			}
		}
		
		if(i == (ModelPotNum - 1))					//�߶�ֵ�������һ�㣬���������
		{
			
			if(Hight >= MaxHig)
			{
				DipVal = MinDip + ((Hight - MinHig)  * ((MaxDip - MinDip)) / (MaxHig - MinHig));
			}
		}
	}
        
    DipVal *= sCtrl.SOilPara.Slope;                 //��������б��
    DipVal /= 10000;
	DipVal += sCtrl.sCalcModel.StartOil;            //����ֵ �� ģ����ʼ����ֵ
    
	
    return  (uint16)DipVal;
}

/*******************************************************************************
 * ��    �ƣ�        GetDipPrsAvg
 * ��    �ܣ�        ȡƽ��ֵ
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
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
    
    //��ͨѶʱ���߶�ֵ��ֱ�Ӹ�0ֵ��
    /*****˵��
    *   ѹ��ֵ���ޣ���ֱֵ��Ϊ��
    */
	if(CurDip1Prs > 10000 || CurDip1Prs < -500 )  
		CurDip1Prs = 0;
	
	if(CurDip2Prs > 10000 || CurDip2Prs < -500 )
		CurDip2Prs = 0;

    
    /*****˵��
    *   ���1�˴�����ֵ�б仯������һ�˴�����ֵ�ޱ仯.
    *   �任�������δ�������120�Σ���һ��С��10�Ρ���Ϊ�ޱ仯���쳣
    *   ������ֵ����
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
    
    /*****˵��
    *   ������⴫������ֵ������������ֵƫ�����2000.
    *   ����2���ӣ�����д�����ֵС��500�����񴫸����쳣��
    *   ������ֵ����
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


    /*****˵��
    *   ����߶�1���߶�2ֵ����Ч��ƽ��ֵΪ���߾�ֵ��
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
        CurDipAvgPrs = 0;                                   //����ֵ��Ϊ0 
    }
	//printfcom0("\r\n11 %d,%d,%d",CurDip1Prs,CurDip2Prs,CurDipAvgPrs);
	return	CurDipAvgPrs;
}

/*******************************************************************************
 * ��    �ƣ�        CalcOil
 * ��    �ܣ�        �������������ݸ߶�ֵ�������ݻ�������ϵ���ȼ�������
 * ��ڲ�����        ��
 * ���ڲ����� 	    ��
 * ���� ���ߣ� 	    redmornigcn
 * �������ڣ� 	    2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void	CalcOil(void)							
{
	int16	Avg30SecHig;
    int16   avghig;
    static  char    times = 0;
    
//    if(times < 15)//���⿪�����㣬�ȴ��ȶ�
//    {
//        times++;
//        return;
//    }
    avghig 	= GetDipPrsAvg(sCtrl.sRec.Myhig1,sCtrl.sRec.Myhig2);    //ƽ��ֵ
        
    Avg30SecHig = GetDipPrs30SecVal(avghig);		                //����30S��ƽ��ֵ
					
	sCtrl.sRec.MstDip = GetMathOilVal(Avg30SecHig);                 //��������ֵ
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
void    BSP_CalcInit(void)
{
    //��ʼ��flash
	InitFlashIO();
    //��ʼ��fram
    GPIO_Fram_Init();
    //��sCtrl
//    FRAM_ReadRecNumMgr((StrRecNumMgr  *)&sCtrl.sRecNumMgr);         //����¼��
//    FRAM_ReadProductInfo((StrProductInfo  *)&sCtrl.sProductInfo);   //����Ʒ���
//    FRAM_ReadCurRecord((stcFlshRec  *)&sCtrl.sRec);                 //����ǰ��¼
////    FRAM_ReadOilPara((StrOilPara  *)&sCtrl.SOilPara);               //���������
//    FRAM_ReadRunPara((stcRunPara  *)&sCtrl.sRunPara);               //�����в���
////    FRAM_ReadCalcModel((stcCalcModel  *)&sCtrl.sCalcModel);         //������ģ��
}

void    GetOilPara(void)
{
//	�ص��ȼ��жϣ����ݲ��������
    CPU_SR  cpu_sr;
CPU_CRITICAL_ENTER() ;

    FRAM_ReadOilPara((StrOilPara  *)&sCtrl.SOilPara);       //������������߶ȣ�����ϵ����ģ�ͱ��

    /***********************************************
    * ������ �����������ж�
    */
    if(sCtrl.SOilPara.Slope < 1000 || sCtrl.SOilPara.Slope > 20000 )
        sCtrl.SOilPara.Slope = 10000;
    
    if(sCtrl.SOilPara.Density < 800 || sCtrl.SOilPara.Density > 900)
        sCtrl.SOilPara.Density = 820;
    
    if(sCtrl.SOilPara.ModelNum > 64)
        sCtrl.SOilPara.ModelNum = 1;
    
    if(sCtrl.SOilPara.Hig > 500 || sCtrl.SOilPara.Hig < -500)
        sCtrl.SOilPara.Hig = 0;
    
    FRAM_StoreOilPara((StrOilPara  *)&sCtrl.SOilPara);      //д����������߶ȣ�����ϵ����ģ�ͱ��
    
    GetOilBoxModel();                                       //ȡ����ģ��

    FRAM_StoreCalcModel((stcCalcModel  *)&sCtrl.sCalcModel);//ȡ��������ģ���ٱ���

CPU_CRITICAL_EXIT() ;

}

/*******************************************************************************
 * ��    �ƣ�  TaskInitCalc
 * ��    �ܣ�  �����ʼ��
 * ��ڲ����� 	��
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmornigcn
 * �������ڣ� 	2017-05-16
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitCalc(void)
{    
//    /***********************************************
//    * ������ ��ʼ���������õ������Ӳ��
//    */
    BSP_CalcInit();			    //��ʼ���ͼ�����صĲ���

    GetOilPara();
    
    //GetMathModelPara(stcModelComplex * sComplexModel,uint8 	ReCalFlg)
    //��ʼ����ʱ��  1s ִ1��
    osal_start_timerEx( OS_TASK_ID_CALC,
                      OS_EVT_CALC_TICKS,
                      10*1000); //10���������
}

extern  MODBUS_CH   *g_pch; 
/*******************************************************************************
 * ��    �ƣ� 	AppTask
 * ��    �ܣ� 	��������
 * ��ڲ����� 	p_arg - �����񴴽���������
 * ���ڲ����� 	��
 * ���� ���ߣ� 	redmorningcn.
 * �������ڣ� 	2017-05-15
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
osalEvt  TaskCalcEvtProcess(INT8U task_id, osalEvt task_event)
{
//    OS_ERR      err;
//    INT32U      ticks;
//    INT32S      dly;
//    CPU_SR_ALLOC();

    if( task_event & OS_EVT_CALC_TICKS ) {

            CalcOil();            //��������, 1Sִ��1��
            
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
