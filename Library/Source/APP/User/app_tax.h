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
 *   All copyrights reserved to Wuming Shen.  现代虚拟仪器仪表研究所
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
* 描述： 
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
    CPU_INT08U          CheCiZhongLei[4];               // 车次种类
        
    CPU_INT08U          SiJiHaoKuoChong;                // 02 司机号扩充
    CPU_INT08U          FuSiJiHaoKuoChong;              // 02 副司机号扩充
    CPU_INT16U          Rsv3;
    CPU_INT08U          JiCheXingHao;                   // 01 机车型号
    CPU_INT08U          ShiJiJiaoLuHao;                 // 实际交路号
    
    CPU_INT16U          ZaYa;                           // 闸压（预留）
    CPU_INT08U          Rsv4[9];
    CPU_INT32U          BenBuKeHuo          : 8;        // 01 本补客货
    CPU_INT32U          CheCiShuZi          :24;        // 02 车次
    //CPU_INT32U          Rsv5;
    CPU_INT08U          SumChk1;                        // 检查和一
    
    //39帧
    CPU_INT08U          Addr;                           // 本板地址
    CPU_INT08U          TeZhengMa;                      // 特征码
    CPU_INT08U          DanYuanDaiHao;                  // 检测单元代号
    /***************************************************
    * 描述： 变量数据：起始地址 = 004
    */
    struct _sLKJTime {
        CPU_INT32U      Sec                 : 6;        // D05~D00：秒
        CPU_INT32U      Min                 : 6;        // D11~D06：分
        CPU_INT32U      Hour                : 5;        // D16~D12：时
        CPU_INT32U      Day                 : 5;        // D21~D17：日
        CPU_INT32U      Mon                 : 4;        // D25~D22：月
        CPU_INT32U      Year                : 6;        // D31~D26：年
    } LKJTime;                                          // 04 LKJ时间

    CPU_INT32U          SuDu                :10;        // D08~D00：速度值
    CPU_INT32U          Rsv6                :14;        // D15~D09：预留
    CPU_INT32U          JiCheXinHao         : 8;        // 01 机车信号
    
    CPU_INT08U          LingWei             : 1;        // D0：=1:零位；=0:无；
    CPU_INT08U          XiangHou            : 1;        // D1：=1:向后；=0:无；
    CPU_INT08U          XiangQian           : 1;        // D2：=1:向前；=0:无；
    CPU_INT08U          ZhiDong             : 1;        // D3：=1:制动；=0:无；
    CPU_INT08U          QianYin             : 1;        // D4：=1:牵引；=0:无；
    CPU_INT08U          Rsv7                : 3;        // D7~D5：预留 

    CPU_INT16U          XinHaoJiBianHao;                // 02 信号机编号
    
    CPU_INT32U          XinHaoJiZhongLei    : 8;        // D31~24：信号机各类           
    CPU_INT32U          GongLiBiao          :24;        // D23~D0：公里标
    
    CPU_INT16U          ZongZhong;                      // 02 总重
    CPU_INT16U          JiChang;                        // 02 计长
    CPU_INT08U          LiangShu;                       // 01 辆数    
    CPU_INT08U          BenBuKeHuo2;                    // 01 本/补，客货
    
    CPU_INT16U          CheCi;                          // 车次
    
    CPU_INT08U          QuDuanHao;                      // 区段号
    CPU_INT08U          CheZhanHao;                     // 车站号
    CPU_INT16U          SiJiHao;                        // 司机号
    CPU_INT16U          FuSiJiHao;                      // 副司机号
    /***************************************************
    * 描述： 常量：起始地址 = 039
    */
    CPU_INT16U          JiCheHao;                       // 02 机车号
    CPU_INT08U          JiCheXingHaoDi;                 // 机车型号低
    CPU_INT16U          LieCheGuanYa        :10;        // 02 列车管压力
    CPU_INT16U          LieCheGuanYaRsv     : 6;        // 02 列车管压力预留
    CPU_INT08U          SheZhiZhuangTai;                // 设置状态
} _StrTax_II_Rec;

