#include <includes.h>
//#include  <stm32f10x_nvic.h>


#define	USER_APP_START_ADDR		(STM32_FLASH_BASE + 0x00010000	)


typedef  void (*pFunction)(void);			    //定义一个函数类型的参数.
pFunction   pApp;
//void __set_CONTROL(uint32_t control);


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
void JMP_Boot(u32 appAddr)
{    
    u32     JumpAddress = 0;
    u8      cpu_sr;
        
    /***********************************************
    * 描述： 保存程序地址
    */
    //IAP_SetAppAddr(appAddr);
    /***********************************************
    * 描述： 关中断，防止值被中断修改
    */
    CPU_CRITICAL_ENTER();
    /***********************************************
    * 描述： 外设恢复默认，避免进入应用程序后影响程序正常运行
    */
    //IAP_DevDeInit();  
    __disable_irq(); //关中断（）如IAP关中断 APP如果没用UCOS系统，APP

    /***********************************************
    * 描述： 获取应用入口及初始化堆栈指针
    */
    SCB->VTOR =     appAddr & 0x1FFFFF80;	//??ò??D???òá?
    JumpAddress   =*(volatile u32*) (appAddr + 4); // 地址+4为PC地址
    pApp          = (pFunction)JumpAddress;         // 函数指针指向APP
//    __set_MSP       (*(volatile u32*) appAddr);    // 初始化主堆栈指针（MSP）
//    __set_PSP       (*(volatile u32*) appAddr);    // 初始化进程堆栈指针（PSP）
    __set_CONTROL   (0);                            // 清零CONTROL
    /***********************************************
    * 描述： 跳转到APP程序
    */
    pApp();
    
    CPU_CRITICAL_EXIT();
}
//__asm void JMP_Boot( uint32_t address ){
//   LDR SP, [R0]		;Load new stack pointer address
//   LDR PC, [R0, #4]	;Load new program counter address
//}

#define zyIrqDisable()  CPU_INT_DIS()
#define zyIrqEnable()   CPU_INT_EN()


//启动app
void Boot( void )
{
//	 void (*userProgram)();   					           /*函数指针*/

	//SCB->VTOR = USER_APP_START_ADDR & 0x1FFFFF80;	//偏移中断向量
    SCB->VTOR = STM32_FLASH_BASE & 0x1FFFFF80;	//偏移中断向量
	JMP_Boot(USER_APP_START_ADDR);
//	userProgram = (void (*)()) (USER_APP_START_ADDR+1);
//	(*userProgram)();													/*启动						*/	 
	
}

void Jumpto_APP(void)
{
    INT32U IapSpInitVal;

    INT32U IapJumpAddr;

    void (*pIapFun)(void);

    RCC_DeInit();//关闭外设

//    NVIC_DeInit();

    //__disable_irq(); //关中断（）如IAP关中断 APP如果没用UCOS系统，APP

    //初始化后要开中断，用UCOS后，在起动任务后会开中断
    
    
    IapSpInitVal = *(INT32U *)USER_APP_START_ADDR;

    IapJumpAddr = *(INT32U *)(USER_APP_START_ADDR + 4);

    if((IapSpInitVal & 0x2FFE0000)==0x20000000)//检查栈顶地址是否合法.

    {
        __set_MSP (IapSpInitVal);
        //__set_MSP       (*(volatile u32*) appAddr);    // 初始化主堆栈指针（MSP）
        __set_PSP       (IapSpInitVal);    // 初始化进程堆栈指针（PSP）
        //__set_CONTROL   (0);    
        __set_CONTROL   (0);
        
        pIapFun = (void (*)(void))IapJumpAddr;

        (*pIapFun) ();

    }
}




typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   
iapfun jump2app; 
//跳转到应用程序段
//appxaddr:用户代码起始地址.
//void iap_load_app(u32 appxaddr)
//{
//	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
//	{ 
//		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
//		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
//		jump2app();									//跳转到APP.
//	}
//}		 

