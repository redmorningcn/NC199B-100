/*******************************************************************************
 *   Filename:       app_task_wifi.c
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ˫��ѡ�� wifi �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Wifi �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� WIFI �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_WIFI_PRIO     ��
 *                                            �� �����ջ�� APP_TASK_WIFI_STK_SIZE ����С
 *
 *   Notes:
 *     				E-mail:261313062@qq.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_comm__c = "$Id: $";
#endif

#define APP_TASK_WIFI_EN     DEF_ENABLED
#if APP_TASK_WIFI_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskWifiTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskWifiStk[ APP_TASK_WIFI_STK_SIZE ];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/***************************************************
* ������ ��ʱ������
*/
OS_TMR          OSTmr1;                     //��ʱ��1

/***********************************************
* ������ ��ʱ���ص�����
*/
void     OSTmr1_callback         (OS_TMR *ptmr,void *p_arg);

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static  void    AppTaskWifi          (void *p_arg);
static  void    APP_WifiInit         (void);
static  void    App_McuStatusInit           (void);
static  void    ReportDevStatusHandle       (void);
static  void    InformWifiConfigMode        (u8 mode);
/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
//UART_HandleTypeDef         				UartHandle;
//Pro_Wait_AckTypeDef           	        Ctrl.Wifi.WaitAck;
////Device_WirteTypeDef   					Device_WirteStruct;
////Device_ReadTypeDef                      Device_ReadStruct;
//
//Pro_M2W_ReturnInfoTypeDef  				Ctrl.Wifi.M2wRetInfo;
//Pro_CommonCmdTypeDef      			 	Ctrl.Wifi.CommCmd;
//Pro_W2D_WifiStatusTypeDef     	 	    Ctrl.Wifi.W2dWifiSts;
//Pro_CommonCmdTypeDef     	 			Ctrl.Wifi.M2wRstCmd;
//Pro_D2W_ConfigWifiTypeDef       	    Ctrl.Wifi.D2wCfgCmd;
//Pro_D2W_ReportDevStatusTypeDef   	    Ctrl.Wifi.D2wRptSts;
//
//AttrCtrlTypeDef                         AttrCtrl;
//uint8_t									SN = 0;
//uint8_t                                 Set_LedStatus = 0;


/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskWifiCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskWifiCreate(void)
{
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskWifiTCB,              // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Wifi",              // ��������
                 (OS_TASK_PTR ) AppTaskWifi,                 // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                                  // ����������
                 (OS_PRIO     ) APP_TASK_WIFI_PRIO,          // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskWifiStk[0],           // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_WIFI_STK_SIZE / 10, // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_WIFI_STK_SIZE,      // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                                 // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                                 // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                                  // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |               // �����ջ��������
                                OS_OPT_TASK_STK_CLR),               // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                               // ָ���������ָ�룬���ڴ����������

}

/*******************************************************************************
 * ��    �ƣ� AppTaskWifi
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static  void  AppTaskWifi (void *p_arg)
{
    OS_ERR      err;

    OS_TICK     dly;
    OS_TICK     ticks;
    /***********************************************
    * ������ �����ʼ��
    */
    APP_WifiInit();
    /***********************************************
    * ������ Task body, always written as an infinite loop.
    */
    while (DEF_TRUE) { 
        /***********************************************
        * ������ �������Ź���־��λ
        */
        OS_FlagPost(( OS_FLAG_GRP *)&WdtFlagGRP,
                    ( OS_FLAGS     ) WDTFLAG_Wifi,
                    ( OS_OPT       ) OS_OPT_POST_FLAG_SET,
                    ( CPU_TS       ) 0,
                    ( OS_ERR      *) &err);        
        
        /***********************************************
        * ������ �ȴ�WIFI������־λ
        */
        OS_FLAGS    flags = 
        OSFlagPend( ( OS_FLAG_GRP *)&Ctrl.Os.WifiEvtFlagGrp,
                    ( OS_FLAGS     ) Ctrl.Os.WifiEvtFlag,
                    ( OS_TICK      ) dly,                                       //��ʱ�ȴ�PendTimeOutMS ms
                    ( OS_OPT       ) OS_OPT_PEND_FLAG_SET_ANY,                  //�κ�һλ��һ
                    ( CPU_TS      *) NULL,
                    ( OS_ERR      *)&err);
        
        OS_ERR      terr;
        ticks   = OSTimeGet(&terr);                        // ��ȡ��ǰOSTick
        
        /***********************************************
        * ������ û�д���,���¼�����
        */
        if ( err == OS_ERR_NONE ) {
            OS_FLAGS    flagClr = 0;
            /***********************************************
            * ������ ����������
            */
            if       ( flags & WIFI_EVT_FLAG_HEART  ) {
                flagClr |= WIFI_EVT_FLAG_HEART;
                
            /***********************************************
            * ������ WIFI��λ
            */
            } else if ( flags & WIFI_EVT_FLAG_RESET ) {
                flagClr |= WIFI_EVT_FLAG_RESET;
                GPIO_InitTypeDef GPIO_InitStructure;
                /**************************************************
                * ������ ��λ�˿���Ϊ���
                */
                RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
                GPIO_InitStructure.GPIO_Pin = KEY_GPIO_NRESET;
                GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
                GPIO_Init(KEY_PORT, &GPIO_InitStructure);
                /**************************************************
                * ������ ����WIFIģ�鸴λ����
                */
                GPIO_ResetBits(GPIOB,GPIO_Pin_14);
                BSP_OS_TimeDly(OS_TICKS_PER_SEC / 50);
                GPIO_SetBits(GPIOB,GPIO_Pin_14);
                
                /**************************************************
                * ������ ��λ�˿���Ϊ����
                */
                GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
                GPIO_Init(KEY_PORT, &GPIO_InitStructure);    
            /***********************************************
            * ������ WIFI����
            */
            } else if ( flags & WIFI_EVT_FLAG_CONNECT ) {
                flagClr |= WIFI_EVT_FLAG_CONNECT;
                
            /***********************************************
            * ������ ���ڽ���
            */
            } else if ( flags & WIFI_EVT_FLAG_RECV ) {
                flagClr |= WIFI_EVT_FLAG_RECV;
                /***********************************************
                * ������ ���ڽ���WIFIģ�����Ϣ������
                */
                Ctrl.Wifi.TimeOut    = 0;                    // ��ʱ����������
                MessageHandle();
            /***********************************************
            * ������ �ϱ���Ϣ
            */
            } else if ( flags & WIFI_EVT_FLAG_REPORT ) {
                flagClr |= WIFI_EVT_FLAG_REPORT;
                /***********************************************
                * ������ ��ʱ��WIFIģ�������ϱ��豸״̬
                */
                ReportDevStatusHandle();
            /***********************************************
            * ������ �Ͽ�
            */
            } else if ( flags & WIFI_EVT_FLAG_CLOSE ) {
                flagClr |= WIFI_EVT_FLAG_CLOSE;
                
            /***********************************************
            * ������ ��ʱ
            */
            } else if ( flags & WIFI_EVT_FLAG_TIMEOUT ) {
                flagClr |= WIFI_EVT_FLAG_TIMEOUT;
                
            /***********************************************
            * ������ ����
            */
            } else if ( flags & WIFI_EVT_FLAG_CONFIG ) {
                flagClr |= WIFI_EVT_FLAG_CONFIG;
                /***********************************************
                * ������ ֪ͨWIFI��������ģʽ
                */
                InformWifiConfigMode(AirLinkMethod);
                //InformWifiConfigMode(SoftApMethod); 
            } 
            
            if ( !flagClr ) {
                flagClr = flags;
            }
            
            /***********************************************
            * ������ �����־λ
            */
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                        ( OS_FLAGS      )flagClr,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
        /***********************************************
        * ������ �����ʱ������һ��������
        */
        } else if ( err == OS_ERR_TIMEOUT ) {
            OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                        ( OS_FLAGS      )Ctrl.Os.WifiEvtFlag,
                        ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
                        ( OS_ERR       *)&err);
            
            Ctrl.Wifi.UartHandle.Package_Flag  = 0;
            if ( ++Ctrl.Wifi.TimeOut > 180 ) {
                Ctrl.Wifi.TimeOut    = 170;
                OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                           ( OS_FLAGS      )WIFI_EVT_FLAG_RESET,
                           ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                           ( OS_ERR       *)&err);
            }
        }
        
        /***********************************************
        * ������ ����ʣ��ʱ��
        */
        dly   = CYCLE_TIME_TICKS - ( OSTimeGet(&err) - ticks );
        if ( dly  < 1 ) {
            dly = 1;
        } else if ( dly > CYCLE_TIME_TICKS ) {
            dly = CYCLE_TIME_TICKS;
        }
    }
}

