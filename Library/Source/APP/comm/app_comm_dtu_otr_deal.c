/*******************************************************************************
*   Filename:       app_comm_dtu_otr_deal.c
*   Revised:        All copyrights reserved to redmorningcn.
*   Revision:       v1.0
*   Writer:	        redmorningcn.
*
*   Description:    双击选中 dtu 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   双击选中 Dtu 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   双击选中 DTU 按 Ctrl + H, 钩选 Match the case, Replace with
*                   输入您要的名字，点击 Replace All
*                   在 app_cfg.h 中指定本任务的 优先级  （ APP_TASK_COMM_PRIO     ）
*                                            和 任务堆栈（ APP_TASK_COMM_STK_SIZE ）大小
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
#include <app_comm_protocol.h>
#include <app_ctrl.h>
#include <app.h>
#include <os_cfg_app.h>
#include <bsp_FRAM.h>
#include <bsp_DS3231.h>
#include  <global.h>



#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_comm_dtu_otr__c = "$Id: $";
#endif

#define APP_TASK_COMM_EN     DEF_ENABLED
#if APP_TASK_COMM_EN == DEF_ENABLED
/*******************************************************************************
* CONSTANTS
*/

/*******************************************************************************
* MACROS
*/
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
* TYPEDEFS
*/

#define		DATA_CARD           0x0001			//数据卡-----暂时未用
#define		DENSITY_CARD		0x0020			//密度卡
#define		MODEL_CARD			0x0040			//模型卡-----暂时未用
#define		TIME_CARD           0x0100			//时间卡-----
#define		COM_MODEL_CARD	    0x0100		    //串口模型卡-暂时未用
#define		HIGHT_CARD			0x0200			//高度卡-----
#define		COPY_CARD           0x0400			//复制卡-----暂时未用
#define		MODEL_SELECT_CARD	0x1000			//模型选择卡-
#define		FIX_CARD            0x2000			//装车卡-----
#define		COPY_FRAM_CARD	    0x4000			//铁电复制卡-暂时未用
#define		SET_DATA_CARD		0x0080			//置数卡-----按时未用
#define		DEBUG_CARD			0x0800			//调试卡-----暂时未用

#define		EXPAND_CARD			0x8000			//扩展卡-----暂时未用

#define		DATA_CARD_DIS		0x10000			//数据卡指示
#define		DATA_CARD_FIN		0x20000			//数据卡结束
#define		SYS_RUN_PARA		0x40000			//系统参数卡--暂未使用
#define		CLEAR_ALL			0x80000			//清零卡，数据记录清零

#define		RST_SYS				0x100000		//复位指令
#define		CALC_PARA           0x200000		//计算斜率
#define		RUN_MODEL_PARA      0x400000		//运算模型卡

#define    OIL_BOX_MODEL_ADDR				2000    //油箱模型存储地址


/*******************************************************************************
* LOCAL VARIABLES
*/

/*******************************************************************************
* GLOBAL VARIABLES
*/

/***********************************************
* 描述： 软定时器声明
*/

/***********************************************
* 描述： 软定时器回调函数
*/


/*******************************************************************************
* LOCAL FUNCTIONS
*/
uint8   comm_record_send_one(StrDevDtu * sDtu,uint8 addrnum);
void    comm_record_send(StrDevDtu * sDtu,uint8 addrnum);
void    comm_para_flow(StrDevDtu * sDtu,uint8 addrnum);
uint8	ReadFlshRec(stcFlshRecNCZ12 * sFlshRec,uint32	FlshRecNum);

//void    Restart(void);



/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* EXTERN VARIABLES
*/

/*******************************************************************************
* EXTERN FUNCTIONS
*/

void    BSP_DispClrAll(void);       
void    GetOilPara(void);
void    SetDispNow(void);
uint8   comm_tinyrec_send(StrDevOtr * sOtr,uint8    addrnum);

/*******************************************************************************/

extern  stcSysCtrl  sCtrl;

/*******************************************************************************
* 名    称： comm_para_flow
* 功    能： 参数流。根据具体参数进行操作
*            
* 入口参数： 无线设备数据结构
* 出口参数： 无
* 作　 　者： redmorningcn.
* 创建日期： 2017-05-19
* 修    改：
* 修改日期：
* 备    注： 任务创建函数需要在app.h文件中声明
*******************************************************************************/
void    comm_para_flow(StrDevDtu * sDtu,uint8 addrnum)
{
    stcTime     sTime;
    uint8       i = 0;
    uint32      modelrecvnum =0;
    uint32      tmp32;
    static     uint32      modelstoreaddr = 0;
    uint16      crc16;
    uint8       *p;
    
    BSP_DispClrAll();                               //清原显示值，立即显示设定值                 
    
    switch (sDtu->RxCtrl.DataCode)  
    {
        //设置密度
    case    DENSITY_CARD: 
//        sCtrl.SOilPara.Density = sDtu->Rd.Density;          
//        
//        FRAM_StoreOilPara((StrOilPara *)&sCtrl.SOilPara);   //存数据。关键数据存储时保护
//        
//        uprintf("%8d",sCtrl.SOilPara.Density);   //立即显示设定值 
        
        break;
        
        //设置时间
    case    TIME_CARD:                
        
        WriteTime(sDtu->Rd.sTime);          
        ReadTime((stcTime *)&sTime);
        
        uprintf("%02d-%02d-%02d",sTime.Year
                ,sTime.Month
                    ,sTime.Date
                        );     
        uprintf("%02d-%02d-%02d",sTime.Hour
                ,sTime.Min
                    ,sTime.Sec
                        );               
        break;
        
        //高度卡
    case    HIGHT_CARD:  
        //高度值可以累计
//        sCtrl.SOilPara.Hig += sDtu->Rd.Hig;
//        if(sCtrl.SOilPara.Hig  > 500)
//            sCtrl.SOilPara.Hig = 500;
//        
//        if(sCtrl.SOilPara.Hig  < -500)
//            sCtrl.SOilPara.Hig = -500;
//        
//        FRAM_StoreOilPara((StrOilPara *)&sCtrl.SOilPara);   //存数据。关键数据存储时保护
//        
//        uprintf("%8d",sCtrl.SOilPara.Hig);     
        
        break;
        
        //模型选择卡
    case    MODEL_SELECT_CARD:   
//        sCtrl.SOilPara.ModelNum = sDtu->Rd.ModelNum;//选择的模型编号
//        
//        FRAM_StoreOilPara((StrOilPara *)&sCtrl.SOilPara);   //存数据。关键数据存储时保护
//        
//        uprintf("%8d",sCtrl.SOilPara.ModelNum);      //立即显示设定值 
        break;
        
        //车型车号
    case    FIX_CARD: 
        sCtrl.sProductInfo.sLocoId.Num = sDtu->Rd.sLocoId.Num;
        sCtrl.sProductInfo.sLocoId.Type = sDtu->Rd.sLocoId.Type;
        
        FRAM_StoreProductInfo((StrProductInfo *)&sCtrl.sProductInfo);
        
        uprintf("%4d.%4d"   ,sCtrl.sProductInfo.sLocoId.Type
                ,sCtrl.sProductInfo.sLocoId.Num
                    );      
        break;
        
        //读数据指示
    case    DATA_CARD_DIS: 
        i=0;
        while(i < 4)
        {
            uprintf("%8d",i); 
            i++;
        }
        break;
        
        //读数据指示结束
    case    DATA_CARD_FIN: 
        i=0;
        while(i < 4)
        {
            uprintf("    good");  
            uprintf("         ");  
            i++;
        }
        break;
        
        //数据清零
    case    CLEAR_ALL: 
        sCtrl.sRecNumMgr.Current = 0;
        sCtrl.sRecNumMgr.GrsRead = 0;
        sCtrl.sRecNumMgr.IcRead  = 0;
        
        FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr);
        
        uprintf("    CALL");  
        
        break;
        
        //装置复位，先应答消息，再
    case    RST_SYS:
        
//        //参数设置，数据原路返回
//        CSNC_SendData(  sCtrl.Dtu.pch,                      //DTU 的PCH：串口号，收发控制等底层信息
//                        sCtrl.Dtu.RxCtrl.DestAddr,          //源地址，
//                        sCtrl.Dtu.RxCtrl.SourceAddr,        //目标地址
//                        sCtrl.Dtu.RxCtrl.FramNum,           //帧序号 
//                        sCtrl.Dtu.RxCtrl.FrameCode,         //命令字
//                        sCtrl.Dtu.Rd.Buf,                   //数据区
//                        sCtrl.Dtu.RxCtrl.Len                //发送长度
//                      );
//        i = 100000;
//        while(i--);
//        
//        Restart();                                          //重启系统
        break;
        
        //设置斜率
    case    CALC_PARA: 
        //斜率具有镭计的效果
//        sCtrl.SOilPara.Slope = (sCtrl.SOilPara.Slope  * sDtu->Rd.Slope)/10000;
//        
//        //0.1~2
//        if(sCtrl.SOilPara.Slope >20000 || sCtrl.SOilPara.Slope < 1000)
//            sCtrl.SOilPara.Slope = 10000;
//        
//        FRAM_StoreOilPara((StrOilPara *)&sCtrl.SOilPara);   //存数据。关键数据存储时保护
//        
//        uprintf("%d",sCtrl.SOilPara.Slope);         //立即显示设定值 
        break;
        
        /***********************************************
        * 描述： 设置油箱模型。
        *   IC 卡模块插入IC卡后，IC卡主动应答模型。  
        *   后续每200ms发送数据，序号从0开始累加。
        数据发送完成后，序号为0xffffffff。
        */    
        //设置斜率
    case    SYS_RUN_PARA: 
        //存运行参数，存储时间，数据格式等信息
        //FRAM_StoreRunPara((stcRunPara *)&sCtrl.sRunPara);   //存数据。关键数据存储时保护
        uint8  bittmp =  0x01 << ( ( (uint8 *)&sCtrl.sRunPara.StoreType ) - (uint8 *)&sCtrl.sRunPara);    //数据格式更改
        
        if((sDtu->Rd.sRunPara.SetBitFlg & bittmp) == bittmp )
        {
            sCtrl.sRunPara.StoreType    = sDtu->Rd.sRunPara.StoreType;
            sCtrl.sRunPara.StoreTypeBak = sDtu->Rd.sRunPara.StoreTypeBak;
        }
      
        FRAM_StoreRunPara((stcRunPara *)&sCtrl.sRunPara);   //存数据。关键数据存储时保护
        uprintf("rt-%d",    sCtrl.sRunPara.StoreType);      //立即显示设定值 （0x02 ）数据新版

        break;
        
    case    MODEL_CARD: 
        //模型数据接收序号
//        memcpy((uint8  *)&modelrecvnum,(uint8 *)&sDtu->Rd.Buf[0],sizeof(modelrecvnum));
//        
//        if( modelrecvnum == 0 )                 //开始    
//            modelstoreaddr = 0;
//        
//        if(sDtu->RxCtrl.Len < sizeof(modelrecvnum))
//            break;
//        //保存接收到的数据
//        WriteFM24CL64(   OIL_BOX_MODEL_ADDR+modelstoreaddr,
//                      (uint8 *)&sDtu->Rd.Buf[sizeof(modelrecvnum)], 
//                      sDtu->RxCtrl.Len - sizeof(modelrecvnum));
//        //存地址累加
//        modelstoreaddr += (sDtu->RxCtrl.Len - sizeof(modelrecvnum));
//        
//        //指示操作
//        //uprintf("   ...");
//        if(modelrecvnum == 0xffffffff)  //结束，指示
//        {
//            uprintf("");
//            uprintf("boX");	
//            uprintf("boX");		
//            GetOilPara();
//            
//            //与单独设置油量模型兼容，写卡成功后，应将单设模型置为无效,并保存。，再重新调用初始化函数
//            l_sCalcModel.valid = 0;
//            FRAM_StoreCalcModel(&sCtrl.sCalcModel);             //数据油量存储
//            
//            GetOilPara();                                       //重新取计算参数
//        }else{
//            uprintf("o..%2d",modelrecvnum);
//        }
//        
        SetDispNow();               //马上显示
        
        
//        if(sDtu->pch->PortNbr == 2)
//        {
//            osal_start_timerEx( OS_TASK_ID_TMR, 
//                               OS_EVT_TMR_OTR, 
//                               OS_TICKS_PER_SEC*10 );          //统计装置和无线发送模块定时器  10s
//        }
//        
//        if(sDtu->pch->PortNbr == 3)
//        {
//            osal_start_timerEx( OS_TASK_ID_TMR, 
//                               OS_EVT_TMR_DTU, 
//                               OS_TICKS_PER_SEC*10 );          //统计装置和无线发送模块定时器  10s
//        }
        break;
        
        //数据
    case    DATA_CARD: 
        
        memcpy((uint8 *)&tmp32,sDtu->Rd.Buf,sizeof(tmp32));     //请求流水号
        if(sDtu->RxCtrl.SourceAddr == SLAVE_ADDR_DTU)           //无线通道请求的
        {
            if(sCtrl.sRecNumMgr.GrsRead  > tmp32){
                sCtrl.sRecNumMgr.GrsRead = tmp32;               //指定位置取数据
            }
            
            comm_record_send_one(sDtu,addrnum);                 
            
            FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr);         //保存记录号
            
            sCtrl.sRecNumMgr.GrsRead++;
            
        }else if(sDtu->RxCtrl.SourceAddr == SLAVE_ADDR_OTR)     //IC卡
        {
            if(sCtrl.sRecNumMgr.IcRead  > tmp32){
                sCtrl.sRecNumMgr.IcRead = tmp32;                //指定位置取数据
            }
            
            comm_record_send_one(sDtu,addrnum);                 //指定位置取数据   
            
            FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr); //保存记录号
            
            sCtrl.sRecNumMgr.IcRead++;           
            
            //快速启动定时器
            if(sCtrl.sRecNumMgr.IcRead < sCtrl.sRecNumMgr.Current ){
                osal_start_timerRl( OS_TASK_ID_TMR, 
                                   OS_EVT_TMR_OTR, 
                                   OS_TICKS_PER_SEC/2 );          //统计装置和无线发送模块定时器  10s
            }
            
        }
        break;
        
    case    RUN_MODEL_PARA:
        
        uprintf("SET--");         //立即显示设定值 
        SetDispNow();
        
        memcpy((uint8 *)&tmp32, &sDtu->Rd.Buf[0],sizeof(tmp32));        //取帧序号
//        if(tmp32 < (1 + sizeof(l_sCalcModel)/128 ))                     //序号有效
//        {
//            p = (uint8 *)&l_sCalcModel;
//            memcpy((uint8 *)(  p + 128*tmp32),
//                               (uint8 *)&sDtu->Rd.Buf[4],
//                               sDtu->RxCtrl.Len-4);
//            
//            if(tmp32 == sizeof(l_sCalcModel)/128)               //接收完成
//            {
//                crc16 = GetCrc16Check((uint8 *)&l_sCalcModel,sizeof(l_sCalcModel)-2);   //计算校验
//                if(crc16 == l_sCalcModel.CrcCheck)              //模型校验通过，赋值给计算使用
//                {
//                    l_sCalcModel.valid = 1;                     //置模型有效位
//                    
//                    l_sCalcModel.CrcCheck = GetCrc16Check((uint8 *)&l_sCalcModel,sizeof(l_sCalcModel)-2); 
//                    
//                    memcpy((uint8 *)&sCtrl.sCalcModel,(uint8 *)&l_sCalcModel,sizeof(l_sCalcModel)); //赋值给正在使用的模型。
//                    
//                    FRAM_StoreCalcModel(&sCtrl.sCalcModel);
//                }
//                
//                //设置成功后，指示
//                uprintf("SET-%d",l_sCalcModel.ModelNum);         //立即显示设定值 
//                SetDispNow();
//                tmp32 = 1000000;
//                while(tmp32--);
//                uprintf("SET-%d",l_sCalcModel.ModelNum);         //立即显示设定值 
//                SetDispNow();
//            }
//        }
        
        break;
        
    default:
        break;

    }
}

/*******************************************************************************
* 名    称： comm_record_send_one
* 功    能： 发送one条数据记录
* 入口参数： 无线设备数据结构
* 出口参数： 无
* 作　 　者： redmorningcn.
* 创建日期： 2017-05-19
* 修    改：
* 修改日期：
* 备    注： 任务创建函数需要在app.h文件中声明
*******************************************************************************/
uint8    comm_record_send_one(StrDevDtu * sDtu,uint8    addrnum)
{
    
    //记录号为0，还未发送数据。不做通讯状态判断
    if( sCtrl.sRecNumMgr.Current == 0 )                                   
    {
        return 0;
    }
    /***********************************************
    * 描述： 记录号合理性判断。已发数据<当前数据 发送数据
    * 
    */ 
    if(sDtu->ConnCtrl[addrnum].SlaveAddr == SLAVE_ADDR_DTU)
    {
        if(sCtrl.sRecNumMgr.Current <= sCtrl.sRecNumMgr.GrsRead )               //已发送记录号比当前记录号大，进行异常处理
        {
            sCtrl.sRecNumMgr.GrsRead = 0;
            if(sCtrl.sRecNumMgr.Current)
                sCtrl.sRecNumMgr.GrsRead = sCtrl.sRecNumMgr.Current - 1;        //最后有效记录赋值     
            
            FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr);             //保存记录号
        }
        
        //sDtu->ConnCtrl[addrnum].SendFramNum++;                                //发送序号由接收控制
        
        /***********************************************
        * 描述： 数据记录准备,根据记录号取数据记录。
        * 
        */ 
        ReadFlshRec((stcFlshRecNCZ12 *)&sDtu->Wr.sRec,
                    sCtrl.sRecNumMgr.GrsRead);
        
        //重新给定时器给初值。osal_start_timerRl可以自动赋值，
        osal_start_timerRl( OS_TASK_ID_TMR, 
                           OS_EVT_TMR_DTU, 
                           OS_TICKS_PER_SEC*10 );  //统计装置和无线发送模块定时器  10s
    }
    
    if(sDtu->ConnCtrl[addrnum].SlaveAddr == SLAVE_ADDR_OTR)
    {
        if(sCtrl.sRecNumMgr.Current <= sCtrl.sRecNumMgr.IcRead )                //已发送记录号比当前记录号大，进行异常处理
        {
            sCtrl.sRecNumMgr.IcRead = 0;
            if(sCtrl.sRecNumMgr.Current)
                sCtrl.sRecNumMgr.IcRead = sCtrl.sRecNumMgr.Current - 1;         //最后有效记录赋值     
            
            sCtrl.Otr.ConnCtrl[addrnum].SendFramNum++;
            FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr);             //保存记录号
        }
        
        /***********************************************
        * 描述： 数据记录准备,根据记录号取数据记录。
        * 
        */ 
        ReadFlshRec((stcFlshRecNCZ12 *)&sDtu->Wr.sRec,
                    sCtrl.sRecNumMgr.IcRead);
        
        osal_start_timerRl( OS_TASK_ID_TMR, 
                           OS_EVT_TMR_OTR, 
                           OS_TICKS_PER_SEC*10 );               //统计装置和无线发送模块定时器  10s
    }
    
    uint8   framecode = 0;
    if(sCtrl.sRunPara.StoreType == 0x02 && sCtrl.sRunPara.StoreTypeBak == 0x02) //新版数据格式  redmorningcn 20170922
    {
        framecode = 0x02;                                       //报文控制字
    }
    //超时值发送SLAVE_ADDR_DTU 的数据
    CSNC_SendData(sDtu->pch,                                    //DTU 的PCH：串口号，收发控制等底层信息
                  sDtu->ConnCtrl[addrnum].MasterAddr,           //源地址，
                  sDtu->ConnCtrl[addrnum].SlaveAddr,            //目标地址
                  sDtu->ConnCtrl[addrnum].SendFramNum,          //帧序号 ，，在接收中累加
                  framecode,                                    //命令字
                  (uint8 *)&sDtu->Wr.sRec,                      //数据区
                  sizeof(sDtu->Wr.sRec)                        //发送长度
                      );

    return 1;
}


/*******************************************************************************
* 名    称： comm_record_send
* 功    能： 通讯数据传输
*            先查询已发送记录，然后发送该记录。记录号加1，帧号加1。起始记录号
从1开始。  
*              （通过帧号判断通讯是否结束正确）
* 入口参数： 无线设备数据结构
* 出口参数： 无
* 作　 　者： redmorningcn.
* 创建日期： 2017-05-19
* 修    改：
* 修改日期：
* 备    注： 任务创建函数需要在app.h文件中声明
*******************************************************************************/
void    comm_record_send(StrDevDtu * sDtu,uint8 addrnum)
{
    /***********************************************
    * 描述： 发送和接收的记录号相符，发送流水号加1
    *       再根据数据量，发送数据。
    * 
    */      
    
    if(sDtu->RxCtrl.FramNum == sDtu->ConnCtrl[addrnum].SendFramNum){
        sDtu->ConnCtrl[addrnum].SendFramNum++;                  //数据正确，序号加1
        sCtrl.sRecNumMgr.GrsRead++;
        
        CPU_SR  cpu_sr;
        CPU_CRITICAL_ENTER() ;
        FRAM_StoreRecNumMgr((StrRecNumMgr *)&sCtrl.sRecNumMgr); //保存记录号
        CPU_CRITICAL_EXIT() ;
        
        if(sCtrl.sRecNumMgr.GrsRead < sCtrl.sRecNumMgr.Current) //数据还未发送完成。收到数据及发送。
        {
            comm_record_send_one(sDtu,addrnum);                 //发送数据记录。
            //重新启动超时定时器
            osal_start_timerRl( OS_TASK_ID_TMR, 
                               OS_EVT_TMR_DTU, 
                               OS_TICKS_PER_SEC*10 );          //统计装置和无线发送模块定时器  10s
        }
        
    }
}


/*******************************************************************************
* 				end of file
*******************************************************************************/
#endif


