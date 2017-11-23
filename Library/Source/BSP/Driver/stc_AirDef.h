/*******************************************************************************
 *   Revised:        All copyrights reserved to redmorningcn.
 *   Revision:       v1.0
 *   Writer:	     redmoringcn.
 *   Description:    
 *   Notes:
 *     				 E-mail: redmorningcn@qq.com
 *
 *******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _STC_AIR_DEF_H
#define _STC_AIR_DEF_H

/*******************************************************************************
 * INCLUDES
 */
#include <app_tax.h>

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
    
/*******************************************************************************
 * TYPEDEFS
 */

//2017-11-16  redmorningcn
//ѹ����������װ�ã�������������  
__packed
typedef struct _sAirQuality{
    uint16				Temperature       ;//�¶�                                 2                    
    uint16				Humidity          ;//ʪ��                                 2                    
    uint16				VOCs              ;//�ӷ��л���                           2                    
    uint16				CO2_EQU           ;//��ֵCO2                              2  
    CPU_INT32U          RS                ;// RS
    uint16				PM1D0_S           ;//��׼������Ũ��ug/m3                  2                    
    uint16				PM2D5_S           ;//��׼������Ũ��ug/m3                  2                    
    uint16				PM10D_S           ;//��׼������Ũ��ug/m3                  2    
    uint16				PM1D0_G           ;// ��������                              
    uint16				PM2D5_G           ;// ��������                    
    uint16				PM10D_G           ;// �������� 
    uint16              Dusts_0D3         ;//0.1��������ֱ����0.3um�Ŀ��������
    uint16              Dusts_0D5         ;//0.1��������ֱ����0.5um�Ŀ��������
    uint16              Dusts_1D0         ;//0.1��������ֱ����1.0um�Ŀ��������
    uint16              Dusts_2D5         ;//0.1��������ֱ����2.5um�Ŀ��������    
} stcAirQuality;      

//2017-11-16  redmorningcn
//ѹ����������װ�ã���������ݽṹ     
typedef __packed struct _stcFlshRec     //�������ݼ�¼���ݽṹ��20171116 ����ѹ��������
{
    uint8				CmdTpye		      ;//��������       1     		        	
    uint8				EvtType           ;//�¼�����       1                  
    uint16				LocoType          ;//�����ͺ�       2                  
    uint16				LocoNum           ;//�� �� ��       2                     
    uint32				RecID             ;//��¼��ˮ��     4
    
    stcAirQuality       sAir              ;//��������       28
    uint8               RunSys            ;//����״̬       1�������Ŵ򿪣���ʼ���ԣ��ȵȣ�      
    uint8				Recv[8]          ;//Ԥ��                               
    uint16				ErrCode           ;//���ϴ���       2                  
    uint8				Air_Hum_Grade     ;//ʪ�ȵȼ�       1                  
    uint8				Air_Voc_Grade     ;//VOC�ȼ�        1                   
    uint8               Air_Dust_Grade    ;//�۳��ȼ�       1    
    struct _sSysTime {
        uint32      Sec                 : 6;        // D05~D00����
        uint32      Min                 : 6;        // D11~D06����
        uint32      Hour                : 5;        // D16~D12��ʱ
        uint32      Day                 : 5;        // D21~D17����
        uint32      Mon                 : 4;        // D25~D22����
        uint32      Year                : 6;        // D31~D26����
    } SysTime;                                          // 04 ʱ��
    
////////////////////////////////////////////////// TAX����������
    uint8           TaxType             ;//TAX������       1                     
    uint8           TaxFlg              ;//TAX������       64       
    /***************************************************
    * ������ TAX:64 bytes
    */
    union __uTAX__Rec {
    _StrTax_II_Rec      Tax2;                           // ��¼����     
    struct __sTAX_III__Rec {
    StrTaxVariable      Vari;
    StrTaxConstant      Cons;
    } Tax3;
    
    uint8_t             buf1[64];
    uint16_t            buf2[64/2];
    uint32_t            buf3[64/4];
    
    } Tax;    
    
    uint16				CrcCheck                        ;//У��   2                 
}stcFlshRec;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
 
/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *
 * ��������         ��������                ��ڲ���
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

#endif 