/*******************************************************************************
 * ��    �ƣ� APP_WifiInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
static void APP_WifiInit(void)
{
    OS_ERR err;
    /***********************************************
    * ������ ��ʼ������Ӳ����ȥ������ע����������
    */
    BSP_UartCfg( 2, 9600);
    App_McuStatusInit();

    /***********************************************
    * ������ ��ʼ��������ģ����������
    */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_AFIO,ENABLE);

    /***********************************************
    * ������ ��ֹ����Jtag ���Զ˿� SWJ-DP
    *        STM32��PA.13,PA.14,PA.15,PB.03,PB.04��Ĭ�ϵ�JTAG����ӳ��
    */
    //#define GPIO_Remap_SWJ_NoJTRST      ((uint32_t)0x00300100)  /*!< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
    //#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled */
    //#define GPIO_Remap_SWJ_Disable      ((uint32_t)0x00300400)  /*!< Full SWJ Disabled (JTAG-DP + SW-DP) */

    /***********************************************
    * ������ �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��������λ��ֹ
    */
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
    /***********************************************
    * ������ �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
    */
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    /***********************************************
    * ������ �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��
    */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
    /***********************************************
    * ������ ��ֹ�첽���٣��ͷ�PB3
    */
    #define DBGMCU_CR    (*((volatile unsigned long *)0xE0042004))    //�ڶ������Ϻ궨��
    DBGMCU_CR &= 0xFFFFFFDF;

    /* Set PB.3 ~ PB.6 GPIO_Mode_IPU*/
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0
                                    | GPIO_Pin_1
                                    | GPIO_Pin_2
                                    | GPIO_Pin_3
                                    | GPIO_Pin_4
                                    | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    /***********************************************
    * ������ ���������¼���־��
    */
    OSFlagCreate(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                 ( CPU_CHAR     *)"App_WifiFlag",
                 ( OS_FLAGS      )0,
                 ( OS_ERR       *)&err);
    
    Ctrl.Os.WifiEvtFlag = WIFI_EVT_FLAG_HEART               // ����������
                        + WIFI_EVT_FLAG_RESET               // WIFI��λ
                        + WIFI_EVT_FLAG_CONNECT             // WIFI����
                        + WIFI_EVT_FLAG_RECV                // ���ڽ���
                        + WIFI_EVT_FLAG_REPORT              // ���ڷ���
                        + WIFI_EVT_FLAG_CLOSE               // �Ͽ�
                        + WIFI_EVT_FLAG_TIMEOUT             // ��ʱ
                        + WIFI_EVT_FLAG_CONFIG;             // ����
        
    /***********************************************
    * ����������һ����ʱ�� OS_OPT_TMR_ONE_SHOT, OS_OPT_TMR_PERIODIC
    */
    OSTmrCreate ((OS_TMR               *)&OSTmr1,
                 (CPU_CHAR             *)"tmr1",
                 (OS_TICK               )1,
                 (OS_TICK               )1,
                 (OS_OPT                )OS_OPT_TMR_PERIODIC,
                 (OS_TMR_CALLBACK_PTR   )OSTmr1_callback,
                 (void                 *)NULL,
                 (OS_ERR               *)&err);
    OSTmrStart ((OS_TMR  *)&OSTmr1,
                (OS_ERR  *)&err);
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDTFLAG_Wifi;
}

/*******************************************************************************
 * ��    �ƣ� App_McuStatusInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
static void App_McuStatusInit(void)
{
    /***********************************************
    * ������ ��ʼ�����ڽṹ��
    */
	memset(&Ctrl.Wifi.UartHandle, 0, sizeof(Ctrl.Wifi.UartHandle));

	/***********************************************
    * ������ ��ʼ��ͨ�������ṹ��
    */
	memset(&Ctrl.Wifi.CommCmd, 0, sizeof(Ctrl.Wifi.CommCmd));
	Ctrl.Wifi.CommCmd.Pro_HeadPart.Head[0] = 0XFF;
	Ctrl.Wifi.CommCmd.Pro_HeadPart.Head[1] = 0XFF;

    /***********************************************
    * ������ ��ʼ���ظ��豸��Ϣ�ṹ��
    */
	memset(&Ctrl.Wifi.M2wRetInfo, 0, sizeof(Ctrl.Wifi.M2wRetInfo));
	Ctrl.Wifi.M2wRetInfo.Pro_HeadPart.Head[0] = 0XFF;
	Ctrl.Wifi.M2wRetInfo.Pro_HeadPart.Head[1] = 0XFF;
	Ctrl.Wifi.M2wRetInfo.Pro_HeadPart.Len = exchangeBytes(sizeof(Ctrl.Wifi.M2wRetInfo) - 4);
	Ctrl.Wifi.M2wRetInfo.Pro_HeadPart.Cmd = Pro_D2W__GetDeviceInfo_Ack_Cmd;
	memcpy(Ctrl.Wifi.M2wRetInfo.Pro_ver, PRO_VER, strlen(PRO_VER));
	memcpy(Ctrl.Wifi.M2wRetInfo.P0_ver, P0_VER, strlen(P0_VER));
	memcpy(Ctrl.Wifi.M2wRetInfo.Hard_ver, HARD_VER, strlen(HARD_VER));
	memcpy(Ctrl.Wifi.M2wRetInfo.Soft_ver, SOFT_VER, strlen(SOFT_VER));
	memcpy(Ctrl.Wifi.M2wRetInfo.Product_Key, PRODUCT_KEY, strlen(PRODUCT_KEY));
	Ctrl.Wifi.M2wRetInfo.Binable_Time = exchangeBytes(0);

	/***********************************************
    * ������ ��ʼ���ϱ�״̬�ṹ��
    */
	memset(&Ctrl.Wifi.D2wRptSts, 0, sizeof(Ctrl.Wifi.D2wRptSts));
	Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Pro_HeadPart.Head[0] = 0XFF;
	Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Pro_HeadPart.Head[1] = 0XFF;
	Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Pro_HeadPart.Len = exchangeBytes(sizeof(Ctrl.Wifi.D2wRptSts) - 4);
	Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Pro_HeadPart.Cmd = Pro_D2W_P0_Cmd;

	/***********************************************
    * ������ ��ʼ����������ṹ��
    */
	memset(&Ctrl.Wifi.M2wRstCmd, 0, sizeof(Ctrl.Wifi.M2wRstCmd));
	Ctrl.Wifi.M2wRstCmd.Pro_HeadPart.Head[0] = 0XFF;
	Ctrl.Wifi.M2wRstCmd.Pro_HeadPart.Head[1] = 0XFF;
	Ctrl.Wifi.M2wRstCmd.Pro_HeadPart.Len = exchangeBytes(sizeof(Ctrl.Wifi.M2wRstCmd) - 4);
	Ctrl.Wifi.M2wRstCmd.Pro_HeadPart.Cmd = Pro_D2W_ResetWifi_Cmd;

	/***********************************************
    * ������ ��ʼ���豸֪ͨWIFIģ���������ģʽ�ṹ��
    */
	memset(&Ctrl.Wifi.D2wCfgCmd, 0, sizeof(Ctrl.Wifi.D2wCfgCmd));
	Ctrl.Wifi.D2wCfgCmd.Pro_HeadPart.Head[0] = 0XFF;
	Ctrl.Wifi.D2wCfgCmd.Pro_HeadPart.Head[1] = 0XFF;
	Ctrl.Wifi.D2wCfgCmd.Pro_HeadPart.Len = exchangeBytes(sizeof(Ctrl.Wifi.D2wCfgCmd) - 4);
	Ctrl.Wifi.D2wCfgCmd.Pro_HeadPart.Cmd = Pro_D2W_ControlWifi_Config_Cmd;

	/***********************************************
    * ������ ��ʼ��д����ṹ��
    */
	memset(&Ctrl.Wifi.Wr, 0, sizeof(Ctrl.Wifi.Wr));

	/***********************************************
    * ������ ��ʼ��������ṹ��
    */
	memset(&Ctrl.Wifi.Rd, 0, sizeof(Ctrl.Wifi.Rd));
}

/*******************************************************************************
 * ��    �ƣ� InformWifiConfigMode
 * ��    �ܣ� ֪ͨWIFIģ���������ģʽ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� //01ΪSoftAP����ģʽ��02 ΪAir Link����ģʽ������ֵΪ����ֵ
 *******************************************************************************/
OPTIMIZE_NONE static void InformWifiConfigMode(u8 mode)
{
    Ctrl.Wifi.D2wCfgCmd.Pro_HeadPart.SN = Ctrl.Wifi.SN++;

    //Ctrl.Wifi.D2wCfgCmd.Config_Method = SoftApMethod; // AirLinkMethod;
    Ctrl.Wifi.D2wCfgCmd.Config_Method = mode;//AirLinkMethod;
    Ctrl.Wifi.D2wCfgCmd.Sum = CheckSum((uint8_t *)&Ctrl.Wifi.D2wCfgCmd, sizeof(Ctrl.Wifi.D2wCfgCmd));
    Pro_UART_SendBuf((uint8_t *)&Ctrl.Wifi.D2wCfgCmd,sizeof(Ctrl.Wifi.D2wCfgCmd), 0);
}

/*******************************************************************************
 * ��    �ƣ� ReportDevStatusHandle
 * ��    �ܣ� �����ϰ��豸״̬
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
OPTIMIZE_NONE static  void ReportDevStatusHandle (void)
{
    Pro_D2W_ReportDevStatusHandle(1);
}

/*******************************************************************************
 * ��    �ƣ� OSTmr1_callback
 * ��    �ܣ� ��ʱ��1�Ļص�����
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ��ʱ���ص���������ʹ����ʱ����
 *******************************************************************************/
void OSTmr1_callback(OS_TMR *ptmr,void *p_arg)
{
    OS_ERR err;
    static  uint16_t ReportTimeCount = 0;
    /***********************************************
    * ������ע�⣺�ص�������ִ�е�ʱ�䲻�˹���
    */
    ReportTimeCount++;
    Ctrl.Wifi.WaitAck.SendTime++;
    
	if(ReportTimeCount >= 3000) {
		ReportTimeCount = 0;
        OSFlagPost(( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
                   ( OS_FLAGS      )WIFI_EVT_FLAG_REPORT,
                   ( OS_OPT        )OS_OPT_POST_FLAG_SET,
                   ( OS_ERR       *)&err);
	}
    
    /***********************************************
    * ������ ���ڽ��ճ�ʱ������
    */
    if ( Ctrl.Wifi.UartHandle.UART_RecvFlag == 1 ) {   // ���յ�����
        Ctrl.Wifi.UartHandle.UART_RecvFlag = 0;
        if( ++Ctrl.Wifi.UartHandle.UART_RecvTimeoutCtr > 50 ) {
            Ctrl.Wifi.UartHandle.Package_Flag  = 0;
            Ctrl.Wifi.UartHandle.Package_Flag  = 0;
            Ctrl.Wifi.UartHandle.UART_Count    = 0;
            Ctrl.Wifi.UartHandle.UART_Flag1    = 0;
            Ctrl.Wifi.UartHandle.UART_Flag2    = 0;
            
            memset(&Ctrl.Wifi.UartHandle.UART_Buf, 
                   0, 
                   sizeof(Ctrl.Wifi.UartHandle.Message_Buf));
            
        }
    } else {
    }
}


/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif