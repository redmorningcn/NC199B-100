/*******************************************************************************
 *   Filename:       app_comm_otr.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Otr �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� OTR �� Ctrl + H, ��ѡ Match the case, Replace with
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

#define		DATA_CARD           0x0001			//���ݿ�-----��ʱδ��
#define		DENSITY_CARD		0x0020			//�ܶȿ�
#define		MODEL_CARD			0x0040			//ģ�Ϳ�-----��ʱδ��
#define		TIME_CARD           0x0100			//ʱ�俨-----
#define		COM_MODEL_CARD	    0x0100		    //����ģ�Ϳ�-��ʱδ��
#define		HIGHT_CARD			0x0200			//�߶ȿ�-----
#define		COPY_CARD           0x0400			//���ƿ�-----��ʱδ��
#define		MODEL_SELECT_CARD	0x1000			//ģ��ѡ��-
#define		FIX_CARD            0x2000			//װ����-----
#define		COPY_FRAM_CARD	    0x4000			//���縴�ƿ�-��ʱδ��
#define		SET_DATA_CARD		0x0080			//������-----��ʱδ��
#define		DEBUG_CARD			0x0800			//���Կ�-----��ʱδ��

#define		EXPAND_CARD			0x8000			//��չ��-----��ʱδ��

#define		DATA_CARD_DIS		0x10000			//���ݿ�ָʾ
#define		DATA_CARD_FIN		0x20000			//���ݿ�����
#define		SYS_RUN_PARA		0x40000			//ϵͳ������--��δʹ��
#define		CLEAR_ALL			0x80000			//���㿨�����ݼ�¼����

#define		RST_SYS				0x100000		//��λָ��
#define		CALC_PARA           0x200000		//����б��

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
 * ��    �ƣ�  app_comm_otr
 * ��    �ܣ� ����IC��ͨѶ��������
 *            ����Ƕ�ʱ����ʱ�����Ӧ��ַ��timeout�����1�����ж�
 *            ֵ�Ƿ�����õ�ַ�����жϴ������Ƿ�ִ��ͨѶ�쳣����
 *            ����ǽ��յ������¼�������ݽ������ݽ��д���
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ�redmorningcn.
 * �������ڣ� 2017-05-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void    app_comm_otr(void)
{
    //�ж϶�Ӧ��ַ�����Ƿ���յ�����
    uint8 i = 0;
    while(i < COMM_DEV_OTR_CONN_NUM){
        if(sCtrl.Otr.ConnCtrl[i].RecvEndFlg == 1)                   //�����ַi���յ����ݣ������ݽ��д���
        {
            /***********************************************
            * ������ �ӽ��յ�revcbuf��ȡ����
            *        ���ݽ��յ��ĵ�ַCA����������  ���ݴ��䣻
            *        ���յ���ַC2������  ����ģʽ��
            */
            
            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_OTR)       //��ַ����C1��IC���ݴ��䡣
            {
//                comm_record_send((StrDevOtr *)&sCtrl.Otr.RxCtrl,i); //���ݵ���Ҫ����  otr����ȷ ��ַ��pch����Ϣ��
                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);   //���ò���������Ϣ����������code������Ӧ������
            }
            
            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_SET)//��ַ����C2���������á�
            {
                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);  //���ò���������Ϣ����������code������Ӧ������
            }
            
            sCtrl.Otr.ConnCtrl[i].RecvEndFlg = 0;           //�����Ѵ��� ����������1
            sCtrl.Otr.ConnCtrl[i].TimeOut    = 0;           //��ʱ��������
            
            return  ;
        }
        i++;
    }
 
    /***********************************************
    *   ������ ����ǳ�ʱ��
    *1. ��ʱʱ�����е�ַ�����1����Ӧ���������յ�����ʱ���㡣
    *2. 10������Ӧ�𣬷���һ�����ݡ�
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_OTR_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_OTR_CONN_NUM){
            sCtrl.Otr.ConnCtrl[i].TimeOut++;

            if(sCtrl.Otr.ConnCtrl[i].TimeOut > 10 * COMM_DEV_OTR_CONN_NUM){
                sCtrl.Otr.ConnCtrl[i].TimeOut = 10 * COMM_DEV_OTR_CONN_NUM + 1;
                sCtrl.Otr.ConnCtrl[i].ErrFlg = 1;               //��ʱ������10�Σ�5S����ͨѶ����
                
            }else{
                sCtrl.Otr.ConnCtrl[i].ErrFlg = 0;               //���յ����ݣ�ͨѶ����
            }
            
            i++;
        }
        
        /***********************************************
        * ������ ����ǳ�ʱ��
        * ��������
        */ 
        if( sCtrl.sRecNumMgr.Current == 0 )                     //��¼��Ϊ0����δ�������ݡ�����ͨѶ״̬�ж�
        {
            uint8 i = 0;
            while( i < COMM_DEV_OTR_CONN_NUM){
                sCtrl.Otr.ConnCtrl[i].TimeOut = 0;
                i++;
            }
            return;
        }
                
        //����1�����ݼ�¼
        i =0 ;
        while(i < COMM_DEV_OTR_CONN_NUM)
            if(sCtrl.Otr.ConnCtrl[i].SlaveAddr == SLAVE_ADDR_OTR){
        //��OTR��ַ�������� 
                comm_tinyrec_send((StrDevOtr *)&sCtrl.Otr,i);
                //comm_record_send_one((StrDevOtr *)&sCtrl.Otr.RxCtrl,i);  
                break;
            }
           i++;
        
    }
}


/*******************************************************************************
 * ��    �ƣ� comm_tinyrec_send
 * ��    �ܣ� ���ͼ�¼�еıȽϹؼ���Ϣ
 * ��ڲ����� ͨѶ�豸���ݽṹ
 * ���ڲ����� ��
 * ���� ���ߣ� redmorningcn.
 * �������ڣ� 2017-05-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
uint8    comm_tinyrec_send(StrDevOtr * sOtr,uint8    addrnum)
{
    //if(sOtr->pch->Ch == 2)      //����ͨ��
    {
        //��¼��Ϊ0����δ�������ݡ�����ͨѶ״̬�ж�
        if( sCtrl.sRecNumMgr.Current == 0 )                                   
        {
            return 0;
        }
        /***********************************************
        * ������ ȡֵ
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
       
        //��ʱֵ����SLAVE_ADDR_OTR ������
        CSNC_SendData(sCtrl.Otr.pch,                                //OTR ��PCH�����ںţ��շ����Ƶȵײ���Ϣ
                      sOtr->ConnCtrl[addrnum].MasterAddr,           //Դ��ַ��
                      sOtr->ConnCtrl[addrnum].SlaveAddr,            //Ŀ���ַ
                      sOtr->ConnCtrl[addrnum].SendFramNum,          //֡��� �����ڽ������ۼ�
                      0,                                            //������
                      (uint8 *)&sCtrl.Otr.Wr.sTinyRec,              //������
                      sizeof(sCtrl.Otr.Wr.sTinyRec)                 //���ͳ���
                      );
    }
    return 1;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif


