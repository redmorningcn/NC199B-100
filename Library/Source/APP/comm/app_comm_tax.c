/*******************************************************************************
 *   Filename:       app_comm_tax.c
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmorningcn.
 *
 *   Description:    ˫��ѡ�� tax �� Ctrl + H, ��ѡ Match the case, Replace with
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

/*******************************************************************************/


/*******************************************************************************
 * ��    �ƣ�  app_comm_tax
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
void    app_comm_tax(void)
{
//    //TAX���ݽ��գ�ֱ����HANDLE��ʵ���ˡ�
//    uint8 i = 0;
//    while(i < COMM_DEV_OTR_CONN_NUM){
//        if(sCtrl.Otr.ConnCtrl[i].RecvEndFlg == 1)                   //�����ַi���յ����ݣ������ݽ��д���
//        {
//            /***********************************************
//            * ������ �ӽ��յ�revcbuf��ȡ����
//            *        ���ݽ��յ��ĵ�ַCA����������  ���ݴ��䣻
//            *        ���յ���ַC2������  ����ģʽ��
//            */
//            
//            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_OTR)       //��ַ����C1��IC���ݴ��䡣
//            {
////                comm_record_send((StrDevOtr *)&sCtrl.Otr.RxCtrl,i); //���ݵ���Ҫ����  tax����ȷ ��ַ��pch����Ϣ��
//                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);   //���ò���������Ϣ����������code������Ӧ������
//            }
//            
//            if(sCtrl.Otr.RxCtrl.SourceAddr == SLAVE_ADDR_SET)//��ַ����C2���������á�
//            {
//                comm_para_flow((StrDevDtu *)&sCtrl.Otr,i);  //���ò���������Ϣ����������code������Ӧ������
//            }
//            
//            sCtrl.Otr.ConnCtrl[i].RecvEndFlg = 0;           //�����Ѵ��� ����������1
//            sCtrl.Otr.ConnCtrl[i].TimeOut    = 0;           //��ʱ��������
//            
//            return  ;
//        }
//        i++;
//    }
// 
    /***********************************************
    *   ������ ����ǳ�ʱ��
    *1. ��ʱʱ�����е�ַ�����1����Ӧ���������յ�����ʱ���㡣
    *2. 10������Ӧ�𣬷���һ�����ݡ�
    */
    if( sCtrl.Os.CommEvtFlag |  COMM_EVT_FLAG_TAX_TIMEOUT){
        uint8 i = 0;
        while( i < COMM_DEV_TAX_CONN_NUM){
            sCtrl.DevTax.ConnCtrl[i].TimeOut++;

            if(sCtrl.DevTax.ConnCtrl[i].TimeOut > 10 * COMM_DEV_TAX_CONN_NUM){
                sCtrl.DevTax.ConnCtrl[i].TimeOut = 10 * COMM_DEV_TAX_CONN_NUM + 1;
                sCtrl.DevTax.ConnCtrl[i].ErrFlg = 1;               //��ʱ������10�Σ�5S����ͨѶ����
                
            }else{
                sCtrl.DevTax.ConnCtrl[i].ErrFlg = 0;               //���յ����ݣ�ͨѶ����
            }
            
            i++;
        }
        
        //��ʱ��������tax�����
        TAX_StartRecv((MODBUS_CH  *)sCtrl.DevTax.pch);
    }
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif


