/*******************************************************************************
 *   Filename:       app_comm_tax.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    双击选中 tax 按 Ctrl + H, 钩选 Match the case, Replace with
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
const  CPU_CHAR  *app_comm_tax__c = "$Id: $";
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

/*******************************************************************************/


/*******************************************************************************
 * 名    称：  app_comm_tax
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
void    app_comm_tax(void)
{
//    //TAX数据接收，直接在HANDLE中实现了。
//    uint8 i = 0;
//    while(i < COMM_DEV_OTR_CONN_NUM){
//        if(sCtrl.Otr.ConnCtrl[i].RecvEndFlg == 1)                   //如果地址i接收到数据，对数据进行处理
//        {
//            /***********************************************
//            * 描述： 从接收的revcbuf中取数据
//            *        根据接收到的地址CA，进行无线  数据传输；
//            *        接收到地址C2，进行  设置模式。
//            */
//            
//            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_OTR)       //地址等于C1，IC数据传输。
//            {
////                comm_record_send((StrDevOtr *)&sCtrl.Otr.RxCtrl,i); //数据的需要传递  tax，明确 地址，pch等信息。
//                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);   //设置参数，或信息交互。依据code进行相应操作。
//            }
//            
//            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_SET)//地址等于C2，参数设置。
//            {
//                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);  //设置参数，或信息交互。依据code进行相应操作。
//            }
//            
//            sCtrl.Otr.ConnCtrl[i].RecvEndFlg = 0;           //数据已处理。 接收数据置1
//            sCtrl.Otr.ConnCtrl[i].TimeOut    = 0;           //超时计数清零
//            
//            return  ;
//        }
//        i++;
//    }
// 
    /***********************************************
    *   描述： 如果是超时。
    *1. 超时时，所有地址计算加1，对应计算器在收到数据时清零。
    *2. 10数据无应答，发送一次数据。
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_TAX_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_TAX_CONN_NUM){
            sCtrl.DevTax.ConnCtrl[i].TimeOut++;

            if(sCtrl.DevTax.ConnCtrl[i].TimeOut > 10 * COMM_DEV_TAX_CONN_NUM){
                sCtrl.DevTax.ConnCtrl[i].TimeOut = 10 * COMM_DEV_TAX_CONN_NUM + 1;
                sCtrl.DevTax.ConnCtrl[i].ErrFlg = 1;               //超时数大于10次，5S，报通讯错误。
                
            }else{
                sCtrl.DevTax.ConnCtrl[i].ErrFlg = 0;               //接收到数据，通讯清零
            }
            
            i++;
        }
        
        //超时重新启动tax箱接收
        TAX_StartRecv((MODBUS_CH  *)sCtrl.DevTax.pch);
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif


