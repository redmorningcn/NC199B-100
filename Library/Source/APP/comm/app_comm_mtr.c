
/*******************************************************************************
 *   Filename:       app_comm_mtr.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 comm 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 Comm 按 Ctrl + H, 钩选 Match the case, Replace with
 *                   输入您要的名字，点击 Replace All
 *                   双击选中 COMM 按 Ctrl + H, 钩选 Match the case, Replace with
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
const  CPU_CHAR  *app_comm_mtr__c = "$Id: $";
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

/*******************************************************************************/


/*******************************************************************************
 * 名    称： app_comm_mtr
 * 功    能： 调用油尺通讯处理函数。
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
void    app_comm_mtr(void)
{
    //判断对应地址连接是否接收到数据
    static  uint8   sendtimes = 0;
    uint8 i = 0;
    while(i < COMM_DEV_MTR_CONN_NUM){
        if(sCtrl.Mtr.ConnCtrl[i].RecvEndFlg ==  1 )   //如果地址i接收到数据，对数据进行处理
        {
            /***********************************************
            * 描述： 从接收的revcbuf中取数据
            */
            
            if(sCtrl.Mtr.RxCtrl.SourceAddr == SLAVE_ADDR_DIP1)    //测量装置1，
            {
                //高度
                sCtrl.sRec.Myhig1   =   sCtrl.Mtr.Rd.sRecv.Hight/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Hight %256)*256;   
                //密度值
                sCtrl.sRec.MyDip1Den=    sCtrl.Mtr.Rd.sRecv.Density/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Density %256)*256; 
                //
                
                //压力值
                sCtrl.sRec.SlvDip1Prs=    sCtrl.Mtr.Rd.sRecv.Pressure[0]/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Pressure[0] %256)*256; 
                //
                
            }
            
            if(sCtrl.Mtr.RxCtrl.SourceAddr == SLAVE_ADDR_DIP2)    //测量装置2，
            {   //高度值    
                sCtrl.sRec.Myhig2   =   sCtrl.Mtr.Rd.sRecv.Hight/256 
                                        +   (sCtrl.Mtr.Rd.sRecv.Hight %256)*256;      
                //密度
                sCtrl.sRec.MyDip2Den=   sCtrl.Mtr.Rd.sRecv.Density/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Density %256)*256;
                
                                //压力值
                sCtrl.sRec.SlvDip2Prs=    sCtrl.Mtr.Rd.sRecv.Pressure[0]/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Pressure[0] %256)*256; 
                //
            }
            
            sCtrl.Mtr.ConnCtrl[i].RecvEndFlg = 0;                   
            sCtrl.Mtr.ConnCtrl[i].TimeOut    = 0; 
            
            return  ;
        }
        
        i++;
    }
 
    /***********************************************
    * 描述： 如果是超时。
    *1. 超时时，所有地址计算加1，对应计算器在收到数据时清零。
    *2. 发送下个地址的数据。
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_MTR_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_MTR_CONN_NUM){
            sCtrl.Mtr.ConnCtrl[i].TimeOut++;
            //超时数大于10次，5S，报通讯错误。
            if(sCtrl.Mtr.ConnCtrl[i].TimeOut > 10 * COMM_DEV_MTR_CONN_NUM){
                
                sCtrl.Mtr.ConnCtrl[i].TimeOut = 10 * COMM_DEV_MTR_CONN_NUM + 1;
                sCtrl.Mtr.ConnCtrl[i].ErrFlg = 1;       
                
                //数据清零
                if(sCtrl.Mtr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DIP1)  
                    sCtrl.sRec.Myhig1 = 0;

                if(sCtrl.Mtr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DIP2)  
                    sCtrl.sRec.Myhig2 = 0;                
            }else{
                //接收到数据，通讯清零
                sCtrl.Mtr.ConnCtrl[i].ErrFlg = 0;                       
            }
            
            i++;
        }
        
        /***********************************************
        * 描述： 如果是超时。
        * 发送下个地址的数据。准备数据，发送下一个数据。
        */ 
        //前4位是油量，高低位掉头
        sCtrl.Mtr.Wr.Buf[0] = sCtrl.sRec.MstDip>>24;
        sCtrl.Mtr.Wr.Buf[1] = sCtrl.sRec.MstDip>>16;
        sCtrl.Mtr.Wr.Buf[2] = sCtrl.sRec.MstDip>>8;
        sCtrl.Mtr.Wr.Buf[3] = sCtrl.sRec.MstDip>>0;

        sCtrl.Mtr.Wr.StrSend.Speed = 0;
        sCtrl.Mtr.Wr.StrSend.Accelerations = 0;
        sCtrl.Mtr.Wr.StrSend.Density = 0;
        //sCtrl.Mtr.Wr.StrSend.Work = 0;
        //sCtrl.Mtr.Wr.StrSend.Rsv = 0;
        
        
                                //需增加串口是否占用判断。    查询标示将会 影响通讯效率（等待调度），采用信号量 （接收开始时（接收计数器为0）
                                //，占用串口；在接收超时时，释放信号）
        sendtimes++;            //发送次数，用于切换地址
        //sendtimes = 0;
        CSNC_SendData(sCtrl.Mtr.pch,                                                        //MTR 的PCH：串口号，收发控制等底层信息
                      sCtrl.Mtr.ConnCtrl[sendtimes % COMM_DEV_MTR_CONN_NUM].MasterAddr,     //源地址，
                      sCtrl.Mtr.ConnCtrl[sendtimes % COMM_DEV_MTR_CONN_NUM].SlaveAddr,      //目标地址
                      sendtimes/COMM_DEV_MTR_CONN_NUM,                                      //帧序号
                      0,                                                                    //命令字
                      (uint8 *)&sCtrl.Mtr.Wr.StrSend,                                       //数据区
                      sizeof(sCtrl.Mtr.Wr.StrSend)                                         //发送长度
                      );
        return;
    }
}


/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif