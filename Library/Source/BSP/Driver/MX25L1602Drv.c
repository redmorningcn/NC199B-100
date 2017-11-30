/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:       MX25L1602.c
** Descriptions:    SST25VF016B驱动
**
**------------------------------------------------------------------------------------------------------
** Created by:      Litiantian
** Created date:    2007-04-16
** Version:         1.0
** Descriptions:    The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:     
** Modified date:   
** Version:         
** Descriptions:    
********************************************************************************************************/
#include   "MX25L1602Drv.h"
#include   "includes.h"
/*
* 移植本软件包时需要修改以下的函数或宏 
*/
//#define     CE_Low()    LPC_GPIO0->FIOCLR |= SPI_CS;
//#define     CE_High()   LPC_GPIO0->FIOSET |= SPI_CS;

void nop(void)
{
	uint8_t i = 10; 
    
	while(i) 
	{ 
		i--; 
	} 
    //    #if DEBUG_MODEL_DISENABLE > 0 
    //        WDT_Feed();                                               /* Determine CPU capacity                               */
    //    #endif
}

extern	void WriteSoftSpiCSF(uint8 temp);

void	CE_Low(void)
{
	WriteSoftSpiCSF(0);
}

void	CE_High(void)
{
	WriteSoftSpiCSF(1);
}

//#define			SCKF					204    	    //p1.20  
//#define			MISOF					203			//p1.23  
//#define			MOSIF					205			//p1.24 
//#define   		SOFT_SPI_CSF			202			//p1.22

#define			SCKF					005         //pA5  
#define			MISOF					006			//pA6  
#define			MOSIF					007			//pA7 
#define   		SOFT_SPI_CSF			004			//pA4

#define	uint8	uint8_t

void	InitFlashIO(void)
{
	GPIO_PinselConfig(MOSIF,0);	
	GPIO_PinselConfig(MISOF,0);
	GPIO_PinselConfig(MISOF,0);	
	GPIO_PinselConfig(SOFT_SPI_CSF,0);
	GPIO_PinselConfig(MOSIF,0);
	GPIO_PinselConfig(SCKF,0);
	GPIO_PinselConfig(SCKF,0);
}

//------------------------------------------------------------------------
//  名  称 ：void WriteSCKF( uint8 temp )
//  功  能 ：设置 SCKF
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteSCKF(uint8 temp)
{
	
	if(temp)		
	{
		GPIO_SetOrClearValue(SCKF,1);
	}
	else
	{
		GPIO_SetOrClearValue(SCKF,0);		
	}	
	nop();	
}

//------------------------------------------------------------------------
//  名  称 ：uint8 ReadSCKF(void)
//  功  能 ：读取 SCKF 的值
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
uint8 ReadSCKF(void)
{
	uint8 temp;
    
	temp = 	GPIO_ReadIoVal(SCKF);
	nop();
	return temp;
}

//------------------------------------------------------------------------
//  名  称 ：void WriteMOSIF( uint8 temp )
//  功  能 ：设置 MOSIF
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteMOSIF(uint8 temp)
{	
	if(temp)		
	{
		GPIO_SetOrClearValue(MOSIF,1);
	}
	else
	{
		GPIO_SetOrClearValue(MOSIF,0);		
	}	
	nop();		
    
}

//------------------------------------------------------------------------
//  名  称 ：uint8 ReadMOSIF(void)
//  功  能 ：读取 MOSIF 的值
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
uint8 ReadMOSIF(void)
{
	uint8 temp;
	
	temp = 	GPIO_ReadIoVal(MOSIF);
	nop();
	return temp;
}

//------------------------------------------------------------------------
//  名  称 ：void WriteMOSIF( uint8 temp )
//  功  能 ：设置 MOSIF
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteMISOF(uint8 temp)
{	
	if(temp)		
	{
		GPIO_SetOrClearValue(MISOF,1);
	}
	else
	{
		GPIO_SetOrClearValue(MISOF,0);		
	}
    
	nop();		
}

//------------------------------------------------------------------------
//  名  称 ：uint8 ReadMOSIF(void)
//  功  能 ：读取 MOSIF 的值
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
uint8 ReadMISOF(void)
{
	uint8 temp;
    
	temp = 	GPIO_ReadIoVal(MISOF);
    
	nop();
	return temp;
}

//------------------------------------------------------------------------
//  名  称 ：void WriteMOSIF( uint8 temp )
//  功  能 ：设置 MOSIF
// 入口参数：无
// 出口参数：无
//------------------------------------------------------------------------
void WriteSoftSpiCSF(uint8 temp)
{
	if(temp)		
	{
		GPIO_SetOrClearValue(SOFT_SPI_CSF,1);
	}
	else
	{
		GPIO_SetOrClearValue(SOFT_SPI_CSF,0);		
	}	
    
	nop();		
}

//---------------------------------------------------------------------------------
//函数名称:             SentByte_SPI()
//功    能:             SPI发送一字节数据
//入口参数:             Data8
//出口参数:             无
//说    明:				以模拟SPI总线方式发送数据
//----------------------------------------------------------------------------------
uint8 SendOrRecByte_SPIF(uint8 ch)
{
	uint8 i,temp;   
	
    for (i=0;i<8;i++)
    {
		temp=ch&0x80;
    	ch=ch<<1;
        if(temp)
        {
            WriteMOSIF( 1 );	
            
            WriteSCKF ( 0 );
            
            WriteSCKF ( 1 );	        	
        }
        else
        {
            WriteMOSIF( 0 );	
            
            WriteSCKF ( 0 );
            
            WriteSCKF ( 1 );
        }
        
        ch |= ReadMISOF();
    }
    
    return	ch;
}

void	SendByte_SPIF(uint8	ch)
{
	SendOrRecByte_SPIF(ch);
}

uint8	RecByte_SPIF(void)
{
	uint8		ch =0;
	return	SendOrRecByte_SPIF(ch);
}

/*********************************************************************************************************
** 函数名称：SPIInit
** 功能说明：初始化SPI
** 输入参数：无
** 输出参数：无
*********************************************************************************************************/
void  SPIInit (void)
{
    
}

/*********************************************************************************************************
** 函数名称: Send_Byte
** 函数功能：SPI发送一个字节数据
** 输入参数: data
** 输出参数: 无
** 返 回 值：无
*********************************************************************************************************/
void Send_Byte (INT8U data)
{
	SendByte_SPIF(data);    
}

/*********************************************************************************************************
** 函数名称:Get_Byte
** 函数功能:SPI接口接收一个字节数据
** 输入参数:无
** 输出参数:无
*********************************************************************************************************/
INT8U Get_Byte (void)
{
	return	RecByte_SPIF();
}

/*********************************************************************************************************
** 用户接口层
** 以下函数在移植时无需修改
*********************************************************************************************************/
/********************************************************************************************************
** 函数名称:MX25L1602_RD
** 函数功能:MX25L1602的读函数,可选择读ID和读数据操作
** 输入参数:
**          INT32U Dst：目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）
**          INT32U NByte:  要读取的数据字节数
**          INT8U* RcvBufPt:接收缓存的指针
** 输出参数:操作成功则返回OK,失败则返回ERROR_0
** 特殊说明:若某功能下,某一入口参数无效,可在该入口参数处填Invalid，该参数将被忽略
**********************************************************************************************************/
INT8U MX25L1602_RD(INT32U Dst, INT32U NByte,INT8U* RcvBufPt)
{
    INT32U i = 0;
    if ((Dst+NByte > MAX_ADDR)||(NByte == 0))  return (ERROR_0);            /*  检查入口参数                */
    
    CE_Low();
    Send_Byte(0x0B);                                                      /* 发送读命令                   */
    Send_Byte(((Dst & 0xFFFFFF) >> 16));                                  /* 发送地址信息:该地址为3个字节 */
    Send_Byte(((Dst & 0xFFFF) >> 8));
    Send_Byte(Dst & 0xFF);
    Send_Byte(0xFF);                                                      /* 发送一个哑字节以读取数据     */
    for (i = 0; i < NByte; i++) {
        RcvBufPt[i] = Get_Byte();
    }
    CE_High();
    return (OK);
}

/*********************************************************************************************************
** 函数名称:MX25L1602_RdID
** 函数功能:MX25L1602的读ID函数,可选择读ID和读数据操作
** 输入参数:
**          idtype IDType:ID类型。用户可在Jedec_ID,Dev_ID,Manu_ID三者里选择
**          INT32U* RcvbufPt:存储ID变量的指针
** 输出参数:操作成功则返回OK,失败则返回ERROR_0
** 特殊说明:若填入的参数不符合要求，则返回ERROR_0
*********************************************************************************************************/
INT8U MX25L1602_RdID(idtype IDType, INT32U* RcvbufPt)
{
    INT32U temp = 0;
    if (IDType == Jedec_ID) {
        CE_Low();
        Send_Byte(0x9F);                                                    /* 发送读JEDEC ID命令(9Fh)      */
        temp = (temp | Get_Byte()) << 8;                                    /* 接收数据                     */
        temp = (temp | Get_Byte()) << 8;
        temp = (temp | Get_Byte());                                         /* 在本例中,temp的值应为0xBF2541*/
        CE_High();
        *RcvbufPt = temp;
        return (OK);
    }
    
    if ((IDType == Manu_ID) || (IDType == Dev_ID) ) {
        CE_Low();
        Send_Byte(0x90);                                                    /* 发送读ID命令 (90h or ABh)    */
        Send_Byte(0x00);                                                    /* 发送地址                     */
        Send_Byte(0x00);                                                    /* 发送地址                     */
        Send_Byte(IDType);                                                  /* 发送地址 - 不是00H就是01H    */
        temp = Get_Byte();                                                  /* 接收获取的数据字节           */
        CE_High();
        *RcvbufPt = temp;
        return (OK);
    } else {
        return (ERROR_0);
    }
}

/*********************************************************************************************************
** 函数名称:MX25L1602_WR
** 函数功能:MX25L1602的写函数，可写1个和多个数据到指定地址
** 输入参数:
**          INT32U Dst：目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）
**          INT8U* SndbufPt:发送缓存区指针
**          INT32U NByte:要写的数据字节数
** 输出参数:操作成功则返回OK,失败则返回ERROR_0
** 特殊说明:若某功能下,某一入口参数无效,可在该入口参数处填Invalid，该参数将被忽略
*********************************************************************************************************/
INT8U MX25L1602_WR(INT32U Dst, INT8U* SndbufPt, INT32U NByte)
{
    INT32U temp = 0,i = 0,StatRgVal = 0;
    if (( (Dst+NByte-1 > MAX_ADDR)||(NByte == 0) )) {
        return (ERROR_0);                                                     /*  检查入口参数                */
    }
    CE_Low();
    Send_Byte(0x05);                                                      /* 发送读状态寄存器命令         */
    temp = Get_Byte();                                                    /* 保存读得的状态寄存器值       */
    CE_High();
    
    CE_Low();
    Send_Byte(0x50);                                                      /* 使状态寄存器可写             */
    //         Send_Byte(0x06);                                                    /* 发送写使能命令               */
    CE_High();
    CE_Low();
    Send_Byte(0x01);                                                      /* 发送写状态寄存器指令         */
    Send_Byte(0);                                                         /* 清0BPx位，使Flash芯片全区可写*/
    CE_High();
    
    for(i = 0; i < NByte; i++) {
        ////新增20131126  跨块擦除扇
        //		printfcom0(" %d,%d, ",Dst+i,Dst+i % 4096);
		if((Dst+i) % 4096 == 0)
		{
			MX25L1602_Erase((Dst+i)/4096,(Dst+i)/4096);
			//printfcom0("\r\n EraseEraseEraseErase");
		}
        //////		
        
        CE_Low();
        Send_Byte(0x06);                                                    /* 发送写使能命令               */
        CE_High();
        
        CE_Low();
        Send_Byte(0x02);                                                    /* 发送字节数据烧写命令         */
        Send_Byte((((Dst+i) & 0xFFFFFF) >> 16));                            /* 发送3个字节的地址信息        */
        Send_Byte((((Dst+i) & 0xFFFF) >> 8));
        Send_Byte((Dst+i) & 0xFF);
        Send_Byte(SndbufPt[i]);                                             /* 发送被烧写的数据             */
        CE_High();
        
        do {
            CE_Low();
            Send_Byte(0x05);                                                  /* 发送读状态寄存器命令         */
            StatRgVal = Get_Byte();                                           /* 保存读得的状态寄存器值       */
            CE_High();
        } while (StatRgVal == 0x03);                                        /* 一直等待，直到芯片空闲       */
    }
    
    CE_Low();
    Send_Byte(0x06);                                                      /* 发送写使能命令               */
    CE_High();
    
    CE_Low();
    Send_Byte(0x50);                                                      /* 使状态寄存器可写             */
    CE_High();
    CE_Low();
    Send_Byte(0x01);                                                      /* 发送写状态寄存器指令         */
    Send_Byte(temp);                                                      /* 恢复状态寄存器设置信息       */
    CE_High();
    
    return (OK);
}

/*********************************************************************************************************
** 函数名称:MX25L1602_Erase
** 函数功能:根据指定的扇区号选取最高效的算法擦除
** 输入参数:
**          INT32U sec1：起始扇区号,范围(0~499)
**          INT32U sec2：终止扇区号,范围(0~499)
** 输出参数:操作成功则返回OK,失败则返回ERROR_0
*********************************************************************************************************/
INT8U MX25L1602_Erase(INT32U sec1, INT32U sec2)
{
    INT8U  temp1 = 0,temp2 = 0,StatRgVal = 0;
    INT32U SecnHdAddr = 0;
    INT32U no_SecsToEr = 0;                                               /* 要擦除的扇区数目             */
    INT32U CurSecToEr = 0;                                                /* 当前要擦除的扇区号           */
    
    /*
    *  检查入口参数
    */
    if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX)) {
        return (ERROR_0);
    }
    
    CE_Low();
    Send_Byte(0x05);                                                     /* 发送读状态寄存器命令          */
    temp1 = Get_Byte();                                                  /* 保存读得的状态寄存器值        */
    CE_High();
    
    CE_Low();
    //  Send_Byte(0x50);                                                     /* 使状态寄存器可写              */
    
    Send_Byte(0x06);
    CE_High();
    
    CE_Low();
    Send_Byte(0x01);                                                     /* 发送写状态寄存器指令          */
    Send_Byte(0);                                                        /* 清0BPx位，使Flash芯片全区可写 */
    CE_High();
    
    CE_Low();
    Send_Byte(0x06);                                                      /* 发送写使能命令               */
    CE_High();
    
    /*
    * 如果用户输入的起始扇区号大于终止扇区号，则在内部作出调整
    */
    if (sec1 > sec2)
    {
        temp2 = sec1;
        sec1  = sec2;
        sec2  = temp2;
    }
    /*
    * 若起止扇区号相等则擦除单个扇区
    */
    if (sec1 == sec2)
    {
        SecnHdAddr = SEC_SIZE * sec1;                                       /* 计算扇区的起始地址           */
        CE_Low();
        Send_Byte(0x20);                                                    /* 发送扇区擦除指令             */
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                         /* 发送3个字节的地址信息        */
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();
        do {
            CE_Low();
            Send_Byte(0x05);                                                  /* 发送读状态寄存器命令        */
            StatRgVal = Get_Byte();                                           /* 保存读得的状态寄存器值      */
            CE_High();
        } while (StatRgVal & 0x01);                                         /* 一直等待，直到芯片空闲      */
        return (OK);
    }
    
    /*
    * 根据起始扇区和终止扇区间距调用最快速的擦除功能
    */
    
    if (sec2 - sec1 == SEC_MAX) {
        CE_Low();
        Send_Byte(0x60);                                                    /* 发送芯片擦除指令(60h or C7h) */
        CE_High();
        do {
            CE_Low();
            Send_Byte(0x05);                                                  /* 发送读状态寄存器命令         */
            StatRgVal = Get_Byte();                                           /* 保存读得的状态寄存器值       */
            CE_High();
        } while (StatRgVal & 0x01);                                         /* 一直等待，直到芯片空闲       */
        return (OK);
    }
    
    no_SecsToEr = sec2 - sec1 +1;                                         /* 获取要擦除的扇区数目         */
    CurSecToEr  = sec1;                                                   /* 从起始扇区开始擦除           */
    
    /*
    * 若两个扇区之间的间隔够大，则采取16扇区擦除算法
    */
    while (no_SecsToEr >= 16)
    {
        CE_Low();
        Send_Byte(0x06);                                                     /* 发送允许写命令              */
        CE_High();
        
        SecnHdAddr = SEC_SIZE * CurSecToEr;                                  /* 计算扇区的起始地址          */
        CE_Low();
        Send_Byte(0xD8);                                                     /* 发送64KB块擦除指令          */
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                          /* 发送3个字节的地址信息       */
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();
        do {
            CE_Low();
            Send_Byte(0x05);                                                  /* 发送读状态寄存器命令        */
            StatRgVal = Get_Byte();                                           /* 保存读得的状态寄存器值      */
            CE_High();
        } while (StatRgVal & 0x01);                                         /* 一直等待，直到芯片空闲      */
        CurSecToEr  += 16;                                                  /* 计算擦除了16个扇区后和擦除区*/
        /* 域相邻的待擦除扇区号        */
        no_SecsToEr -=  16;                                                 /* 对需擦除的扇区总数作出调整  */
    }
    
    /*
    * 采用扇区擦除算法擦除剩余的扇区
    */
    while (no_SecsToEr >= 1) {
        CE_Low();
        Send_Byte(0x06);                                                   /* 发送允许写命令              */
        CE_High();
        
        SecnHdAddr = SEC_SIZE * CurSecToEr;                                /* 计算扇区的起始地址          */
        CE_Low();
        Send_Byte(0x20);                                                   /* 发送扇区擦除指令            */
        Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));                        /* 发送3个字节的地址信息       */
        Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Send_Byte(SecnHdAddr & 0xFF);
        CE_High();
        do {
            CE_Low();
            Send_Byte(0x05);                                                 /* 发送读状态寄存器命令        */
            StatRgVal = Get_Byte();                                          /* 保存读得的状态寄存器值      */
            CE_High();
        } while (StatRgVal & 0x01 );                                       /* 一直等待，直到芯片空闲      */
        CurSecToEr  += 1;
        no_SecsToEr -=  1;
    }
    /*
    * 擦除结束,恢复状态寄存器信息
    */
    CE_Low();
    Send_Byte(0x06);                                                    /* 发送写使能命令               */
    CE_High();
    
    CE_Low();
    Send_Byte(0x50);                                                    /* 使状态寄存器可写             */
    CE_High();
    CE_Low();
    Send_Byte(0x01);                                                    /* 发送写状态寄存器指令         */
    Send_Byte(temp1);                                                   /* 恢复状态寄存器设置信息       */
    CE_High();
    return (OK);
}

#define		FLSH_ERR  36
uint8	flsherrflg = 0xff;
uint8 JudgeFlashIDErrFlg(void)   
{
	uint32_t 	GulChipID = 0;
    
	//if(GetSysTime() %100*30==1 || GetSysTime() < 60*100)
	{
	    MX25L1602_RdID(Jedec_ID, (INT32U*)&GulChipID);		                        /* 单步运行到此处时,在IAR里查看 */
        
	    GulChipID &= ~0xFF000000;                                           /* 仅保留低24位数据             */            
	    if (GulChipID != 0x00C22017)//c22015 
		{
			flsherrflg = FLSH_ERR;
	   	}
		else
		{
			flsherrflg = 0xff;
		}
	}
    
	return 	flsherrflg;
}

uint8 ReadFlsh(uint32 Addr,uint8 *buf,uint32 Len)
{	
	return	MX25L1602_RD(Addr, Len,(INT8U* )buf);
}

INT8U	WriteFlsh(INT32U Dst, uint8 * sFlshRec, INT32U NByte)
{
	return	MX25L1602_WR(Dst,(INT8U*)sFlshRec,NByte);
}


//uint8   testbuf[256];
//
//void    TestFlsh(void)
//{
//    
//    for(uint16 i = 0; i < 256; i++)
//    {
//        testbuf[i] = i;
//    }
//    
//    WriteFlsh(0,testbuf,sizeof(testbuf));
//    ReadFlsh(0,testbuf,sizeof(testbuf));
//    return;
//
//}



/*********************************************************************************************************
** End Of File
*********************************************************************************************************/