__packed
typedef struct _StrTAX_II {

    // 38帧
    /***************************************************
    * 描述： 包头：起始地址 = 000
    */    
    CPU_INT08U          Head;                           // 01 本板地址  不记录  
    CPU_INT32U          Rsv2;                           // 不记录
    CPU_INT08U          CheZhanHaoKuoChong;             // 01 车站号扩充
    _StrTax_II_Rec      Record;                         // 记录数据
    CPU_INT08U          Rsv9;                           // 02 预留
    CPU_INT08U          SumChk;                         // 01 校验区累加和补码 // 不记录
} StrTAX_II;
#endif

/***************************************************
* 描述： TAX III数据结构：共72字节
*/
#if TAX_III_EN == DEF_ENABLED
__packed
typedef struct _StrTAX_III_HEAD {
    /***************************************************
    * 描述： 包头：起始地址 = 000
    */    
    CPU_INT08U          SheBeiLeiXing;                  // 01 TAX设备类型       0XF0:TAX2;  0XFA:TAX3/TAX07
    union __uTeZhengMa {
    struct __sTeZhengMa {
        CPU_INT08U      LkjSheBei           : 1;        // D0：LKJ设备类型：=0：LKJ2000；=1：新一代LKJ
        CPU_INT08U      TaxChangJia         : 1;        // D1：TAX厂家标识：=0：思维；=1：株所
        CPU_INT08U      Rsv                 : 1;        // D2：预留
        CPU_INT08U      TongXinFlag         : 1;        // D3：LKJ通信状态：=0：通信正常；=1通信故障
        CPU_INT08U      RecvFlag            : 4;        // D7~D4：接收状态标志：=0X03：接收成功；=0X0C：接收失败；=其他：无效数据               
    } Udat;                                        // 01 特征码
    CPU_INT08U          Dat;
    } TeZhengMa;
    CPU_INT08U          DanYuanDaiHao;                  // 01 单元代号  要求应答的单元号：=0X20：语音箱（外接口1）；=0X21：LAIS（外接口2）；=0X22：CMD/6A（外接口3）
    CPU_INT08U          XianYiBanBen;                   // 01 协议版本 
} StrTaxHead;

__packed
typedef struct _StrTAX_III_VARIABLE { 
    /***************************************************
    * 描述： 变量数据：起始地址 = 004
    */
    struct __sLKJTime {
        CPU_INT32U      Sec                 : 6;        // D05~D00：秒
        CPU_INT32U      Min                 : 6;        // D11~D06：分
        CPU_INT32U      Hour                : 5;        // D16~D12：时
        CPU_INT32U      Day                 : 5;        // D21~D17：日
        CPU_INT32U      Mon                 : 4;        // D25~D22：月
        CPU_INT32U      Year                : 6;        // D31~D26：年
    } LKJTime;                                          // 04 LKJ时间
        struct __sShiSu {
        CPU_INT16U      Speed               : 9;        // D08~D00：速度值
        CPU_INT16U      Rsv                 : 7;        // D15~D09：预留
    } ShiSu;                                            // 04 实速
    CPU_INT08U          LianCheGuanYa;                  // 01 列车管压力        10KPa
    CPU_INT16U          JiCheXinHao;                    // 02 机车信号
    struct __sJiCheGongKuang {
        CPU_INT08U      LingWei             : 1;        // D0：=1:零位；=0:无；
        CPU_INT08U      XiangHou            : 1;        // D1：=1:向后；=0:无；
        CPU_INT08U      XiangQian           : 1;        // D2：=1:向前；=0:无；
        CPU_INT08U      ZhiDong             : 1;        // D3：=1:制动；=0:无；
        CPU_INT08U      QianYin             : 1;        // D4：=1:牵引；=0:无；
        CPU_INT08U      Rsv                 : 3;        // D7~D5：预留
    } JiCheGongKuang;                                   // 01 机车工况
    CPU_INT08U          XingBie;                        // 01 行别
    CPU_INT16U          XianLuHao;                      // 02 线路号
    struct __sLiCheng {
        CPU_INT32U      LiCheng             :23;        // D22~D0：里程数，单位米：=1:；=0:无；
        CPU_INT32U      BianHuaQuShi        : 1;        // D23：里程变化趋势：=1：趋势增；=0：趋势减；
        CPU_INT32U      Rsv                 : 5;        // D28~D24：预留；
        CPU_INT32U      GongLiBiaoZhi       : 1;        // D29：公里标标志：=1:A；=0:无；
        CPU_INT32U      BaiMiBiaoZhi        : 1;        // D30：百米标标志：=1:（a）；=0:无；
        CPU_INT32U      FuHaoWei            : 1;        // D31：符号位：0：正数；=1：负数
    } LiCheng;                                          // 04 里程
    CPU_INT08U          XinHaoJiBianHao[8];             // 08 信号机编号
    CPU_INT08U          XinHaoJiZhongLei;               // 01 信号机种类
    struct __sJianKongZhuangTai {
        CPU_INT08U      BenBu               : 1;        // D0：=1：本机；=0：补机；；
        CPU_INT08U      DiaoChe             : 1;        // D1：=1：非调车；=0：调车；
        CPU_INT08U      JianKong            : 1;        // D2：=1：监控；=0：降级；
        CPU_INT08U      Rsv                 : 5;        // D7~D3：预留
    } JianKongZhuangTai;                                // 01 监控状态
    CPU_INT08U          ShuRuJiaoLu;                    // 01 输入交路号
    CPU_INT08U          ShiJiJiaoLu;                    // 01 实际交路号
    CPU_INT16U          CheZhanHao;                     // 02 车站号
    CPU_INT32U          LKJCheZhanHao;                  // 04 LKJ车站号
} StrTaxVariable;

__packed
typedef struct _StrTAX_III_CONSTANT {     
    /***************************************************
    * 描述： 常量：起始地址 = 039
    */
    CPU_INT32U          SiJiHao;                        // 04 司机号
    CPU_INT32U          FuSiJiHao;                      // 04 副司机号
    CPU_INT08U          LianCheLeiXing;                 // 01 列车类型  D7：=1客车；=0：货车；D6~D0：预留
    CPU_INT08U          CheZhongBiaoShi[4];             // 04 车种标识  ASIIC码
    union __uCheCiHao {
        struct __sCheCiHao2000 {
            CPU_INT32U  CheCi               :24;        // D23~D0：车次号码
            CPU_INT32U  Rsv                 : 8;        // D31~24：预留
        }Udat2000;
        struct __sCheCiHaoNew {
            CPU_INT32U  CheCi               :30;        // D29~D0：车次号码
            CPU_INT32U  CheCiZiMu           : 2;        // D31~30： 车次号码后缀字母：=0：无扩充；=1：A；=2：B；=3：C；
        }UdatNew;
        CPU_INT32U      Dat;                            // 04 车次号码
    } CheCiHao;
    CPU_INT16U          ZongZhong;                      // 02 总重
    CPU_INT16U          JiChang;                        // 02 计长
    CPU_INT16U          LiangSuo;                       // 02 辆数      D15~D09：预留；D08~D0：辆数值
    CPU_INT16U          JiCheXingHao;                   // 02 机车型号  D15~D14：预留；D13~D0：型号值
    struct __sJiCheHao {
        CPU_INT32U      JiCheHao            :21;        // D20~D0：机车号值
        CPU_INT32U      JiCheHaoKuChong     : 3;        // D23~21：机车号扩充：=0X00：无；=0X01：A节；=0X02：B节；=其他：无意义；
        CPU_INT32U      Rsv                 : 8;        // D31~24：预留
    } JiCheHao;                                         // 04 机车号
} StrTaxConstant;

__packed
typedef struct _StrTAX_III_TAIL { 
    /***************************************************
    * 描述： 校验区：起始地址 = 068
    */    
    CPU_INT08U          Rsv;                           // 01 校验区预留
    CPU_INT16U          CrcChk;                        // 02 校验区CRC校验
    CPU_INT08U          SumChk;                        // 01 校验区累加和补码
} StrTaxTail;

__packed
typedef struct _StrTAX_III {
    /***************************************************
    * 描述： 包头：起始地址 = 000
    */    
    StrTaxHead          Head;
    
    /***************************************************
    * 描述： 变量：起始地址 = 004
    */
    StrTaxVariable      Vari;
    
    /***************************************************
    * 描述： 常量：起始地址 = 039
    */
    StrTaxConstant      Cons;
    /***************************************************
    * 描述： 校验区：起始地址 = 068
    */    
    StrTaxTail          Tail;
} StrTAX_III;
#endif


#if TAX_EN == DEF_ENABLED
#if ( TAX_II_EN == DEF_DISABLED ) && ( TAX_III_EN == DEF_DISABLED )
#error "没有TAX协议被使能！！！"
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