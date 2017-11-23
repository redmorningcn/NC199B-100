/*******************************************************************************
 *   Filename:      bsp_rtc.c
 *   Revised:       $Date: 2013-06-30
 *   Revision:      $V1.0
 *   Writer:        wumingshen.
 *
 *   Description:
 *
 *   Notes:         STM32F103�ڲ�RTC����ģ��
 *                  ʹ��RTC����ֱ�ӵ���bsp_time.c�еĺ�����ȡ���޸�RTCʵʱʱ��
 *
 *   All copyrights reserved to wumingshen
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <bsp_rtc.h>

#define BSP_RTC_MODULE_EN  1
#if BSP_RTC_MODULE_EN >  0

/*******************************************************************************
 * ������ ��ģ���Ƿ����uCOSϵͳʹ��
 */
#if defined( UCOS_EN )
#undef UCOS_EN
#endif
     
#define UCOS_EN  1

/*******************************************************************************
 * ������ ����ʱ��64��Ƶ��PC13�����ʱ��
 */
#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

/*******************************************************************************
 * TYPEDEFS
 */

#if (UCOS_EN == ENABLE)
    #if OS_VERSION > 30000U
    static  OS_SEM                   RtcSem;    // �ź���
    #else
    static  OS_EVENT                *RtcSem;    // �ź���
    #endif
#endif

//============================================================================//

/*******************************************************************************
* ��    �ƣ� BSP_RTC_Alarm_ISR_Handler
* ��    �ܣ� RTC�����жϷ�����
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void BSP_RTC_Alarm_ISR_Handler (void)
{
//	if(RTC_GetITStatus(RTC_IT_ALR) != RESET) {
//        EXTI_ClearITPendingBit(EXTI_Line17);		// ��EXTI_Line17����λ
//        if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET){// ��黽�ѱ�־�Ƿ�����
//            PWR_ClearFlag(PWR_FLAG_WU);				// ������ѱ�־
//        }
//        RTC_WaitForLastTask();						// �ȴ����һ������д���
//
//        RTC_ClearITPendingBit(RTC_IT_ALR);			// ��RTC�����жϹ���
//        RTC_WaitForLastTask();						// �ȴ����һ������д���
//    }
}

/*******************************************************************************
* ��    �ƣ� RTC_Configuration
* ��    �ܣ� RTC���ú���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void RTC_Configuration(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);

    /* Reset Backup Domain */
    BKP_DeInit();

    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){}

    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    //RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

    /* Enable the RTC Second */
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
    //RTC_WaitForLastTask();

    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32768); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}


/*******************************************************************************
* ��    �ƣ� BSP_RTC_GetTime
* ��    �ܣ� RTC��ȡʵʱʱ��
* ��ڲ����� INT08U *time ����СΪ3���ֽڵ����飻time[0]:ʱ time[1]:�� time[2]:��
* ���ڲ����� TRUE �� ʱ���ȡ�ɹ�
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
BOOL BSP_RTC_GetTime( INT08U *time )
{
    INT32U TimeCnt;

#if (UCOS_EN == ENABLE)
#if OS_VERSION > 30000U
    CPU_BOOLEAN err;
    err = BSP_OS_SemWait(&RtcSem,1000);             // �ȴ��ź���
    if ( err != OS_ERR_NONE )
        return FALSE;
#else
    INT08U   err;
    OSSemPend(RtcSem,1000,&err);                    // �ź���
    if ( err != OS_ERR_NONE )
        return FALSE;
#endif
#endif
    /* Display current time */
    TimeCnt = RTC_GetCounter();

    /* Compute  hours */
    *time++ = TimeCnt / 3600 % 24;
    /* Compute minutes */
    *time++ = (TimeCnt % 3600) / 60;
    /* Compute seconds */
    *time++ = (TimeCnt % 3600) % 60;

#if (UCOS_EN == ENABLE)

#if OS_VERSION > 30000U
    BSP_OS_SemPost(&RtcSem);                     // �����ź���
#else
    OSSemPost(RtcSem);                              // �ź���
#endif

#endif

    return TRUE;
}


/*******************************************************************************
* ��    �ƣ� BSP_RTC_SetTime
* ��    �ܣ� ����RTC��ʱ��
* ��ڲ����� INT08U *time ����СΪ3���ֽڵ����飻time[0]:ʱ time[1]:�� time[2]:��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
BOOL BSP_RTC_SetTime(INT08U *time)
{
    INT32U  cnts;

#if (UCOS_EN == ENABLE)
    #if OS_VERSION > 30000U
         CPU_BOOLEAN err;
         err = BSP_OS_SemWait(&RtcSem,1000);           // �ȴ��ź���
         if ( err != OS_ERR_NONE )
          return FALSE;
    #else
        INT08U       err;
        OSSemPend(RtcSem,1000,&err);                   // �ȴ��ź���
        if ( err != OS_ERR_NONE )
          return FALSE;
    #endif
#endif


  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  cnts  = time[0] * 3600 + time[1] * 60 + time[2];

  /* Change the current time */
  RTC_SetCounter(cnts);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

#if (UCOS_EN == ENABLE)
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&RtcSem);                        // �����ź���
#else
    INT08U       err;
    OSSemPost(RtcSem);                              // �����ź���
#endif
#endif
  return TRUE;
}


/*******************************************************************************
* ��    �ƣ� BSP_RTC_DispTime
* ��    �ܣ� RTC ʵʱʱ����ʾ
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void BSP_RTC_DispTime(void)
{
    INT08U TimeBuf[3];

	BSP_RTC_GetTime(TimeBuf);

//    GUI_SetDrawMode(GUI_DM_XOR);
//    GUI_SetFont(&GUI_FontComic18B_ASCII);
//
//    GUI_DispDecAt   (TimeBuf[0],    LCD_XSIZE - 20 * 8, LCD_YSIZE - 20, 2);
//    GUI_DispStringAt(":",           LCD_XSIZE - 20 * 6, LCD_YSIZE - 20);
//    GUI_DispDecAt   (TimeBuf[1],    LCD_XSIZE - 20 * 5, LCD_YSIZE - 20, 2);
//    GUI_DispStringAt(":",           LCD_XSIZE - 20 * 3, LCD_YSIZE - 20);
//    GUI_DispDecAt   (TimeBuf[2],    LCD_XSIZE - 20 * 2, LCD_YSIZE - 20, 2);
    //UartPrintf(COMM_PORT_MSG, "��ǰʱ��: %0.2d:%0.2d:%0.2d\r\n", TimeBuf[0], TimeBuf[1], TimeBuf[2]);
}


/*******************************************************************************
* ��    �ƣ� BSP_RTC_Init
* ��    �ܣ� RTC��ʼ������
* ��ڲ����� ��
* ���ڲ����� TRUE : ��ʼ���ɹ�
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
BOOL BSP_RTC_Init(void)
{
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5) {
        INT08U time[] = {20, 4, 0};
        // Backup data register value is not correct or not yet programmed (when
        // the first time the program is executed)

        //UartPutString(COMM_PORT_MSG, "ʱ��δ����...\r\n");

        // RTC Configuration
        RTC_Configuration();

        //UartPutString(COMM_PORT_MSG, "ʱ��������...\r\n");

        // Adjust time by values entred by the user on the hyperterminal
        BSP_RTC_SetTime(time);

        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    } else {
        // Check if the Power On Reset flag is set
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
          //UartPutString(COMM_PORT_MSG, "�ϵ縴λ...\r\n");
        }
        // Check if the Pin Reset flag is set
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
          //UartPutString(COMM_PORT_MSG, "�ⲿ��λ...\r\n");
        }

        //UartPutString(COMM_PORT_MSG, "ʱ����������...\r\n");
        // Wait for RTC registers synchronization
        RTC_WaitForSynchro();

        // Enable the RTC Second
        //RTC_ITConfig(RTC_IT_SEC, ENABLE);
        // Wait until last write operation on RTC registers has finished
        //RTC_WaitForLastTask();
    }

#ifdef RTCClockOutput_Enable
    // Enable PWR and BKP clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    // Allow access to BKP Domain
    PWR_BackupAccessCmd(ENABLE);

    // Disable the Tamper Pin
    BKP_TamperPinCmd(DISABLE); // To output RTCCLK/64 on Tamper pin, the tamper
                               //  functionality must be disabled

    // Enable RTC Clock Output on Tamper Pin */
    // BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif

	/***********************************************
	* �����������ж���ں������ж����ȼ�
	*/
	BSP_IntVectSet(BSP_INT_ID_RTC, BSP_RTC_Alarm_ISR_Handler);
	BSP_IntEn(BSP_INT_ID_RTC);
	/***********************************************
	* ����������RTC�����ж�
	*/
	RTC_ITConfig(RTC_IT_ALR, ENABLE);				// ����RTC�����ж�
	RTC_WaitForLastTask();							// �ȴ����һ��дָ�����

    // Clear reset flags
    RCC_ClearFlag();

#if (UCOS_EN == ENABLE)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&RtcSem,1, "Bsp RtcSem");      // �����ź���
#else
    RtcSem     = OSSemCreate(1);                    // �����ź���
#endif
#endif
    return TRUE;
}


/*******************************************************************************
* ��    �ƣ� RTC_Configuration
* ��    �ܣ� RTC���ú���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//void BSP_RTC_Main(void)
//{
//    BSP_RTC_Init();
//
//    while(1){
//        BSP_RTC_DispTime();
//        OSTimeDly(OS_TICKS_PER_SEC);
//        break;
//    }
//}

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif