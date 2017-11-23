/*******************************************************************************
 *   Filename:       iap.c
 *   Revised:        $Date: 2014-04-23
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:    
 *
 *   Notes:        
 *					   
 *			
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <global.h>
#include <iap.h>
#include <os.h>

#define BSP_IAP_MODULE_EN 1
#if BSP_IAP_MODULE_EN > 0
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
typedef  void (*pFunction)(void);			    //定义一个函数类型的参数.

StrIapState Iap  = {0};
/*******************************************************************************
 * LOCAL VARIABLES
 */
/***********************************************
* 描述：定义函数指针
*/
pFunction   pApp;

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

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
 * 名    称： IAP_DevDeInit()
 * 功    能： 设备恢复默认值
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_DevDeInit(void)
{    
    /***********************************************
    * 描述： 端口恢复默认
    */
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    
    /***********************************************
    * 描述： 串口恢复默认
    */
    USART_DeInit(USART1);
    USART_DeInit(USART2);
    USART_DeInit(USART3);
        
    /***********************************************
    * 描述： 定时器恢复默认
    */
    TIM_DeInit(TIM1);
    TIM_DeInit(TIM2);
    TIM_DeInit(TIM2);
    TIM_DeInit(TIM3);
    TIM_DeInit(TIM4);
    TIM_DeInit(TIM5);
    TIM_DeInit(TIM6);
    TIM_DeInit(TIM7);
    TIM_DeInit(TIM8);
}

/*******************************************************************************
 * 名    称： IAP_GetStatus()
 * 功    能： 获取程序状态
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_GetStatus(void)
{
    BSP_FlashReadBytes(FLASH_APP_STATUS, (u8 *)&Iap.Status, 1);
    
    if ( Iap.Status > IAP_STS_SUCCEED ) 
        Iap.Status = 0;
    
    return  Iap.Status;
}

/*******************************************************************************
 * 名    称： IAP_SetStatus()
 * 功    能： 设置程序状态
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_SetStatus(u8 status)
{
    if ( Iap.Status == status )
        return;
    Iap.Status  = status;
    BSP_FlashWriteBytes(FLASH_APP_STATUS, (u8 *)&status, 1);
}

/*******************************************************************************
 * 名    称： IAP_GetAppAddr()
 * 功    能： 获取应用程序地址
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u32 IAP_GetAppAddr(void)
{
    u32     chkAddr;
    BSP_FlashReadBytes(FLASH_APP_ADDR_SAVE, (u8 *)&chkAddr, 4);
    return  chkAddr;
}

/*******************************************************************************
 * 名    称： IAP_SetAppAddr()
 * 功    能： 设置应用程序地址
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_SetAppAddr(u32 appAddr)
{   
    /***********************************************
    * 描述： 保存程序地址
    */
    if ( appAddr != IAP_GetAppAddr() ) {
        if ( 4 == 
            BSP_FlashWriteBytes(FLASH_APP_ADDR_SAVE, (u8 *)&appAddr, 4) )
            return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_JumpTo()
 * 功    能： 跳转到应用程序段
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_JumpTo(u32 appAddr)
{    
    u32     JumpAddress = 0;
    u8      cpu_sr;
        
    /***********************************************
    * 描述： 保存程序地址
    */
    IAP_SetAppAddr(appAddr);
    /***********************************************
    * 描述： 关中断，防止值被中断修改
    */
    CPU_CRITICAL_ENTER();
    /***********************************************
    * 描述： 外设恢复默认，避免进入应用程序后影响程序正常运行
    */
    IAP_DevDeInit();  
    /***********************************************
    * 描述： 获取应用入口及初始化堆栈指针
    */
    JumpAddress   =*(volatile u32*) (appAddr + 4); // 地址+4为PC地址
    pApp          = (pFunction)JumpAddress;         // 函数指针指向APP
    __set_MSP       (*(volatile u32*) appAddr);    // 初始化主堆栈指针（MSP）
    __set_PSP       (*(volatile u32*) appAddr);    // 初始化进程堆栈指针（PSP）
    __set_CONTROL   (0);                            // 清零CONTROL
    /***********************************************
    * 描述： 跳转到APP程序
    */
    pApp();
    
    CPU_CRITICAL_EXIT();
}

/*******************************************************************************
 * 名    称： IAP_GetAppAStatus()
 * 功    能： 检查程序A是否OK
 * 入口参数：
 * 出口参数： 无
 * 作　　 者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_GetAppAStatus(void)
{
    /***********************************************
    * 描述： 检查堆栈是否合法
    */
    if (((*(volatile u32*)FLASH_ADDR_APP_A) & 0x2FFE0000 ) == 0x20000000) {
        /***********************************************
        * 描述： 关中断，防止值被中断修改
        */
        return TRUE;
    }
    
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_GetAppAStatus()
 * 功    能： 检查程序A是否OK
 * 入口参数：
 * 出口参数： 无
 * 作　　 者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_GetAppBStatus(void)
{
    /***********************************************
    * 描述： 检查堆栈是否合法
    */
    if (((*(volatile u32*)FLASH_ADDR_APP_B) & 0x2FFE0000 ) == 0x20000000) {
        /***********************************************
        * 描述： 关中断，防止值被中断修改
        */
        return TRUE;
    }
    
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_JumpToAppA()
 * 功    能： 跳转到应用程序段
 * 入口参数：
 * 出口参数： 无
 * 作　　 者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_JumpToAppA(void)
{
    /***********************************************
    * 描述： 检查堆栈是否合法
    */
    if ( IAP_GetAppAStatus() ) {
        /***********************************************
        * 描述： 关中断，防止值被中断修改
        */
        IAP_JumpTo(FLASH_ADDR_APP_A);
    }
    
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_JumpToAppB()
 * 功    能： 跳转到应用程序段
 * 入口参数：
 * 出口参数： 无
 * 作　　 者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_JumpToAppB(void)
{
    /***********************************************
    * 描述： 检查堆栈是否合法
    */
    if ( IAP_GetAppBStatus() ) {
        /***********************************************
        * 描述： 关中断，防止值被中断修改
        */
        IAP_JumpTo(FLASH_ADDR_APP_B);
    }
    
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_JumpToApp()
 * 功    能： 跳转到应用程序段地址
 * 入口参数： appxaddr:用户代码起始地址.
 * 出口参数： 无
 * 作　　 者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_JumpToAddr(u32 appAddr)
{    
    /***********************************************
    * 描述： 如果跳转程序为本身则不跳转
    */
    if ( appAddr == IAP_SELF_APP_ADDR ) {
        /***********************************************
        * 描述： 保存程序地址
        */
        IAP_SetAppAddr(appAddr);
        return FALSE;
    }
    /***********************************************
    * 描述： 检查堆栈是否合法
    */
    if (((*(volatile u32*)appAddr) & 0x2FFE0000 ) == 0x20000000) {
        IAP_JumpTo(appAddr);
    }
    
    IAP_SetStatus(IAP_STS_FAILED);
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_Programing()
 * 功    能： IAP_编程中，复位超时计数器
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2016-01-09
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_Programing(void)
{
    IAP_SetStatus(IAP_STS_PROGRAMING);
    /***********************************************
    * 描述： 串口通讯超时计数器，收到口数据时会清零，此处只增加
    */
    Iap.TimeOut = 0;
}

/*******************************************************************************
 * 名    称： IAP_Exit()
 * 功    能： 退出IAP检测
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2016-01-09
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_Exit(u8 timeout)
{    
    if ( Iap.Status == IAP_STS_DEF )
        return;
    if ( Iap.Status == IAP_STS_SUCCEED )
        return;
    if ( Iap.Status == IAP_STS_FAILED )
        return;
    /***********************************************
    * 描述： 串口通讯超时计数器，收到口数据时会清零，此处只增加
    */
    if ( Iap.TimeOut < timeout ) {
        Iap.TimeOut++;
    } else {
        /***********************************************
        * 描述： 
        */ 
        IAP_JumpToApp(0);        
        IAP_SetStatus(IAP_STS_FAILED);
        IAP_FileInfoInit();
    }
}

/*******************************************************************************
 * 名    称： IAP_Reset()
 * 功    能： IAP复位，如果是引导程序，则不复位，只更新状态
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_Reset(void)
{    
    
    IAP_FileInfoInit();                             // 初始化升级信息
    IAP_SetStatus(IAP_STS_RST); 
#if defined     (IMAGE_A)
    /***********************************************
    * 描述： 程序A
    */
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);
    NVIC_SystemReset();
#elif defined   (IMAGE_B)
    /***********************************************
    * 描述： 程序B
    */ 
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);
    NVIC_SystemReset();
#else
    /***********************************************
    * 描述： 程序引导
    */
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);
    
    /***********************************************
    * 描述： FLASH解锁
    */
    FLASH_Unlock();
    /***********************************************
    * 描述： FLASH擦除整页
    */
    for ( int i = 0; i < FLASH_SIZE_A / STM_SECTOR_SIZE; i++ ) {
        FLASH_ErasePage (FLASH_ADDR_APP_A + i * STM_SECTOR_SIZE);
    } 
    /***********************************************
    * 描述： FLASH上锁
    */ 
    FLASH_Lock(); 
#endif
    IAP_SetStatus(IAP_STS_START);  
}

/*******************************************************************************
 * 名    称： IAP_Restart()
 * 功    能： IAP复位，如果是引导程序，则不复位，只更新状态
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_Restart(void)
{    
    IAP_FileInfoInit();                          // 初始化升级信息
    IAP_SetStatus(IAP_STS_START); 
#if defined     (IMAGE_A)
    /***********************************************
    * 描述： 程序A
    */
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);
    NVIC_SystemReset();
#elif defined   (IMAGE_B)
    /***********************************************
    * 描述： 程序B
    */ 
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);
    NVIC_SystemReset();
#else
    /***********************************************
    * 描述： 程序引导
    */
    //IAP_JumpToAddr(FLASH_BOOT_ADDR);

    /***********************************************
    * 描述： FLASH解锁
    */
    FLASH_Unlock(); 
    /***********************************************
    * 描述： FLASH擦除整页
    */
    for ( int i = 0; i < FLASH_SIZE_A / STM_SECTOR_SIZE; i++ ) {
        FLASH_ErasePage (FLASH_ADDR_APP_A + i * STM_SECTOR_SIZE);
    } 
    /***********************************************
    * 描述： FLASH上锁
    */ 
    FLASH_Lock(); 
#endif   
}
/*******************************************************************************
 * 名    称： IAP_Finish()
 * 功    能： IAP升级结束
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
void IAP_Finish(void)
{        
    /***********************************************
    * 描述： FLASH上锁
    */
    //FLASH_Lock(); 
    IAP_SetStatus(IAP_STS_FINISH);
    //return;
    
#if defined     (IMAGE_A)
    /***********************************************
    * 描述： 程序A
    */
    IAP_JumpToAddr(FLASH_ADDR_APP_B);
#elif defined   (IMAGE_B)
    /***********************************************
    * 描述： 程序B
    */ 
    IAP_JumpToAddr(FLASH_ADDR_APP_A);
#else
    /***********************************************
    * 描述： 程序引导
    */
    IAP_JumpToAddr(FLASH_ADDR_APP_A);
#endif
    IAP_FileInfoInit();                          // 初始化升级信息
}

/*******************************************************************************
 * 名    称： IAP_JumpToApp()
 * 功    能： 跳转到应用程序段
 *            跳转逻辑：检查升级状态，如果升级状态为成功或未知，则跳转到B程序，
 *            其他状态不跳转
 * 入口参数：
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改： 
 * 修改日期： 
 *******************************************************************************/
u8 IAP_JumpToApp(u32 flag)
{
    u8  status  = IAP_GetStatus();
    u32 appAddr = IAP_GetAppAddr();
    
    if ( flag == 1 ) {
        IAP_FileInfoInit();
    }
    
    switch ( status ) { 
    /***********************************************
    * 描述： 0未升级
    */
    case IAP_STS_DEF:
#if defined     (IMAGE_A)
        /***********************************************
        * 描述： 程序A
        */
#elif defined   (IMAGE_B)
        /***********************************************
        * 描述： 程序B
        */
#else
        /***********************************************
        * 描述： 程序引导
        */
        //IAP_JumpToAddr(FLASH_ADDR_APP_A);
#endif   
        break;
    /***********************************************
    * 描述： 1重启
    */
    case IAP_STS_RST:
        IAP_SetStatus(IAP_STS_START);
        IAP_FileInfoInit();                          // 初始化升级信息
        break;
    /***********************************************
    * 描述： 2开始
    */
    case IAP_STS_START:
        IAP_SetStatus(IAP_STS_DEF);
#if defined     (IMAGE_A)
        /***********************************************
        * 描述： 程序A
        */
#elif defined   (IMAGE_B)
        /***********************************************
        * 描述： 程序B
        */
#else
        /***********************************************
        * 描述： 程序引导
        */
#endif
        break;
    /***********************************************
    * 描述： 3升级中
    */
    case IAP_STS_PROGRAMING:
#if defined     (IMAGE_A)
        /***********************************************
        * 描述： 程序A
        */
#elif defined   (IMAGE_B)
        /***********************************************
        * 描述： 程序B
        */
#else
        /***********************************************
        * 描述： 程序引导
        */
#endif  
        //IAP_SetStatus(IAP_STS_SUCCEED);
        //IAP_JumpToAddr(appAddr);
        //IAP_JumpToAddr(FLASH_ADDR_APP_A);
        IAP_SetStatus(IAP_STS_FAILED);
        break;
    /***********************************************
    * 描述： 4结束
    */
    case IAP_STS_FINISH:
        Iap.TimeOut = 0;
        IAP_SetStatus(IAP_STS_SUCCEED);
        //IAP_JumpToAddr(appAddr);
        IAP_JumpToAddr(FLASH_ADDR_APP_A);
        IAP_SetStatus(IAP_STS_FAILED);
        break;
    /***********************************************
    * 描述： 5失败
    */
    case IAP_STS_FAILED:
        Iap.TimeOut = 0;
        break;
    /***********************************************
    * 描述： 6成功
    */
    case IAP_STS_SUCCEED:
#if defined     (IMAGE_A)
        /***********************************************
        * 描述： 程序A
        */
#elif defined   (IMAGE_B)
        /***********************************************
        * 描述： 程序B
        */
#else
        /***********************************************
        * 描述： 程序引导
        */
        //IAP_SetStatus(IAP_STS_SUCCEED);
        IAP_JumpToAddr(appAddr);
        IAP_JumpToAddr(FLASH_ADDR_APP_A);
        IAP_SetStatus(IAP_STS_FAILED);
#endif  
        break;
    /***********************************************
    * 描述： 其他：无程序
    */
    default:
        IAP_SetStatus(IAP_STS_DEF);
        //IAP_JumpToAddr(FLASH_ADDR_APP_A);
        break;
    }
    return FALSE;
}

/*******************************************************************************
 * 名    称： IAP_Program
 * 功    能： 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 
 * 入口参数： 
 * 出口参数： 
 * 作　　者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改：
 * 修改日期：
 *******************************************************************************/	
BOOL IAP_FileInfoInit(void)
{   
    Iap.Sectors     = Iap.FileSize / STM_SECTOR_SIZE;       // 整扇区数
    Iap.FileSize    = Iap.FileSize % STM_SECTOR_SIZE;       // 余字节数
    Iap.Sectors    += Iap.FileSize > 0 ? 1 : 0;             // 如果余字数不为0，扇区数+1
    Iap.FrameIdx    = 0;
    Iap.SysNbr      = 0;
    Iap.WrittenSize = 0;
    
#if defined     (IMAGE_A)
    /***********************************************
    * 描述： 程序A   
    */
    INT32U num_sector   = (FLASH_ADDR_APP_B - STM32_FLASH_BASE) / STM_SECTOR_SIZE;
    Iap.SectorAddr      = num_sector * STM_SECTOR_SIZE + STM32_FLASH_BASE;  // 获取超始页
#elif defined   (IMAGE_B)
    /***********************************************
    * 描述： 程序B
    */
    INT32U num_sector   = (FLASH_ADDR_APP_A - STM32_FLASH_BASE) / STM_SECTOR_SIZE;
    Iap.SectorAddr      = num_sector * STM_SECTOR_SIZE + STM32_FLASH_BASE;  // 获取超始页
#else
    /***********************************************
    * 描述： 程序引导
    */
    INT32U num_sector   = (FLASH_ADDR_APP_A - STM32_FLASH_BASE) / STM_SECTOR_SIZE;
    Iap.SectorAddr      = num_sector * STM_SECTOR_SIZE + STM32_FLASH_BASE;  // 获取超始页
#endif    
    Iap.pBuf        = (INT16U *)FlashBuf.buf2;
    
    return TRUE;
}

/*******************************************************************************
 * 名    称： IAP_Program
 * 功    能： 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 
 * 入口参数： 
 * 出口参数： 
 * 作　　者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改：
 * 修改日期：
 *******************************************************************************/	
//BOOL IAP_Program(StrIapState *iap, INT16U *buf, INT16U len, INT16U idx )
//{  
//    /***********************************************
//    * 描述： 
//    */
//    int     i   = 0;
//    INT08U  add = 0;                                // 奇数字节数，长度补1
//    
//    /***********************************************
//    * 描述： 将值放入写FLASH缓冲区
//    */
//    for( ; i < len/2 ; i++) {
//        iap->pBuf[i] = buf[i];
//    }
//    if ( ( len % 2 == 1 ) && ( i < 64) ) {
//        add = 1;
//        iap->pBuf[i] = ( buf[i] & 0xff ) | 0xff00;
//    }
//    /***********************************************
//    * 描述： FLASH解锁
//    */
//    FLASH_Unlock(); 
//    
//    /***********************************************
//    * 描述： FLASH页首地址时整页擦除
//    *
//    if( ( iap->FrameIdx % 16 == 0 ) ||
//        ( iap->SectorAddr % STM_SECTOR_SIZE == 0 ) ) {
//        /***********************************************
//        * 描述： FLASH擦除整页
//        * 
//        if ( FLASH_COMPLETE != FLASH_ErasePage (iap->SectorAddr) ) {
//            //FLASH_Lock(); 
//            //return FALSE;
//        }
//    }
//    /***********************************************
//    * 描述： 直接将要写入的数据写入flash
//    */ 
//    if ( TRUE == FlashWriteNoCheck(iap->SectorAddr,iap->pBuf, len/2 + add) ) {
//    }
//        
//    if ( ( Iap.SysNbr == 0 ) || ( Iap.SysNbr != idx ) ) {
//        Iap.SysNbr = idx;
//        /***********************************************
//        * 描述： 计数值增加
//        */ 
//        iap->SectorAddr  += len;
//        iap->WrittenSize += len;
//        iap->FrameIdx++;
//    }
//    /***********************************************
//    * 描述： FLASH上锁
//    */ 
//    FLASH_Lock(); 
//    return TRUE;
//}
/*******************************************************************************
 * 名    称： IAP_Program
 * 功    能： 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 
 * 入口参数： 
 * 出口参数： 
 * 作　　者： 无名沈
 * 创建日期： 2014-04-23
 * 修    改：
 * 修改日期：
 *******************************************************************************/	
BOOL IAP_Program(StrIapState *iap, INT16U *buf, INT16U len, INT16U idx )
{  
    /***********************************************
    * 描述： 
    */
    INT08U  add = 0;                                // 奇数字节数，长度补1
    
    /***********************************************
    * 描述： 将值放入写FLASH缓冲区
    */
    if ( len % 2 == 1 ) {
        add         = 1;
        buf[len]    = 0xff;
    }
    /***********************************************
    * 描述： FLASH解锁
    */
    FLASH_Unlock(); 
    
    /***********************************************
    * 描述： 直接将要写入的数据写入flash
    */ 
    if ( TRUE == FlashWriteNoCheck(iap->SectorAddr,buf, len/2 + add) ) {
    }
        
    /***********************************************
    * 描述： FLASH上锁
    */ 
    FLASH_Lock(); 
    if ( iap->FrameIdx+1 == idx ) {
        /***********************************************
        * 描述： 计数值增加
        */ 
        iap->SectorAddr  += len;
        iap->WrittenSize += len;
        iap->FrameIdx++;
    }
    return TRUE;
}

typedef struct {
    INT08U      Cmd;                    // 升级命令
    INT08U      Sts;                    // 状态
    INT16U      HwVer;                  // 硬件版本
    INT16U      SwVer;                  // 软件版本
    INT32U      FileSize;               // 程序大小
    INT32U      CurAddr;                // 当前地址
    INT16U      CurIdx;                 // 当前帧号
    INT32U      Chk;                    // 校验信息
} StrConfirm;   
StrConfirm IapConfirm;

/*******************************************************************************
 * 名    称： 首字母大写
 * 功    能： 升级数据接收及处理
 * 入口参数： 无
 * 出口参数： 无
 * 作    者： 无名沈
 * 创建日期： 2017/8/19
 * 修    改： 
 * 修改日期： 
 * 备    注： 
 *******************************************************************************/
INT08U IAP_CommRxDataDealCB(MODBUS_CH  *pch, StrNcProtocol  *pack)
{
    INT08U         *pdat        = pack->Data;
    INT08U          status      = pdat[1];
    INT08U          addr        = pack->DstAddr;
    
    pack->DstAddr   = pack->SrcAddr;
    pack->SrcAddr   = addr;
    /***********************************************
    * 描述： 2017/8/19：升级命令解析
    */
    switch ( status ) {
    /***********************************************
    * 描述： 升级握手信息
    */
    case IAP_CMD_START: 
        StrConfirm     *pConfirm    = (StrConfirm *)pdat; 
        
        /***********************************************
        * 描述： 重新开始下载，从零开始
        */
        if( pConfirm->Sts == IAP_SR_START_START ) {
            /***********************************************
            * 描述： 硬件版本不一致
            */
            if ( IapConfirm.HwVer != pConfirm->HwVer ) {
                pConfirm->Sts   = 3;
                break;
            }
            IapConfirm.FileSize = pConfirm->FileSize;
            IapConfirm.CurAddr  = pConfirm->CurAddr = 0;
            IapConfirm.CurIdx   = pConfirm->CurIdx  = 0;
            IapConfirm.Chk      = pConfirm->Chk;
            
            pConfirm->Sts       = 1;
            Iap.FrameIdx        = 0;
            Iap.CurAddr         = FLASH_ADDR_APP_A;
            Iap.CurIdx          = 0;
        /***********************************************
        * 描述： 续传
        */
        } else if ( pConfirm->Sts == IAP_SR_START_CONTINUE ) {
            /***********************************************
            * 描述： 硬件版本不一致
            */
            if ( IapConfirm.HwVer != pConfirm->HwVer ) {
                pConfirm->Sts   = IAP_SR_START_ERR_HV;
                break;
            }
            if ( IapConfirm.FileSize != pConfirm->FileSize ) {
                pConfirm->Sts   = IAP_SR_START_ERR_SV;
                break;
            }
            if ( IapConfirm.CurAddr != pConfirm->CurAddr ) {
                pConfirm->Sts   = IAP_SR_START_ERR_ADDR;
                break;
            }
            if ( IapConfirm.CurIdx != pConfirm->CurIdx ) {
                pConfirm->Sts   = IAP_SR_START_ERR_IDX;
                break;
            }
            if ( IapConfirm.Chk != pConfirm->Chk ) {
                pConfirm->Sts   = IAP_SR_START_ERR_CHK;
                break;
            }
            
            pConfirm->Sts   = IAP_SR_START_OK;
            Iap.FrameIdx    = 0;
            Iap.CurAddr     = IAP_ADDR_BACK;
            Iap.CurIdx      = 0;
        } else {
            
        }
        
        //BSP_FlashReadBytes(FLASH_APP_STATUS, (u8 *)&status, 1);
        //BSP_FlashWriteBytes(FLASH_APP_STATUS, (u8 *)&status, 1);
        
        break;
    /***********************************************
    * 描述： 2017/8/19：升级程序信息
    */
    case IAP_CMD_PRG: {
        /***********************************************
        * 描述： 2017/8/19：写数据到Flash
        */
        INT16U  idx = BUILD_INT16U(pack->Data[2],pack->Data[3]);
        IAP_Program((StrIapState   *)&Iap, 
                    (INT16U        *)&pack->Data[4], 
                    (INT16U         ) pack->Len, idx);
        pack->Data[4]   = 0;
        pack->Data[5]   = 0;
        pack->Len       = 6;
        pack->SendSn    = pack->RecvSn;
    } break;
    /***********************************************
    * 描述： 2017/8/19：升级结束信息
    */
    case IAP_CMD_FINISH:        
        /***********************************************
        * 描述： 2017/8/19：不运行
        */
        if( pConfirm->Sts == IAP_SR_FINISH_NO_RUN ) {
            /***********************************************
            * 描述： 硬件版本不一致
            */
            if ( IapConfirm.HwVer != pConfirm->HwVer ) {
                pConfirm->Sts   = IAP_SR_START_ERR_HV;
                break;
            }
            IapConfirm.FileSize = pConfirm->FileSize;
            IapConfirm.CurAddr  = pConfirm->CurAddr = 0;
            IapConfirm.CurIdx   = pConfirm->CurIdx  = 0;
            IapConfirm.Chk      = pConfirm->Chk;
            
            pConfirm->Sts       = 1;
            Iap.FrameIdx        = 0;
            Iap.CurAddr         = FLASH_ADDR_APP_A;
            Iap.CurIdx          = 0;
        /***********************************************
        * 描述： 2017/8/19：立即运行
        */
        } else if ( pConfirm->Sts == IAP_SR_FINISH_RUN ) {
            /***********************************************
            * 描述： 硬件版本不一致
            */
            if ( IapConfirm.HwVer != pConfirm->HwVer ) {
                pConfirm->Sts   = IAP_SR_START_ERR_HV;
                break;
            }
            if ( IapConfirm.FileSize != pConfirm->FileSize ) {
                pConfirm->Sts   = IAP_SR_START_ERR_SV;
                break;
            }
            if ( IapConfirm.CurAddr != pConfirm->CurAddr ) {
                pConfirm->Sts   = IAP_SR_START_ERR_ADDR;
                break;
            }
            if ( IapConfirm.CurIdx != pConfirm->CurIdx ) {
                pConfirm->Sts   = IAP_SR_START_ERR_IDX;
                break;
            }
            if ( IapConfirm.Chk != pConfirm->Chk ) {
                pConfirm->Sts   = IAP_SR_START_ERR_CHK;
                break;
            }
            
            pConfirm->Sts   = IAP_SR_START_OK;
            Iap.FrameIdx    = 0;
            Iap.CurAddr     = IAP_ADDR_BACK;
            Iap.CurIdx      = 0;
        /***********************************************
        * 描述： 2017/8/19：重启后运行
        */
        } else if ( pConfirm->Sts == IAP_SR_FINISH_RESTART ) {
            
        } else {
            
        }
        IAP_Reset();
        break;
    /***********************************************
    * 描述： 2017/8/19：升级中止或暂停
    */
    case IAP_CMD_STOP:
        break;
    /***********************************************
    * 描述： 其他信息
    */
    default:
        break;
    }
#if defined     (IMAGE_A) || defined   (IMAGE_B)

#else 
    
#endif

    /***********************************************
    * 描述： 发送数据处理
    */    
    CSNC_SendData( (MODBUS_CH      *) pch,
                   (StrNcProtocol  *) pack);
    return TRUE;
}

/*******************************************************************************
 * 				end of file
 *******************************************************************************/ 
#endif