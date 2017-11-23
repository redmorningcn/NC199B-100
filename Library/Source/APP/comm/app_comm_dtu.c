/*******************************************************************************
 *   Filename:       app_comm_dtu.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� dtu �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Dtu �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� DTU �� Ctrl + H, ��ѡ Match the case, Replace with
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
//IAP ����֡������
#define     IAP_FRAME_CODE          10 
//SET ���ÿ�����
#define     SET_FRAME_CODE          2      


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
 * ��    �ƣ� app_comm_dtu
 * ��    �ܣ� ��������ͨѶ��������
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
void    app_comm_dtu(void)
{
    //�ж϶�Ӧ��ַ�����Ƿ���յ�����
    uint8   i = 0;
    //uint8   status = 0;
    uint8   echolen = 0;

    while(i < COMM_DEV_DTU_CONN_NUM){
        if(sCtrl.Dtu.ConnCtrl[i].RecvEndFlg == 1)                   //�����ַi���յ����ݣ������ݽ��д���
        {
            /***********************************************
            * ������ �ӽ��յ�revcbuf��ȡ����
            *        ���ݽ��յ��ĵ�ַCA����������  ���ݴ��䣻
            *        ���յ���ַC2������  ����ģʽ��
            */
            switch(sCtrl.Dtu.RxCtrl.FrameCode)
            {
                case 0:     //V1.0
            
                    if(sCtrl.Dtu.RxCtrl.SourceAddr == SLAVE_ADDR_DTU)       //��ַ����CA���������ݴ��䡣
                    {
                        comm_record_send((StrDevDtu *)&sCtrl.Dtu,i);        //���ݵ���Ҫ����  dtu����ȷ ��ַ��pch����Ϣ��
                    }
                    
                    if(sCtrl.Dtu.RxCtrl.SourceAddr == SLAVE_ADDR_SET)       //��ַ����C2���������á�
                    {
                        comm_para_flow((StrDevDtu *)&sCtrl.Dtu,i);          //���ò���������Ϣ����������code������Ӧ������
                        
                        //�������ã�����ԭ·����
                        CSNC_SendData(  sCtrl.Dtu.pch,                        //DTU ��PCH�����ںţ��շ����Ƶȵײ���Ϣ
                                        sCtrl.Dtu.RxCtrl.DestAddr,            //Դ��ַ��
                                        sCtrl.Dtu.RxCtrl.SourceAddr,          //Ŀ���ַ
                                        sCtrl.Dtu.RxCtrl.FramNum,             //֡��� 
                                        sCtrl.Dtu.RxCtrl.FrameCode,           //������
                                        sCtrl.Dtu.Rd.Buf,                     //������
                                        sCtrl.Dtu.RxCtrl.Len                  //���ͳ���
                                      );
                    }
                    break;
                case 1:     //V2.0
                    break;
                case SET_FRAME_CODE:        //V2.0 IAP��������
                    
                    break;
                    
                case IAP_FRAME_CODE:        //V2.0 IAP��������
                        //status = IAP_PragramDeal(sCtrl.Dtu.Rd.Buf , sCtrl.Dtu.RxCtrl.Len ); //���ó������غ���   
//                        IAP_PragramDeal(sCtrl.Dtu.Rd.Buf , sCtrl.Dtu.RxCtrl.Len ); //���ó������غ���  
//                        
//                        echolen = 4;
//                        //Ӧ������          //�����IAP����֡�������ݳ���4������ǿ�ʼ�����������س���Ϊ
//                        if(     sCtrl.Dtu.Rd.Buf[0] == IAP_START_CODE 
//                            ||  sCtrl.Dtu.Rd.Buf[0] == IAP_END_CODE )
//                        {
//                            echolen = sCtrl.Dtu.RxCtrl.Len;   
//                        }
//                        
//                        CSNC_SendData(  sCtrl.Dtu.pch,                        //DTU ��PCH�����ںţ��շ����Ƶȵײ���Ϣ
//                                        sCtrl.Dtu.RxCtrl.DestAddr,            //Դ��ַ��
//                                        sCtrl.Dtu.RxCtrl.SourceAddr,          //Ŀ���ַ
//                                        sCtrl.Dtu.RxCtrl.FramNum,             //֡��� ���ڽ������ۼ�
//                                        sCtrl.Dtu.RxCtrl.FrameCode,           //������
//                                        sCtrl.Dtu.Rd.Buf,                     //������
//                                        echolen                               //���ͳ���
//                                    );
                    break;
                    
                default:;  //V2.0
            }
            sCtrl.Dtu.ConnCtrl[i].RecvEndFlg = 0;                   //�����Ѵ��� ����������1
            sCtrl.Dtu.ConnCtrl[i].TimeOut    = 0;                   //��ʱ��������

            return  ;
        }
        
        i++;
    }
 
    /***********************************************
    *   ������ ����ǳ�ʱ��
    *1. ��ʱʱ�����е�ַ�����1����Ӧ���������յ�����ʱ���㡣
    *2. 10������Ӧ�𣬷���һ�����ݡ�
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_DTU_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_DTU_CONN_NUM){
            sCtrl.Dtu.ConnCtrl[i].TimeOut++;
            
            if(sCtrl.Dtu.ConnCtrl[i].TimeOut > 10 * COMM_DEV_DTU_CONN_NUM){
                sCtrl.Dtu.ConnCtrl[i].TimeOut = 10 * COMM_DEV_DTU_CONN_NUM + 1;
                sCtrl.Dtu.ConnCtrl[i].ErrFlg = 1;               //��ʱ������10�Σ�5S����ͨѶ����
                
            }else{
                sCtrl.Dtu.ConnCtrl[i].ErrFlg = 0;               //���յ����ݣ�ͨѶ����
            }
            i++;
        }
        
        /***********************************************
        * ������ ����ǳ�ʱ��
        * ��������
        */ 
        if( sCtrl.sRecNumMgr.Current == 0 )                                     //��¼��Ϊ0����δ�������ݡ�����ͨѶ״̬�ж�
        {
            uint8 i = 0;
            while( i < COMM_DEV_DTU_CONN_NUM){
                sCtrl.Dtu.ConnCtrl[i].TimeOut = 0;
                i++;
            }
            return;
        }
                
        //����1�����ݼ�¼
        i =0 ;
        while(i < COMM_DEV_DTU_CONN_NUM){
            if(sCtrl.Dtu.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_DTU){              //��DTU��ַ�������� 
                                                                                    
                if(     sCtrl.sRecNumMgr.Current > sCtrl.sRecNumMgr.GrsRead      //����δ���꣬10ÿ�Σ������������ݲŷ���
                   ||   sCtrl.sRecNumMgr.GrsRead >  sCtrl.sRecNumMgr.Current   ) //��¼�쳣��Ҳ����
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


