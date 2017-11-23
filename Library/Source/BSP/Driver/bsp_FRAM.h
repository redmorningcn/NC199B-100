/*******************************************************************************
 *   Filename:       bsp_FRAM.h
 *   Revised:        $Date: 2017-05-15$
 *   Revision:       $
 *   Writer:	     redmorningcn.
 *
 *   Description:
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/


#ifndef  _FRAM_h_
#define  _FRAM_h_


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
//#include <includes.h>
#include <global.h>
#include <bsp_IIC.h>
#include "App_ctrl.h"


#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif
    
/*******************************************************************************
 * CONSTANTS
 */
        

/*******************************************************************************
 * MACROS
 */
   //铁电参数
#define		FRAM_SIZE               		8192		//1024*8空间容量
#define		FRAM_START_ADDR	        		0x0000	   	//铁电的起始地址
#define		FRAM_DIVICE_ADDR     			0xa0	     	//铁电的设备地址
#define		FRAM_END_ADDR	      			(FRAM_START_ADDR + FRAM_SIZE)	        
														//铁电的结束地址
//参数存储地址
#define		FRAM_RECNUM_MGR_ADDR		    4			                                            //数据记录管理存储地址
#define		FRAM_PRODUCT_INFO_ADDR		    (FRAM_RECNUM_MGR_ADDR   + sizeof(stcRecNumMgr))		//产品ID地址
#define		FRAM_REC_ADDR				    (FRAM_PRODUCT_INFO_ADDR + sizeof(stcProductInfo))		//当前数据记录存储地址
#define		FRAM_AIR_PARA_ADDR			    (FRAM_REC_ADDR          + sizeof(stcFlshRec))			//油量计算参数
#define		FRAM_RUN_PARA_ADDR			    (FRAM_AIR_PARA_ADDR     + sizeof(stcAirPara))			//系统运行参数
//#define		FRAM_CALC_MODEL_ADDR		(FRAM_RUN_PARA_ADDR     + sizeof(stcRunPara))			//计算油箱模型
//////参数存储地址
//#define		FRAM_RECNUM_MGR_ADDR		4			//数据记录管理存储地址
//#define		FRAM_PRODUCT_INFO_ADDR		20			//产品ID地址
//#define		FRAM_REC_ADDR				32			//当前数据记录存储地址
//#define		FRAM_OIL_PARA_ADDR			160			//油量计算参数
//#define		FRAM_RUN_PARA_ADDR			172			//系统运行参数
//#define		FRAM_CALC_MODEL_ADDR		180			//计算油箱模型    
//    
    
#define		OIL_BOX_MODEL_ADDR			    2000		//油箱模型地址	

//错误标识
#define		FRAM_ERR_EVT					33			//铁电异常
 
/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
 
/*******************************************************************************
 * LOCAL FUNCTIONS
 */



//------------------------------------------------------------------------
//  名  称 ：void void InitI2CIO(void)
//  功  能 ：设置 SDA SCL 为普通IO口 进行模拟总线操作
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
extern	void InitI2CIO(void);

//-------------------------------------------------------------------------------------------------------
//函数名称:         WriteFM24CL16()
//功    能:         对FM24CL16指定地址进行写数据
//入口参数:         ADDR:       操作地址    地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ; 

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadFM24CL16()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ;

//-------------------------------------------------------------------------------------------------------
//函数名称:         TestFM24CL64()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//说明：            
//--------------------------------------------------------------------------------------------------------
extern	uint8 TestFM24CL64(void)  ;

uint8 FRAM_StoreRecNumMgr(stcRecNumMgr  *SRecNumMgr);
uint8 FRAM_ReadRecNumMgr(stcRecNumMgr  *SRecNumMgr);

uint8 FRAM_StoreProductInfo(stcProductInfo  *sProductInfo);
uint8 FRAM_ReadProductInfo(stcProductInfo  *sProductInfo);

uint8 FRAM_StoreRunPara(stcRunPara  *sRunPara);
uint8 FRAM_ReadRunPara(stcRunPara  *sRunPara);

uint8 FRAM_ReadCurRecord(stcFlshRec  *sFlshRec);

uint8 FRAM_ReadAirPara(stcAirPara  *sAir);


#endif
