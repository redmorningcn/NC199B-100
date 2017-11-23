/*******************************************************************************
*   Filename:       bsp_eeprom.c
*   Revised:        All copyrights reserved to Roger.
*   Date:           2015-08-18
*   Revision:       v1.0
*   Writer:	        wumingshen.
*
*   Description:    AT24Cxxxϵ��EEPROM����ģ��
*
*
*   Notes:
*
*   All copyrights reserved to wumingshen
*******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <bsp_eeprom.h>


#define  BSP_EEPROM_MODULE_EN   1
#if  BSP_EEPROM_MODULE_EN > 0

/*******************************************************************************
 * ������ ��ģ���Ƿ����uCOSϵͳʹ��
 */
#define  UCOS_EN            DEF_ENABLED
#define  GPIO_I2C1_PORT_EN  DEF_ENABLED
#define  GPIO_I2C2_PORT_EN  DEF_DISABLED
/***********************************************
* ������ AT24Cxxʱ��
*
Symbol  Parameter               |1.8-volt       |2.7-volt       |5.0-volt       Units
                                |Min    Max     |Min    Max     |Min    Max
fSCL    Clock Frequency, SCL    |       100     |       400     |       1000    kHz
tLOW    Clock Pulse Width Low   |4.7            |1.3            |0.6            ��s
tHIGH   Clock Pulse Width High  |4.0            |1.0            |0.4            ��s
tAA     Clock Low to Data Out   |               |               |
        Valid                   |0.1     4.5    |0.05    0.9    |0.05   0.55    ��s
tBUF    Time the bus must be    |               |               |
        free before a new       |               |               |
        transmission can start  |4.7            |1.3            |0.5            ��s
tHD.STA Start Hold Time         |4.0            |0.6            |0.25           ��s
tSU.STA Start Set-up Time       |4.7            |0.6            |0.25           ��s
tHD.DAT Data In Hold Time       |0              |0              |0              ��s
tSU.DAT Data In Set-up Time     |200            |100            |100            ns
tR      Inputs Rise Time(1)     |        1.0    |        0.3    |       0.3     ��s
tF      Inputs Fall Time(1)     |        300    |        300    |       100     ns
tSU.STO Stop Set-up Time        |4.7            |0.6            |0.25           ��s
tDH     Data Out Hold Time      |100            |50             |50             ns
tWR     Write Cycle Time        |        20     |         10    |10             ms
Endurance(1)                    |               |               |
        5.0V, 25��C, Page Mode   |100K           |100K           |100K           Write Cycles
��2.7V�������ʱ��
*/
/***********************************************
* ������ AT24Cxxʱ�䣨us��
*/
#define DLY_CLK_SDA             10               // >=1.0us
#define DLY_CLK_LOW             10               // >=1.3us
#define DLY_CLK_HIGH            10               // >=1.0us
#define DLY_SU_STA              10               // >=0.6us
#define DLY_HD_STA              10               // >=0.6us
#define DLY_SU_STO              10               // >=0.6us
#define DLY_SDA_BUF             10               // >=1.3us

/***********************************************
* ������ AT24Cxxʱ�䣨ms��
*/
#define DLY_WR_STO              5               // >=10ms

/***********************************************
* ������ AT24CXXӦ����
*/
#ifndef I2C_ACK
#define I2C_ACK                 0X01            // Ӧ��
#endif
#ifndef I2C_NACK
#define I2C_NACK                0X00            // ��Ӧ��
#endif
#ifndef HIGH
#define LOW                     0X00            // �͵�ƽ
#endif
#ifndef HIGH
#define HIGH                    0X01            // �ߵ�ƽ
#endif
/***********************************************
* ������ AT24CXX��дģʽ����
*/
#define MODE_PAGE_RW            0               // ҳ��д
#define MODE_SINGLE_RW          1               // ������д
#define BSP_EEP_READ_MODE       MODE_PAGE_RW    // 0��ҳ����1���ֽڶ�
#define BSP_EEP_WRITE_MODE      MODE_PAGE_RW    // 0��ҳд��1���ֽ�д

INT32U  AT24CXX_SCL_PIN         = GPIO_Pin_6;
INT32U  AT24CXX_SDA_PIN         = GPIO_Pin_7;

/***********************************************
* ������ AT24CXXʹ�õ�IO������Ϊ��©���
*/
#define I2C1_SCL_RCC            RCC_APB2Periph_GPIOE
#define I2C1_SCL_PORT           GPIOE
#define I2C1_SCL_PIN            GPIO_Pin_7

