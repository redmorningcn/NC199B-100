/*******************************************************************************
 *   Filename:       Protocol.h
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2014-08-11
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ������WIFIͨ��Э��ͷ�ļ�
 *
 *
 *   Notes:
 *
 *   All copyrights reserved to Roger-WY
 *******************************************************************************/
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stdio.h>
#include <stm32f10x.h>

/***********************************************
* ������ WIFIģ�鸴λ���Ŷ���
*/
#define WIFIMODE_RESET_RCC      RCC_APB2Periph_GPIOB
#define WIFIMODE_RESET_PIN      GPIO_Pin_14
#define WIFIMODE_RESET_PORT     GPIOB

#define WIFIMODERESET_HIGH()    GPIO_SetBits(WIFIMODE_RESET_PORT, WIFIMODE_RESET_PIN)
#define WIFIMODERESET_LOW()     GPIO_ResetBits(WIFIMODE_RESET_PORT, WIFIMODE_RESET_PIN)


/*******************************************************************************
*
*   AppID��ʲô��
*
*   ��AppID��Ӧ�ñ�ʶ�롣����������ҪΪһ�����ܲ�Ʒ����Ӧ�ã�����iOS��Android��WebӦ�õȣ�ʱ��
*   ��̨���Զ�����һ��AppID��������豸���й�����Ӧ�ÿ���ʱ��Ҫ�����AppID��
*   ÿ��AppID��Ӧ���û�ϵͳ���Ƕ����ģ�Ҳ������AppID A��Ӧ��Appע����û���
*   �޷���AppID B��Ӧ��App�е�¼����Ҫ����ע�Ტ���豸��
*   Ӧ�ñ�ʶ�루ProductKey ���Ͳ�Ʒ��ʶ�루AppID���ڲ�Ʒ����ʱͬʱ���ɣ�
*   ������֮��û��ֱ�ӵĶ�Ӧ��ϵ��һ��AppID���Զ�Ӧ���ProductKey��һ��ProductKeyҲ���Զ�Ӧ���AppID��
*
*
*   ProductKey��ʲô��
*
*   ��ProductKey�ǲ�Ʒ��ʶ�룬������ͨ�������ƺ�̨�����²�Ʒ���Զ����ɵ�һ��32λ�ַ�����
*   �ڻ����Ƶ����ݿ�����һ��Ψһ�ĺ��룬��������ɿ���д���豸����MCU�󣬻�����ͨ���˱�ʶ����豸����ʶ���Զ����ע�ᡣ
*   ʹ�û����Ƶ�SDK����Appʱ��Ҳ��Ҫʹ�õ���ProductKey�����豸�б���й��ˣ���Appֻ����������Ӧ���豸��
*   �������ȡ�����ڱ�Ĳ�Ʒ���豸��һ��ProductKey��Ӧ����һ����Ʒ��������ֻ��Ӧ�ò�Ʒ�ĵ����豸��
*   ����һ��ProductKey��Ӧ����ĳ����������������ĸÿ����ʹ�õĶ���ͬһ��ProductKey��������һ̨һ��ProductKey��
*
*
*   did��ʲô��
*
*   ��did���豸�ţ�Device ID������һ���豸���ν��������ʱ���������Զ�����ProductKey�Լ��豸Wi-Fiģ��MAC��ַΪ���豸ע��һ��did��
*   ��didȫ��Ψһ���������û��İ󶨼�����Զ�̿��Ʋ�����did��Ƶĳ����Ǹ�ÿ̨�豸һ��Ψһ��ʶ�룬
*   Զ�̿�����ͨ����Ψһ��ʶ��ʶ��Ŀ���豸��ͬʱ�����ڰ�ȫ�Կ��ǣ����豸����Ҫ���û��˺Ű󶨲���Զ�̿��ơ�
*   ���⣬����ͨ������ģ��ķ�ʽ����ģ���������ƶ�ע�Ტ�����µ�did��������������󶨵��û��޷��ٿ���Ŀ���豸��
*
*
*   passcode��ʲô?
*
*   ��passcode���豸ͨ��֤������У���û��İ�/����Ȩ�ޡ��豸��������ʱ��Wi-Fiģ�������һ��passcode�����豸���ϴ��ƶˣ�
*   ���û������豸��ʱ��ֻҪ�ǺϷ��������ɻ�ȡ��ͨ��֤��ͨ����ͨ��֤���԰��豸����ͨ��֤�Ļ�ȡʱ�޿�����MCU����Э����ָ����
*   ����趨�˻�ȡʱ�ޣ���ֻ���ڸ�ʱ���ڻ�ȡ�Ͱ󶨣�����Ƿ��û�������豸��ֻ���û�ʹ���豸���������ù����Ժ�
*   �������»�ȡ��passcode�����⣬����ͨ������ģ��ķ�ʽ����ģ����������һ��passcode��
*
*   productkey.JSON�ļ���ʲô��
*
*   �𣺻����Ƶ�SDK����Appʹ�õ�ʱ���Զ�����һ���Բ�Ʒ��productkey������JSON�����ļ���
*   SDKͨ���������ļ�����JSONָ�����Ϊ������ָ�������豸����֮ͬ��
*   ���SDK�Ѿ���⵽�ò�Ʒ��Ӧ�������ļ������أ��򲻻��������أ����Ը������ƶ˵����ݵ㣬��Ҫ���¸��������ļ���
*   ��ͨ�����App������ߵ���SDK��updateDeviceFromServer�ӿ��������������ļ���
*
*
*******************************************************************************/

/*******************************************************************************
* ������ �궨���豸��Ϣ��Э��汾�� ��Ӳ���汾�š�����汾�š���Ʒ��Կ
*/
#define		PRO_VER						"00000004"
#define		P0_VER						"00000004"
#define		HARD_VER					"00000001"
#define		SOFT_VER					"00000001"
#define		PRODUCT_KEY					"71d8060076e34c14af812c6ef606d131"

#define     EXT_DATA_LEN                16

/***********************************************
* ������ ����ͨ�Žṹ��
*/
#define Max_UartBuf             150

//#pragma pack(push)  //�������״̬
//#pragma pack(1)     //�趨Ϊ4�ֽڶ���

__packed	
typedef struct
{
	uint8_t				UART_RecvTimeoutCtr     : 6;    	
	uint8_t				UART_RecvFlag           : 1;    	
	uint8_t				Package_Flag            : 1;    	//�ж��Ƿ���յ�һ�������Ĵ������ݰ�
	uint8_t				UART_Flag1;
	uint8_t				UART_Flag2;
	uint16_t 			UART_Count;					//���ڻ������������ݳ���
	uint8_t				UART_Cmd_len;				//ָ���
	uint8_t 			UART_Buf[Max_UartBuf]; 		//���ڻ�����
	uint8_t             Message_Buf[Max_UartBuf];   //������յ�ָ���Buf
	uint8_t             Message_Len;	            //������Ϣ����

}UART_HandleTypeDef;

/***********************************************
* ����������Э��ָ���
*/
typedef enum
{
    Pro_W2D_GetDeviceInfo_Cmd 				= 0x01,   //WIFI���豸 ��ȡ�豸��Ϣ
    Pro_D2W__GetDeviceInfo_Ack_Cmd			= 0x02,   //�豸��WIFI �ظ��豸��Ϣ

    Pro_W2D_P0_Cmd 							= 0x03,   //WIFI���豸 ��ȡ�豸״̬
    Pro_D2W_P0_Ack_Cmd						= 0x04,   //�豸��WIFI �ظ��豸״̬

	Pro_D2W_P0_Cmd 							= 0x05,   //�豸��WIFI �����ϱ��豸״̬
    Pro_W2D_P0_Ack_Cmd						= 0x06,   //WIFI���ظ�

	Pro_W2D_Heartbeat_Cmd 					= 0x07,   //WIFI���豸 ����������
    Pro_D2W_heartbeatAck_Cmd				= 0x08,   //�豸��WIFI �ظ�������

	Pro_D2W_ControlWifi_Config_Cmd 			= 0x09,   //�豸��WIFI ֪ͨWIFI��������ģʽ
    Pro_W2D_ControlWifi_Config_Ack_Cmd	    = 0x0A,   //WIFI���豸 �ظ�ģ���������ģʽ

	Pro_D2W_ResetWifi_Cmd 					= 0x0B,   //�豸��WIFI ��������WIFIģ��ָ��
    Pro_W2D_ResetWifi_Ack_Cmd				= 0x0C,   //WIFI���豸 �ظ�����WIFIģ��

	Pro_W2D_ReportWifiStatus_Cmd 			= 0x0D,   //WIFI���豸 ����WIFIģ�鹤��״̬�ı仯
    Pro_D2W_ReportWifiStatus_Ack_Cmd		= 0x0E,   //�豸��WIFI �ظ�WIFIģ�鹤��״̬�ı仯

	Pro_W2D_ReportWifiReset_Cmd 			= 0x0F,   //WIFI���豸 ������������MCU
    Pro_D2W_ReportWifiReset_Ack_Cmd			= 0x10,   //�豸��WIFI �ظ���������MCU

	Pro_W2D_ErrorPackage_Cmd 				= 0x11,   //WIFI���豸 ���ͷǷ����ݰ�����֪ͨ
    Pro_D2W_ErrorPackage_Ack_Cmd			= 0x12,   //�豸��WIFI �ظ��Ƿ����ݰ�����֪ͨ

}Pro_CmdTypeDef;

/***********************************************
* ������ö��WIFIģ���������ģʽ�ķ���
*/
typedef enum
{
    SoftApMethod  = 0x01,       //SoftAp�ķ�������
	AirLinkMethod = 0x02,       //AirLink�ķ�������
                                //��������
}WifiConfigMethodTypeDef;

__packed
typedef union _uEXTENDED {
    struct _sEXTENDED {
        u32     AirVolume;
        u8      Pm2d5L;
        u8      Pm2d5H;
    } dat;
    u8          buf1[EXT_DATA_LEN];
    u16         buf2[EXT_DATA_LEN/2];
    u32         buf3[EXT_DATA_LEN/4];
} UnionExtData;
    
/***********************************************
* ������ֻ���豸�б��Ժ�Ҫ���ֻ���豸����ӵ�����
*/
__packed	
typedef struct
{
    uint8_t         FilterLife;
	uint16_t        Voc;
    int8_t          Temperature;
	uint8_t         Humidity;
    uint16_t        PM25Val;
    uint8_t         PM25Level;
}Device_ReadTypeDef;

/***********************************************
* ��������д�豸�б��Ժ�Ҫ��ӿ�д�豸����ӵ�����
*/
__packed	
typedef struct
{
    /***********************************************
    * ������ ��ͯ����־λ
    */
    uint8_t  ChildrenLockFlag;
    /***********************************************
    * ��������������
    */
    uint8_t  FilterLife;
    /***********************************************
    * �����������ظ�
    */
    uint8_t  WeekRepeat;
    /***********************************************
    * ����������ʱ����
    */
    uint16_t CountDownOnMin;
    /***********************************************
    * ����������ʱ�ػ�
    */
    uint16_t CountDownOffMin;
    /***********************************************
    * ��������ʱ����
    */
    uint16_t TimingOn;
    /***********************************************
    * ��������ʱ�ػ�
    */
    uint16_t TimingOff;
    /***********************************************
    * ��������ͯ��
    */
    uint8_t  ChildSecurityLock ;
    /***********************************************
    * ������PM2.5ƫ��
    */
    uint16_t Offset;
    /***********************************************
    * ��������չͨ��
    */
    UnionExtData    Ext;

} Device_WirteTypeDef;

/***********************************************
* ����������ָ��ö��
*/
typedef enum
{
    SetLED_OnOff    = 0x01,
	SetLED_Color    = 0x02,
	SetLED_R       	= 0x04,
	SetLED_G    	= 0x08,
	SetLED_B  		= 0x10,
	SetMotor		= 0x20,
}Attr_FlagsTypeDef;

typedef enum
{
    LED_OnOff       = 0x00,
	LED_OnOn        = 0x01,
	LED_Costom      = 0x00,
	LED_Yellow    	= 0x02,
	LED_Purple  	= 0x04,
	LED_Pink		= 0x06,

}LED_ColorTypeDef;

/***********************************************
* �������豸���������ϵ���Ϣ
*/
__packed	
typedef struct
{
	uint8_t		    Alert;
	uint8_t		    Fault;

}Device_RestsTypeDef;

/***********************************************
* �����������豸���в����Ľṹ��
*/
__packed	
typedef struct
{
	Device_WirteTypeDef     Device_Wirte;
	Device_ReadTypeDef      Device_Read;
	Device_RestsTypeDef     Device_Rests;
}Device_AllTypeDef;

/***********************************************
* ������WiFiģ���ȡ�豸�ĵ�ǰ״̬���ϱ���
*/
__packed
typedef struct {
    /***********************************************
    * ������״̬�� ��bety0/bety1��
    */
    union __statusword{
        struct __ustatusword {
            uint16_t  Switch              :1;//��Դ����        bool��
            uint16_t  SwitchPlasma        :1;//�����ӿ���      bool��
            uint16_t  LedAirQuality       :1;//��������ָʾ��  bool��
            uint16_t  ChildSecurityLock   :1;//��ͯ��          bool��
            uint16_t  WindVelocity        :2;//����
            uint16_t  AirSensitivity      :3;//�������������
            uint16_t  FanLevel            :3;//���ת��
            uint16_t  CtrlMode            :3;//����ģʽ
            uint16_t  RSV                 :1;//Ԥ��
        } udat;
        uint16_t uStatusWord;
    } StatusWord;
    /***********************************************
    * �������������� ��bety2��
    */
    uint8_t  FilterLife;
    /***********************************************
    * �����������ظ� ��bety3��
    */
    uint8_t  WeekRepeat;
    /***********************************************
    * ����������ʱ������bety4/bety5��
    */
    uint16_t CountDownOnMin;
    /***********************************************
    * ����������ʱ�ػ� ��bety6/bety7��
    */
    uint16_t CountDownOffMin;
    /***********************************************
    * ��������ʱ���� ��bety8/bety9��
    */
    uint16_t TimingOn;
    /***********************************************
    * ��������ʱ�ػ� ��bety10/bety11��
    */
    uint16_t TimingOff;
    /***********************************************
    * ������ƫ�� ��bety12/bety13��
    */
    uint16_t Offset;
    /***********************************************
    * ��������չͨ�� ��bety14/bety29��
    */
    UnionExtData    Ext;
    /***********************************************
    * ���������������������� ��bety30��
    */
    uint8_t  AirQualty;
    /***********************************************
    * ����������������ζ��VOC�� ��bety31��
    */
    uint8_t  PeculiarAirQuality;
    /***********************************************
    * ������ �¶�ֵ ��bety32��
    */
    int8_t  Temperature;
    /***********************************************
    * ������ �¶�ֵ ��bety33��
    */
    uint8_t  Humidity;
    /***********************************************
    * ���������������۳���PM2.5�� ��bety34��35��
    */
    uint16_t  DustAirQuality;
    /***********************************************
    * ������������bety36��
    */
    union __alert {
        struct __ualert {
            uint8_t  FilterLife :1;//��о��������
            uint8_t  AirQuality :1;//������������
            uint8_t  RSV        :6;//Ԥ��
        }udat;
        uint8_t  uAlert;
    }Alert;
    /***********************************************
    * ���������� ��bety37��
    */
    union __fault {
        struct __ufault {
            uint8_t  Motor       :1;  //�������
            uint8_t  AirSensor   :1;  //������������VOC������
            uint8_t  DustSensor  :1;  //�ҳ�����������
            uint8_t  RSV         :5;  //Ԥ��
        }udat;
        uint8_t  uFault;
    }Fault;
}Dev_StatusTypeDef;

/***********************************************
* ������WIFIģ������豸�ṹ��
*/
__packed
typedef struct
{
    /***********************************************
    * ���������ñ�־λ (ֻ�е����ñ�־λλ1ʱ�����������ֵ��������Ч)
    */
    union __attrflags{
        struct __uattrflags {
           uint16_t  Switch              :1;
           uint16_t  SwitchPlasma        :1;
           uint16_t  LedAirQuality       :1;
           uint16_t  ChildSecurityLock   :1;
           uint16_t  WindVelocity        :1;
           uint16_t  AirSensitivity      :1;
           uint16_t  FanLevel            :1;
           uint16_t  CtrlMode            :1;
           uint16_t  FilterLife          :1;
           uint16_t  WeekRepeat          :1;
           uint16_t  CountDownOnMin      :1;
           uint16_t  CountDownOffMin     :1;
           uint16_t  TimingOn            :1;
           uint16_t  TimingOff           :1;
           uint16_t  Offset              :1;
           uint16_t  ExtendedData        :1;
        }udat;
        uint16_t uAttrFlags;
    }AttrFlags;

    struct __attrvals {
        /***********************************************
        * ������״̬�� ��bety0/bety1��
        */
        union __setstatusword{
            struct __usetstatusword {
                uint16_t  Switch              :1;//��Դ����        bool��
                uint16_t  SwitchPlasma        :1;//�����ӿ���      bool��
                uint16_t  LedAirQuality       :1;//��������ָʾ��  bool��
                uint16_t  ChildSecurityLock   :1;//��ͯ��    bool��
                uint16_t  WindVelocity        :2;//����
                uint16_t  AirSensitivity      :3;//�������������
                uint16_t  FanLevel            :3;//���ת��
                uint16_t  CtrlMode            :3;//����ģʽ
                uint16_t  RSV                 :1;//Ԥ��
            }udat;
            uint16_t uSetStatusWord;
        }SetStatusWord;
        /***********************************************
        * �������������� ��bety2��
        */
        uint8_t  SetFilterLife;
        /***********************************************
        * �����������ظ� ��bety3��
        */
        uint8_t  SetWeekRepeat;
        /***********************************************
        * ����������ʱ������bety4/bety5��
        */
        uint16_t SetCountDownOnMin;
        /***********************************************
        * ����������ʱ�ػ� ��bety6/bety7��
        */
        uint16_t SetCountDownOffMin;
        /***********************************************
        * ��������ʱ���� ��bety8/bety9��
        */
        uint16_t SetTimingOn;
        /***********************************************
        * ��������ʱ�ػ� ��bety10/bety11��
        */
        uint16_t SetTimingOff;
        /***********************************************
        * ������PM2.5ƫ�� ��bety12/bety13��
        */
        uint16_t Offset;
        /***********************************************
        * ��������չͨ�� ��bety14/bety23��
        */
        UnionExtData    Ext;
    }AttrVals;
}AttrCtrlTypeDef;

/***********************************************
* ������ACK �ظ�����
*/
#define Send_MaxTime   200
#define Send_MaxNum    3

__packed	
typedef struct
{
	uint8_t			SendTime;
	uint8_t			SendNum;

}Pro_Wait_AckTypeDef;

/***********************************************
* ������ͨ��Э��Ĺ��ò���
*/
__packed	
typedef struct
{
	uint8_t			Head[2];
	uint16_t		Len;
	uint8_t			Cmd;
	uint8_t			SN;
	uint8_t			Flags[2];

}Pro_HeadPartTypeDef;

/***********************************************
* ������WiFiģ�������豸��Ϣ ���������ƶ�ע����豸��
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef  		    Pro_HeadPart;
	uint8_t							Pro_ver[8];
	uint8_t							P0_ver[8];
	uint8_t							Hard_ver[8];
	uint8_t							Soft_ver[8];
	uint8_t							Product_Key[32];
	uint16_t						Binable_Time;
	uint8_t							Sum;

}Pro_M2W_ReturnInfoTypeDef;

/***********************************************
* ������ͨ������ظ�֡ ��������ack�ȿ��Ը��ô�֡��
*       ����һ������֡�Ļظ���
*       WiFiģ�����豸MCU������
*       �豸MCU����WiFiģ��
*       WiFiģ����������MCU
*       Wifiģ��ظ�
*       �豸MCU�ظ�
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	uint8_t					Sum;
}Pro_CommonCmdTypeDef;

/***********************************************
* �������豸MCUͨ��WiFiģ���������ģʽ
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef  	Pro_HeadPart;
	uint8_t                 Config_Method;
	uint8_t					Sum;
}Pro_D2W_ConfigWifiTypeDef;


/***********************************************
* ������WiFiģ�����豸MCU֪ͨWiFiģ�鹤��״̬�ı仯
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef    	Pro_HeadPart;
	union __wifistatus {
        struct __wifi_status {
            uint16_t   SoftApMode       : 1; //�Ƿ���SoftAPģʽ 0���ر� 1������
            uint16_t   StationMode      : 1; //�Ƿ���Stationģʽ 0���ر� 1������
            uint16_t   ConfigMode       : 1; //�Ƿ�������ģʽ 0���ر� 1������
            uint16_t   BindingMode      : 1; //�Ƿ�����ģʽ 0���ر� 1������
            uint16_t   ConnRouter       : 1; //WIFIģ���Ƿ�ɹ�����·���� 0��δ���� 1������
            uint16_t   ConnClouds       : 1; //WIFIģ���Ƿ�ɹ������ƶ�   0��δ���� 1������
            uint16_t   RSV              : 2; //bit6-bit7Ԥ��
            uint16_t   Signal           : 3; //����WIFIģ���ѳɹ�����·����ʱ����Ч����ʾWIFIģ�鵱ǰ����AP���ź�ǿ�ȣ�RSSI�� ��Χ��0-7 ��7���ź�ǿ����ǿ��
            uint16_t   Phone            : 1; //�Ƿ����а󶨵��ֻ����� 0���ر� 1������
            uint16_t   ProductionTest   : 1; //�Ƿ��ڲ���ģʽ�� 0��û��   1����
            uint16_t   RSV1             : 3; //bit13-bit15Ԥ��
        }ustatus;
        uint16_t       uWifiStatus;
    }WifiStatus;
	uint8_t					Sum;
}Pro_W2D_WifiStatusTypeDef;

/***********************************************
* �������Ƿ���Ϣ֪ͨö���б�
*/
typedef enum
{
    Error_AckSum = 0x01,        //У��ʹ���
    Error_Cmd 	 = 0x02,		//�����ʶ��
    Error_Other  = 0x03,        //��������
}Error_PacketsTypeDef;

/***********************************************
* �������Ƿ���Ϣ֪ͨ
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	Error_PacketsTypeDef		Error_Packets;
	uint8_t						Sum;
}Pro_ErrorCmdTypeDef;

/***********************************************
* ������P0 �����б�
*/
typedef enum
{
	P0_W2D_Control_Devce_Action 		= 0x01,  //�����豸����
    P0_W2D_ReadDevStatus_Action 		= 0x02,  //��ȡ�豸״̬����
	P0_D2W_ReadDevStatus_Action_ACK     = 0x03,  //�ظ���ȡ�豸״̬����
	P0_D2W_ReportDevStatus_Action       = 0X04,  //�����ϱ��豸״̬����

}P0_ActionTypeDef;


/***********************************************
* ��������P0ָ��Ĺ�������
*/
__packed	
typedef struct
{
	Pro_HeadPartTypeDef  		Pro_HeadPart;
	P0_ActionTypeDef            Action;
}Pro_HeadPartP0CmdTypeDef;

/***********************************************
* ������WiFiģ���ȡ�豸�ĵ�ǰ״̬  ��WIFIģ��ѯ�ʣ�
*/
__packed	
typedef struct
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	uint8_t								Sum;
}Pro_W2D_ReadDevStatusTypeDef;

/***********************************************
* ��������ȡ�豸�ĵ�ǰ״̬  ���豸�����ϱ���
*/
__packed	
typedef struct
{
	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	//Device_AllTypeDef       			Device_All;
    Dev_StatusTypeDef                   Dev_Status;
	uint8_t								Sum;
}Pro_D2W_ReportDevStatusTypeDef;

/***********************************************
* ������WiFiģ������豸
*       WiFiģ�鷢��
*/
__packed	
typedef struct
{

	Pro_HeadPartP0CmdTypeDef 			Pro_HeadPartP0Cmd;
	//Attr_FlagsTypeDef                 Attr_Flags;
	//Device_WirteTypeDef     			Device_Wirte;
    AttrCtrlTypeDef                     AttrCtrl;
	uint8_t							  	Sum;
}Pro_P0_ControlTypeDef;
//#pragma pack(pop)   //�������״̬

//extern UART_HandleTypeDef  				UART_HandleStruct;
//extern Pro_M2W_ReturnInfoTypeDef        Pro_M2W_ReturnInfoStruct;
//extern Pro_HeadPartTypeDef              Pro_HeadPartStruct;
//extern Pro_CommonCmdTypeDef 			Pro_CommonCmdStruct;
//extern Pro_Wait_AckTypeDef              Wait_AckStruct;
//extern Pro_D2W_ReportDevStatusTypeDef   Pro_D2W_ReportStatusStruct;
//extern Pro_W2D_WifiStatusTypeDef        Pro_W2D_WifiStatusStruct;
//extern Dev_StatusTypeDef                Dev_Status;
//extern AttrCtrlTypeDef                  AttrCtrl;

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
void        MessageHandle                       (void);
void        Pro_W2D_GetMcuInfo                  (void);
void        Pro_W2D_CommonCmdHandle             (void);
void        Pro_W2D_WifiStatusHandle            (void);
void        Pr0_W2D_RequestResetDeviceHandle    (void);
void        Pro_W2D_ErrorCmdHandle              (void);
void        Pro_W2D_P0CmdHandle                 (void);
void        Pro_W2D_Control_DevceHandle         (void);
void        Pro_W2D_ReadDevStatusHandle         (uint8_t ucmd);
void        Pro_D2W_ReportDevStatusHandle       (uint8_t ucmd);
    
void        Pro_UART_SendBuf                    (uint8_t *Buf, uint16_t PackLen, uint8_t Tag);
int16_t	    exchangeBytes                       (int16_t	value);
uint8_t     CheckSum                            ( uint8_t *buf, int packLen );

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif /*_PROTOCOL_H*/



