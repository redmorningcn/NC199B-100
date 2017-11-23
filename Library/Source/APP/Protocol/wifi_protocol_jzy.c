/*******************************************************************************
 *   Filename:       Protocol.c
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2014-08-11
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ������WIFIͨ��Э��
 *
 *
 *   Notes:
 *
 *   All copyrights reserved to Roger-WY
 *******************************************************************************/

#define  SNL_APP_SOURCE
#include <app.h>
#include <bsp_control.h>
#include <includes.h>
#include <bsp_flash.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *Protocol__c = "$Id: $";
#endif

//extern Pro_M2W_ReturnInfoTypeDef  Ctrl.Wifi.M2wRetInfo;
//extern Pro_P0_ControlTypeDef      Ctrl.Wifi.P0Ctrl;
//extern Pro_P0_ControlTypeDef      Ctrl.Wifi.P0Ctrl;
//extern Pro_D2W_ReportDevStatusTypeDef   Ctrl.Wifi.D2wRptSts;



/*******************************************************************************
* ��    �ƣ� MessageHandle
* ��    �ܣ� ���������ݷ����ˣ��ȼ�������Ƿ�Ϸ����ٽ�������֡������Ӧ����
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void MessageHandle(void)
{
    Pro_HeadPartTypeDef   Recv_HeadPart;
	memset(&Recv_HeadPart, 0, sizeof(Recv_HeadPart));
    memset(&Ctrl.Wifi.UartHandle.Message_Buf, 0, sizeof(16));
    /***********************************************
    * ������ �����յ�һ�����������ݰ�
    */
    Ctrl.Wifi.UartHandle.Message_Len   = Ctrl.Wifi.UartHandle.UART_Cmd_len + 4;
    
    memcpy(&Ctrl.Wifi.UartHandle.Message_Buf, 
           Ctrl.Wifi.UartHandle.UART_Buf, 
           Ctrl.Wifi.UartHandle.Message_Len );
    memcpy(&Recv_HeadPart, 
           Ctrl.Wifi.UartHandle.Message_Buf, 
           sizeof(Recv_HeadPart));
    memset(&Ctrl.Wifi.UartHandle.UART_Buf, 
           0, 
           sizeof(Ctrl.Wifi.UartHandle.Message_Buf));
    
    Ctrl.Wifi.UartHandle.Package_Flag  = 0;
    Ctrl.Wifi.UartHandle.UART_Count    = 0;
    /***********************************************
    * ������ ������ݰ��Ƿ����
    */
    if(CheckSum(Ctrl.Wifi.UartHandle.Message_Buf,
                Ctrl.Wifi.UartHandle.Message_Len) != 
       Ctrl.Wifi.UartHandle.Message_Buf[Ctrl.Wifi.UartHandle.Message_Len - 1]) {
       /***********************************************
       * ������ ���ݰ��쳣����
       */
       Pro_W2D_ErrorCmdHandle();
       return ;
    }
    /***********************************************
    * ������ �鿴���ݰ�������ָ��
    */
    switch (Recv_HeadPart.Cmd) { 
    /***********************************************
    * ������ ��ȡMCU���豸��Ϣ���������ƶ˰��豸
    */
    case Pro_W2D_GetDeviceInfo_Cmd 				://= 0x01,   //WIFI���豸 ��ȡ�豸��Ϣ
        Pro_W2D_GetMcuInfo();
        break;
    case Pro_D2W__GetDeviceInfo_Ack_Cmd			://= 0x02,   //�豸��WIFI �ظ��豸��Ϣ
        break;
    /***********************************************
    * ������ WiFiʹ��P0Э����豸�����������
    */
    case Pro_W2D_P0_Cmd 						://= 0x03,   //WIFI���豸 ��ȡ�豸״̬
        Pro_W2D_P0CmdHandle();
        break;
    case Pro_D2W_P0_Ack_Cmd						://= 0x04,   //�豸��WIFI �ظ��豸״̬
        break;
    case Pro_D2W_P0_Cmd 						://= 0x05,   //�豸��WIFI �����ϱ��豸״̬
        break;
    case Pro_W2D_P0_Ack_Cmd						://= 0x06,   //WIFI���ظ�
        break;
    /***********************************************
    * ������ WIFIģ�鷢����������
    */
    case Pro_W2D_Heartbeat_Cmd 					://= 0x07,   //WIFI���豸 ����������
        Pro_W2D_CommonCmdHandle();
        break;
    case Pro_D2W_heartbeatAck_Cmd				://= 0x08,   //�豸��WIFI �ظ�������
        break;
    case Pro_D2W_ControlWifi_Config_Cmd 		://= 0x09,   //�豸��WIFI ֪ͨWIFI��������ģʽ
        break;
    case Pro_W2D_ControlWifi_Config_Ack_Cmd	    ://= 0x0A,   //WIFI���豸 �ظ�ģ���������ģʽ
        break;
    case Pro_D2W_ResetWifi_Cmd 					://= 0x0B,   //�豸��WIFI ��������WIFIģ��ָ��
        break;
    case Pro_W2D_ResetWifi_Ack_Cmd				://= 0x0C,   //WIFI���豸 �ظ�����WIFIģ��
        break;
    /***********************************************
    * ������ WIFIģ����MCU֪ͨ����״̬�ı仯
    */
    case Pro_W2D_ReportWifiStatus_Cmd 			://= 0x0D,   //WIFI���豸 ����WIFIģ�鹤��״̬�ı仯
        Pro_W2D_WifiStatusHandle();
        break;
    case Pro_D2W_ReportWifiStatus_Ack_Cmd		://= 0x0E,   //�豸��WIFI �ظ�WIFIģ�鹤��״̬�ı仯
        break;
    /***********************************************
    * ������ WIFIģ����������MCU
    */
    case Pro_W2D_ReportWifiReset_Cmd 			://= 0x0F,   //WIFI���豸 ������������MCU
        Pr0_W2D_RequestResetDeviceHandle();
        break;
    case Pro_D2W_ReportWifiReset_Ack_Cmd		://= 0x10,   //�豸��WIFI �ظ���������MCU
        break;
    /***********************************************
    * ������ �Ƿ�ָ���
    */
    case Pro_W2D_ErrorPackage_Cmd 				://= 0x11,   //WIFI���豸 ���ͷǷ����ݰ�����֪ͨ
        Pro_W2D_ErrorCmdHandle();
        break;
    case Pro_D2W_ErrorPackage_Ack_Cmd			://= 0x12,   //�豸��WIFI �ظ��Ƿ����ݰ�����֪ͨ
        break;
    default:
        break;
    }
}

/*******************************************************************************
* ��    �ƣ� Pro_GetMcuInfo
* ��    �ܣ� WiFiģ�������豸��Ϣ
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_GetMcuInfo(void)
{
	memcpy(&Ctrl.Wifi.CommCmd, 
           Ctrl.Wifi.UartHandle.Message_Buf, 
           sizeof(Ctrl.Wifi.CommCmd));
	Ctrl.Wifi.M2wRetInfo.Pro_HeadPart.SN    = Ctrl.Wifi.CommCmd.Pro_HeadPart.SN;
	Ctrl.Wifi.M2wRetInfo.Sum                = CheckSum((uint8_t *)&Ctrl.Wifi.M2wRetInfo,
                                                           sizeof(Ctrl.Wifi.M2wRetInfo));
	Pro_UART_SendBuf(                            (uint8_t *)&Ctrl.Wifi.M2wRetInfo,
                                                  sizeof(Ctrl.Wifi.M2wRetInfo), 0);

	//Log_UART_SendBuf((uint8_t *)&Ctrl.Wifi.M2wRetInfo,sizeof(Ctrl.Wifi.M2wRetInfo));

    /******************************�����־*********************************************/
    //	printf("W2D_GetMcuInfo...\r\n");
    //	printf("PRO_VER:"); 	printf(PRO_VER); 		printf("\r\n");
    //	printf("P0_VER:");		printf(P0_VER);			printf("\r\n");
    //	printf("P0_VER:");      printf(HARD_VER);		printf("\r\n");
    //	printf("SOFT_VER:");    printf(SOFT_VER);		printf("\r\n");
    //	printf("PRODUCT_KEY:"); printf(PRODUCT_KEY);    printf("\r\n");
    /***********************************************************************************/
}

/*******************************************************************************
* ��    �ƣ� Pro_W2D_CommonCmdHandle
* ��    �ܣ�  1��WiFiģ�����豸MCU������(4.2)
*             2���豸MCU֪ͨWiFiģ���������ģʽ(4.3)
*             3���豸MCU����WiFiģ��(4.4)
*             4, WiFiģ����������MCU(4.6)
*             5, WiFiģ����������MCU ( 4.9 WiFiģ�������ϱ���ǰ��״̬)
*             6���豸MCU�ظ� (WiFiģ������豸)
*                   4.6 	WiFiģ����������MCU
*                   4.9 	Wifiģ��ظ�
*                   4.10    �豸MCU�ظ�
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_CommonCmdHandle(void)
{
	/***********************************************
    * ������ �������յ������ݰ�������ͨ�������ṹ����
    */
    memcpy(&Ctrl.Wifi.CommCmd, 
           Ctrl.Wifi.UartHandle.Message_Buf, 
           sizeof(Ctrl.Wifi.CommCmd));
	/***********************************************
    * ������ �Ե�ǰָ���1��Ϊ��Ӧ�Ļظ�ָ��
    */
    Ctrl.Wifi.CommCmd.Pro_HeadPart.Cmd    = Ctrl.Wifi.CommCmd.Pro_HeadPart.Cmd + 1;
	Ctrl.Wifi.CommCmd.Sum                 = CheckSum((uint8_t *)&Ctrl.Wifi.CommCmd, 
                                                       sizeof(Ctrl.Wifi.CommCmd));
	Pro_UART_SendBuf(                        (uint8_t *)&Ctrl.Wifi.CommCmd,
                                              sizeof(Ctrl.Wifi.CommCmd), 0);
	memset(&Ctrl.Wifi.CommCmd, 
           0, 
           sizeof(Ctrl.Wifi.CommCmd));
}

/*******************************************************************************
* ��    �ƣ� Pro_W2D_WifiStatusHandle
* ��    �ܣ� ��WiFi��״̬���浽 Pro_W2D_WifiStatusStruct�С����ظ�ACK
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_WifiStatusHandle(void)
{
	memcpy(&Ctrl.Wifi.W2dWifiSts, 
           Ctrl.Wifi.UartHandle.Message_Buf, 
           sizeof(Ctrl.Wifi.W2dWifiSts));
	Ctrl.Wifi.W2dWifiSts.WifiStatus.uWifiStatus     = exchangeBytes(Ctrl.Wifi.W2dWifiSts.WifiStatus.uWifiStatus);
    Pro_W2D_CommonCmdHandle();
}


/*******************************************************************************
* ��    �ƣ� Pr0_W2D_RequestResetDeviceHandle
* ��    �ܣ� WiFiģ������λ�豸MCU��MCU�ظ�ACK����ִ���豸��λ
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� Ϊ�˱��� WIFIģ��û���յ�ȷ��ָ����ط�ָ�����MCU�������
*            ��Ҫ�ڻظ�WIFIģ���µȴ�600MS������
*******************************************************************************/
void Pr0_W2D_RequestResetDeviceHandle(void)
{
	OS_ERR err;

    Pro_W2D_CommonCmdHandle();

    OSTimeDly(600, OS_OPT_TIME_DLY, &err);

    /****************************����������豸��λ����****************************/
	__set_FAULTMASK(1); // �ر������ж�
	NVIC_SystemReset(); // ϵͳ��λ

    /******************************************************************************/
}

/*******************************************************************************
* ��    �ƣ� Pro_W2D_ErrorCmdHandle
* ��    �ܣ� WiFi�����յ��Ƿ���Ϣ֪ͨ���豸MCU�ظ�ACK����ִ����Ӧ�Ķ���
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
extern OS_SEM  SemReportStatus;            //�����ϱ�״̬�ź���
void Pro_W2D_ErrorCmdHandle(void)
{
	Pro_ErrorCmdTypeDef           	 Pro_ErrorCmdStruct;       //4.7 �Ƿ���Ϣ֪ͨ
	memcpy(&Pro_ErrorCmdStruct, Ctrl.Wifi.UartHandle.Message_Buf, sizeof(Pro_ErrorCmdStruct));

	Pro_ErrorCmdStruct.Pro_HeadPart.Cmd = Pro_ErrorCmdStruct.Pro_HeadPart.Cmd;
	Pro_ErrorCmdStruct.Sum = CheckSum((uint8_t *)&Ctrl.Wifi.CommCmd, sizeof(Ctrl.Wifi.CommCmd));
	Pro_UART_SendBuf((uint8_t *)&Ctrl.Wifi.CommCmd,sizeof(Ctrl.Wifi.CommCmd), 0);

	/*************************��������ӹ��ϴ�����*****************************/
	switch (Pro_ErrorCmdStruct.Error_Packets)
	{
		case Error_AckSum:
        //printf("W2D Error Command ->Error_AckSum\r\n");
        break;
		case Error_Cmd:
        //printf("W2D Error Command ->Error_Cmd\r\n");
        break;
		case Error_Other:
        //printf("W2D Error Command ->Error_Other\r\n");
        break;
		default:
        break;
	}
    /******************************************************************************/
}

/*******************************************************************************
* ��    �ƣ� Pro_W2D_P0CmdHandle
* ��    �ܣ� WiFi ʹ��P0Э����豸�����������
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_P0CmdHandle(void)
{
	Pro_P0_ControlTypeDef      			 P0Ctrl; 			 //WiFiģ������豸

	memcpy(&P0Ctrl, 
           Ctrl.Wifi.UartHandle.Message_Buf, 
           sizeof(P0Ctrl));
    
	switch (P0Ctrl.Pro_HeadPartP0Cmd.Action){
		case P0_W2D_Control_Devce_Action:       //�����豸����
            Pro_W2D_Control_DevceHandle();                 
        break;
		case P0_W2D_ReadDevStatus_Action:       //��ȡ�豸״̬����
            Pro_W2D_ReadDevStatusHandle(0);
        break;
		case P0_D2W_ReportDevStatus_Action:     //�����ϱ��豸״̬����
            Pro_D2W_ReportDevStatusHandle(1);
        break;

		default:
        break;
	}
}

/*******************************************************************************
* ��    �ƣ� Pro_W2D_Control_DevceHandle
* ��    �ܣ� WiFi ʹ��P0Э����豸�����������
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_Control_DevceHandle(void)
{
	//static Pro_P0_ControlTypeDef    	 Pro_P0_ControlStruct; 			 //4WiFiģ������豸
	Pro_W2D_CommonCmdHandle();                                           //�ظ�WIFIģ�����յ�����
    
    static Pro_P0_ControlTypeDef    	 LastProP0Ctrl;
    u8  len = sizeof(Pro_P0_ControlTypeDef);
    
	/***********************************************
    * ������ �������ڻ��������ݣ���������
    */
    memcpy(&Ctrl.Wifi.P0Ctrl, Ctrl.Wifi.UartHandle.Message_Buf, sizeof(Ctrl.Wifi.P0Ctrl));
    
    /***********************************************
    * ������ ��⵱ǰ�����Ƿ����ϴ���ͬ��ֱ�ӷ���
    */
    if ( 0 == memcmp(&LastProP0Ctrl, &Ctrl.Wifi.P0Ctrl, len ) )   {
        memset(&Ctrl.Wifi.P0Ctrl, 0, sizeof(Ctrl.Wifi.P0Ctrl));
        return;
    }
    // ���浱ǰ����
    memcpy(&LastProP0Ctrl, &Ctrl.Wifi.P0Ctrl, len);
    
    /***********************************************
    * ������ ����һ�½��յ�������
    */
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.uAttrFlags
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.uAttrFlags);
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.uSetStatusWord
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.uSetStatusWord);
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOnMin
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOnMin);
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOffMin
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOffMin);
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOn
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOn);
    Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOff
        = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOff);
    
    //==========================��Ӹ����д�豸==============================//
    //�����ж����õı�־λ�Ƿ���λ��������Ӧ�Ĳ�����дֵ
    //========================================================================//
    /***********************************************
    * ������ �޸Ŀ���״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.Switch) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.Switch = 0;
        Ctrl.Sys.SysSts =  Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.udat.Switch;
        //       Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.Switch =  Ctrl.Sys.SysSts;
        
        if(Ctrl.Sys.SysSts) {
            /***********************************************
            * ������ ��������
            */
            Ctrl.Fan.CurScale   = FAN_DUTY_SCALE_0;
            Ctrl.Fan.Mode       = FAN_MODE_SMART;
            Beep( BEEP_FLAG_PWR_ON );
        } else {
            Ctrl.Fan.Mode       = FAN_MODE_PWR_OFF;
            Ctrl.Fan.CurScale   = FAN_DUTY_SCALE_0;
            /***********************************************
            * ������ �ػ�����
            */
            Beep( BEEP_FLAG_PWR_OFF );
        }
    }
    /***********************************************
    * ������ �޸ĸ����ӿ���״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.SwitchPlasma) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.SwitchPlasma = 0;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ŀ���������״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.LedAirQuality) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.LedAirQuality = 0;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ķ�ͯ��״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.ChildSecurityLock) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.ChildSecurityLock = 0;
        Ctrl.Wifi.Wr.ChildSecurityLock = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.udat.ChildSecurityLock;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸ķ���״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.WindVelocity) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.WindVelocity = 0;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ŀ������������״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.AirSensitivity) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.AirSensitivity = 0;
        Ctrl.Led.SetLevel   = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.udat.AirSensitivity;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸ķ���״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.FanLevel) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.FanLevel = 0;
        Ctrl.Fan.Level  = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.udat.FanLevel;
        Ctrl.Fan.Mode   = FAN_MODE_HAND;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ŀ���ģʽ״̬
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CtrlMode) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CtrlMode = 0;
        Ctrl.Fan.Mode =  Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetStatusWord.udat.CtrlMode;        
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸���о����
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.FilterLife) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.FilterLife = 0;
        Ctrl.Wifi.Rd.FilterLife         = 100;
        Ctrl.Wifi.Wr.FilterLife         = 100;
        Ctrl.Para.dat.FilterLife.Life   = FILTER_LIFE_TIME;
        BSP_FlashWriteBytes((u32) 0*2, (u8 *)&Ctrl.Para.dat.FilterLife, (u16) 2*2);
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸İ����ظ���ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.WeekRepeat) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.WeekRepeat = 0;
        Ctrl.Wifi.Wr.WeekRepeat = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetWeekRepeat;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸ĵ���ʱ������ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CountDownOnMin) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CountDownOnMin = 0;
        Ctrl.Wifi.Wr.CountDownOnMin = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOnMin;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸ĵ���ʱ�ػ���ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CountDownOffMin) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.CountDownOffMin = 0;
        Ctrl.Wifi.Wr.CountDownOffMin    = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetCountDownOffMin;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ķ�ʱ������ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.TimingOn) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.TimingOn = 0;
        Ctrl.Wifi.Wr.TimingOn = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOn;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸Ķ�ʱ�ػ���ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.TimingOff) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.TimingOff = 0;
        Ctrl.Wifi.Wr.TimingOff = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.SetTimingOff;
        Beep( BEEP_FLAG_KEY_EVT );
    }
    /***********************************************
    * ������ �޸�PM2.5�궨ƫ��ֵ
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.Offset) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.Offset = 0;
        INT16S  offset      = exchangeBytes(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Offset)
                             - 2000;
        
        /***********************************************
        * ������ �޸�PM2.5�궨��ֵ
        */
#define OFFSET_VALUE_L    400
#define OFFSET_VALUE_H    600
        /***********************************************
        * ������ �ָ�Ĭ��
        */
        if ( offset == 1000 ) {
            BSP_PM2D5Chk( PM2D5_CHK_TYPE_DEF, 0 );
        /***********************************************
        * ������ У׼�͵�
        */
        } else if ( ( offset > OFFSET_VALUE_L ) && ( offset < OFFSET_VALUE_H ) ) {
            offset  = offset - OFFSET_VALUE_L;
            /***********************************************
            * ������ �޸�PM2.5�궨��ֵ
            */
            BSP_PM2D5Chk( PM2D5_CHK_TYPE_K_Y1, offset );
        /***********************************************
        * ������ У׼�ߵ�
        */
        } else if ( offset > OFFSET_VALUE_H ) {
            offset  = offset - OFFSET_VALUE_H;
            BSP_PM2D5Chk( PM2D5_CHK_TYPE_K_Y2, offset );
        /***********************************************
        * ������ У׼ƫ��
        */
        } else {
            BSP_PM2D5Chk( PM2D5_CHK_TYPE_B_OFFSET, offset );
        }
    
        Beep( BEEP_FLAG_KEY_EVT );
            
        /***********************************************
        * ������ ����У׼ֵ
        */
#if (STORAGE_TYPE == INTNER_FLASH ) 
        //BSP_FlashWriteBytes((u32) 10*2, (u8 *)&Ctrl.Para.dat.Pm2d5Chk.Offset, (u16) 10*2);
#elif (STORAGE_TYPE == INTNER_EEPROM )
#elif (STORAGE_TYPE == EXTERN_EEPROM )
        BSP_EEP_WriteBytes(Ctrl.Para.dat.idx->PM2D5Offset, 
                           &Ctrl.Para.buf1[2 * Ctrl.Para.dat.idx->PM2D5Offset],
                           2 );
#endif
    }
    /***********************************************
    * ������ �޸���չ����
    */
    if(Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.ExtendedData) {
        Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrFlags.udat.ExtendedData = 0;
        memcpy(&Ctrl.Wifi.Wr.Ext.buf1[0],
               &Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Ext.buf1[0],EXT_DATA_LEN);
        /***********************************************
        * ������ �޸�PM2.5�궨��ֵ
        */
        char i  = 0;
        if ( ( Ctrl.Wifi.Wr.Ext.dat.Pm2d5L != 0 ) ||
             ( Ctrl.Wifi.Wr.Ext.dat.Pm2d5H != 0 ) ){ 
                 
            if ( Ctrl.Wifi.Wr.Ext.dat.Pm2d5L != 0 ) {
                if ( Ctrl.PM2D5.Type    == 0 ) {
                    Ctrl.Para.dat.Pm2d5Chk.Value.y1[0]    = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Ext.dat.Pm2d5L;
                    Ctrl.Para.dat.Pm2d5Chk.Adc.x1[0]      = Ctrl.PM2D5.G1AdVal;
                } else {
                    Ctrl.Para.dat.Pm2d5Chk.Value.y1[1]    = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Ext.dat.Pm2d5L;
                    Ctrl.Para.dat.Pm2d5Chk.Adc.x1[1]      = Ctrl.PM2D5.G2AdVal;
                }
            } else if ( Ctrl.Wifi.Wr.Ext.dat.Pm2d5H != 0 ) {
                i  = 1;
                if ( Ctrl.PM2D5.Type    == 0 ) {
                    Ctrl.Para.dat.Pm2d5Chk.Value.y2[0]    = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Ext.dat.Pm2d5H;
                    Ctrl.Para.dat.Pm2d5Chk.Adc.x2[0]      = Ctrl.PM2D5.G1AdVal;
                } else {
                    Ctrl.Para.dat.Pm2d5Chk.Value.y2[1]    = Ctrl.Wifi.P0Ctrl.AttrCtrl.AttrVals.Ext.dat.Pm2d5H; 
                    Ctrl.Para.dat.Pm2d5Chk.Adc.x2[1]      = Ctrl.PM2D5.G2AdVal;
                }
            }
                
            Ctrl.Wifi.Wr.Offset = 0;
            Ctrl.Para.dat.Pm2d5Chk.Offset.b[i]       = Ctrl.Wifi.Wr.Offset;
            Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                                / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
            Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]);  
    
            BSP_FlashWriteBytes((u32) 10*2, (u8 *)&Ctrl.Para.dat.Pm2d5Chk.Offset, (u16) 10*2);
        }
    }
    //========================================================================//
    /***********************************************
    * ������ �����ϱ��豸������״̬
    */    
	//OSSemPost ((OS_SEM  *)&SemReportStatus,
    //           (OS_OPT   )OS_OPT_POST_1,
    //           (OS_ERR  *)&err);
	memset(&Ctrl.Wifi.P0Ctrl, 0, sizeof(Ctrl.Wifi.P0Ctrl));
}


/*******************************************************************************
* ��    �ƣ� Pro_W2D_ReadDevStatusHandle
* ��    �ܣ� ��ȡ�豸״̬����
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_W2D_ReadDevStatusHandle(uint8_t ucmd)
{
	Pro_D2W_ReportDevStatusHandle(ucmd);
}

/*******************************************************************************
* ��    �ƣ� Pro_D2W_ReportDevStatusHandle
* ��    �ܣ� �ϱ��豸״̬����
* ��ڲ����� ucmd;�����������ϱ�����ѯ�ʻظ� 0����ʾѯ�ʻظ� 1����ʾ�����ϱ�
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void Pro_D2W_ReportDevStatusHandle(uint8_t ucmd)
{
	Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Pro_HeadPart.SN = Ctrl.Wifi.SN++;
    switch(ucmd) {
        case 0:{
           Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Action = P0_D2W_ReadDevStatus_Action_ACK;//WIFIģ��ѯ���豸״̬//�ظ���ȡ�豸״̬����
        };break;
        case 1:{
           Ctrl.Wifi.D2wRptSts.Pro_HeadPartP0Cmd.Action = P0_D2W_ReportDevStatus_Action;  //�����ϱ��豸״̬
        };break;
        default:break;

    }
	/***********************************************
    * ������ �ϱ��豸״̬��
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.Switch            = Ctrl.Sys.SysSts;
    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.ChildSecurityLock = Ctrl.Wifi.Wr.ChildSecurityLock;
    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.FanLevel          = Ctrl.Fan.Level;
    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.CtrlMode          = Ctrl.Fan.Mode;
    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.udat.AirSensitivity    = Ctrl.Led.SetLevel;

    Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.uStatusWord = exchangeBytes(Ctrl.Wifi.D2wRptSts.Dev_Status.StatusWord.uStatusWord);
    /***********************************************
    * ������ �ϱ��豸��������
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.FilterLife       = Ctrl.Wifi.Rd.FilterLife;
    /***********************************************
    * ������ �ϱ��豸�����ظ���ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.WeekRepeat       = Ctrl.Wifi.Wr.WeekRepeat;
    /***********************************************
    * ������ �ϱ��豸����ʱ������ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.CountDownOnMin   = exchangeBytes(Ctrl.Wifi.Wr.CountDownOnMin);
    /***********************************************
    * ������ �ϱ��豸����ʱ�ػ���ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.CountDownOffMin  = exchangeBytes(Ctrl.Wifi.Wr.CountDownOffMin);
    /***********************************************
    * ������ �ϱ��豸��ʱ������ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.TimingOn         = exchangeBytes(Ctrl.Wifi.Wr.TimingOn);
    /***********************************************
    * ������ �ϱ��豸��ʱ�ػ���ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.TimingOff        = exchangeBytes(Ctrl.Wifi.Wr.TimingOff);
    
    /***********************************************
    * ������ �ϱ�PM2.5ƫ��
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.Offset           = exchangeBytes(Ctrl.Wifi.Wr.Offset);
    
    /***********************************************
    * ������ �ϱ���չ����
    */
    memcpy(&Ctrl.Wifi.D2wRptSts.Dev_Status.Ext.buf1[0],
           &Ctrl.Wifi.Wr.Ext.buf1[0],EXT_DATA_LEN);    
    
    /***********************************************
    * ������ PM2.5��ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.DustAirQuality  = exchangeBytes(Ctrl.Wifi.Rd.PM25Val);
    Ctrl.Wifi.D2wRptSts.Dev_Status.AirQualty       = Ctrl.Wifi.Rd.PM25Level;

    /***********************************************
    * ������ ��ʪ�ȵ�ֵ
    */
    Ctrl.Wifi.D2wRptSts.Dev_Status.Temperature     = Ctrl.Wifi.Rd.Temperature + 45;//Э�����У���������Сֵ-45�����Լ���45������ʵֵ
    Ctrl.Wifi.D2wRptSts.Dev_Status.Humidity        = Ctrl.Wifi.Rd.Humidity;


	Ctrl.Wifi.D2wRptSts.Sum = CheckSum((uint8_t *)&Ctrl.Wifi.D2wRptSts, sizeof(Ctrl.Wifi.D2wRptSts));

    Pro_UART_SendBuf((uint8_t *)&Ctrl.Wifi.D2wRptSts,sizeof(Ctrl.Wifi.D2wRptSts), 0);
}

/*******************************************************************************
* ��    �ƣ� Pro_UART_SendBuf
* ��    �ܣ� �򴮿ڷ�������֡
* ��ڲ����� buf:������ʼ��ַ�� packLen:���ݳ��ȣ� tag=0,���ȴ�ACK��tag=1,�ȴ�ACK��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ���ȴ�ACK������Э��ʧ���ط�3�Σ�����������FF�����������55
*******************************************************************************/
void Pro_UART_SendBuf(uint8_t *Buf, uint16_t PackLen, uint8_t Tag)
{
	uint16_t i;
	Pro_HeadPartTypeDef   Send_HeadPart;
	Pro_HeadPartTypeDef   Recv_HeadPart;

	for(i=0;i<PackLen;i++){
		USART_Send_Char(USART3,Buf[i]);
		if(i >=2 && Buf[i] == 0xFF) USART_Send_Char(USART3,0x55);
	}

	if(Tag == 0) return ;

	memcpy(&Send_HeadPart, Buf, sizeof(Send_HeadPart));
	memset(&Ctrl.Wifi.WaitAck, 0, sizeof(Ctrl.Wifi.WaitAck));

	while(Ctrl.Wifi.WaitAck.SendNum < Send_MaxNum) {
		if(Ctrl.Wifi.WaitAck.SendTime < Send_MaxTime) {
			if(Ctrl.Wifi.UartHandle.Package_Flag) {
				memcpy(&Recv_HeadPart, Ctrl.Wifi.UartHandle.UART_Buf, sizeof(Recv_HeadPart));
				Ctrl.Wifi.UartHandle.Package_Flag = 0;
				if((Send_HeadPart.Cmd == (Recv_HeadPart.Cmd - 1)) && (Send_HeadPart.SN == Recv_HeadPart.SN))
					break;
			}
		} else {
			Ctrl.Wifi.WaitAck.SendTime = 0;
			for(i=0;i<PackLen;i++) {
				USART_Send_Char(USART3,Buf[i]);;
				if(i >=2 && Buf[i] == 0xFF)
					USART_Send_Char(USART3,0x55);
			}
			Ctrl.Wifi.WaitAck.SendNum++ ;
		}
	}
}

/*******************************************************************************
* ��    �ƣ� Log_UART_SendBuf
* ��    �ܣ� �򴮿ڷ�������֡
* ��ڲ����� buf:������ʼ��ַ�� packLen:���ݳ��ȣ� tag=0,���ȴ�ACK��tag=1,�ȴ�ACK��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ���ȴ�ACK������Э��ʧ���ط�3�Σ�����������FF�����������55
*******************************************************************************/
void Log_UART_SendBuf(uint8_t *Buf, uint16_t PackLen)
{
	uint16_t i;
	for(i=0;i<PackLen;i++){
		USART_Send_Char(USART1,Buf[i]);
		if(i >=2 && Buf[i] == 0xFF)
            USART_Send_Char(USART1,0x55);
	}
}

/*******************************************************************************
* ��    �ƣ� exchangeBytes
* ��    �ܣ� ģ���htons ���� ntohs�����ϵͳ֧�ֽ�����Ŀ�ֱ���滻��ϵͳ����
* ��ڲ����� value
* ���ڲ����� ���Ĺ��ֽ����short��ֵ
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
int16_t	exchangeBytes(int16_t	value)
{
	int16_t		tmp_value;
	uint8_t		*index_1, *index_2;

	index_1 = (uint8_t *)&tmp_value;
	index_2 = (uint8_t *)&value;

	*index_1 = *(index_2+1);
	*(index_1+1) = *index_2;

	return tmp_value;
}

/*******************************************************************************
* ��    �ƣ� CheckSum
* ��    �ܣ� У����㷨
* ��ڲ����� buf:������ʼ��ַ�� packLen:���ݳ��ȣ�
* ���ڲ����� У����
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t CheckSum( uint8_t *buf, int packLen )
{
    uint16_t	i;
	uint8_t		sum;
	if(buf == NULL || packLen <= 0) return 0;
	sum = 0;
	for(i=2; i<packLen-1; i++)
		sum += buf[i];

	return sum;
}
/*******************************************************************************
 * 				end of file
 *******************************************************************************/

