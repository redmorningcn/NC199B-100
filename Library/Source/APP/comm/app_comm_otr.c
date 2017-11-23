/*******************************************************************************
 *   Filename:       app_comm_otr.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Otr 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 OTR 按 Ctrl + H, 钩选 Match the case, Replace with
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


#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_comm_otr__c = "$Id: $";
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
uint8   comm_tinyrec_send(StrDevOtr * sOtr,uint8    addrnum);



/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */
uint8   FRAM_StoreRecNumMgr(StrRecNumMgr  *SRecNumMgr);
uint8   FRAM_StoreOilPara(StrOilPara  *SOilPara);
void    BSP_DispClrAll(void);
uint8   FRAM_StoreProductInfo(StrProductInfo  *sProductInfo);
void    comm_para_flow(StrDevDtu * sDtu,uint8 addrnum);
uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen);            



/*******************************************************************************/

extern  stcSysCtrl  sCtrl;

/*******************************************************************************
 * 名    称：  app_comm_otr
 * 功    能： 调用IC卡通讯处理函数。
 *            如果是定时器超时，则对应地址的timeout计算加1，并判断
 *            值是否到满足该地址故障判断次数，是否执行通讯异常处理。
 *            如果是接收到数据事件，则根据接收数据进行处理。
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者：redmorningcn.
 * 创建日期： 2017-05-20
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
void    app_comm_otr(void)
{
    //判断对应地址连接是否接收到数据
    uint8 i = 0;
    while(i < COMM_DEV_OTR_CONN_NUM){
        if(sCtrl.Otr.ConnCtrl[i].RecvEndFlg == 1)                   //如果地址i接收到数据，对数据进行处理
        {
            /***********************************************
            * 描述： 从接收的revcbuf中取数据
            *        根据接收到的地址CA，进行无线  数据传输；
            *        接收到地址C2，进行  设置模式。
            */
            
            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_OTR)       //地址等于C1，IC数据传输。
            {
//                comm_record_send((StrDevOtr *)&sCtrl.Otr.RxCtrl,i); //数据的需要传递  otr，明确 地址，pch等信息。
                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);   //设置参数，或信息交互。依据code进行相应操作。
            }
            
            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_SET)//地址等于C2，参数设置。
            {
                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);  //设置参数，或信息交互。依据code进行相应操作。
            }
            
            sCtrl.Otr.ConnCtrl[i].RecvEndFlg = 0;           //数据已处理。 接收数据置1
            sCtrl.Otr.ConnCtrl[i].TimeOut    = 0;           //超时计数清零
            
            return  ;
        }
        i++;
    }
 
    /***********************************************
    *   描述： 如果是超时。
    *1. 超时时，所有地址计算加1，对应计算器在收到数据时清零。
    *2. 10数据无应答，发送一次数据。
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_OTR_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_OTR_CONN_NUM){
            sCtrl.Otr.ConnCtrl[i].TimeOut++;

            if(sCtrl.Otr.ConnCtrl[i].TimeOut > 10 * COMM_DEV_OTR_CONN_NUM){
                sCtrl.Otr.ConnCtrl[i].TimeOut = 10 * COMM_DEV_OTR_CONN_NUM + 1;
                sCtrl.Otr.ConnCtrl[i].ErrFlg = 1;               //超时数大于10次，5S，报通讯错误。
                
            }else{
                sCtrl.Otr.ConnCtrl[i].ErrFlg = 0;               //接收到数据，通讯清零
            }
            
            i++;
        }
        
        /***********************************************
        * 描述： 如果是超时。
        * 发送数据
        */ 
        if( sCtrl.sRecNumMgr.Current == 0 )                     //记录号为0，还未发送数据。不做通讯状态判断
        {
            uint8 i = 0;
            while( i < COMM_DEV_OTR_CONN_NUM){
                sCtrl.Otr.ConnCtrl[i].TimeOut = 0;
                i++;
            }
            return;
        }
                
        //发送1条数据记录
        i =0 ;
        while(i < COMM_DEV_OTR_CONN_NUM)
            if(sCtrl.Otr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_OTR){
        //对OTR地址发送数据 
                comm_tinyrec_send((StrDevOtr *)&sCtrl.Otr,i);
                //comm_record_send_one((StrDevOtr *)&sCtrl.Otr.RxCtrl,i);  
                break;
            }
           i++;
        
    }
}


/*******************************************************************************
 * 名    称： comm_tinyrec_send
 * 功    能： 发送记录中的比较关键信息
 * 入口参数： 通讯设备数据结构
 * 出口参数： 无
 * 作　 　者： redmorningcn.
 * 创建日期： 2017-05-20
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
uint8    comm_tinyrec_send(StrDevOtr * sOtr,uint8    addrnum)
{
    //if(sOtr->pch->Ch == 2)      //无线通道
    {
        //记录号为0，还未发送数据。不做通讯状态判断
        if( sCtrl.sRecNumMgr.Current == 0 )                                   
        {
            return 0;
        }
        /***********************************************
        * 描述： 取值
        * 
        */ 
        sOtr->Wr.sTinyRec.Addr      = 0xc1;
        sOtr->Wr.sTinyRec.CurRecNum = sCtrl.sRecNumMgr.Current;
        sOtr->Wr.sTinyRec.Hig1      = sCtrl.sRec.Myhig1;
        sOtr->Wr.sTinyRec.Hig2      = sCtrl.sRec.Myhig2;
        sOtr->Wr.sTinyRec.LocoWorkSta=sCtrl.sRec.MyLocoWorkState;
        sOtr->Wr.sTinyRec.ModelNum  = sCtrl.sRec.OilBoxModel;
        sOtr->Wr.sTinyRec.Oil       = sCtrl.sRec.MstDip;
        sOtr->Wr.sTinyRec.Speed1    = sCtrl.sRec.Myspeed1;
        sOtr->Wr.sTinyRec.Speed2    = sCtrl.sRec.Myspeed2;
        sOtr->Wr.sTinyRec.Speedcs   = sCtrl.sRec.MyEngRotSpd;
       
        //超时值发送SLAVE_ADDR_OTR 的数据
        CSNC_SendData(sCtrl.Otr.pch,                                //OTR 的PCH：串口号，收发控制等底层信息
                      sOtr->ConnCtrl[addrnum].MasterAddr,           //源地址，
                      sOtr->ConnCtrl[addrnum].SlaveAddr,            //目标地址
                      sOtr->ConnCtrl[addrnum].SendFramNum,          //帧序号 ，，在接收中累加
                      0,                                            //命令字
                      (uint8 *)&sCtrl.Otr.Wr.sTinyRec,              //数据区
                      sizeof(sCtrl.Otr.Wr.sTinyRec)                 //发送长度
                      );
    }
    return 1;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif


