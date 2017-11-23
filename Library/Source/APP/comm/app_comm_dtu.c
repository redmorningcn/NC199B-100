/*******************************************************************************
 *   Filename:       app_comm_dtu.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
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
#include  "IAP_program_mcu.h"

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_comm_dtu__c = "$Id: $";
#endif

#define APP_TASK_COMM_EN     DEF_ENABLED
#if APP_TASK_COMM_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
//#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)
#define IAP_START_CODE              1
#define IAP_DATA_CODE               2
#define IAP_END_CODE                3

/*******************************************************************************
 * TYPEDEFS
 */
//IAP 下载帧控制字
#define     IAP_FRAME_CODE          10 
//SET 设置控制字
#define     SET_FRAME_CODE          2      


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




/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */
//uint8   FRAM_StoreRecNumMgr(StrRecNumMgr  *SRecNumMgr);
//uint8   FRAM_StoreOilPara(StrOilPara  *SOilPara);
//void    BSP_DispClrAll(void);
//uint8   FRAM_StoreProductInfo(StrProductInfo  *sProductInfo);
//uint8   ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen);            
//uint8   WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen);            
//void    GetOilPara(void);


uint8   comm_record_send_one(StrDevDtu * sDtu,uint8 addrnum);
void    comm_record_send(StrDevDtu * sDtu,uint8 addrnum);
void    comm_para_flow(StrDevDtu * sDtu,uint8 addrnum);
uint8   comm_tinyrec_send(StrDevOtr * sOtr,uint8    addrnum);
//int8    IAP_PragramDeal(uint8 *databuf,char datalen);

/*******************************************************************************/

extern  stcSysCtrl  sCtrl;

/*******************************************************************************
 * 名    称： app_comm_dtu
 * 功    能： 调用无线通讯处理函数。
 *            如果是定时器超时，则对应地址的timeout计算加1，并判断
 *            值是否到满足该地址故障判断次数，是否执行通讯异常处理。
 *            如果是接收到数据事件，则根据接收数据进行处理。
 * 入口参数： 无
 * 出口参数： 无
 * 作　 　者： redmorningcn.
 * 创建日期： 2017-05-18
 * 修    改：
 * 修改日期：
 * 备    注： 任务创建函数需要在app.h文件中声明
 *******************************************************************************/
void    app_comm_dtu(void)
{
    //判断对应地址连接是否接收到数据
    uint8   i = 0;
    //uint8   status = 0;
    uint8   echolen = 0;

    while(i < COMM_DEV_DTU_CONN_NUM){
        if(sCtrl.Dtu.ConnCtrl[i].RecvEndFlg == 1)                   //如果地址i接收到数据，对数据进行处理
        {
            /***********************************************
            * 描述： 从接收的revcbuf中取数据
            *        根据接收到的地址CA，进行无线  数据传输；
            *        接收到地址C2，进行  设置模式。
            */
            switch(sCtrl.Dtu.RxCtrl.FrameCode)
            {
                case 0:     //V1.0
            
                    if(sCtrl.Dtu.RxCtrl.SourceAddr == SLAVE_ADDR_DTU)       //地址等于CA，无线数据传输。
                    {
                        comm_record_send((StrDevDtu *)&sCtrl.Dtu,i);        //数据的需要传递  dtu，明确 地址，pch等信息。
                    }
                    
                    if(sCtrl.Dtu.RxCtrl.SourceAddr == SLAVE_ADDR_SET)       //地址等于C2，参数设置。
                    {
                        comm_para_flow((StrDevDtu *)&sCtrl.Dtu,i);          //设置参数，或信息交互。依据code进行相应操作。
                        
                        //参数设置，数据原路返回
                        CSNC_SendData(  sCtrl.Dtu.pch,                        //DTU 的PCH：串口号，收发控制等底层信息
                                        sCtrl.Dtu.RxCtrl.DestAddr,            //源地址，
                                        sCtrl.Dtu.RxCtrl.SourceAddr,          //目标地址
                                        sCtrl.Dtu.RxCtrl.FramNum,             //帧序号 
                                        sCtrl.Dtu.RxCtrl.FrameCode,           //命令字
                                        sCtrl.Dtu.Rd.Buf,                     //数据区
                                        sCtrl.Dtu.RxCtrl.Len                  //发送长度
                                      );
                    }
                    break;
                case 1:     //V2.0
                    break;
                case SET_FRAME_CODE:        //V2.0 IAP参数设置
                    
                    break;
                    
                case IAP_FRAME_CODE:        //V2.0 IAP程序下载
                        //status = IAP_PragramDeal(sCtrl.Dtu.Rd.Buf , sCtrl.Dtu.RxCtrl.Len ); //调用程序下载函数   
//                        IAP_PragramDeal(sCtrl.Dtu.Rd.Buf , sCtrl.Dtu.RxCtrl.Len ); //调用程序下载函数  
//                        
//                        echolen = 4;
//                        //应答数据          //如果是IAP传包帧，则传数据长度4，如果是开始、结束。返回长度为
//                        if(     sCtrl.Dtu.Rd.Buf[0] == IAP_START_CODE 
//                            ||  sCtrl.Dtu.Rd.Buf[0] == IAP_END_CODE )
//                        {
//                            echolen = sCtrl.Dtu.RxCtrl.Len;   
//                        }
//                        
//                        CSNC_SendData(  sCtrl.Dtu.pch,                        //DTU 的PCH：串口号，收发控制等底层信息
//                                        sCtrl.Dtu.RxCtrl.DestAddr,            //源地址，
//                                        sCtrl.Dtu.RxCtrl.SourceAddr,          //目标地址
//                                        sCtrl.Dtu.RxCtrl.FramNum,             //帧序号 ，在接收中累加
//                                        sCtrl.Dtu.RxCtrl.FrameCode,           //命令字
//                                        sCtrl.Dtu.Rd.Buf,                     //数据区
//                                        echolen                               //发送长度
//                                    );
                    break;
                    
                default:;  //V2.0
            }
            sCtrl.Dtu.ConnCtrl[i].RecvEndFlg = 0;                   //数据已处理。 接收数据置1
            sCtrl.Dtu.ConnCtrl[i].TimeOut    = 0;                   //超时计数清零

            return  ;
        }
        
        i++;
    }
 
    /***********************************************
    *   描述： 如果是超时。
    *1. 超时时，所有地址计算加1，对应计算器在收到数据时清零。
    *2. 10数据无应答，发送一次数据。
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_DTU_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_DTU_CONN_NUM){
            sCtrl.Dtu.ConnCtrl[i].TimeOut++;
            
            if(sCtrl.Dtu.ConnCtrl[i].TimeOut > 10 * COMM_DEV_DTU_CONN_NUM){
                sCtrl.Dtu.ConnCtrl[i].TimeOut = 10 * COMM_DEV_DTU_CONN_NUM + 1;
                sCtrl.Dtu.ConnCtrl[i].ErrFlg = 1;               //超时数大于10次，5S，报通讯错误。
                
            }else{
                sCtrl.Dtu.ConnCtrl[i].ErrFlg = 0;               //接收到数据，通讯清零
            }
            i++;
        }
        
        /***********************************************
        * 描述： 如果是超时。
        * 发送数据
        */ 
        if( sCtrl.sRecNumMgr.Current == 0 )                                     //记录号为0，还未发送数据。不做通讯状态判断
        {
            uint8 i = 0;
            while( i < COMM_DEV_DTU_CONN_NUM){
                sCtrl.Dtu.ConnCtrl[i].TimeOut = 0;
                i++;
            }
            return;
        }
                
        //发送1条数据记录
        i =0 ;
        while(i < COMM_DEV_DTU_CONN_NUM){
            if(sCtrl.Dtu.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DTU){              //对DTU地址发送数据 
                                                                                    
                if(     sCtrl.sRecNumMgr.Current > sCtrl.sRecNumMgr.GrsRead      //数据未发完，10每次，否则，有新数据才发送
                   ||   sCtrl.sRecNumMgr.GrsRead >  sCtrl.sRecNumMgr.Current   ) //记录异常，也发送
                {
                    comm_record_send_one((StrDevDtu *)&sCtrl.Dtu,i);  
                    break;
                }
            }
            i++;
        }
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif


