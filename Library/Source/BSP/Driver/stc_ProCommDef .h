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
#ifndef _STC_PRO_COMM_DEF_H
#define _STC_PRO_COMM_DEF_H

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

////机车型号及机车编号
typedef struct {     
 	uint16	Type;				//机车类型	2	参见机车类型代码表
 	uint16	Num;				//机车号		2	
} stcLocoId;

//产品信息:  型号+ 编号
//12 bytes
typedef struct _StrProductInfo {
	uint32		    Type;			//产品类别，0 :NDP02  ,1:NDP02-B
	uint32		    ID;				//产品编号	16110002
	stcLocoId		sLocoId;		//机车信息	104(DF4B) - 1000
}stcProductInfo;

//记录号管理:数据记录管理
//16 bytes
typedef struct _StrRecNumMgr {
    uint32		Current;				//当前记录号	0~0xffffffff
    uint32		IcRead;				//IC卡已读取记录号
    uint32		GrsRead;			//无线已读取记录号
    uint32		Back;				//备用
}stcRecNumMgr;


//油量计算参数: 模型编号，高度，密度，斜率
//32 bytes
__packed
typedef struct _StrAirPara {
     struct _sStandard_Hum {
        uint16      GoodLimit;          //大于为优，                         蓝色指示
        uint16      NormalLimit;        //大于该值，小于good，为一般         绿色指示  小于该值，为差，红色表示  
    } sStandard_Hum;                    //湿度阈值                                 
    
    struct _sStandard_VOC {
        uint16      GoodLimit;          //大于为优，                         蓝色指示
        uint16      NormalLimit;        //大于该值，小于good，为一般         绿色指示  小于该值，为差，红色表示  
    } sStandard_VOC;                    //VOC阈值
    
    struct _sStandard_Dust {
        uint16      GoodLimit;          //大于为优，                         蓝色指示
        uint16      NormalLimit;        //大于该值，小于good，为一般         绿色指示  小于该值，为差，红色表示  
    } sStandard_Dust;                   //粉尘阈值
    
    int16           Temp_modefy;        //温度修正值，用于补偿
    int16           Hum_modefy;         //湿度修正值，用于补偿
    int16           Dust_modefy;        //粉尘修正值，用于补偿
    int16           Voc_modefy;         //VOC修正值 ，用于补偿          

    uint8           tmp[14];            //预留
    
    struct __SetFlg {
    uint16      SetHumStand             : 1;        // D0：=1:打开气路；=0:无；
    uint16      SetVocStand             : 1;        // D1：=1 关闭气路；=0:无；
    uint16      SetDustStand            : 1;        // D2：=1:开始测量；=0:无；
    uint16      SetHumModefy            : 1;        // D3：=1:结束测量；=0:无；
    uint16      SetVocModefy            : 1;        // D4：=1:数据保存；=0:无；
    uint16      SetDustModefy           : 1;        // D5：=1:数据保存；=0:无；
    uint16      Rsv                     : 10;       // D6~D15：预留
    } SetFlg;    
}stcAirPara;

//
//运行参数，装置运行相关，数据存储周期，显示参数，恢复出厂设置
//8 bytes
__packed
typedef struct _stcRunPara_					
{
	uint8		StoreTime;					// 1 		数据记录存储周期
	uint8		RecClear;					// 1		数据记录清零，清StrRecNumMgr内容
	uint8		SysReset;					// 1    	系统参数重置，清StrRecNumMgr + StrOilPara 	中的内容。
	uint8		StartFlg;					// 1    	首次运行

    struct __SysSta {
        uint8      OpenAir              : 1;        // D0：=1:打开气路；=0:无；
        uint8      CloseAir             : 1;        // D1：=1 关闭气路；=0:无；
        uint8      StartMeasure         : 1;        // D2：=1:开始测量；=0:无；
        uint8      EndMeasure           : 1;        // D3：=1:结束测量；=0:无；
        uint8      Store                : 1;        // D4：=1:数据保存；=0:无；
        uint8      mtrsim               : 1;        //debug use
        uint8      mtrsend              : 1;        //debug use

        uint8      Rsv                  : 1;        // D7~D5：预留
    } SysSta;                                       // 系统运行状态
    
	uint8		StoreType;                  // 1   	    系统运行状态
	uint8		StoreTypeBak;               // 1   	    系统运行状态

	uint8		SetBitFlg;					// 1        设置有效位指示
    
}stcRunPara;


