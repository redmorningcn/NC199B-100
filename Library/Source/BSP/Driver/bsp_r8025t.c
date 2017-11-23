/*******************************************************************************
*   Filename:      bsp_r8025t.c
*   Revised:       $Date: 2015-01-26
*   Revision:      $V1.0
*   Writer:        Roger-WY.
*
*   Description:   RX8025ʵʱʱ��оƬ����ģ�顣
*
*   Notes:
*
*
*   All copyrights reserved to Roger-WY.
*
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include "bsp_r8025t.h"


#define  BSP_RX8025T_MODULE_EN   1
#if  BSP_RX8025T_MODULE_EN > 0

/*******************************************************************************
 * ������ ��ģ���Ƿ����uCOSϵͳʹ��
 */
#define  UCOS_EN        DEF_ENABLED

/*******************************************************************************
 * ������ RX8025оƬ�ͺŶ���
 */
#define RX8025AC                    0
#define RX8025T                     1

/*******************************************************************************
* ������ RX8025�ľ����ͺŶ��壨һ��Ҫע���ͺ�Ҫ�ԣ������޷�������������
*/
static  uint8_t    DevType    = RX8025AC;


/***********************************************
* ������ ʱ�䣨us��
*/
#define DLY_CLK_SDA             2               // >=1.0us
#define DLY_CLK_LOW             2               // >=1.3us
#define DLY_CLK_HIGH            3               // >=0.6us
#define DLY_SU_STA              2               // >=0.6us
#define DLY_HD_STA              2               // >=0.6us
#define DLY_SU_STO              2               // >=0.6us
#define DLY_SDA_BUF             3               // >=1.3us
/***********************************************
* ������ ʱ�䣨us��
*/
#define DLY_WR_STO              70              // >61us
/***********************************************
* ������ Ӧ����
*/
#define I2C_ACK                 0X01            // Ӧ��
#define I2C_NACK                0X00            // ��Ӧ��
/***********************************************
* ������ ��ƽ����
*/
#define LOW                     0X00            // �͵�ƽ
#define HIGH                    0X01            // �ߵ�ƽ

/***********************************************
* ������ RX8025T�Ĵ�����ַ����
*/
/* r8025N�Ĵ������� */
#define RX8025_REG_SEC          0x00
#define RX8025_REG_MIN          0x01
#define RX8025_REG_HOUR         0x02
#define RX8025_REG_WDAY         0x03
#define RX8025_REG_MDAY         0x04
#define RX8025_REG_MONTH        0x05
#define RX8025_REG_YEAR         0x06
#define RX8025_REG_DIGOFF       0x07//#define RX8025_REG_DIGOFF       0x07  8025Digital Offset
#define RX8025_REG_ALWMIN       0x08
#define RX8025_REG_ALWHOUR      0x09
#define RX8025_REG_ALWWDAY      0x0a
#define RX8025_REG_TMCNT0       0x0b//#define RX8025_REG_ALDMIN       0x0b
#define RX8025_REG_TMCNT1       0x0c//#define RX8025_REG_ALDHOUR      0x0c
/* 0x0d is reserved */
#define RX8025_REG_FLAG         0x0e//#define RX8025_REG_CTRL1        0x0e
#define RX8025_REG_CTRL         0x0f//#define RX8025_REG_CTRL2        0x0f

#define RX8025_BIT_FLAG_CT0    (1<<0)//
#define RX8025_BIT_FLAG_CT1    (1<<1)//
#define RX8025_BIT_FLAG_CT2    (1<<2)//
#define RX8025_BIT_FLAG_TEST   (1<<3)// ���Ҳ���
#define RX8025_BIT_FLAG_RSV    (1<<4)// Ԥ��
#define RX8025_BIT_FLAG_12_24  (1<<5)// 0��12Сʱ�ƣ�1��24Сʱ��
#define RX8025_BIT_FLAG_DALE   (1<<6)// ʱ��һ�µľ�����������
#define RX8025_BIT_FLAG_WALE   (1<<7)// ���ڣ�ʱ����һ�±���

#define RX8025_BIT_CTRL_DAFG   (1<<0)// ֻ��DAFG?bitΪ1ʱ��ЧAlarm_D���趨ʱ��������ʱ��һ��
#define RX8025_BIT_CTRL_WAFG   (1<<1)//
#define RX8025_BIT_CTRL_CTFG   (1<<2)//
#define RX8025_BIT_CTRL_RSV    (1<<3)//
#define RX8025_BIT_CTRL_PON    (1<<4)// ����Դ��λ���ʱPON?bitΪ1
#define RX8025_BIT_CTRL_XSTL   (1<<5)//
#define RX8025_BIT_CTRL_VDET   (1<<6)//
#define RX8025_BIT_CTRL_VDSL   (1<<7)//

#define RX8025_ADDR_SEC          0x00
#define RX8025_ADDR_MIN          0x01
#define RX8025_ADDR_HOUR         0x02
#define RX8025_ADDR_WEEK         0x03
#define RX8025_ADDR_DAY          0x04
#define RX8025_ADDR_MON          0x05
#define RX8025_ADDR_YEAR         0x06

#define RX8025_ADDR_CONTROL1     0x0E
#define RX8025_ADDR_CONTROL2     0x0F

#define BSP_RX8025T_ADDR         0x64
#define BSP_RX8025T_ADDR_R       0x65

#define I2C_MAX_BYTES            16



/***********************************************
* ������ OS�ӿ�
*/
#if (UCOS_EN     == DEF_ENABLED)
    #if OS_VERSION > 30000U
    static  OS_SEM                   Bsp_RX8025tSem;    // �ź���
    #else
    static  OS_EVENT                *Bsp_RX8025tSem;    // �ź���
    #endif
#endif

/*******************************************************************************
* GLOBAL FUNCTIONS
*/
static void    I2C_Start               (void);
static void    I2C_Stop                (void);

static uint8_t    I2C_SendByte         (uint8_t  byte);                         //���ݴӸ�λ����λ
static uint8_t    I2C_RecvByte         (uint8_t    ack, uint8_t *dat);          //���ݴӸ�λ����λ

//============================================================================//
/***********************************************
* ������ I2C�ؼ�ʱ��
*
*                ����ʼ/����/��ʼʱ��
*                : SCL ������������\______/��������\______/������������������������������\___
*                : SDA ��������\_______/������������\_______/����������������������\_____
*                :       |   |                 |   |         |   |
*                :       START      <0.5S      STOP   >61uS  START
*                ������ʱ��
*                : SCL ______/������������\___/����������������\______
*                : SDA _____/������������������X����������������������\
*                :     ������������������������������ ����������������������������������
*                ��      |���ݱ���  |   |���ݱ���|
*                ��                ���ݸı�
*/

/*******************************************************************************
* ��    �ƣ� I2C_Start
* ��    �ܣ� ����I2C���ߣ�������I2C��ʼ����
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��  ��ʼʱ��
*             SDA ��������\______
*             SCL ������������\____
*                   |   |
*                   START
*******************************************************************************/
static void I2C_Start(void)
{
    SET_RX8025x_SDA_OUTPUT();				/*SDA��Ϊ���                       */
    RX8025_SDA_H();				            /*������ʼ�����������ź�            */
    Delay_Nus(DLY_CLK_SDA);
    RX8025_SCL_H();
    Delay_Nus(DLY_SU_STA);		            /*��ʼ��������ʱ�����4.7us,��ʱ    */
    RX8025_SDA_L();					        /*������ʼ�ź�                      */
    Delay_Nus(DLY_HD_STA);			        /*��ʼ��������ʱ�����4��s           */
    RX8025_SCL_L();					        /*���������߸ı䣬׼�����ͻ��������*/
    Delay_Nus(DLY_CLK_LOW);                 /*CLK��SDA֮����ʱ                  */
}

/*******************************************************************************
* ��    �ƣ� I2C_Stop
* ��    �ܣ� ����I2C���ߣ�������I2C��������
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ֹͣʱ��
*            SDA _______/����������
*            SCL _____/��������������
*                      |   |
*                      STOP
*******************************************************************************/
static void I2C_Stop(void)
{
	SET_RX8025x_SDA_OUTPUT();				            /*SDA��Ϊ���*/
    RX8025_SCL_L(); 					            /*ǯסI2C���ߣ�׼�����ͻ�������� */
	Delay_Nus(DLY_CLK_SDA);
    RX8025_SDA_L(); 					            /*���ͽ��������������ź�*/
    Delay_Nus(DLY_CLK_SDA);
    RX8025_SCL_H(); 					            /*���ͽ���������ʱ���ź�*/
	Delay_Nus(DLY_SU_STO);			            /*������������ʱ�����4��s*/
    RX8025_SDA_H(); 					            /*����I2C���߽����ź�*/
    Delay_Nus(DLY_SDA_BUF); 		            /*>4us*/
}

/*******************************************************************************
* ��    �ƣ� I2C_SendByte
* ��    �ܣ� �ֽ����ݴ��ͺ����������� c ���ͳ�ȥ�������ǵ�ַ��Ҳ���������ݣ�
             �����ȴ�Ӧ�𣬲��Դ�״̬λ���в���
* ��ڲ����� uint8_t ������
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʱ��
*            SDA _____/������������������X����������������������\
*                ������������������������������������������������������������������
*            SCL ______/������������\___/����������������\______
*                  |���ݱ���  |   |���ݱ���|
*                            ���ݸı�
*******************************************************************************/
static uint8_t I2C_SendByte(uint8_t byte) 			//���ݴӸ�λ����λ//
{
    INT08S  ctr     = 0;                            // ��ʼ������λ������
    uint8_t  ack     = 0;

    /***********************************************
    * ��������������
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ����Ƿ��Ѿ��������
        RX8025x_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼����������
        RX8025x_SET_SDA((byte & 0x80) == 0x80);         // SDA = 0 �������ݣ�0;SDA = 1 �������ݣ�1
        RX8025x_SET_SCL(HIGH);                          // SCL = 1 �������ݣ��ӻ����Զ�ȡ
        byte <<= 1;                                 // ��ǰֵ����һλ
    };
    /***********************************************
    * ���������Ӧ��
    */
    RX8025x_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼������Ӧ��λ
    SET_RX8025x_SDA_INPUT();                        // SDA��Ϊ����
    RX8025x_SET_SDA(HIGH);                              // SDA = 1 �����ͷ�����

    RX8025x_SET_SCL(HIGH);                              // SCL = 1 ��������
    RX8025x_GET_SDA(ack);                               // ��ȡ�ӻ�Ӧ��ACK
    SET_RX8025x_SDA_INPUT();                        // SDA��Ϊ���
    RX8025x_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼������Ӧ��λ

    if (ack)                                    	// ���Ӧ���ź�
        return (FALSE);                             // NACK����
    else
        return (TRUE);                              // ACK����
}

/*******************************************************************************
* ��    �ƣ� I2C_RecvByte
* ��    �ܣ� ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
             ��Ҫ����Ӧ������
* ��ڲ����� ��
* ���ڲ����� uint8_t ������
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʱ��
*            SDA _____/������������������X����������������������\
*                ������������������������������������������������������������������
*            SCL ______/������������\___/����������������\______
*                  |���ݱ���  |   |���ݱ���|
*                            ���ݸı�
*******************************************************************************/
static uint8_t I2C_RecvByte(uint8_t ack, uint8_t *dat)  	//���ݴӸ�λ����λ
{
    uint8_t  ctr     = 0;
    INT08S  data    = 0;

    SET_RX8025x_SDA_INPUT();                        // SDA��Ϊ����
    RX8025x_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    /***********************************************
    * ��������������
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ��������һλ�����ڵ�һ��������0�������ƻ���0
        data <<= 1;                                 // ׼����һλ����
        RX8025x_SET_SCL(HIGH);                          // SCL = 1 �������ݣ����Զ�ȡ�ӻ�����
        RX8025x_GET_SDA(data);                          // ���Ϊ1����1�����򲻸ı䣨Ĭ��Ϊ0��
        RX8025x_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    };

    SET_RX8025x_SDA_OUTPUT();                       // SDA��Ϊ���
    /***********************************************
    * ���������Ӧ��
    */
    RX8025x_SET_SDA(!ack);                              // ACK,����0

    RX8025x_SET_SCL(LOW);
    RX8025x_SET_SCL(HIGH);
    RX8025x_SET_SDA(LOW);
    RX8025x_SET_SDA(HIGH);

    *dat    = data;

    return (TRUE);                            	    // Return 8-bit data byte
}

/*******************************************************************************
* ��    �ƣ� AT24CXX_Enabled
* ��    �ܣ� I2Cʹ��
* ��ڲ����� ��
* ���ڲ�����
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
static uint8_t BSP_I2C_Enabled( void )
{
    I2C_Start();
    return ( I2C_SendByte( BSP_RX8025T_ADDR ) );
}

/*******************************************************************************
* ��    �ƣ� BSP_I2C_GetReady
* ��    �ܣ� I2C�����Ƿ����
* ��ڲ����� ��
* ���ڲ�����
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
static uint8_t BSP_I2C_GetReady(void)
{
    return TRUE;
    //    int   timeout = 100;
    //
    //    while ( --timeout ) {
    //        if ( BSP_I2C_Enabled() == TRUE )
    //            return TRUE;
    //#if UCOS_EN     == DEF_ENABLED
    //#if OS_VERSION > 30000U
    //        BSP_OS_TimeDly(1);
    //#else
    //        OSTimeDly(1);
    //#endif
    //#else
    //        Delay_Nms(1);
    //#endif
    //    }
    //    return FALSE;
}

/*******************************************************************************
* ��    �ƣ� BSP_I2C_SetAddr
* ��    �ܣ� ���õ�ǰ��ַ
* ��ڲ����� suba - ��ַ�� opt - 0:д�� 1:��
* ���ڲ�����
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
static uint8_t BSP_I2C_SetAddr( INT16U suba, uint8_t opt)
{
    uint8_t      ack;

    ack = I2C_SendByte(BSP_RX8025T_ADDR);           // �����豸��ַ
    if(!ack)    goto exit;                          // ת������

#if I2C_MAX_BYTES > 256u
    ack = I2C_SendByte(suba>>8);                    // �����ڴ�ߵ�ַ
    if(!ack)    goto exit;                          // ת������
#endif

    ack = I2C_SendByte(suba);                       // �����ڴ�͵�ַ
    if(!ack)    goto exit;                          // ת������
    /***********************************************
    * ������ ���Ϊ�������Ͷ���ַ
    */
    if ( opt ) {
        I2C_Start();                                  // ���·�����ʼ����
        ack = I2C_SendByte(BSP_RX8025T_ADDR+1);
        if(!ack)  goto exit;                          // ת������
    }
    exit:
    return ack;
}

#if UCOS_EN     == DEF_ENABLED
/*******************************************************************************
* ��    �ƣ� BSP_RX8025T_WaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ�����  0���������󣩣�1�������ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static uint8_t BSP_RX8025T_WaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    return BSP_OS_SemWait(&Bsp_RX8025tSem,0);       // �ȴ��ź���
#else
    uint8_t       err;
    OSSemPend(Bsp_RX8025tSem,0,&err);               // �ȴ��ź���
    if ( err = OS_ERR_NONE )
        return TRUE;
    else
        return FALSE;
#endif
}

/*******************************************************************************
* ��    �ƣ� BSP_RX8025T_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
static void BSP_RX8025T_SendEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&Bsp_RX8025tSem);                // �����ź���
#else
    uint8_t       err;
    OSSemPost(Bsp_RX8025tSem);                      // �����ź���
#endif
}
#endif /* end of (UCOS_EN     == DEF_ENABLED)*/


/*******************************************************************************
* ��    �ƣ� BSP_I2C_WriteByte
* ��    �ܣ� ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ���̣��������1����ʾ
             �����ɹ��������������
* ��ڲ����� suba�� ��ַ  byteд�����ֽ�
* ���ڲ����� 0���������󣩣�1�������ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_I2C_WriteByte(INT16U suba,uint8_t byte )
{
    uint8_t   ack;

    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_WaitEvent();                        // ���ź���
    Delay_Nus(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();                                    // ������ʼ����

    ack = BSP_I2C_SetAddr(suba,0);                  // ���ö���ַ
    if(!ack)    goto exit;                          // ת������

    ack = I2C_SendByte(byte);                       // ����Ҫд�������
    if(!ack)    goto exit;                          // ת������

    exit:
    I2C_Stop();                                     // ����ֹͣ����

    BSP_RX8025T_SendEvent();                        // �ͷ��ź���

    return(ack);
}

/*******************************************************************************
* ��    �ƣ� BSP_I2C_ReadByte
* ��    �ܣ� ��һ���ֽ�
* ��ڲ����� suba�� ��ַ  byte �������ֽ�
* ���ڲ����� 0���������󣩣�1�������ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_I2C_ReadByte( INT16U suba, uint8_t *byte)
{
    uint8_t       ack;
    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_WaitEvent();                        // ���ź���
    Delay_Nus(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();                                    // ������ʼ����

    ack = BSP_I2C_SetAddr(suba,1);                  // ���ö���ַ
    if(!ack)    goto exit;                          // ת������

    ack = I2C_RecvByte(I2C_NACK,byte);              // �������ݲ����ط�Ӧ��
    exit:
    I2C_Stop();                                     // ����ֹͣ����

    BSP_RX8025T_SendEvent();                        // �ͷ��ź���

    return ack;
}

/*******************************************************************************
* ��    �ƣ� BSP_I2C_ReadBytes
* ��    �ܣ� ��һ���ַ���
* ��ڲ����� suba�� ��ַ  buf ����������ֽ����� len ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
INT16U BSP_I2C_ReadBytes(INT16U suba,uint8_t *buf, INT16U len)
{
    INT16U      bytes   = 0;
    uint8_t      ack;
    /***********************************************
    * ������ ����Ƿ񳬳��洢����
    */
    if ( (suba + len) > (I2C_MAX_BYTES) )
        return 0;
    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_WaitEvent();                        // ���ź���

    /***********************************************
    * ������ ����Ƿ�ɲ���
    */
    if ( BSP_I2C_GetReady() == FALSE )
        return 0;

    Delay_Nus(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms
    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    I2C_Start();                                    // ������ʼ����
    ack = BSP_I2C_SetAddr(suba,1);                  // ���ö���ַ

    if(!ack) {
        I2C_Stop();                                 // ����ֹͣ����
        return 0;
    }

    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    for( bytes = 0; bytes < len; bytes++ ) {
        /***********************************************
        * ������ ����Ƿ����һ�ֽ�
        */
        if ( bytes == len - 1 ) {
            ack = I2C_RecvByte(I2C_NACK, buf++);
        } else {
            ack = I2C_RecvByte(I2C_ACK, buf++);
        }
        /***********************************************
        * ������ ���Ӧ���ź��Ƿ���ȷ
        */
        if(!ack) {
            I2C_Stop();                             // ����ֹͣ����
            break;
        }
    }
    if(ack)
        I2C_Stop();                             // ����ֹͣ����
    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_SendEvent();                        // �ͷ��ź���

    return bytes;
}


/*******************************************************************************
* ��    �ƣ� BSP_I2C_WriteBytes
* ��    �ܣ� д����ֽ�
* ��ڲ����� suba�� ��ַ  buf ��Ҫд����ֽ����� len ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
INT16U BSP_I2C_WriteBytes( INT16U suba, uint8_t *pbuf, INT16U len )
{
    uint8_t      bytes   = 0;
    uint8_t      ack;
    /***********************************************
    * ������ ����ַ�Ƿ���������������ֱ�ӷ���
    */
    if ( (suba + len) > (I2C_MAX_BYTES) )
        return 0;

    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_WaitEvent();                        // ���ź���

    if ( BSP_I2C_GetReady() == FALSE )
        return 0;

    Delay_Nus(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();

    ack = BSP_I2C_SetAddr(suba,0);                  // ���ö���ַ
    if(!ack)    goto exit;                          // ת������

    for ( ; bytes < len ; bytes++ ) {
        ack = I2C_SendByte( pbuf[ bytes ] );
        if(!ack)    goto exit;                      // ת������
    }
    exit:
    I2C_Stop();

    /***********************************************
    * ������ OS�ӿ�
    */
    BSP_RX8025T_SendEvent();                        // �ͷ��ź���

    return bytes;
}

/*******************************************************************************
* ��    �ƣ� BSP_RX8025_Init
* ��    �ܣ� RX8025ʱ��оƬ��ʼ��
* ��ڲ����� ��
* ���ڲ����� 0����ʼ��ʧ�ܣ���1����ʼ���ɹ���
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_RX8025T_Init( void )
{
    GPIO_InitTypeDef    gpio_init;

    RCC_APB2PeriphClockCmd(RX8025x_SCL_RCC, ENABLE);
    gpio_init.GPIO_Pin   = RX8025x_SCL_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(RX8025x_SCL_PROT, &gpio_init);
    RCC_APB2PeriphClockCmd(RX8025x_SDA_RCC, ENABLE);
    gpio_init.GPIO_Pin   = RX8025x_SDA_PIN;
    GPIO_Init(RX8025x_SDA_PROT, &gpio_init);


    /***********************************************
    * ������ OS�ӿ�
    */
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
        BSP_OS_SemCreate(&Bsp_RX8025tSem,1, "Bsp RX8025tSem");  // �����ź���
    #else
        Bsp_RX8025tSem     = OSSemCreate(1);                    // �����ź���
    #endif
#endif

    /***********************************************
    * ������ ��ʼ��I2C���ߵ�ƽ
    */
    I2C_Start();
    I2C_Stop();
#if UCOS_EN     == DEF_ENABLED
    BSP_OS_TimeDly(5);
#else
    Delay_Nms(5);
#endif
    // SEC    MIN     HOUR    WEEK    DAY     MONTH   YEAR
    uint8_t time[16]  =   {  0x00,   0x57,   0x20,   0x02,   0x19,   0x05,   0x15,
    // RAM    MIN_AL  HOUR_AL WEEK_AL DAY_AL  CTR0    CTR1
    0X00,   0X00,   0X00,   0X00,           0X00,   0X00,
    //EXTENSION       FLAG    CTRL
    0X00,           0X20,   0X00};
    /***********************************************
    * ������ ��ȡ�Ĵ���״̬
    */
    BSP_I2C_ReadBytes(0, &time[0], 16);
    if ( time[2] == 0xFF ) {
        DevType = RX8025T;
    }

    if ( DevType == RX8025AC ) {
        //        BSP_I2C_ReadBytes(0, &time[0], 16);
        //        /***********************************************
        //        * ������ �ж�д��������Ƿ�ָ�
        //        */
        //        if ( RX8025_BIT_CTRL_PON == ( time[RX8025_REG_CTRL] & RX8025_BIT_CTRL_PON ) ) {
        //            BSP_I2C_WriteByte( RX8025_REG_DIGOFF, 0x00 );
        //            time[RX8025_REG_SEC]    = Hex2Bcd(30);
        //            time[RX8025_REG_MIN]    = Hex2Bcd(55);
        //            time[RX8025_REG_HOUR]   = Hex2Bcd(9);
        //            time[RX8025_REG_WDAY]   = 5;
        //            time[RX8025_REG_MDAY]   = Hex2Bcd(30);
        //            time[RX8025_REG_MONTH]  = Hex2Bcd(4);
        //            time[RX8025_REG_YEAR]   = Hex2Bcd(15);
        //            time[7]                 = 0x00;
        //            //for ( char i = 0; i < 16; i++ )
        //            BSP_I2C_WriteBytes(0, &time[0],16);
        //        }
    } else {
        for ( char i = 7; i < 8; i++ )
            BSP_I2C_ReadByte(i, &time[i]);
        if ( 0x05 != ( time[7] & 0x05 ) ) {
            time[RX8025_REG_SEC]    = Hex2Bcd(30);
            time[RX8025_REG_MIN]    = Hex2Bcd(55);
            time[RX8025_REG_HOUR]   = Hex2Bcd(9);
            time[RX8025_REG_WDAY]   = 5;
            time[RX8025_REG_MDAY]   = Hex2Bcd(30);
            time[RX8025_REG_MONTH]  = Hex2Bcd(4);
            time[RX8025_REG_YEAR]   = Hex2Bcd(15);
            time[7]                 = 0x05;

            for ( char i = 8; i < 16; i++)
                time[i] = 0x00;
            for ( char i = 0; i < 16; i++ )
                BSP_I2C_WriteByte(i, time[i]);
        }
    }

    return(TRUE);
}


/*******************************************************************************
* ��    �ƣ� BSP_RX8025T_GetTime
* ��    �ܣ� ��ȡʱ�䣺�꣬�£��գ����ڣ�ʱ���֣���
* ��ڲ����� *t_tm - ʱ����սṹ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ��bsp_time.c�е��øú���
*******************************************************************************/
uint8_t BSP_RX8025T_GetTime(struct tm *t_tm)
{
    u8 time[7];
    u8 i = 0;
    if ( DevType == RX8025AC ) {
        /***********************************************
        * ������ ��ȡʱ��
        */
        if ( 7 == BSP_I2C_ReadBytes(0, time, 7) ) {
            t_tm->tm_sec     = Bcd2Hex(time[0]);
            t_tm->tm_min     = Bcd2Hex(time[1]);
            t_tm->tm_hour    = Bcd2Hex(time[2]);
            t_tm->tm_wday    = time[3];
            t_tm->tm_mday    = Bcd2Hex(time[4]);
            t_tm->tm_mon     = Bcd2Hex(time[5]);
            t_tm->tm_year    = 2000+Bcd2Hex(time[6]);
            return TRUE;
        }
    } else {
        for (i = 0; i < 7; i++ ) {
            BSP_I2C_ReadByte(i,&time[i]);
        }
        if ( i == 7) {
            t_tm->tm_sec     = Bcd2Hex(time[0]);
            t_tm->tm_min     = Bcd2Hex(time[1]);
            t_tm->tm_hour    = Bcd2Hex(time[2]);
            t_tm->tm_wday    = time[3];
            t_tm->tm_mday    = Bcd2Hex(time[4]);
            t_tm->tm_mon     = Bcd2Hex(time[5]);
            t_tm->tm_year    = 2000+Bcd2Hex(time[6]);
            return TRUE;
        }
    }
    return FALSE;
}

/*******************************************************************************
* ��    �ƣ� BSP_RX8025T_SetDate
* ��    �ܣ� ����ʱ�䣺�꣬�£�ʱ�����ڣ�ʱ���֣���
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ��bsp_time.c�е��øú���
*******************************************************************************/
uint8_t BSP_RX8025T_SetDate(struct tm t_tm)
{
    uint8_t  time[16];
    uint8_t  i;
    /***********************************************
    * ������ �ж����Ƿ�Ϸ�
    */
    if ( t_tm.tm_year < 2000 )
        t_tm.tm_year = 2000;

    if ( DevType == RX8025AC ) {
        /***********************************************
        * ������ �ȶ�ȡ����
        */
        if ( 16 != BSP_I2C_ReadBytes(0, &time[0], 16) )
            return FALSE;

        time[0]     = Hex2Bcd(t_tm.tm_sec);
        time[1]     = Hex2Bcd(t_tm.tm_min);
        time[2]     = Hex2Bcd(t_tm.tm_hour);
        time[3]     = Hex2Bcd(t_tm.tm_wday);
        time[4]     = Hex2Bcd(t_tm.tm_mday);
        time[5]     = Hex2Bcd(t_tm.tm_mon);
        time[6]     = Hex2Bcd(t_tm.tm_year - 2000);
        /***********************************************
        * ������ RAM
        */
        time[7]     = 0X00;
        /***********************************************
        * ������ 24Сʱ��
        */
        time[14]   |= 0x20;

        /***********************************************
        * ������ д��ʱ��
        */
        if ( 16 == BSP_I2C_WriteBytes(0, &time[0], 16) )
            return TRUE;
    } else {
        /***********************************************
        * ������ �ȶ�ȡ����
        */
        time[0]     = Hex2Bcd(t_tm.tm_sec);
        time[1]     = Hex2Bcd(t_tm.tm_min);
        time[2]     = Hex2Bcd(t_tm.tm_hour);
        time[3]     = Hex2Bcd(t_tm.tm_wday);
        time[4]     = Hex2Bcd(t_tm.tm_mday);
        time[5]     = Hex2Bcd(t_tm.tm_mon);
        time[6]     = Hex2Bcd(t_tm.tm_year - 2000);
        /***********************************************
        * ������ RAM
        */
        //time[7]     = 0X00;
        /***********************************************
        * ������ 24Сʱ��
        */
        //time[14]   |= 0x20;
        /***********************************************
        * ������ д��ʱ��
        */
        for ( i = 0; i < 7; i++ ) {
            BSP_I2C_WriteByte(i,time[i]);
        }
        if ( i == 7)
            return TRUE;
    }
    return FALSE;
}

#endif
/*******************************************************************************
* 				end of file
*******************************************************************************/