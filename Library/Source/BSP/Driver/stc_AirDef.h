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
//压缩空气质量装置，测量数据内容  
__packed
typedef struct _sAirQuality{
    uint16				Temperature       ;//温度                                 2                    
    uint16				Humidity          ;//湿度                                 2                    
    uint16				VOCs              ;//挥发有机物                           2                    
    uint16				CO2_EQU           ;//等值CO2                              2  
    CPU_INT32U          RS                ;// RS
    uint16				PM1D0_S           ;//标准颗粒物浓度ug/m3                  2                    
    uint16				PM2D5_S           ;//标准颗粒物浓度ug/m3                  2                    
    uint16				PM10D_S           ;//标准颗粒物浓度ug/m3                  2    
    uint16				PM1D0_G           ;// 大气环境                              
    uint16				PM2D5_G           ;// 大气环境                    
    uint16				PM10D_G           ;// 大气环境 
    uint16              Dusts_0D3         ;//0.1升空气中直径在0.3um的颗粒物个数
    uint16              Dusts_0D5         ;//0.1升空气中直径在0.5um的颗粒物个数
    uint16              Dusts_1D0         ;//0.1升空气中直径在1.0um的颗粒物个数
    uint16              Dusts_2D5         ;//0.1升空气中直径在2.5um的颗粒物个数    
} stcAirQuality;      

//2017-11-16  redmorningcn
//压缩空气质量装置，保存的数据结构     
typedef __packed struct _stcFlshRec     //定义数据记录数据结构（20171116 机车压缩空气）
{
    uint8				CmdTpye		      ;//命令类型       1     		        	
    uint8				EvtType           ;//事件类型       1                  
    uint16				LocoType          ;//机车型号       2                  
    uint16				LocoNum           ;//机 车 号       2                     
    uint32				RecID             ;//记录流水号     4
    
    stcAirQuality       sAir              ;//空气质量       28
    uint8               RunSys            ;//运行状态       1（阀门门打开，开始测试，等等）      
    uint8				Recv[8]          ;//预留                               
    uint16				ErrCode           ;//故障代码       2                  
    uint8				Air_Hum_Grade     ;//湿度等级       1                  
    uint8				Air_Voc_Grade     ;//VOC等级        1                   
    uint8               Air_Dust_Grade    ;//粉尘等级       1    
    struct _sSysTime {
        uint32      Sec                 : 6;        // D05~D00：秒
        uint32      Min                 : 6;        // D11~D06：分
        uint32      Hour                : 5;        // D16~D12：时
        uint32      Day                 : 5;        // D21~D17：日
        uint32      Mon                 : 4;        // D25~D22：月
        uint32      Year                : 6;        // D31~D26：年
    } SysTime;                                          // 04 时间
    
////////////////////////////////////////////////// TAX的数据内容
    uint8           TaxType             ;//TAX特征码       1                     
    uint8           TaxFlg              ;//TAX特征码       64       
    /***************************************************
    * 描述： TAX:64 bytes
    */
    union __uTAX__Rec {
    _StrTax_II_Rec      Tax2;                           // 记录数据     
    struct __sTAX_III__Rec {
    StrTaxVariable      Vari;
    StrTaxConstant      Cons;
    } Tax3;
    
    uint8_t             buf1[64];
    uint16_t            buf2[64/2];
    uint32_t            buf3[64/4];
    
    } Tax;    
    
    uint16				CrcCheck                        ;//校验   2                 
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
 * 返回类型         函数名称                入口参数
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

#endif 