/***********************************************
* 描述： 2017/8/19：保存参数定义
*/
__packed
typedef struct {
                                                    //序号  字段名称	类型	    长度	说明	备注
    CHAR                XinHao;                     // 0	产品型号	BYTE[10]	10		ASCII：NCS27
    CHAR                BianHao;                    //10	产品编号	BYTE[10]	10		ASCII：2017070001
    WORD                HWVer;                      //20	硬件版本	WORD	    2		高8位为大版本，低8位为小版本：V1.0
    WORD                SWVer;                      //22	软件版本	WORD	    2		高8位为大版本，低8位为小版本：V1.0
    WORD                JiCheXingHao;               //24	机车型号	WORD	    2		
    WORD                JiCheHao;                   //26	机车号	    WORD	    2		
    WORD                CeShiShiJian[6];            //28	测试时间	BYTE[6]	    6		170731
    WORD                CeShiRenYuan[8];            //34	测试人员	BYTE[8]	    8		
    CHAR                AnZhuangShiJian[6];         //42	安装时间	BYTE[6]	    6		
    CHAR                AnZhuangRenYuan[8];         //48	安装人员	BYTE[10]	8		
    CHAR                WeiXiuShiJian[6];           //56	维修时间	BYTE[6]	    6		
    CHAR                WeiXiuRenYuan[8];           //62	维修人员	BYTE[10]	8		
    BYTE                WeiXiuCiShu;                //70	维修次数	BYTE	    1		
    BYTE                MuKuanPeiZhi;               //71	模块配置	BYTE	    1		配置通讯的设备数量，每一位代表一个设备，相应位为1，表示使用该设备，相应位为0，表示禁用，根据具体应用配置
    DWORD               KaiJiCiShuo         :18;    //72	工作状态	DWORD	    4		D31~21：开机次数，每重启一次加1
    DWORD               GongZuoShiJian      :14;    //                                  D20~0：工作时长，以小时为单位
    BYTE                Rsv[50];                    //76	预留	    BYTE[50]	50		自定义
    WORD                Chk;                        //126	校验码	    WORD	    2		CRC
}stcProLog;             //产品日志

/***************************************************
* 描述： 控制器参数：共256 + 4 = 260字节
*/
__packed
typedef struct _StrCtrlPara {
    /***************************************************
    * 描述： 系统参数：起始地址 = 000
    */    
    INT16U              Password;                       // 用于MODEBUS通讯确认密钥，默认为6237，防止非法改数据
    /***************************************************
    * 描述： 软件版本：起始地址 = 002
    */
    INT16U		        SoftWareID;                     //	2  	软件版本
    //
    /***************************************************
    * 描述：记录号管理地址：起始地址 = 004
    */
    stcRecNumMgr        sRecNumMgr;			        //	16	记录号管理: 当前记录号，卡已读记录号，无线已读记录号
       //
    /***************************************************
    * 描述：产品信息：起始地址 = 020
    */
    stcProductInfo	    sProductInfo;			    //	12	产品信息: 产品信号及产品编号；
    //
    /***************************************************
    * 描述：油量计算参数：起始地址 = 32
    */
	stcAirPara          sAirPara;                       //32

    //
    /***************************************************
    * 描述：系统运行参数：起始地址 = 64
    */
	stcRunPara	        sRunPara;				        // 8		//数据存储周期，数据清零，参数清零
    
    /***************************************************
    * 描述：产品操作日志：起始地址 = 72               
    */ 
    stcProLog           sProLog;                       //126     

//    /***************************************************
//    * 描述： 恢复出厂设置：起始地址 = 198
//    */
//    INT16S          RestoreSetting;                     // 恢复出厂设置

    const struct _StrCtrlPara *idx;                    // 序号指针，占4字节

} StrCtrlPara;


// 发送数据结构		无线-->统计		
__packed
typedef union {
    StrCtrlPara	dat;   //	
    uint8		buf1[256];   //	
    uint8		buf2[256];   //	
} UniCtrlPara;
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