#define I2C1_SDA_RCC            RCC_APB2Periph_GPIOE
#define I2C1_SDA_PORT           GPIOE
#define I2C1_SDA_PIN            GPIO_Pin_8

#define I2C2_SCL_RCC            RCC_APB2Periph_GPIOB
#define I2C2_SCL_PORT           GPIOB
#define I2C2_SCL_PIN            GPIO_Pin_10

#define I2C2_SDA_RCC            RCC_APB2Periph_GPIOB
#define I2C2_SDA_PORT           GPIOB
#define I2C2_SDA_PIN            GPIO_Pin_11


#define AT24CXX_SCL_RCC         RCC_APB2Periph_GPIOB
#define AT24CXX_SCL_PORT        GPIOB
extern INT32U  AT24CXX_SCL_PIN;//         GPIO_Pin_6

#define AT24CXX_SDA_RCC         RCC_APB2Periph_GPIOB
#define AT24CXX_SDA_PORT        GPIOB
extern INT32U  AT24CXX_SDA_PIN;//         GPIO_Pin_7
/*******************************************************************************
* ������ AT24CXXʹ�õĶ˿�����(ֱ�Ӳ����Ĵ���)��ֱ���޸ĸô��˿�
*/
#define AT_SCL_1                GPIOB->BSRR = AT24CXX_SCL_PIN
#define AT_SCL_0                GPIOB->BRR  = AT24CXX_SCL_PIN

#define AT_SDA_1                GPIOB->BSRR = AT24CXX_SDA_PIN
#define AT_SDA_0                GPIOB->BRR  = AT24CXX_SDA_PIN

#define AT_SCL_S                ( GPIOB->IDR  & AT24CXX_SCL_PIN )
#define AT_SDA_S                ( GPIOB->IDR  & AT24CXX_SDA_PIN )


//============================================================================//

/***********************************************
* ������ AT24CXXʹ�õĶ˿����ã�ֱ���޸ĸô��˿�
*/
#define AT24CXX_SET_SCL(sta)    { if(sta) {AT_SCL_1; Delay_Nus(DLY_CLK_HIGH);} else { AT_SCL_0; Delay_Nus(DLY_CLK_SDA); } }
#define AT24CXX_SET_SDA(sta)    { if(sta) {AT_SDA_1; Delay_Nus(DLY_CLK_LOW); } else { AT_SDA_0; Delay_Nus(DLY_CLK_LOW); } }
#define AT24CXX_GET_SCL(sta)    { if( AT_SCL_S ) sta |= 1; else sta &= ~1; }
#define AT24CXX_GET_SDA(sta)    { if( AT_SDA_S ) sta |= 1; else sta &= ~1; }

/***********************************************
* ������ AT24CXX��������Ϊ��©������������IO�ķ���
*/
#define DIR_SEL         0

#if DIR_SEL == 0
#define SET_SDA_INPUT()
#define SET_SDA_OUTPUT()
#define SET_SCL_INPUT()
#define SET_SCL_OUTPUT()
#else
#define SET_SDA_INPUT()   { GPIO_InitTypeDef  GPIO_InitStructure;\
                            GPIO_InitStructure.GPIO_Pin =  AT24CXX_SDA_PIN;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;\
                            GPIO_Init(AT24CXX_SDA_PORT, &GPIO_InitStructure);  }

#define SET_SDA_OUTPUT()  { GPIO_InitTypeDef  GPIO_InitStructure;\
                            GPIO_InitStructure.GPIO_Pin =  AT24CXX_SDA_PIN;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                            GPIO_Init(AT24CXX_SDA_PORT, &GPIO_InitStructure);  }

#define SET_SCL_INPUT()   { GPIO_InitTypeDef  GPIO_InitStructure;\
                            GPIO_InitStructure.GPIO_Pin =  AT24CXX_SCL_PIN;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;\
                            GPIO_Init(AT24CXX_SCL_PORT, &GPIO_InitStructure);  }

#define SET_SCL_OUTPUT()  { GPIO_InitTypeDef  GPIO_InitStructure;\
                            GPIO_InitStructure.GPIO_Pin =  AT24CXX_SCL_PIN;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                            GPIO_Init(AT24CXX_SCL_PORT, &GPIO_InitStructure);  }
#endif

/***********************************************
* ������ OS�ӿ�
*/
#if UCOS_EN     == DEF_ENABLED
    #if OS_VERSION > 30000U
    static  OS_SEM                   Bsp_EepSem;    // �ź���
    #else
    static  OS_EVENT                *Bsp_EepSem;    // �ź���
    #endif
#endif

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
static void    I2C_Start      (void);
static void    I2C_Stop       (void);

static uint8_t I2C_SendByte   (uint8_t  byte);                //���ݴӸ�λ����λ
static uint8_t I2C_RecvByte   (uint8_t    ack, uint8_t *dat); //���ݴӸ�λ����λ

//============================================================================//

/*******************************************************************************
* ������ AT24Cxx�ؼ�ʱ��
*
*                ����ʼ/����ʱ��
*                : SDA ��������\_______/������������\_______/����������
*                : SCL ������������\______/��������\______/��������������
*                :       |   |                   |   |
*                :       START                   STOP
*                ������ʱ��
*                : SDA _____/������������������X����������������������\
*                :     ������������������������������������������������������������������
*                : SCL ______/������������\___/����������������\______
*                ��      |���ݱ���  |   |���ݱ���|
*                ��                ���ݸı�
*/


/*******************************************************************************
* ��    �ƣ� I2C_Start
* ��    �ܣ� ����I2C���ߣ�������I2C��ʼ����
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
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
    SET_SDA_OUTPUT();				/*SDA��Ϊ���                       */
    AT_SDA_1;						/*������ʼ�����������ź�            */
    Delay_Nus(DLY_CLK_SDA);
    AT_SCL_1;
    Delay_Nus(DLY_SU_STA);		    /*��ʼ��������ʱ�����4.7us,��ʱ    */
    AT_SDA_0;						/*������ʼ�ź�                      */
    Delay_Nus(DLY_HD_STA);			/*��ʼ��������ʱ�����4��s           */
    AT_SCL_0;						/*���������߸ı䣬׼�����ͻ��������*/
    Delay_Nus(DLY_CLK_LOW);         /*CLK��SDA֮����ʱ                  */
}

/*******************************************************************************
* ��    �ƣ� I2C_Stop
* ��    �ܣ� ����I2C���ߣ�������I2C��������
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
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
	SET_SDA_OUTPUT();				/*SDA��Ϊ���*/
    AT_SCL_0; 						/*ǯסI2C���ߣ�׼�����ͻ�������� */
	Delay_Nus(DLY_CLK_SDA);
    AT_SDA_0; 						/*���ͽ��������������ź�*/
    Delay_Nus(DLY_CLK_SDA);
    AT_SCL_1; 						/*���ͽ���������ʱ���ź�*/
	Delay_Nus(DLY_SU_STO);			/*������������ʱ�����4��s*/
    AT_SDA_1; 						/*����I2C���߽����ź�*/
    Delay_Nus(DLY_SDA_BUF); 		/*>4us*/
}

/*******************************************************************************
* ��    �ƣ� I2C_SendByte
* ��    �ܣ� �ֽ����ݴ��ͺ����������� c ���ͳ�ȥ�������ǵ�ַ��Ҳ���������ݣ�
             �����ȴ�Ӧ�𣬲��Դ�״̬λ���в���
* ��ڲ����� uint8_t ������
* ���ڲ����� ��
* ���� ���ߣ� ������
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
static uint8_t I2C_SendByte(uint8_t byte) 					    //���ݴӸ�λ����λ//
{
    INT08S  ctr     = 0;                            // ��ʼ������λ������
    uint8_t  ack     = 0;

    /***********************************************
    * ��������������
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ����Ƿ��Ѿ��������
        AT24CXX_SET_SCL(LOW);                       // SCL = 0 ���������߸ı䣬׼����������
        AT24CXX_SET_SDA((byte & 0x80) == 0x80);     // SDA = 0 �������ݣ�0;SDA = 1 �������ݣ�1
        AT24CXX_SET_SCL(HIGH);                      // SCL = 1 �������ݣ��ӻ����Զ�ȡ
        byte <<= 1;                                 // ��ǰֵ����һλ
    };
    /***********************************************
    * ���������Ӧ��
    */
    AT24CXX_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼������Ӧ��λ
    SET_SDA_INPUT();                                // SDA��Ϊ����
    AT24CXX_SET_SDA(HIGH);                          // SDA = 1 �����ͷ�����

    AT24CXX_SET_SCL(HIGH);                          // SCL = 1 ��������
    AT24CXX_GET_SDA(ack);                           // ��ȡ�ӻ�Ӧ��ACK
    SET_SDA_OUTPUT();                               // SDA��Ϊ���
    AT24CXX_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼������Ӧ��λ

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
* ���� ���ߣ� ������
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
static uint8_t I2C_RecvByte(uint8_t ack, uint8_t *dat)  	        //���ݴӸ�λ����λ
{
    uint8_t  ctr     = 0;
    INT08S  data    = 0;

    SET_SDA_INPUT();                                // SDA��Ϊ����
    /***********************************************
    * ��������������
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ��������һλ�����ڵ�һ��������0�������ƻ���0
        data <<= 1;                                 // ׼����һλ����
        AT24CXX_SET_SCL(HIGH);                      // SCL = 1 �������ݣ����Զ�ȡ�ӻ�����
        AT24CXX_GET_SDA(data);                      // ���Ϊ1����1�����򲻸ı䣨Ĭ��Ϊ0��
        AT24CXX_SET_SCL(LOW);                       // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    };

    SET_SDA_OUTPUT();                               // SDA��Ϊ���
    /***********************************************
    * ���������Ӧ��
    */
    AT24CXX_SET_SDA(!ack);                          // ACK,����0

    AT24CXX_SET_SCL(HIGH);
    AT24CXX_SET_SCL(LOW);
    AT24CXX_SET_SDA(HIGH);

    *dat    = data;

    return (TRUE);                            	    // Return 8-bit data byte
}

/*******************************************************************************
* ��    �ƣ� AT24CXX_Enabled
* ��    �ܣ� I2Cʹ��
* ��ڲ����� ��
* ���ڲ�����
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t AT24CXX_Enabled( void )
{
    INT32S timeout  = 100;
    do {
        I2C_Start();
        if ( I2C_SendByte( 0xA0 ) == TRUE )
            return TRUE;
    } while ( --timeout );
    
    return FALSE;
}

/*******************************************************************************
* ��    �ƣ� AT24CXX_Init
* ��    �ܣ� ��ʼ��IIC
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ֹͣʱ��
*            SDA _______/����������
*            SCL _____/��������������
*                      |   |
*                      STOP
*******************************************************************************/
void AT24CXX_Init( void )
{
    uint8_t size = 0;
    do {
        AT24CXX_SET_SCL(LOW);                       // SCL = 0 ���������߸ı䣬׼����������
        AT24CXX_SET_SDA(HIGH);                      // SDA = 1 �����ͷ�����
        AT24CXX_SET_SCL(HIGH);                      // SCL = 1 �������ݣ��ӻ����Զ�ȡ
        size        += 1;
    } while ( ( size < 10 ) && ( AT_SDA_S == 0 ) );

    I2C_Start();
    I2C_Stop();
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_SetAddr
* ��    �ܣ� ���õ�ǰ��ַ
* ��ڲ����� suba - ��ַ�� opt - 0:д�� 1:��
* ���ڲ�����
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_SetAddr( uint16_t suba, uint8_t opt)
{
    uint8_t      ack;
#if    AT_DEV_NO_SEL_PIN ==  DEF_ENABLED 
    //uint8_t    ctrl = BSP_EEP_I2C_ADDR | ((suba >> 7 ) & 0x0E);
    uint8_t    ctrl = BSP_EEP_I2C_ADDR | ((suba / 256) * 2 );
    ack = I2C_SendByte(ctrl);                   // �����豸��ַ + �洢��
#else
    ack = I2C_SendByte(BSP_EEP_I2C_ADDR);       // �����豸��ַ
#endif
    if(!ack)    goto exit;                      // ת������

#if    AT_DEV_NO_SEL_PIN ==  DEF_ENABLED 
    #if AT_MAX_BYTES > 32*1024u
    ack = I2C_SendByte(suba>>8);                // �����ڴ�ߵ�ַ
    if(!ack)    goto exit;                      // ת������
    #endif
#else
    #if AT_MAX_BYTES > 256u
    ack = I2C_SendByte(suba>>8);                // �����ڴ�ߵ�ַ
    if(!ack)    goto exit;                      // ת������
    #endif
#endif
    ack = I2C_SendByte(suba);                   // �����ڴ�͵�ַ
    if(!ack)    goto exit;                      // ת������
    /***********************************************
    * ������ ���Ϊ�������Ͷ���ַ
    */
    if ( opt ) {
      I2C_Start();                              // ���·�����ʼ����
      ack = I2C_SendByte(BSP_EEP_I2C_ADDR+1);
      if(!ack)  goto exit;                      // ת������
    }
exit:
    return ack;
}

#if (UCOS_EN     == DEF_ENABLED)
/*******************************************************************************
* ��    �ƣ� EEP_WaitEvent
* ��    �ܣ� �ȴ��ź���
* ��ڲ����� ��
* ���ڲ�����  0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
uint8_t EEP_WaitEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */

#if OS_VERSION > 30000U
    return BSP_OS_SemWait(&Bsp_EepSem,0);           // �ȴ��ź���
#else
    uint8_t       err;
    OSSemPend(Bsp_EepSem,0,&err);                   // �ȴ��ź���
    if ( err = OS_ERR_NONE )
      return TRUE;
    else
      return FALSE;
#endif
}

/*******************************************************************************
* ��    �ƣ� EEP_SendEvent
* ��    �ܣ� �ͷ��ź���
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ����ʹ��UCOS����ϵͳʱʹ��
*******************************************************************************/
void EEP_SendEvent(void)
{
    /***********************************************
    * ������ OS�ӿ�
    */
#if OS_VERSION > 30000U
    BSP_OS_SemPost(&Bsp_EepSem);                        // �����ź���
#else
    uint8_t       err;
    OSSemPost(Bsp_EepSem);                              // �����ź���
#endif
}
#endif /* end of (UCOS_EN     == DEF_ENABLED)*/


/*******************************************************************************
* ��    �ƣ� BSP_EEP_WriteByte
* ��    �ܣ� ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ���̣��������1����ʾ
             �����ɹ��������������
* ��ڲ����� suba�� EEPROM��ַ  byteд�����ֽ�
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_WriteByte(uint16_t suba,uint8_t byte )
{
    uint8_t   ack;

#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif

    I2C_Start();                                // ������ʼ����

    ack = BSP_EEP_SetAddr(suba,0);              // ���ö���ַ
    if(!ack)    goto exit;                      // ת������

    ack = I2C_SendByte(byte);                   // ����Ҫд�������
    if(!ack)    goto exit;                      // ת������

exit:
    I2C_Stop();                                 // ����ֹͣ����

#if (UCOS_EN     == DEF_ENABLED)
    EEP_SendEvent();                            // �ͷ��ź���
#endif

    return(ack);
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_ReadByte
* ��    �ܣ� ��1���ֽڡ�
* ��ڲ����� suba�� EEPROM��ַ  byte�������ֽ�
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_ReadByte( uint16_t suba, uint8_t *byte)
{
    uint8_t       ack;

#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif

    I2C_Start();                                // ������ʼ����

    ack = BSP_EEP_SetAddr(suba,1);              // ���ö���ַ
    if(!ack)    goto exit;                      // ת������

    ack = I2C_RecvByte(I2C_NACK,byte);          // �������ݲ����ط�Ӧ��
exit:
    I2C_Stop();                                 // ����ֹͣ����

#if (UCOS_EN     == DEF_ENABLED)
    EEP_SendEvent();                            // �ͷ��ź���
#endif

    return ack;
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_WriteByteChk
* ��    �ܣ� дһ���ֽڲ��ҽ�д����ֽ��ٶ�������
* ��ڲ����� suba�� EEPROM��ַ  byteд����ֽ�
* ���ڲ����� ����д����ֽ�byte
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_WriteByteChk(uint16_t suba,uint8_t byte )
{
    uint8_t   chk;
    uint8_t   retrys  = 10;

    do {
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
        BSP_OS_TimeDly(DLY_WR_STO);
#else
        OSTimeDly(DLY_WR_STO);
#endif
#else
        Delay_Nms(DLY_WR_STO);
#endif
        if( BSP_EEP_WriteByte( suba, byte ) ) {
            chk     = 0;
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
            BSP_OS_TimeDly(DLY_WR_STO);
#else
            OSTimeDly(DLY_WR_STO);
#endif
#else
            Delay_Nms(DLY_WR_STO);
#endif
            BSP_EEP_ReadByte( suba, &chk );
            if ( chk == byte )
                return TRUE;
        }
    } while(--retrys);

    return(FALSE);
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_WritePage
* ��    �ܣ� дһҳ����
* ��ڲ����� suba�� EEPROM��ַ  pbuf �� д���ֽڵ����� len : ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_WritePage( uint16_t suba , uint8_t * pbuf , uint8_t len )
{
    uint8_t    size   = 0;
    uint8_t    ack;

    if ( len > AT_PAGE_SIZE )
      return 0;

    if ( AT24CXX_Enabled() == FALSE )
        return 0;
    
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_TimeDly(DLY_WR_STO);
#else
    OSTimeDly(DLY_WR_STO);
#endif
#else
    Delay_Nms(DLY_WR_STO);                      // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms
#endif

    I2C_Start();

    ack = BSP_EEP_SetAddr(suba,0);              // ���ö���ַ
    if(!ack)    goto exit;                      // ת������

    for ( ; size < len ; size ++ ) {
        ack = I2C_SendByte( pbuf[ size ] );
        if(!ack)    goto exit;                      // ת������
    }
exit:
    I2C_Stop();
    return size;
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_ReadPage
* ��    �ܣ� ��һҳ����
* ��ڲ����� suba�� EEPROM��ַ  pbuf �� д���ֽڵ����� len : ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint16_t BSP_EEP_ReadPage(uint16_t suba,uint8_t *pbuf, uint16_t len)
{
    uint8_t    size   = 0;
    uint8_t    ack;
    /***********************************************
    * ������ ����Ƿ񳬳��洢����
    */
    if ( (suba + len) > (AT_MAX_BYTES) )
        return 0;

    /***********************************************
    * ������ ����Ƿ�ɲ���
    */
    if ( AT24CXX_Enabled() == FALSE )
        return 0;
    
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_TimeDly(DLY_WR_STO);
#else
    OSTimeDly(DLY_WR_STO);
#endif
#else
    Delay_Nms(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms
#endif
    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    I2C_Start();                                    // ������ʼ����
    ack = BSP_EEP_SetAddr(suba,1);                  // ���ö���ַ

    if(!ack) {
        I2C_Stop();                               // ����ֹͣ����
        return 0;
    }

    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    for( size = 0; size < len; size++ ) {
        /***********************************************
        * ������ ����Ƿ����һ�ֽ�
        */
        if ( size == len - 1 ) {
            ack = I2C_RecvByte(I2C_NACK, pbuf++);
        } else {
            ack = I2C_RecvByte(I2C_ACK, pbuf++);
        }
        /***********************************************
        * ������ ���Ӧ���ź��Ƿ���ȷ
        */
        if(!ack) {
            //I2C_Stop();                   // ����ֹͣ����
            break;
        }
    }
 
    I2C_Stop();                   // ����ֹͣ����

    return size;
}

static uint8_t  buf[AT_PAGE_SIZE];
/*******************************************************************************
* ��    �ƣ� BSP_EEP_WriteBytes
* ��    �ܣ� д����ֽ�
* ��ڲ����� suba�� EEPROM��ַ  pbuf ��д���ֽڵ����� len : ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint16_t BSP_EEP_WriteBytes( uint16_t suba, uint8_t *pbuf, uint16_t len )
{
    uint16_t      bytes   = 0;
    /***********************************************
    * ������ ����ַ�Ƿ���������������ֱ�ӷ���
    */
    if ( (suba + len) > (AT_MAX_BYTES) )
        return 0;

#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif

    uint8_t    size    = 0;

    /***********************************************
    * ������ ��ҳ�׿�ʼ����ȡ֮ǰ���ݽ������
    */
    if ( ( suba % AT_PAGE_SIZE ) != 0 ) {
        uint8_t  readLen;
        uint16_t  pageAddr;

        size        = AT_PAGE_SIZE - ( suba % AT_PAGE_SIZE );
        readLen     = AT_PAGE_SIZE - size;
        pageAddr    = suba - readLen;
        BSP_EEP_ReadPage(pageAddr, buf, AT_PAGE_SIZE);

        if ( size > len )
            size = len;
        memcpy(&buf[readLen],pbuf,size);

        BSP_EEP_WritePage( pageAddr , buf , AT_PAGE_SIZE );

        suba    += size;
        pbuf    += size;
        len     -= size;
        bytes   += size;
    }

    /***********************************************
    * ������ ҳ�׿�ʼ����ҳ
    */
    size = len / AT_PAGE_SIZE;
    while ( size > 0 ) {
        BSP_EEP_WritePage( suba , pbuf, AT_PAGE_SIZE );
        suba    += AT_PAGE_SIZE;
        pbuf    += AT_PAGE_SIZE;
        len     -= AT_PAGE_SIZE;
        bytes   += AT_PAGE_SIZE;
        size    -= 1;
    }

    /***********************************************
    * ������ ҳ�׿�ʼ������һҳ
    */
    if ( len > 0 ) {
        BSP_EEP_WritePage( suba , pbuf , len );
        bytes   += len;
    }

#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_SendEvent();                            // �ͷ��ź���
#endif

    return(bytes);
}


/*******************************************************************************
* ��    �ƣ� BSP_EEP_ReadBytes
* ��    �ܣ� ������ֽ�
* ��ڲ����� suba�� EEPROM��ַ  pbuf �������ֽڵ����� len : ����Ĵ�С
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint16_t BSP_EEP_ReadBytes(uint16_t suba,uint8_t *buf, uint16_t len)
{
    uint16_t      bytes   = 0;
    uint8_t      ack;
    /***********************************************
    * ������ ����Ƿ񳬳��洢����
    */
    if ( (suba + len) > (AT_MAX_BYTES) )
        return 0;
#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_WaitEvent();                            // ���ź���
#endif

    /***********************************************
    * ������ ����Ƿ�ɲ���
    */
    if ( AT24CXX_Enabled() == FALSE )
        goto exit;
    
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_TimeDly(DLY_WR_STO);
#else
    OSTimeDly(DLY_WR_STO);
#endif
#else
    Delay_Nms(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms
#endif
    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    I2C_Start();                                    // ������ʼ����
    ack = BSP_EEP_SetAddr(suba,1);                  // ���ö���ַ

    if(!ack) {
        //I2C_Stop();                                 // ����ֹͣ����
        bytes   = 0;
        goto exit;
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
            bytes++;
            break;
        } else {
            ack = I2C_RecvByte(I2C_ACK, buf++);
        }
        /***********************************************
        * ������ ���Ӧ���ź��Ƿ���ȷ
        */
        if(!ack) {
            break;
        }
    }
exit:
    I2C_Stop();                   // ����ֹͣ����
#if (UCOS_EN     == DEF_ENABLED)
    /***********************************************
    * ������ OS�ӿ�
    */
    EEP_SendEvent();                            // �ͷ��ź���
#endif

    return bytes;
}


/*******************************************************************************
* ��    �ƣ� BSP_EEP_Writeuint16_t
* ��    �ܣ� д��һ��16λ������ռ�����ֽڣ�
* ��ڲ����� suba�� EEPROM��ַ   dat��16����
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_WriteINT16U( uint16_t suba, uint16_t dat )
{
    uint8_t       bytes   = 0;
    uint8_t      *pbuf   = (uint8_t *)&dat;

    if ( suba > (AT_MAX_BYTES-1u-2u) )
        return 0;


    /***********************************************
    * ���������ֽ�д�룬��д���ֽ�
    */
    for( bytes = 0; bytes < 2; bytes++ ) {
        if( !BSP_EEP_WriteByteChk( suba++, *pbuf++ ) ) {
            break;
        }
    }

    return(bytes);
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_Readuint16_t
* ��    �ܣ� ��һ��16λ������ռ�����ֽڣ�
* ��ڲ����� suba�� EEPROM��ַ   dat��16����
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_ReadINT16U( uint16_t suba,uint16_t *dat)
{
    uint8_t       bytes   = 0;
    uint8_t      *pbuf   = (uint8_t *)dat;

    if ( suba > (AT_MAX_BYTES-1u-2u) )
        return 0;

    memset(buf,0x00,2);

    /***********************************************
    * ���������ֽڶ�ȡ���ȶ����ֽ�
    */
    for( bytes = 0; bytes < 2; bytes++ ) {
        if( !BSP_EEP_ReadByte( suba++, pbuf++ ) ) {
            break;
        }
    }

    return bytes;
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_Writeuint32_t
* ��    �ܣ� д��һ��32λ������ռ�ĸ��ֽڣ�
* ��ڲ����� suba�� EEPROM��ַ   dat��32λ����
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_WriteINT32U( uint16_t suba, uint32_t dat )
{
    uint8_t       bytes;
    uint8_t      *pbuf   = (uint8_t *)&dat;

    if ( suba > (AT_MAX_BYTES-1u-4u) )
        return 0;

    /***********************************************
    * ���������ֽ�д�룬��д���ֽ�
    */
    for( bytes = 0; bytes < 4; bytes++ ) {
        if( !BSP_EEP_WriteByteChk( suba++, *pbuf++ ) ) {
            break;
        }
    }

    return(bytes);
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_ReadINT32U
* ��    �ܣ� ��һ��32λ������ռ�ĸ��ֽڣ�
* ��ڲ����� suba�� EEPROM��ַ   dat��32λ����
* ���ڲ����� 0���������󣩣�1�������ɹ���
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_EEP_ReadINT32U( uint16_t suba,uint32_t *dat)
{
    uint8_t       bytes   = 0;
    uint8_t      *pbuf   = (uint8_t *)dat;

    if ( suba > (AT_MAX_BYTES-1u-4u) )
        return 0;

    memset(buf,0x00,4);

    /***********************************************
    * ���������ֽ�д�룬��д���ֽ�
    */
    for( bytes = 0; bytes < 4; bytes++ ) {
        if( !BSP_EEP_ReadByte( suba++, pbuf++ ) ) {
            break;
        }
    }

    return bytes;
}
/*******************************************************************************
* ��    �ƣ� BSP_I2CSetPort
* ��    �ܣ� �˿�ѡ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� �ⲿ���ô˺�����ʼ��EEPROM
*******************************************************************************/
void BSP_I2CSetPort(INT08U port)
{
    if ( port == 2 ) {        
        AT24CXX_SCL_PIN         = GPIO_Pin_10;
        AT24CXX_SDA_PIN         = GPIO_Pin_11;
    }else {        
        AT24CXX_SCL_PIN         = GPIO_Pin_6;
        AT24CXX_SDA_PIN         = GPIO_Pin_7;
    }  
}

/*******************************************************************************
* ��    �ƣ� BSP_EEP_Init
* ��    �ܣ� EEPROM��ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2015-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� �ⲿ���ô˺�����ʼ��EEPROM
*******************************************************************************/
BOOL BSP_EEP_Init(void)
{
    GPIO_InitTypeDef    gpio_init;
    uint8_t              dat;

    /***********************************************
    * ������ I2C1
    */
#if GPIO_I2C1_PORT_EN == DEF_ENABLED
    RCC_APB2PeriphClockCmd(I2C1_SCL_RCC , ENABLE);
    gpio_init.GPIO_Pin   = I2C1_SCL_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
    
    GPIO_Init(I2C1_SCL_PORT, &gpio_init);
    RCC_APB2PeriphClockCmd(I2C1_SDA_RCC , ENABLE);
    gpio_init.GPIO_Pin   = I2C1_SDA_PIN;
    GPIO_Init(I2C1_SDA_PORT, &gpio_init);
#endif
    /***********************************************
    * ������ I2C2
    */
#if GPIO_I2C2_PORT_EN == DEF_ENABLED
    RCC_APB2PeriphClockCmd(I2C2_SCL_RCC , ENABLE);
    gpio_init.GPIO_Pin   = I2C2_SCL_PIN;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
    
    GPIO_Init(I2C2_SCL_PORT, &gpio_init);
    RCC_APB2PeriphClockCmd(I2C2_SDA_RCC , ENABLE);
    gpio_init.GPIO_Pin   = I2C2_SDA_PIN;
    GPIO_Init(I2C2_SDA_PORT, &gpio_init);
#endif
    
#if GPIO_I2C1_PORT_EN == DEF_ENABLED
    BSP_I2CSetPort(1);                              // �л���I2C1
    AT24CXX_Init();
#endif
#if GPIO_I2C2_PORT_EN == DEF_ENABLED
    BSP_I2CSetPort(2);                              // �л���I2C1
    AT24CXX_Init();
#endif
    BOOL ack    = BSP_EEP_ReadByte( 0, &dat );
    /***********************************************
    * ������ OS�ӿ�
    */
#if (UCOS_EN     == DEF_ENABLED)
#if OS_VERSION > 30000U
    BSP_OS_SemCreate(&Bsp_EepSem,1, "Bsp EepSem");      // �����ź���
#else
    Bsp_EepSem     = OSSemCreate(1);                    // �����ź���
#endif
#endif
    return ack;
}

/*******************************************************************************
* Function Name  :
* Description    :
* Input          :
* Output         :
* Return         : None
********************************************************************************/
#define     TEST_BUF_LEN    1024
INT32U      idx;
CHAR        atBuf[TEST_BUF_LEN];
CHAR        chkBuf[TEST_BUF_LEN];
void BSP_EEP_Main(void)
{

    uint16_t      baseAddr        = 100;
    idx         = 0;

    BSP_EEP_Init();                                 // ��ʼ���洢��

    memset(atBuf,0x00,TEST_BUF_LEN);
    memset(chkBuf,0x00,TEST_BUF_LEN);

    for(idx = 0; idx < TEST_BUF_LEN; idx++ ) {
      if ( idx % 2 )
        atBuf[idx]  = 0xAA;
      else
        atBuf[idx]  = idx / 2;
    }

    while (1) {
        memset(chkBuf,0x00,TEST_BUF_LEN);
        BSP_EEP_ReadBytes(baseAddr,(uint8_t*)&chkBuf[0],TEST_BUF_LEN);

        for (  idx = 0; idx < TEST_BUF_LEN; idx++ ){
          if ( atBuf[idx] != chkBuf[idx] ) {
            BSP_EEP_WriteBytes  (baseAddr,(uint8_t*)&atBuf[0],TEST_BUF_LEN);
            break;
          }
        }
        if ( idx == TEST_BUF_LEN )
          break;
        Delay_Nms(1000);
        BSP_LED_Toggle(2);
    }
}
/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif
