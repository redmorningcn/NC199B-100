#include  "includes.h"
#include  "IAP_program_mcu.h"
#include  "app_ctrl.h"

//#define	CVI	0

//#define	USER_APP_START_ADDR		0x00020000	
//#define	USER_APP_PRO_SIZE		0x0000FFFF	

#define	IAP_DATA_LEN		    128
#define SEC_DIV_TIMENS          (IAP_WRITE_1024/IAP_DATA_LEN)

//存放IAP参数地址
//#define	IAP_PARA_START_ADDR     0x00070000	
//#define	IAP_PARA_PRO_SIZE		0x00010000	
//#define	IAP_PARA_START_ADDR     0x00030000	
//#define	IAP_PARA_PRO_SIZE		0x0000FFFF	



#define	USER_APP_START_ADDR		(STM32_FLASH_BASE + 0x00020000	)
#define	USER_APP_PRO_SIZE		0x0000FFFF	

//#define	USER_BACK_START_ADDR	(STM32_FLASH_BASE + 0x00020000	)

#define IAP_WRITE_1024          1024
#define	IAP_PARA_START_ADDR     (STM32_FLASH_BASE + 0x00030000)
#define	IAP_PARA_PRO_SIZE		0x0000FFFF	


#define IAP_START_CODE              1
#define IAP_DATA_CODE               2
#define IAP_END_CODE                3

#define  IAP_WRITE_1024             1024


#define zyIrqDisable()  __disable_irq()
#define zyIrqEnable()   __enable_irq()


void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);


/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
stcIAPPara  gsIAPPara;
stcIAPCtrl  gsIAPCtrl;      //升级参数

///*******************************************************************************
// * 名    称： IAP_ReadDeviceInfo
// * 功    能： 读芯片信息
// * 入口参数： 无
// * 出口参数： 无
// * 作　　者： redmorningcn.
// * 创建日期： 2017-08-08
// * 修    改：
// * 修改日期：
// *******************************************************************************/
//void    IAP_ReadDeviceInfo(void)
//{
//    IAP_STATUS_CODE status;    
//    uint8_t ver_major, ver_minor; 
//    uint32_t result[4];
//
//    zyIrqDisable();										//关中断
//    
//    status = ReadPartID(result);                        //读芯片ID
//    status = ReadBootCodeVer(&ver_major, &ver_minor);   //读启动代码版本
//    status = ReadDeviceSerialNum(result);               //读设备ID号    
//    
//    zyIrqEnable();                                      //写flash完成时，开全局中断
//}

/*******************************************************************************
 * 名    称： IAP_WriteParaFlash
 * 功    能： 将IAP参数数据写入Flash区
 * 入口参数： stcIAPCtrl
 * 出口参数： 无
 * 作　　者： redmorningcn.
 * 创建日期： 2017-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void    IAP_WriteParaFlash(stcIAPPara *sIAPPara)
{
//    IAP_STATUS_CODE status;
//    uint32_t result[4];
//
//    uint32_t flash_prog_area_sec_start;
//    uint32_t flash_prog_area_sec_end;
    
    zyIrqDisable();										                                //关中断

//    flash_prog_area_sec_start   = 	GetSecNum(IAP_PARA_START_ADDR);
//    flash_prog_area_sec_end 	=  	GetSecNum(IAP_PARA_START_ADDR + IAP_PARA_PRO_SIZE); //准备擦除
//
//    status = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end);           //一次所写区间
//    status = BlankCheckSector(flash_prog_area_sec_start, flash_prog_area_sec_end,
//                &result[0], &result[1]);
//    
//    //开始flash编写,写入整块数据
//    status 	= CopyRAM2Flash( (uint8_t *)IAP_PARA_START_ADDR,
//                                  (uint8 *)sIAPPara,
//                                   256
//                            );                                                          //拷贝数据
    
     iap_write_appbin(     IAP_PARA_START_ADDR,
                            (uint8 *)sIAPPara,
                            256
                            );              
//    status =  Compare((uint8_t *)IAP_PARA_START_ADDR,
//                            (uint8 *)sIAPPara,
//                            sizeof(stcIAPPara)
//                    );                                                                  //比较数据
            
    zyIrqEnable();                                                                      //写flash完成时，开全局中断
}


/*******************************************************************************
 * 名    称： IAP_WriteParaFlash
 * 功    能： 将IAP参数数据写入Flash区
 * 入口参数： stcIAPCtrl
 * 出口参数： 无
 * 作　　者： redmorningcn.
 * 创建日期： 2017-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void    IAP_ReadParaFlash(stcIAPPara *sIAPPara)
{
//    IAP_STATUS_CODE status;
//    uint32_t result[4];
//    
//    uint32_t flash_prog_area_sec_start;
//    uint32_t flash_prog_area_sec_end;
    
    zyIrqDisable();                                 //关中断
                                                    //数据copy
    memcpy((uint8 *)sIAPPara,(uint8 *)IAP_PARA_START_ADDR,sizeof(stcIAPPara));
    
            
    zyIrqEnable();                                  //开全局中断
}

/*******************************************************************************
 * 名    称： IAP_WriteFlash
 * 功    能： 将数据写入Flash区
 * 入口参数： stcIAPCtrl
 * 出口参数： 无
 * 作　　者： redmorningcn.
 * 创建日期： 2017-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
void    IAP_WriteFlash(stcIAPCtrl * sIAPCtrl)
{
//    IAP_STATUS_CODE status;
//    uint32_t result[4];
//
//    uint32_t flash_prog_area_sec_start;
//    uint32_t flash_prog_area_sec_end;
    
    zyIrqDisable();										    //关中断

    //如果从程序下载地址开始，则先插除所有IAP地址数据。
//    if(sIAPCtrl->addr == USER_APP_START_ADDR)
//    {
//        flash_prog_area_sec_start   = 	GetSecNum(USER_APP_START_ADDR);
//        flash_prog_area_sec_end 	=  	GetSecNum(USER_APP_START_ADDR + USER_APP_PRO_SIZE);//准备擦除
//
//        status = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end);           //一次性擦除所有
//
//        status = BlankCheckSector(flash_prog_area_sec_start, flash_prog_area_sec_end,
//                      &result[0], &result[1]);
//    }
//    
    //开始flash编写
//    status 	= CopyRAM2Flash((uint8 *)sIAPCtrl->addr,
//                                     sIAPCtrl->buf,
//                                     IAP_WRITE_1024
//                            );                      //拷贝数据
//    
//    status =  Compare((uint8 *) sIAPCtrl->addr,
//                                sIAPCtrl->buf,
//                                IAP_WRITE_1024);    //比较数据
    
    iap_write_appbin(                sIAPCtrl->addr,
                                     (u8 *)sIAPCtrl->buf,
                                     IAP_WRITE_1024
                            );
    
//   sIAPCtrl->addr += IAP_WRITE_1024;              //数据地址累加  
//      sIAPCtrl->addr += IAP_WRITE_1024;              //数据地址累加  
        
    zyIrqEnable();                                  //写flash完成时，开全局中断
}
                       
                       

/*******************************************************************************
 * 名    称： IAP_PragramDeal
 * 功    能： 对传入数据处理
 * 入口参数： 数据区，数据长度
 * 出口参数： 指令成功，返回0；（发在指令后一位返回）
        否则，返回错误值
                1，发送帧序号不连续
                2，指令不识别
                3，程序大小不符
 * 作　　者： redmorningcn.
 * 创建日期： 2017-08-08
 * 修    改：
 * 修改日期：
 *******************************************************************************/
int8    IAP_PragramDeal(uint8 *databuf,char datalen)
{
    uint16  iapcode;                                        //升级命令字
    uint16  iapnum;                                         //升级序号
//    uint16  crc16;
    static  uint16  lastiapnum = 0;                         //上一帧序号
//    static  uint8   times = 0;
    static  uint16  bufsize = 0;
    stcIAPPara  sIAPParatmp;

    if(datalen < 2 || datalen > 250)                                //数据异常，退出
        return  0;
    
    memcpy((char *)&iapcode,databuf,sizeof(iapcode));              //取升级命令字
    databuf[1] = 0;                                                 //返回状态标示，默认正常
    
    switch(iapcode & 0xff)                                          //地8位指令区
    {
        case IAP_START_CODE:                                        //开始升级指令（考虑断续传）
                                                                    //端点续传，更改地址。（如需考虑，根据序号计算地址）
            memcpy(&gsIAPPara,&databuf[sizeof(iapcode)],2+2+4+4+2); //cpoy硬件版本，软件版本，程序大小，当前地址，当前帧号
            
            lastiapnum = 0;
            ////////////////断点续传，读出已存信息
            IAP_ReadParaFlash(&sIAPParatmp);
            
            gsIAPPara.softver = Ctrl.Para.dat.SoftWareID;                   //返回版本信息
            
            if(
                        sIAPParatmp.softsize == gsIAPPara.softsize  //软件大小
                   &&   gsIAPPara.framenum > 20                    //开始包序号不为0  ( >20  20171226调整，防止数据位保存，而上位机认为已经保存了）  
               ) 
            {                                                       //数据未接收完成，发送当前序号
                gsIAPPara.framenum  = sIAPParatmp.framenum;         //返回当前序号
                lastiapnum          = sIAPParatmp.framenum;         //下载序号。断点续传 
                gsIAPCtrl.addr      = sIAPParatmp.addr;
                gsIAPPara.addr      = gsIAPCtrl.addr;
                
            }else
            {
                gsIAPPara.framenum  = 0;                            //信息不正确，
                lastiapnum = 0;
                gsIAPCtrl.addr = USER_APP_START_ADDR;               //开始发送时，初始化地址。
            }
            
            memcpy(&databuf[sizeof(iapcode)],&gsIAPPara,2+2+4+4+2); //cpoy硬件版本，软件版本，程序大小，当前地址，当前帧号

            gsIAPPara.code = 0x00;                                  //升级有效指令变为0，需要全部下载完成，才能改写成功1            
            IAP_WriteParaFlash(&gsIAPPara);

            break;
        case IAP_DATA_CODE:                                         //传输数据包
            memcpy((char *)&iapnum,&databuf[sizeof(iapcode)],sizeof(iapnum));   //取帧序号
            
            if(     iapnum == lastiapnum+1 
                ||  iapnum == lastiapnum )                  //相同帧号（重发数据）
            {
                memcpy(&gsIAPCtrl.buf[(iapnum % SEC_DIV_TIMENS)*IAP_DATA_LEN],
                &databuf[2 + 2],                            //数据区是从第4字节开始
                datalen - 4);                                //拷贝数据到升级缓冲区

                bufsize += datalen - 4;                     //4字节代码，数据区前标示
                                                            //准备数据
                if(     (iapnum % SEC_DIV_TIMENS ) == (SEC_DIV_TIMENS - 1) 
                    || (datalen -4) != IAP_DATA_LEN )       //如果数据凑满1024字节，或者升级结束。进行写flash操作。
                {
                    if((datalen - 4) != IAP_DATA_LEN)       //如果升级结束，将1024字节剩余空间写0xff
                    {
                        for(int i = bufsize;i < IAP_WRITE_1024;i++ )
                            gsIAPCtrl.buf[i] = 0xff;	
                    }

                    if(iapnum != lastiapnum && iapnum )     //重复接收退出
                    {
                        if(gsIAPCtrl.addr >= USER_APP_START_ADDR)
                        {
                            IAP_WriteFlash(&gsIAPCtrl);        //写数据(地址，gsIAPCtrl.addr依次写入)
                            gsIAPCtrl.addr += IAP_WRITE_1024;  //数据地址累加  
                             
                            gsIAPPara.addr =  gsIAPCtrl.addr;  //已写地址
                             
                            bufsize = 0;
                        
                            gsIAPPara.code      = 0x00;         //未完成，存序号
                            gsIAPPara.framenum  = iapnum;       //当前序号
                            gsIAPPara.crc16 = GetCrc16Check((uint8 *)&gsIAPPara,sizeof(gsIAPPara)-2);
                            IAP_WriteParaFlash(&gsIAPPara);
                        }
                        else
                        {
                            databuf[1] = 4;                             //返回状态
                            return 4;                            
                        }
                           
                    }
                    
                    bufsize = 0;                                //重复数据清空（满足写条件的，不论是否已写，都清空缓冲）
                }
                
                lastiapnum = iapnum;                            //序号赋值
            }
            else
            {
                databuf[1] = 1;
                return 1; 
            }
        
            break;
            
         //结束升级指令   //做升级后程序完整性判断。程序大小？ //写程序完成标示
        case IAP_END_CODE:                                          
                                                            
            if( bufsize )                                   //最后的数据未写入，补充写入
            {
                 IAP_WriteFlash(&gsIAPCtrl);                //写数据(地址，gsIAPCtrl.addr依次写入)
                 gsIAPCtrl.addr += IAP_WRITE_1024;          //数据地址累加  

                 gsIAPPara.addr =  gsIAPCtrl.addr;          //已写地址
                 bufsize = 0;
            }
                
            memcpy(&gsIAPPara,&databuf[sizeof(iapcode)],2+2+4+4+2); //cpoy硬件版本，软件版本，程序大小，当前地址，当前帧号
           
            if(     gsIAPPara.softsize > (gsIAPCtrl.addr - USER_APP_START_ADDR)     //程序大小不符，软件退出。
               ||   (gsIAPCtrl.addr - USER_APP_START_ADDR - IAP_WRITE_1024) > gsIAPPara.softsize
               )
            {
                databuf[1] = 3;                             //返回状态
                return 3;
            }
            
            //如果程序大小相符，认为下载正确。修改下载成功标示
            gsIAPPara.code = 0x01;
            gsIAPPara.framenum  = 0;                        //恢复开始状态
            gsIAPPara.crc16 = GetCrc16Check((uint8 *)&gsIAPPara,sizeof(gsIAPPara)-2);
            IAP_WriteParaFlash(&gsIAPPara);
            IAP_ReadParaFlash(&gsIAPPara);
            
            
            break;
        case 0x04:                                          //终止升级指令
            gsIAPCtrl.addr = USER_APP_START_ADDR;           //初始化地址。

            break;  
        default:                                            //其他指令，直接返回
            databuf[1] = 2;
            return 2;
    }
    
    databuf[1] = 0;
    return 0;
}

//
//typedef  void (*pFunction)(void);			                //定义一个函数类型的参数.
//
///*******************************************************************************
// * LOCAL VARIABLES
// */
///***********************************************
//* 描述：定义函数指针
//*/
//pFunction   pApp;
//
//void Boot( void )
//{
//    u32     JumpAddress = 0;
//    u8      cpu_sr;
//    //SCB->VTOR = USER_APP_START_ADDR & 0x1FFFFF80;	
//
//	//JMP_Boot(USER_APP_START_ADDR);
//    
////   __ASM void JMP_Boot( uint32_t address ){
////   LDR SP, [R0]		;Load new stack pointer address
////   LDR PC, [R0, #4]	;Load new program counter address
////   }
//
//    CPU_CRITICAL_ENTER();
//
//    JumpAddress   =*(volatile u32*) (USER_APP_START_ADDR + 4); // 地址+4为PC地址
//    pApp          = (pFunction)JumpAddress;                     // 函数指针指向APP
//    __set_MSP       (*(volatile u32*) USER_APP_START_ADDR);    // 初始化主堆栈指针（MSP）
//    __set_PSP       (*(volatile u32*) USER_APP_START_ADDR);    // 初始化进程堆栈指针（PSP）
//    __set_CONTROL   (0);                                        // 清零CONTROL
//    
//    pApp();                                                     //跳转运行
//    
//    //(*pApp)();
//    
//    CPU_CRITICAL_EXIT();
//
//}
//
//void    Restart(void)
//{
//    u32     JumpAddress = 0;
//    u8      cpu_sr;
//    //SCB->VTOR = USER_APP_START_ADDR & 0x1FFFFF80;	
//
//	//JMP_Boot(USER_APP_START_ADDR);
//    
////   __ASM void JMP_Boot( uint32_t address ){
////   LDR SP, [R0]		;Load new stack pointer address
////   LDR PC, [R0, #4]	;Load new program counter address
////   }
//
//    CPU_CRITICAL_ENTER();
//
//    JumpAddress   =*(volatile u32*) (0 + 4);    // 地址+4为PC地址
//    pApp          = (pFunction)JumpAddress;      // 函数指针指向APP
//    __set_MSP       (*(volatile u32*) 0);       // 初始化主堆栈指针（MSP）
//    __set_PSP       (*(volatile u32*) 0);       // 初始化进程堆栈指针（PSP）
//    __set_CONTROL   (0);                                        // 清零CONTROL
//    
//    pApp();                                                     //跳转运行
//    
//    //(*pApp)();
//    
//    CPU_CRITICAL_EXIT();
//}

