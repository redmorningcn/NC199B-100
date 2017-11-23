
/*******************************************************************************
 *   Filename:       app_comm_mtr.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� comm �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Comm �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� COMM �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_COMM_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_COMM_STK_SIZE ����С
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
* ������ ��ʱ������
*/

/***********************************************
* ������ ��ʱ���ص�����
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
 * ��    �ƣ� app_comm_mtr
 * ��    �ܣ� �����ͳ�ͨѶ��������
 *            ����Ƕ�ʱ����ʱ�����Ӧ��ַ��timeout�����1�����ж�
 *            ֵ�Ƿ�����õ�ַ�����жϴ������Ƿ�ִ��ͨѶ�쳣����
 *            ����ǽ��յ������¼�������ݽ������ݽ��д���
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� redmorningcn.
 * �������ڣ� 2017-05-18
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void    app_comm_mtr(void)
{
    //�ж϶�Ӧ��ַ�����Ƿ���յ�����
    static  uint8   sendtimes = 0;
    uint8 i = 0;
    while(i < COMM_DEV_MTR_CONN_NUM){
        if(sCtrl.Mtr.ConnCtrl[i].RecvEndFlg ==  1 )   //�����ַi���յ����ݣ������ݽ��д���
        {
            /***********************************************
            * ������ �ӽ��յ�revcbuf��ȡ����
            */
            
            if(sCtrl.Mtr.RxCtrl.SourceAddr == SLAVE_ADDR_DIP1)    //����װ��1��
            {
                //�߶�
                sCtrl.sRec.Myhig1   =   sCtrl.Mtr.Rd.sRecv.Hight/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Hight %256)*256;   
                //�ܶ�ֵ
                sCtrl.sRec.MyDip1Den=    sCtrl.Mtr.Rd.sRecv.Density/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Density %256)*256; 
                //
                
                //ѹ��ֵ
                sCtrl.sRec.SlvDip1Prs=    sCtrl.Mtr.Rd.sRecv.Pressure[0]/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Pressure[0] %256)*256; 
                //
                
            }
            
            if(sCtrl.Mtr.RxCtrl.SourceAddr == SLAVE_ADDR_DIP2)    //����װ��2��
            {   //�߶�ֵ    
                sCtrl.sRec.Myhig2   =   sCtrl.Mtr.Rd.sRecv.Hight/256 
                                        +   (sCtrl.Mtr.Rd.sRecv.Hight %256)*256;      
                //�ܶ�
                sCtrl.sRec.MyDip2Den=   sCtrl.Mtr.Rd.sRecv.Density/256 
                                    +   (sCtrl.Mtr.Rd.sRecv.Density %256)*256;
                
                                //ѹ��ֵ
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
    * ������ ����ǳ�ʱ��
    *1. ��ʱʱ�����е�ַ�����1����Ӧ���������յ�����ʱ���㡣
    *2. �����¸���ַ�����ݡ�
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_MTR_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_MTR_CONN_NUM){
            sCtrl.Mtr.ConnCtrl[i].TimeOut++;
            //��ʱ������10�Σ�5S����ͨѶ����
            if(sCtrl.Mtr.ConnCtrl[i].TimeOut > 10 * COMM_DEV_MTR_CONN_NUM){
                
                sCtrl.Mtr.ConnCtrl[i].TimeOut = 10 * COMM_DEV_MTR_CONN_NUM + 1;
                sCtrl.Mtr.ConnCtrl[i].ErrFlg = 1;       
                
                //��������
                if(sCtrl.Mtr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DIP1)  
                    sCtrl.sRec.Myhig1 = 0;

                if(sCtrl.Mtr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DIP2)  
                    sCtrl.sRec.Myhig2 = 0;                
            }else{
                //���յ����ݣ�ͨѶ����
                sCtrl.Mtr.ConnCtrl[i].ErrFlg = 0;                       
            }
            
            i++;
        }
        
        /***********************************************
        * ������ ����ǳ�ʱ��
        * �����¸���ַ�����ݡ�׼�����ݣ�������һ�����ݡ�
        */ 
        //ǰ4λ���������ߵ�λ��ͷ
        sCtrl.Mtr.Wr.Buf[0] = sCtrl.sRec.MstDip>>24;
        sCtrl.Mtr.Wr.Buf[1] = sCtrl.sRec.MstDip>>16;
        sCtrl.Mtr.Wr.Buf[2] = sCtrl.sRec.MstDip>>8;
        sCtrl.Mtr.Wr.Buf[3] = sCtrl.sRec.MstDip>>0;

        sCtrl.Mtr.Wr.StrSend.Speed = 0;
        sCtrl.Mtr.Wr.StrSend.Accelerations = 0;
        sCtrl.Mtr.Wr.StrSend.Density = 0;
        //sCtrl.Mtr.Wr.StrSend.Work = 0;
        //sCtrl.Mtr.Wr.StrSend.Rsv = 0;
        
        
                                //�����Ӵ����Ƿ�ռ���жϡ�    ��ѯ��ʾ���� Ӱ��ͨѶЧ�ʣ��ȴ����ȣ��������ź��� �����տ�ʼʱ�����ռ�����Ϊ0��
                                //��ռ�ô��ڣ��ڽ��ճ�ʱʱ���ͷ��źţ�
        sendtimes++;            //���ʹ����������л���ַ
        //sendtimes = 0;
        CSNC_SendData(sCtrl.Mtr.pch,                                                        //MTR ��PCH�����ںţ��շ����Ƶȵײ���Ϣ
                      sCtrl.Mtr.ConnCtrl[sendtimes % COMM_DEV_MTR_CONN_NUM].MasterAddr,     //Դ��ַ��
                      sCtrl.Mtr.ConnCtrl[sendtimes % COMM_DEV_MTR_CONN_NUM].SlaveAddr,      //Ŀ���ַ
                      sendtimes/COMM_DEV_MTR_CONN_NUM,                                      //֡���
                      0,                                                                    //������
                      (uint8 *)&sCtrl.Mtr.Wr.StrSend,                                       //������
                      sizeof(sCtrl.Mtr.Wr.StrSend)                                         //���ͳ���
                      );
        return;
    }
}


/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif