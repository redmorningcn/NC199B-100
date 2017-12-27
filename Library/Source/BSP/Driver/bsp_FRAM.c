/*******************************************************************************
 *   Revised:       $Date: 2017-05-15$
 *   Revision:      $
 *   Writer:	    redmorningcn.
 *
 *   Description:
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include "bsp_FRAM.h"



/***********************************************
* 描述: OS接口
*/
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
//    static  OS_SEM			Bsp_Fram_Sem;    	//信号量
    #else
    static  OS_EVENT		*Bsp_Fram_Sem;    //信号量
    #endif
#endif
    

#if (UCOS_EN     == DEF_ENABLED)
/*******************************************************************************
* 名    称： 		FRAM_WaitEvent
* 功    能： 		等待信号量
* 入口参数： 	无
* 出口参数：  	0（操作有误），1（操作成功）
* 作　 　者： 	redmorningcn
* 创建日期： 	2017-05-15
* 修    改：
* 修改日期：
* 备    注： 仅在使用UCOS操作系统时使用
*******************************************************************************/
static uint8_t FRAM_WaitEvent(void)
{
    /***********************************************
    * 描述： OS接口
    */

//#if OS_VERSION > 30000U
//    return BSP_OS_SemWait(&Bsp_Fram_Sem,0);           	// 等待信号量
//#else
//    uint8_t       err;
//    OSSemPend(Bsp_Fram_Sem,0,&err);                   		// 等待信号量
//    if ( err = OS_ERR_NONE )
//      return TRUE;
//    else
//      return FALSE;
//#endif
    return 1;
}

/*******************************************************************************
* 名    称：	 	FRAM_SendEvent
* 功    能： 		释放信号量
* 入口参数： 	无
* 出口参数： 	无
* 作　 　者： 	redmorningcn
* 创建日期： 	2017-05-15
* 修    改：
* 修改日期：
* 备    注： 仅在使用UCOS操作系统时使用
*******************************************************************************/
static void FRAM_SendEvent(void)
{
//    /***********************************************
//    * 描述： OS接口
//    */
//#if OS_VERSION > 30000U
//    BSP_OS_SemPost(&Bsp_Fram_Sem);                        	// 发送信号量
//#else
//    uint8_t       err;
//    OSSemPost(Bsp_Fram_Sem);                             		 // 发送信号量
//#endif
}

#endif /* end of (UCOS_EN     == DEF_ENABLED)*/


//-------------------------------------------------------------------------------------------------------
//函数名称: 		WriteFM24CL16()
//功    能:			对FM24CL16指定地址进行写数据
//入口参数:         ADDR:       操作地址    地址：0 ~ sizeof(FM24CL16)
//                  		*DataBuf:   数据缓冲
//                  Len: 	数据长度
//作　 　者： 	redmorningcn
//创建日期： 	2017-05-15
//出口参数:    	无
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //从机地址
    uint16  AddrTemp = 0;                                   //地址缓存
    
    AddrTemp = Addr;
    AddrTemp += FRAM_START_ADDR;                            //计算地址

    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  FALSE;                                      			//地址未超阶
    }
        
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL64地址
    CheckSlaveAckI2C();
 
    WriteByteWithI2C(Addr >> 8);                            //写FM24CL64高位地址
    CheckSlaveAckI2C();
    
    WriteByteWithI2C( Addr );                               //写FM24CL64低位地址
    CheckSlaveAckI2C();     
  
    for(i = 0; i < DataBufLen; i++)                         //发送数据
    {
        WriteByteWithI2C(DataBuf[i]);                       //写数据
        CheckSlaveAckI2C();                                 //读从机应答信号
    }
    
    StopI2C();                                              //停止IIC操作   
    
    return  TRUE;                                           //正确
}

