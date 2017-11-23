#ifndef	APP_TAX_H
#define	APP_TAX_H
/*******************************************************************************
 *   Filename:       app_tax.h
 *   Revised:        $Date: 2017-07-26	20:15 (Fri) $
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.  �ִ����������Ǳ��о���
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <global.h>
#include <os.h>
#include <osal.h>
#include <mb.h>
     
#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
/*******************************************************************************
 * CONSTANTS
 */
 /***************************************************
* ������ 
*/
#ifndef TAX_EN
#define TAX_EN                              DEF_ENABLED
#if defined(TAX_EN)
#define TAX_II_EN                           DEF_ENABLED
#define TAX_III_EN                          DEF_ENABLED
#endif
#endif

/*******************************************************************************
 * TYPEDEFS
 */
#if TAX_II_EN == DEF_ENABLED

__packed
typedef struct _StrTAX_II_ {    
    CPU_INT08U          CheCiZhongLei[4];               // ��������
        
    CPU_INT08U          SiJiHaoKuoChong;                // 02 ˾��������
    CPU_INT08U          FuSiJiHaoKuoChong;              // 02 ��˾��������
    CPU_INT16U          Rsv3;
    CPU_INT08U          JiCheXingHao;                   // 01 �����ͺ�
    CPU_INT08U          ShiJiJiaoLuHao;                 // ʵ�ʽ�·��
    
    CPU_INT16U          ZaYa;                           // բѹ��Ԥ����
    CPU_INT08U          Rsv4[9];
    CPU_INT32U          BenBuKeHuo          : 8;        // 01 �����ͻ�
    CPU_INT32U          CheCiShuZi          :24;        // 02 ����
    //CPU_INT32U          Rsv5;
    CPU_INT08U          SumChk1;                        // ����һ
    
    //39֡
    CPU_INT08U          Addr;                           // �����ַ
    CPU_INT08U          TeZhengMa;                      // ������
    CPU_INT08U          DanYuanDaiHao;                  // ��ⵥԪ����
    /***************************************************
    * ������ �������ݣ���ʼ��ַ = 004
    */
    struct _sLKJTime {
        CPU_INT32U      Sec                 : 6;        // D05~D00����
        CPU_INT32U      Min                 : 6;        // D11~D06����
        CPU_INT32U      Hour                : 5;        // D16~D12��ʱ
        CPU_INT32U      Day                 : 5;        // D21~D17����
        CPU_INT32U      Mon                 : 4;        // D25~D22����
        CPU_INT32U      Year                : 6;        // D31~D26����
    } LKJTime;                                          // 04 LKJʱ��

    CPU_INT32U          SuDu                :10;        // D08~D00���ٶ�ֵ
    CPU_INT32U          Rsv6                :14;        // D15~D09��Ԥ��
    CPU_INT32U          JiCheXinHao         : 8;        // 01 �����ź�
    
    CPU_INT08U          LingWei             : 1;        // D0��=1:��λ��=0:�ޣ�
    CPU_INT08U          XiangHou            : 1;        // D1��=1:���=0:�ޣ�
    CPU_INT08U          XiangQian           : 1;        // D2��=1:��ǰ��=0:�ޣ�
    CPU_INT08U          ZhiDong             : 1;        // D3��=1:�ƶ���=0:�ޣ�
    CPU_INT08U          QianYin             : 1;        // D4��=1:ǣ����=0:�ޣ�
    CPU_INT08U          Rsv7                : 3;        // D7~D5��Ԥ�� 

    CPU_INT16U          XinHaoJiBianHao;                // 02 �źŻ����
    
    CPU_INT32U          XinHaoJiZhongLei    : 8;        // D31~24���źŻ�����           
    CPU_INT32U          GongLiBiao          :24;        // D23~D0�������
    
    CPU_INT16U          ZongZhong;                      // 02 ����
    CPU_INT16U          JiChang;                        // 02 �Ƴ�
    CPU_INT08U          LiangShu;                       // 01 ����    
    CPU_INT08U          BenBuKeHuo2;                    // 01 ��/�����ͻ�
    
    CPU_INT16U          CheCi;                          // ����
    
    CPU_INT08U          QuDuanHao;                      // ���κ�
    CPU_INT08U          CheZhanHao;                     // ��վ��
    CPU_INT16U          SiJiHao;                        // ˾����
    CPU_INT16U          FuSiJiHao;                      // ��˾����
    /***************************************************
    * ������ ��������ʼ��ַ = 039
    */
    CPU_INT16U          JiCheHao;                       // 02 ������
    CPU_INT08U          JiCheXingHaoDi;                 // �����ͺŵ�
    CPU_INT16U          LieCheGuanYa        :10;        // 02 �г���ѹ��
    CPU_INT16U          LieCheGuanYaRsv     : 6;        // 02 �г���ѹ��Ԥ��
    CPU_INT08U          SheZhiZhuangTai;                // ����״̬
} _StrTax_II_Rec;

__packed
typedef struct _StrTAX_II {

    // 38֡
    /***************************************************
    * ������ ��ͷ����ʼ��ַ = 000
    */    
    CPU_INT08U          Head;                           // 01 �����ַ  ����¼  
    CPU_INT32U          Rsv2;                           // ����¼
    CPU_INT08U          CheZhanHaoKuoChong;             // 01 ��վ������
    _StrTax_II_Rec      Record;                         // ��¼����
    CPU_INT08U          Rsv9;                           // 02 Ԥ��
    CPU_INT08U          SumChk;                         // 01 У�����ۼӺͲ��� // ����¼
} StrTAX_II;
#endif

/***************************************************
* ������ TAX III���ݽṹ����72�ֽ�
*/
#if TAX_III_EN == DEF_ENABLED
__packed
typedef struct _StrTAX_III_HEAD {
    /***************************************************
    * ������ ��ͷ����ʼ��ַ = 000
    */    
    CPU_INT08U          SheBeiLeiXing;                  // 01 TAX�豸����       0XF0:TAX2;  0XFA:TAX3/TAX07
    union __uTeZhengMa {
    struct __sTeZhengMa {
        CPU_INT08U      LkjSheBei           : 1;        // D0��LKJ�豸���ͣ�=0��LKJ2000��=1����һ��LKJ
        CPU_INT08U      TaxChangJia         : 1;        // D1��TAX���ұ�ʶ��=0��˼ά��=1������
        CPU_INT08U      Rsv                 : 1;        // D2��Ԥ��
        CPU_INT08U      TongXinFlag         : 1;        // D3��LKJͨ��״̬��=0��ͨ��������=1ͨ�Ź���
        CPU_INT08U      RecvFlag            : 4;        // D7~D4������״̬��־��=0X03�����ճɹ���=0X0C������ʧ�ܣ�=��������Ч����               
    } Udat;                                        // 01 ������
    CPU_INT08U          Dat;
    } TeZhengMa;
    CPU_INT08U          DanYuanDaiHao;                  // 01 ��Ԫ����  Ҫ��Ӧ��ĵ�Ԫ�ţ�=0X20�������䣨��ӿ�1����=0X21��LAIS����ӿ�2����=0X22��CMD/6A����ӿ�3��
    CPU_INT08U          XianYiBanBen;                   // 01 Э��汾 
} StrTaxHead;

__packed
typedef struct _StrTAX_III_VARIABLE { 
    /***************************************************
    * ������ �������ݣ���ʼ��ַ = 004
    */
    struct __sLKJTime {
        CPU_INT32U      Sec                 : 6;        // D05~D00����
        CPU_INT32U      Min                 : 6;        // D11~D06����
        CPU_INT32U      Hour                : 5;        // D16~D12��ʱ
        CPU_INT32U      Day                 : 5;        // D21~D17����
        CPU_INT32U      Mon                 : 4;        // D25~D22����
        CPU_INT32U      Year                : 6;        // D31~D26����
    } LKJTime;                                          // 04 LKJʱ��
        struct __sShiSu {
        CPU_INT16U      Speed               : 9;        // D08~D00���ٶ�ֵ
        CPU_INT16U      Rsv                 : 7;        // D15~D09��Ԥ��
    } ShiSu;                                            // 04 ʵ��
    CPU_INT08U          LianCheGuanYa;                  // 01 �г���ѹ��        10KPa
    CPU_INT16U          JiCheXinHao;                    // 02 �����ź�
    struct __sJiCheGongKuang {
        CPU_INT08U      LingWei             : 1;        // D0��=1:��λ��=0:�ޣ�
        CPU_INT08U      XiangHou            : 1;        // D1��=1:���=0:�ޣ�
        CPU_INT08U      XiangQian           : 1;        // D2��=1:��ǰ��=0:�ޣ�
        CPU_INT08U      ZhiDong             : 1;        // D3��=1:�ƶ���=0:�ޣ�
        CPU_INT08U      QianYin             : 1;        // D4��=1:ǣ����=0:�ޣ�
        CPU_INT08U      Rsv                 : 3;        // D7~D5��Ԥ��
    } JiCheGongKuang;                                   // 01 ��������
    CPU_INT08U          XingBie;                        // 01 �б�
    CPU_INT16U          XianLuHao;                      // 02 ��·��
    struct __sLiCheng {
        CPU_INT32U      LiCheng             :23;        // D22~D0�����������λ�ף�=1:��=0:�ޣ�
        CPU_INT32U      BianHuaQuShi        : 1;        // D23����̱仯���ƣ�=1����������=0�����Ƽ���
        CPU_INT32U      Rsv                 : 5;        // D28~D24��Ԥ����
        CPU_INT32U      GongLiBiaoZhi       : 1;        // D29��������־��=1:A��=0:�ޣ�
        CPU_INT32U      BaiMiBiaoZhi        : 1;        // D30�����ױ��־��=1:��a����=0:�ޣ�
        CPU_INT32U      FuHaoWei            : 1;        // D31������λ��0��������=1������
    } LiCheng;                                          // 04 ���
    CPU_INT08U          XinHaoJiBianHao[8];             // 08 �źŻ����
    CPU_INT08U          XinHaoJiZhongLei;               // 01 �źŻ�����
    struct __sJianKongZhuangTai {
        CPU_INT08U      BenBu               : 1;        // D0��=1��������=0����������
        CPU_INT08U      DiaoChe             : 1;        // D1��=1���ǵ�����=0��������
        CPU_INT08U      JianKong            : 1;        // D2��=1����أ�=0��������
        CPU_INT08U      Rsv                 : 5;        // D7~D3��Ԥ��
    } JianKongZhuangTai;                                // 01 ���״̬
    CPU_INT08U          ShuRuJiaoLu;                    // 01 ���뽻·��
    CPU_INT08U          ShiJiJiaoLu;                    // 01 ʵ�ʽ�·��
    CPU_INT16U          CheZhanHao;                     // 02 ��վ��
    CPU_INT32U          LKJCheZhanHao;                  // 04 LKJ��վ��
} StrTaxVariable;

__packed
typedef struct _StrTAX_III_CONSTANT {     
    /***************************************************
    * ������ ��������ʼ��ַ = 039
    */
    CPU_INT32U          SiJiHao;                        // 04 ˾����
    CPU_INT32U          FuSiJiHao;                      // 04 ��˾����
    CPU_INT08U          LianCheLeiXing;                 // 01 �г�����  D7��=1�ͳ���=0��������D6~D0��Ԥ��
    CPU_INT08U          CheZhongBiaoShi[4];             // 04 ���ֱ�ʶ  ASIIC��
    union __uCheCiHao {
        struct __sCheCiHao2000 {
            CPU_INT32U  CheCi               :24;        // D23~D0�����κ���
            CPU_INT32U  Rsv                 : 8;        // D31~24��Ԥ��
        }Udat2000;
        struct __sCheCiHaoNew {
            CPU_INT32U  CheCi               :30;        // D29~D0�����κ���
            CPU_INT32U  CheCiZiMu           : 2;        // D31~30�� ���κ����׺��ĸ��=0�������䣻=1��A��=2��B��=3��C��
        }UdatNew;
        CPU_INT32U      Dat;                            // 04 ���κ���
    } CheCiHao;
    CPU_INT16U          ZongZhong;                      // 02 ����
    CPU_INT16U          JiChang;                        // 02 �Ƴ�
    CPU_INT16U          LiangSuo;                       // 02 ����      D15~D09��Ԥ����D08~D0������ֵ
    CPU_INT16U          JiCheXingHao;                   // 02 �����ͺ�  D15~D14��Ԥ����D13~D0���ͺ�ֵ
    struct __sJiCheHao {
        CPU_INT32U      JiCheHao            :21;        // D20~D0��������ֵ
        CPU_INT32U      JiCheHaoKuChong     : 3;        // D23~21�����������䣺=0X00���ޣ�=0X01��A�ڣ�=0X02��B�ڣ�=�����������壻
        CPU_INT32U      Rsv                 : 8;        // D31~24��Ԥ��
    } JiCheHao;                                         // 04 ������
} StrTaxConstant;

__packed
typedef struct _StrTAX_III_TAIL { 
    /***************************************************
    * ������ У��������ʼ��ַ = 068
    */    
    CPU_INT08U          Rsv;                           // 01 У����Ԥ��
    CPU_INT16U          CrcChk;                        // 02 У����CRCУ��
    CPU_INT08U          SumChk;                        // 01 У�����ۼӺͲ���
} StrTaxTail;

__packed
typedef struct _StrTAX_III {
    /***************************************************
    * ������ ��ͷ����ʼ��ַ = 000
    */    
    StrTaxHead          Head;
    
    /***************************************************
    * ������ ��������ʼ��ַ = 004
    */
    StrTaxVariable      Vari;
    
    /***************************************************
    * ������ ��������ʼ��ַ = 039
    */
    StrTaxConstant      Cons;
    /***************************************************
    * ������ У��������ʼ��ַ = 068
    */    
    StrTaxTail          Tail;
} StrTAX_III;
#endif


#if TAX_EN == DEF_ENABLED
#if ( TAX_II_EN == DEF_DISABLED ) && ( TAX_III_EN == DEF_DISABLED )
#error "û��TAXЭ�鱻ʹ�ܣ�����"
#else
typedef union _StrTAX {
#if TAX_II_EN == DEF_ENABLED
    StrTAX_II               Tax2;
#endif
#if TAX_III_EN == DEF_ENABLED
    StrTAX_III              Tax3;
#endif   
    uint8_t                 buf1[72];
    uint8_t                 buf2[72/2];
    uint8_t                 buf3[72/4];
} UnionTAX;
#endif
#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */
APP_EXT void            TAX_StopRecv        (MODBUS_CH  *pch, osalTime time);
APP_EXT CPU_BOOLEAN     TAX_FCxx_Handler    (MODBUS_CH  *pch);

/*******************************************************************************
 * 				end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif	/* APP_TAX_H */