//-------------------------------------------------------------------------------------------------------
//函数名称:         ReadFM24CL16()
//功    能:         读取FM24CL16指定地址进行数据
//入口参数:         ADDR:       操作地址   地址：0 ~ sizeof(FM24CL16)
//                  *DataBuf:   数据缓冲 
//                  Len:        数据长度
//出口参数:         无
//作　 　者： 	redmorningcn
//创建日期： 	2017-05-15
//说明：            
//--------------------------------------------------------------------------------------------------------
uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen)            
{
    uint32  i = 0;
    uint8   SlaveAddr;                                      //从机地址
    uint16  AddrTemp;                                       //地址缓存
    
    AddrTemp = Addr;
    
    AddrTemp += FRAM_START_ADDR;                            //计算地址
    
    if( (uint16)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
    {
        return  FALSE;                                      //地址未超阶
    }
    
    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //求FM24CL64设备写地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL64设备写地址
    CheckSlaveAckI2C();                                     //读从机应答信号
    
    WriteByteWithI2C(Addr >> 8 );                           //写数据高位地址
    CheckSlaveAckI2C();                                     //读从机应答信号
    
    WriteByteWithI2C( Addr );                               //写数据低位地址
    CheckSlaveAckI2C();                                     //读从机应答信号
        
    SlaveAddr = (FRAM_DIVICE_ADDR)|(0x01);                  //求FM24CL64设备读地址
    
    StartI2C();                                             //启动IIC总线
    
    WriteByteWithI2C(SlaveAddr);                            //写FM24CL16地址
    CheckSlaveAckI2C();                                     //读从机应答信号
   
    for(i = 0; i < (DataBufLen -1); i++)                    //发送数据
    {
        DataBuf[i] = ReadByteWithI2C();                     //写数据
        MasterAckI2C();                                     //主机应答从机
    }
    DataBuf[i] = ReadByteWithI2C();                         //写数据
    
    MasterNoAckI2C();                                       //主机应答从机
    
    StopI2C();
    
    return  TRUE;                                           //正确
}


//  RecNumMgr
//
//-------------------------------------------------------------------------------------------------------
//函数名称	:       FRAM_StoreRecNumMgr()
//功    		能	:       存储数据记录管理地址
//入口参数	:       待存储的数据记录指针
//出口参数  :       成功TURE，失败FALSE
//作　 　者： 	redmorningcn
//创建日期： 	2017-05-15
//说		 明：            
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_StoreRecNumMgr(stcRecNumMgr  *SRecNumMgr)
{
	uint8	flg = TRUE;
	
//	FRAM_WaitEvent();
    
    CPU_SR  cpu_sr;
CPU_CRITICAL_ENTER() ;
	if(!WriteFM24CL64(FRAM_RECNUM_MGR_ADDR, (uint8 *)SRecNumMgr, sizeof(stcRecNumMgr)))		//存数据管理
		flg = FALSE;
CPU_CRITICAL_EXIT();
//	FRAM_SendEvent();
	return	flg;
}


//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_ReadRecNumMgr()
//功    		能	:      读数据记录管理指令
//入口参数	:      待读取的数据记录的储存位置
//出口参数  :      成功TURE，失败FALSE
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：  
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_ReadRecNumMgr(stcRecNumMgr  *SRecNumMgr)
{
	uint8	flg = TRUE;
	
//	FRAM_WaitEvent();
    
    CPU_SR  cpu_sr;
CPU_CRITICAL_ENTER() ;
	if(!ReadFM24CL64(FRAM_RECNUM_MGR_ADDR, (uint8 *)SRecNumMgr, sizeof(stcRecNumMgr)))		//读数据管理
		flg = FALSE;
CPU_CRITICAL_EXIT();	
//	FRAM_SendEvent();
	return	flg;
}

//  ProductInfo
//
//-------------------------------------------------------------------------------------------------------
//函数名称:       FRAM_StoreProductInfo()
//功    能:       存产品信息
//入口参数:      
//出口参数:       
//作　 　者： 	    redmorningcn
//创建日期： 	    2017-05-15
//说		 明：            
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_StoreProductInfo(stcProductInfo  *sProductInfo)
{
	uint8	flg = TRUE;
	
	FRAM_WaitEvent();
	if(!WriteFM24CL64(FRAM_PRODUCT_INFO_ADDR, (uint8 *)sProductInfo, sizeof(stcProductInfo)))		
		flg = FALSE;
	
	FRAM_SendEvent();
	return	flg;
}

//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_ReadProductInfo()
//功    		能	:      读产品信息
//入口参数	:      产品的
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：  
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_ReadProductInfo(stcProductInfo  *sProductInfo)
{
	uint8	flg = TRUE;
	
	FRAM_WaitEvent();
	if(!ReadFM24CL64(FRAM_PRODUCT_INFO_ADDR, (uint8 *)sProductInfo, sizeof(stcProductInfo)))		
		flg = FALSE;
	
	FRAM_SendEvent();
	return	flg;
}



//  currecord
//
//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_StoreCurRecord()
//功    		能	:      存当前记录的 
//入口参数	:      
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：            
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_StoreCurRecord(stcFlshRec  *sFlshRec)
{
	uint8	flg = TRUE;
	
	//FRAM_WaitEvent();
	if(!WriteFM24CL64(FRAM_REC_ADDR, (uint8 *)sFlshRec, sizeof(stcFlshRec)))		
		flg = FALSE;
	
	//FRAM_SendEvent();
	return	flg;
}

//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_ReadProductInfo()
//功    		能	:      读当前记录的 
//入口参数	:      产品的
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：  
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_ReadCurRecord(stcFlshRec  *sFlshRec)
{
	uint8	flg = TRUE;
	
	//FRAM_WaitEvent();
	if(!ReadFM24CL64(FRAM_REC_ADDR, (uint8 *)sFlshRec, sizeof(stcFlshRec)))		
		flg = FALSE;
	
	//FRAM_SendEvent();
	return	flg;
}

//  oilpara
//
//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_StoreOilPara()
//功    		能	:      
//入口参数	:      
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：            
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_StoreAirPara(stcAirPara  *sAir)
{
	uint8	flg = TRUE;
	
	FRAM_WaitEvent();
	if(!WriteFM24CL64(FRAM_AIR_PARA_ADDR, (uint8 *)sAir, sizeof(stcAirPara)))		
		flg = FALSE;
	
	FRAM_SendEvent();
	return	flg;
}

//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_ReadOilPara()
//功    		能	:      
//入口参数	:     
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：  
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_ReadAirPara(stcAirPara  *sAir)
{
	uint8	flg = TRUE;
	stcAirPara  sAirTmp;
    
    
	FRAM_WaitEvent();
   
//	if(!ReadFM24CL64(FRAM_AIR_PARA_ADDR, (uint8 *)sAir, sizeof(stcAirPara)))		
//		flg = FALSE;
    //数据有效性判断
    if(ReadFM24CL64(FRAM_AIR_PARA_ADDR, (uint8 *)&sAirTmp, sizeof(stcAirPara)))
    {
        if(         sAirTmp.sStandard_Dust.GoodLimit    == 0 
               ||   sAirTmp.sStandard_Dust.NormalLimit  == 0
               ||   sAirTmp.sStandard_Dust.NormalLimit <= sAirTmp.sStandard_Dust.GoodLimit 
           )
            return 0;
        
        if(         sAirTmp.sStandard_VOC.GoodLimit    == 0 
               ||   sAirTmp.sStandard_VOC.NormalLimit  == 0
               ||   sAirTmp.sStandard_VOC.NormalLimit <= sAirTmp.sStandard_VOC.GoodLimit 
            )
            return 0;     
        
        if(         sAirTmp.sStandard_Hum.GoodLimit    == 0 
               ||   sAirTmp.sStandard_Hum.NormalLimit  == 0
               ||   sAirTmp.sStandard_Hum.NormalLimit <= sAirTmp.sStandard_Hum.GoodLimit 
            )
            return 0;
        
        memcpy((uint8 *)sAir,(uint8 *)&sAirTmp,sizeof(stcAirPara));         //数据有效，退出
    }

	FRAM_SendEvent();
	return	flg;
}


//  runpara
//
//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_StoreRunPara()
//功    		能	:      
//入口参数	:      
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：            
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_StoreRunPara(stcRunPara  *sRunPara)
{
	uint8	flg = TRUE;
	
	FRAM_WaitEvent();
	
	if(!WriteFM24CL64(FRAM_RUN_PARA_ADDR, (uint8 *)sRunPara, sizeof(stcRunPara)))		
		flg = FALSE;
	
	FRAM_SendEvent();
	return	flg;
}

//-------------------------------------------------------------------------------------------------------
//函数名称	:      FRAM_ReadRunPara()
//功    		能	:      
//入口参数	:     
//出口参数  :       
//作　 　者 ： 	redmorningcn
//创建日期  ： 	2017-05-15
//说		 明：  
//--------------------------------------------------------------------------------------------------------
uint8 FRAM_ReadRunPara(stcRunPara  *sRunPara)
{
	uint8	flg = TRUE;
	
	FRAM_WaitEvent();
	
	if(!ReadFM24CL64(FRAM_RUN_PARA_ADDR, (uint8 *)sRunPara, sizeof(stcRunPara)))		
		flg = FALSE;
	
	FRAM_SendEvent();
	return	flg;
}


//#endif
    
