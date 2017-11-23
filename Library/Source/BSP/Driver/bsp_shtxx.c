/*******************************************************************************
 *   Filename:      bsp_shtxx.c
 *   Revised:       $Date: 2015-01-26
 *   Revision:      $
 *   Writer:        Wuming Shen.
 *
 *   Description:   SHTxx��д���¶����ʪ�ȶ�ȡ
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
#include "bsp_shtxx.h"
#include "math.h"
#include "global.h"

/*******************************************************************************
 * CONSTANTS
 */
/***********************************************
* ������ SHTXX�Ĵ���
*/
#define SHTCMD_WRITE_STATUS     0x06            // "д״̬�Ĵ���"����
#define SHTCMD_READ_STATUES     0x07            // "��״̬�Ĵ���"����
#define SHTCMD_MEASURE_TEMP     0x03            // "�����¶�"����
#define SHTCMD_MEASURE_HUMI     0x05            // "�������ʪ��"����
#define SHTCMD_RESET            0x1e            // "��λ"����
/***********************************************
* ������ ʱ�䣨us��
*/
#define DLY_CLK_SDA             1               // >=1.0us
#define DLY_CLK_LOW             1               // >=1.3us
#define DLY_CLK_HIGH            1               // >=0.6us
#define DLY_SU_STA              1               // >=0.6us
#define DLY_HD_STA              1               // >=0.6us
#define DLY_SU_STO              1               // >=0.6us
#define DLY_SDA_BUF             1               // >=1.3us
/***********************************************
* ������ ʱ�䣨us��
*/
#define DLY_WR_STO              60              // >61us
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

#define I2C_MAX_BYTES            16
/*******************************************************************************
 * MACROS
 */
/***********************************************
* ������ AT24CXXʹ�õ�IO������Ϊ��©���
*/
#define I2C_SCL_RCC             RCC_APB2Periph_GPIOB
#define I2C_SCL_GPIO            GPIOB
#define I2C_SCL_PIN             GPIO_Pin_8

#define I2C_SDA_RCC             RCC_APB2Periph_GPIOB
#define I2C_SDA_GPIO            GPIOB
#define I2C_SDA_PIN             GPIO_Pin_9

#define SET_SCL_H()             (I2C_SCL_GPIO->BSRR = I2C_SCL_PIN)
#define SET_SCL_L()             (I2C_SCL_GPIO->BRR  = I2C_SCL_PIN)

#define SET_SDA_H()             (I2C_SDA_GPIO->BSRR = I2C_SDA_PIN)
#define SET_SDA_L()             (I2C_SDA_GPIO->BRR  = I2C_SDA_PIN)

#define GET_SCL()               (I2C_SCL_GPIO->IDR  & I2C_SCL_PIN)
#define GET_SDA()               (I2C_SDA_GPIO->IDR  & I2C_SDA_PIN)

#define I2C_SET_SCL(sta)        { if(sta) {SET_SCL_H(); SHT_Delay_Nus(DLY_CLK_HIGH);} else { SET_SCL_L(); SHT_Delay_Nus(DLY_CLK_SDA); } }
#define I2C_SET_SDA(sta)        { if(sta) {SET_SDA_H(); SHT_Delay_Nus(DLY_CLK_LOW); } else { SET_SDA_L(); SHT_Delay_Nus(DLY_CLK_LOW); } }
#define I2C_GET_SCL(sta)        { if( GET_SCL() ) sta |= 1; else sta &= ~1; }
#define I2C_GET_SDA(sta)        { if( GET_SDA() ) sta |= 1; else sta &= ~1; }

//#define SET_SDA_INPUT()
//#define SET_SDA_OUTPUT()
//#define SET_SCL_INPUT()
//#define SET_SCL_OUTPUT()

void SET_SDA_INPUT(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//    GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
//    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
}

void SET_SDA_OUTPUT(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//    GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);
}

void SET_SCL_OUTPUT(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);
}

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */
/***********************************************
* ������ �豸���� (SHT1x/SHT2x)
*/
#define DEV_SHT1X           0
#define DEV_SHT2X           1

static  uint8            DevType    = DEV_SHT1X;
/*******************************************************************************
 * GLOBAL VARIABLES
 */
SHT_DATA_TYPE               SHT_Data;           //ת�����ݱ����ڸñ�����

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
void            SHTBoot                 (void);
void            SHTXX_Init              (void);
void            SHTXX_PIN_Init          (void);
void            SHT_Delay_Nus           (uint32  dly);
int8_t          sht_write_byte          (uint8_t value);
int8_t          sht_read_byte           (uint8_t ack);
void            sht_transstart          (void);
void            sht_connectionreset     (void);
int8_t          sht_softreset           (void);
int8_t          sht_read_statusreg      (uint8_t *p_value, uint8_t *p_checksum);
int8_t          sht_write_statusreg     (uint8_t *p_value);
int8_t          sht_measure             (u16 *p_value, int8_t *p_checksum, uint8_t mode, uint8_t step);
void            calc_shtxx              (float *p_humidity ,float *p_temperature);
float           calc_dewpoint           (float h,float t);
uint16          SHT_MeasureOnce         (uint8_t mode , uint8_t step);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
static void     I2C_Start               (void);
static void     I2C_Stop                (void);

static uint8    I2C_SendByte            (uint8  byte);                           //���ݴӸ�λ����λ
static uint8    I2C_RecvByte            (uint8  ack, uint8 *dat);              //���ݴӸ�λ����λ

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/*******************************************************************************
* ��    �ƣ� Delay_Nus()
* ��    �ܣ� ��ʱ���1us
* ��ڲ����� dly		��ʱ������ֵԽ����ʱԽ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2009-01-03
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
OPTIMIZE_NONE void  SHT_Delay_Nus( uint32  dly )
{
    INT08U  i;

    while(dly--) {
        for(i=0; i<2; i++);             // clk = 72000000 1027.7778611111111111111111111111
    }
}

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
* Function Name  : I2C_Start
* Description    : ����I2C���ߣ�������I2C��ʼ����
*                ����ʼʱ��
*                : SCL ������������\____
*                : SDA ��������\______
*                :       |   |
*                :       START
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
static void I2C_Start(void)
{
    SET_SDA_OUTPUT();				            /*SDA��Ϊ���                       */
    SET_SDA_H();				                /*������ʼ�����������ź�            */
    SHT_Delay_Nus(DLY_CLK_SDA);
    SET_SCL_H();
    SHT_Delay_Nus(DLY_SU_STA);		            /*��ʼ��������ʱ�����4.7us,��ʱ    */
    SET_SDA_L();					            /*������ʼ�ź�                      */
    SHT_Delay_Nus(DLY_HD_STA);			        /*��ʼ��������ʱ�����4��s           */
    SET_SCL_L();					            /*���������߸ı䣬׼�����ͻ��������*/
    SHT_Delay_Nus(DLY_CLK_LOW);                 /*CLK��SDA֮����ʱ                  */
}

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : ����I2C���ߣ�������I2C��������
*                ��ֹͣʱ��
*                : SCL _____/��������������
*                : SDA _________/����������
*                :            |   |
*                :            STOP
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
static void I2C_Stop(void)
{
	SET_SDA_OUTPUT();				            /*SDA��Ϊ���*/
    SET_SCL_L(); 					            /*ǯסI2C���ߣ�׼�����ͻ�������� */
	SHT_Delay_Nus(DLY_CLK_SDA);
    SET_SDA_L(); 					            /*���ͽ��������������ź�*/
    SHT_Delay_Nus(DLY_CLK_SDA);
    SET_SCL_H(); 					            /*���ͽ���������ʱ���ź�*/
	SHT_Delay_Nus(DLY_SU_STO);			            /*������������ʱ�����4��s*/
    SET_SDA_H(); 					            /*����I2C���߽����ź�*/
    SHT_Delay_Nus(DLY_SDA_BUF); 		            /*>4us*/
}

/*******************************************************************************
* Function Name  : I2C_SendByte(uint8 byte)
* Description    : �ֽ����ݴ��ͺ����������� c ���ͳ�ȥ�������ǵ�ַ��Ҳ���������ݣ�
                   �����ȴ�Ӧ�𣬲��Դ�״̬λ���в���
*                ������ʱ��
*                : SCL ______/������������\___/����������������\______
*                : SDA _____/������������������X����������������������\
*                :     ������������������������������������������������������������������
*                ��      |���ݱ���  |   |���ݱ���|
*                ��                ���ݸı�
* Input          : uint8 ������
* Output         : None
* Return         : None
********************************************************************************/
static uint8 I2C_SendByte(uint8 byte)
{
    int16  ctr     = 0;                            // ��ʼ������λ������
    uint8  ack     = 0;

    /***********************************************
    * ���������ݴӸ�λ����λ
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ����Ƿ��Ѿ��������
        I2C_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼����������
        I2C_SET_SDA((byte & 0x80) == 0x80);         // SDA = 0 �������ݣ�0;SDA = 1 �������ݣ�1
        I2C_SET_SCL(HIGH);                          // SCL = 1 �������ݣ��ӻ����Զ�ȡ
        byte <<= 1;                                 // ��ǰֵ����һλ
    };
    /***********************************************
    * ���������Ӧ��
    */
    I2C_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼������Ӧ��λ
    SET_SDA_INPUT();                                // SDA��Ϊ����
    I2C_SET_SDA(HIGH);                              // SDA = 1 �����ͷ�����

    I2C_SET_SCL(HIGH);                              // SCL = 1 ��������
    I2C_GET_SDA(ack);                               // ��ȡ�ӻ�Ӧ��ACK
    SET_SDA_OUTPUT();                               // SDA��Ϊ���
    I2C_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼������Ӧ��λ

    if (ack)                                    	// ���Ӧ���ź�
        return (FALSE);                             // NACK����
    else
        return (TRUE);                              // ACK����
}

/*******************************************************************************
* Function Name  : I2C_RecvByte(void)
* Description    : ���մ��������������ݣ����ж����ߴ��󣨲���Ӧ���źţ��������
                   ��Ҫ����Ӧ������
*                ������ʱ��
*                : SDA _____/������������������X����������������������\
*                :     ������������������������������������������������������������������
*                : SCL ______/������������\___/����������������\______
*                ��      |���ݱ���  |   |���ݱ���|
*                ��                ���ݸı�
* Input          : None
* Output         : uint8 ������
* Return         : None
********************************************************************************/
static uint8 I2C_RecvByte(uint8 ack, uint8 *dat)  	//���ݴӸ�λ����λ
{
    uint8  ctr     = 0;
    int16  data    = 0;

    SET_SDA_INPUT();                                // SDA��Ϊ����
    I2C_SET_SCL(LOW);                               // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    /***********************************************
    * ��������������
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ��������һλ�����ڵ�һ��������0�������ƻ���0
        data <<= 1;                                 // ׼����һλ����
        I2C_SET_SCL(HIGH);                          // SCL = 1 �������ݣ����Զ�ȡ�ӻ�����
        I2C_GET_SDA(data);                          // ���Ϊ1����1�����򲻸ı䣨Ĭ��Ϊ0��
        I2C_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    };

    SET_SDA_OUTPUT();                               // SDA��Ϊ���
    /***********************************************
    * ���������Ӧ��
    */
    I2C_SET_SDA(!ack);                              // ACK,����0

    I2C_SET_SCL(LOW);
    I2C_SET_SCL(HIGH);
    I2C_SET_SDA(LOW);
    I2C_SET_SDA(HIGH);

    *dat    = data;

    return (TRUE);                            	    // Return 8-bit data byte
}

/*******************************************************************************
* Function Name  : BSP_I2C_SetCmd
* Description    : ���õ�ǰ��ַ
* Input          : suba - ��ַ�� opt - 0:д�� 1:��
* Output         :
* Return         : None
********************************************************************************/
static uint8 BSP_I2C_SetCmd(uint8 cmd, uint8 opt)
{
    uint8      ack;
    /***********************************************
    * ������ ���Ϊ�������Ͷ���ַ
    */
    if ( opt ) {
        ack = I2C_SendByte(0x80+1);
    } else {
        ack = I2C_SendByte(0x80);                           // �����豸��ַ
    }

    if(!ack)    goto exit;                                  // ת������

    ack = I2C_SendByte(cmd);                                // �����ڴ�͵�ַ
exit:
    return ack;
}

/*******************************************************************************
* Function Name  : uint8 BSP_I2C_WriteByte(uint16 suba,uint8 c)
* Description    : ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ���̣��������1����ʾ
                   �����ɹ��������������
* Input          : ��������ַ sla���ӵ�ַ suba, �����ֽ� c
* Output         : 0���������󣩣�1�������ɹ���
* Return         : None
********************************************************************************/
uint8 BSP_I2C_WriteByte(uint8 suba,uint8 byte )
{
    uint8   ack;

    /***********************************************
    * ������ OS�ӿ�
    */
    SHT_Delay_Nus(DLY_WR_STO);                              // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();                                            // ������ʼ����

    ack = BSP_I2C_SetCmd(byte,0);                           // ���ö���ַ
    if(!ack)    goto exit;                                  // ת������

    //ack = I2C_SendByte(byte);                             // ����Ҫд�������
    //if(!ack)    goto exit;                                // ת������

exit:
    I2C_Stop();                                             // ����ֹͣ����

    return(ack);
}
/*******************************************************************************
* Function Name  : uint8 BSP_EEP_WriteBytes
* Description    : ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ���̣��������1����ʾ
                   �����ɹ��������������
* Input          : ��������ַ sla���ӵ�ַ suba, �����ֽ� c
* Output         : 0���������󣩣�1�������ɹ���
* Return         : None
********************************************************************************/
uint16 BSP_I2C_WriteBytes( uint8 suba, uint8 *pbuf, uint16 len )
{
    uint8      bytes   = 0;
    uint8      ack;
    /***********************************************
    * ������ ����ַ�Ƿ���������������ֱ�ӷ���
    */
    if ( (suba + len) > (256) )
        return 0;

    /***********************************************
    * ������ OS�ӿ�
    */
    //if ( BSP_I2C_GetReady() == FALSE )
    //  return 0;

    SHT_Delay_Nus(DLY_WR_STO);                              // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();

    ack = BSP_I2C_SetCmd(suba,0);                           // ���ö���ַ
    if(!ack)    goto exit;                                  // ת������

    for ( ; bytes < len ; bytes++ ) {
        ack = I2C_SendByte( pbuf[ bytes ] );
        if(!ack)    goto exit;                              // ת������
    }
exit:
    I2C_Stop();
    if ( len == 0 )
        return ack;
    /***********************************************
    * ������ OS�ӿ�
    */

    return bytes;
}

/*******************************************************************************
* Function Name  : BSP_EEP_ReadByte
* Description    : ��N�ֽ�
* Input          :
* Output         :
* Return         : None
********************************************************************************/
uint8 BSP_I2C_ReadByte( uint8 suba, uint8 *byte)
{
    uint8       ack;
    /***********************************************
    * ������ OS�ӿ�
    */
    SHT_Delay_Nus(DLY_WR_STO);                              // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms

    I2C_Start();                                            // ������ʼ����

    ack = BSP_I2C_SetCmd(suba,1);                           // ���ö���ַ
    if(!ack)    goto exit;                                  // ת������

    ack = I2C_RecvByte(I2C_NACK,byte);                      // �������ݲ����ط�Ӧ��
exit:
    I2C_Stop();                                             // ����ֹͣ����

    return ack;
}

/*******************************************************************************
* Function Name  :BSP_EEP_ReadBytes
* Description    :��N�ֽ�
* Input          :
* Output         :
* Return         : None
********************************************************************************/
uint16 BSP_I2C_ReadBytes(uint8 suba,uint8 *buf, uint16 len)
{
    uint16      bytes   = 0;
    uint8      ack;
    /***********************************************
    * ������ ����Ƿ񳬳��洢����
    */
    if ( (suba + len) > (I2C_MAX_BYTES) )
        return 0;
    /***********************************************
    * ������ OS�ӿ�
    */

    /***********************************************
    * ������ ����Ƿ�ɲ���
    */
    //if ( BSP_I2C_GetReady() == FALSE )
    //    return 0;

    SHT_Delay_Nus(DLY_WR_STO);                          // ��һ��ֹͣ����һ����ʼ֮��Ҫ�����ʱ10ms
    /***********************************************
    * ������ ����Ƿ����һ�ֽ�
    */
    I2C_Start();                                        // ������ʼ����
    ack = BSP_I2C_SetCmd(suba,1);                       // ���ö���ַ

    if(!ack) {
        I2C_Stop();                                     // ����ֹͣ����
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
            //I2C_Stop();                               // ����ֹͣ����
            break;
        }
    }
exit:
    I2C_Stop();                                         // ����ֹͣ����
    /***********************************************
    * ������ OS�ӿ�
    */

    return bytes;
}

/*******************************************************************************
 * �������ƣ�sht_write_byte
 *
 * ������������SHT11д1���ֽڲ����SHT11�Ƿ�Ӧ��
 *
 * ��    ����value  Ҫд���1�ֽ�����
 *
 * �� �� ֵ��error=1����SHT11δӦ��
 ******************************************************************************/
int8_t sht_write_byte(uint8_t byte)
{
    int16  ctr     = 0;                             // ��ʼ������λ������
    uint8  ack     = 0;

    SET_SDA_OUTPUT();                               // DATA����Ϊ���

    /***********************************************
    * �������Ӹߵ�����λ����
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ����Ƿ��Ѿ��������
        I2C_SET_SDA((byte & 0x80) == 0x80);         // SDA = 0 �������ݣ�0;SDA = 1 �������ݣ�1
        I2C_SET_SCL(HIGH);                          // SCL = 0 ���������߸ı䣬׼����������
        //SHT_Delay_Nus(5);
        I2C_SET_SCL(LOW);                           // SCL = 1 �������ݣ��ӻ����Զ�ȡ
        //SHT_Delay_Nus(5);
        byte <<= 1;                                 // ��ǰֵ����һλ
    };

    I2C_SET_SDA(HIGH);                              // �ͷ�DATA��
    I2C_SET_SCL(HIGH);                              // ��9��SCK
    //SHT_Delay_Nus(5);
    SET_SDA_INPUT();
    I2C_GET_SDA(ack);
    //SHT_Delay_Nus(5);
    I2C_SET_SCL(LOW);

    return ack;                                     // error=1����SHT11δӦ��
}

/*******************************************************************************
 * �������ƣ�sht_read_byte
 *
 * ������������SHT11��1���ֽڲ����������ack=1ʱ����Ӧ��
 *
 * ��    ����ack  Ӧ���־
 *
 * �� �� ֵ��error=1����SHT11δӦ��
 ******************************************************************************/
int8_t sht_read_byte(uint8_t ack)
{
    uint8  ctr     = 0;
    int16  data    = 0;

    SET_SDA_OUTPUT();                               // DATA����Ϊ���
    I2C_SET_SDA(HIGH);                              // �ͷ�DATA��
    SET_SDA_INPUT();

    /***********************************************
    * �������Ӹߵ�����λ��ȡ
    */
    for ( ctr = 0 ; ctr < 8 ; ctr ++ ) {            // ��������һλ�����ڵ�һ��������0�������ƻ���0
        data <<= 1;                                 // ׼����һλ����
        I2C_SET_SCL(HIGH);                          // SCL = 1 �������ݣ����Զ�ȡ�ӻ�����
        I2C_GET_SDA(data);                          // ���Ϊ1����1�����򲻸ı䣨Ĭ��Ϊ0��
        I2C_SET_SCL(LOW);                           // SCL = 0 ���������߸ı䣬׼����ȡ�ӻ�����
    };

    SET_SDA_OUTPUT();                               // DATA����Ϊ���

    I2C_SET_SDA(!ack);                              // ACK,����0
    I2C_SET_SCL(HIGH);                              // ��9��SCK
    //SHT_Delay_Nus(5);
    I2C_SET_SCL(LOW);
    I2C_SET_SDA(HIGH);                              // �ͷ�DATA��

    return data;
}

/*******************************************************************************
 * �������ƣ�sht_transstart
 *
 * ��������������һ��"��������"����
 *                 ____         ________
 *           SDA:      |_______|
 *                    ___     ___
 *           SCK : __|   |___|   |______
 *
 * ��    ������
 *
 * �� �� ֵ����
 ******************************************************************************/
void sht_transstart(void)
{
   SET_SDA_OUTPUT();                                        // DATA����Ϊ���

   I2C_SET_SDA(HIGH);
   I2C_SET_SCL(LOW);                                        // ��ʼ״̬
   //SHT_Delay_Nus(10);

   I2C_SET_SCL(HIGH);
   //SHT_Delay_Nus(10);
   I2C_SET_SDA(LOW);
   //SHT_Delay_Nus(10);

   I2C_SET_SCL(LOW);
   //SHT_Delay_Nus(10);
   I2C_SET_SCL(HIGH);
   //SHT_Delay_Nus(10);

   I2C_SET_SDA(HIGH);
   //SHT_Delay_Nus(10);
   I2C_SET_SCL(LOW);
   //SHT_Delay_Nus(10);

   I2C_SET_SDA(LOW);
   //SHT_Delay_Nus(20);
}

/*******************************************************************************
 * �������ƣ�sht_connectionreset
 *
 * ����������ͨ�Ÿ�λ
 *                 _____________________________________________________         ________
 *           DATA:                                                      |_______|
 *                    _    _    _    _    _    _    _    _    _        ___     ___
 *           SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
 *
 * ��    ������
 *
 * �� �� ֵ����
 ******************************************************************************/
void sht_connectionreset(void)
{
    uint8_t i;

    SET_SDA_OUTPUT();                                       // DATA����Ϊ���

    I2C_SET_SDA(HIGH);
    I2C_SET_SCL(LOW);                                       // ��ʼ״̬

    /* 9��SCK ����*/
    for(i=0;i<9;i++) {
        I2C_SET_SCL(HIGH);
        //SHT_Delay_Nus(10);
        I2C_SET_SCL(LOW);
        //SHT_Delay_Nus(10);
    }
}

/*******************************************************************************
* ��    �ƣ� sht_softreset
* ��    �ܣ� �����λ
* ��ڲ�����
* ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
int8_t sht_softreset(void)
{
    uint8_t error = 0xfe;
    for ( int8_t i = 0; i < 5; i++ ) {
        if ( 1 == BSP_I2C_WriteByte(0x80, error) ) {
            DevType    = DEV_SHT2X;
            //SHT_Delay_Nus(1000*10);
            return 0;                                       // error=1��ʾSHT11δ��Ӧ
        }
        SHT_Delay_Nus(1000*20);
    }
    DevType    = DEV_SHT1X;
    sht_connectionreset();                                  // ͨ�Ÿ�λ
    error   = 0;
    error  += sht_write_byte(SHTCMD_RESET);                 // ����"��λ"�����SHT11
    return error;                                           // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
 * �������ƣ�sht_read_statusreg
 *
 * ������������ȡ״̬�Ĵ�����У���
 *
 * ��    ����p_value      ״̬�Ĵ�����ֵ
 *           p_checksum   У���
 *
 * �� �� ֵ������ֵΪ1��ʾSHT11δ��Ӧ
 ******************************************************************************/
int8_t sht_read_statusreg(uint8_t *p_value, uint8_t *p_checksum)
{
    uint8_t error=0;

    sht_transstart();                                       // ����һ��"��������"����
    error       = sht_write_byte(SHTCMD_READ_STATUES);      // ����"��״̬�Ĵ���"����
    *p_value    = sht_read_byte(I2C_ACK);                   // ��״̬�Ĵ���
    *p_checksum = sht_read_byte(I2C_NACK);                  // ��У���

    return error;                                           // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
* ��    �ƣ� sht_write_statusreg
* ��    �ܣ� д״̬�Ĵ���
* ��ڲ����� p_value      ״̬�Ĵ�����ֵ
* ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
int8_t sht_write_statusreg(uint8_t *p_value)
{
    uint8_t error=0;

    sht_transstart();                                       // ����һ��"��������"����
    error+=sht_write_byte(SHTCMD_WRITE_STATUS);             // ����"д״̬�Ĵ���"����
    error+=sht_write_byte(*p_value);                        // д״̬�Ĵ���

    return error;                                           // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
* ��    �ƣ� sht_measure
* ��    �ܣ� ����һ�β���(���ʪ�Ȼ��¶�)
* ��ڲ����� p_value      ����ֵ
*            checksum     У���
*            mode         TEMP��ʾ�����¶Ȳ���
*                         HUMI��ʾ�������ʪ�Ȳ���
* ���ڲ�����
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
int8_t sht_measure(u16 *p_value,  int8_t *p_checksum, uint8_t mode, uint8_t step)
{
    uint8_t error=0,err = 0;

    switch(step)
    {
        case 0:
        sht_transstart();                                   // ����һ��"��������"����
        switch(mode)                                        // �����������mode����һ����Ӧ�Ĳ���
        {
            case TEMP:
            error+=sht_write_byte(SHTCMD_MEASURE_TEMP);
            break;
            case HUMI:
            error+=sht_write_byte(SHTCMD_MEASURE_HUMI);
            break;
            default:
            break;
        } break;
        case 1:
        I2C_GET_SDA(err);
        if(err) {
            error+=1;
            break;
        }                                                   // ��������
        /*
        * Note:�ڴ����ݶ�ȡ�����ֽڣ����ݴ洢�迼�Ǵ�С�˸�ʽ�������
        */
        static u8 buf[2];
        buf[0]         = sht_read_byte(I2C_ACK);            // ����1���ֽ� (MSB)
        buf[1]         = sht_read_byte(I2C_ACK);            // ����2���ֽ� (LSB)
        *p_checksum     = sht_read_byte(I2C_NACK);          // ��У���
        *p_value        = BUILD_INT16U(buf[1],buf[0]);
        break;
    }

    return error;
}

/*******************************************************************************
* ��    �ƣ� calc_shtxx
* ��    �ܣ� �������ʪ�Ⱥ��¶�
* ��ڲ����� p_humidity      SHT11�ɼ��������ʪ��ֵ(������������ת�����ò�������ʵ����������ֵ)
*            p_temperature   SHT11�ɼ������¶�ֵ(������������ת�����ò�������ʵ����������ֵ)
* ���ڲ�����
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void calc_shtxx(float *p_humidity ,float *p_temperature)
{
    const float C1  = -4.0;                                 // 12λ
    const float C2  = +0.0405;                              // 12 Bit
    const float C3  = -0.0000028;                           // 12 Bit
    const float T1  = +0.01;                                // 14λ 5V
    const float T2  = +0.00008;                             // 14λ 5V

    float rh        = *p_humidity;                          // ���ʪ�Ȳɼ�ֵ 12λ
    float t         = *p_temperature;                       // �¶Ȳɼ�ֵ 14λ
    float t_C;                                              // �¶�������ֵ
    float rh_lin;                                           // ���ʪ�ȵķ����Բ���
    float rh_true;                                          // ���ʪ��������ֵ

    /***********************************************
    * ������
    */
    if ( DevType == DEV_SHT1X ) {
        t_C             = t * 0.01 - 39.60;                 // �����¶�������ֵ
        rh_lin          = C3*rh*rh + C2*rh + C1;            // �������ʪ�ȵķ����Բ���
    } else {
        t_C             = t * 175.72 / 16384 - 46.85;       // t*0.010725 - 46.85;
        rh_lin          = 125.0 * rh / 4096 - 6;            // �������ʪ�ȵķ����Բ���
    }

    /***********************************************
    * ������
    */
    rh_true         = (t_C-25) * (T1+T2*rh) + rh_lin;       // �������ʪ��������ֵ

    /***********************************************
    * ������ ��������������ʪ��������ֵ����Χ��ض�
    */
    if(rh_true>100)
        rh_true     = 100;
    if(rh_true<0.1)
        rh_true     = 0.1;
    /***********************************************
    * ������
    */
    *p_temperature  = t_C;                                  // �����¶�������ֵ
    *p_humidity     = rh_true;                              // �������ʪ��������ֵ
}

/*******************************************************************************
* ��    �ƣ� calc_dewpoint
* ��    �ܣ� ����¶��
* ��ڲ����� h      ���ʪ��������ֵ
*            t      �¶�������ֵ
* ���ڲ�����
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
float calc_dewpoint(float h,float t)
{
    float logEx,dew_point;

    logEx       = 0.66077 + 7.5 * t / (237.3 + t) + (log10(h) - 2);
    dew_point   = (logEx - 0.66077) * 237.3 / (0.66077 + 7.5 - logEx);

    return dew_point;
}

/*******************************************************************************
* ��    �ƣ� BSP_ShtMeasureOnce
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ���� ���ߣ�
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint16 BSP_ShtMeasureOnce(uint8_t mode , uint8_t step)
{
    int8_t                checksum;
    int8_t                error   = 0;
    static s16          humi    = 0;
    static s16          temp    = 0;

    /***********************************************
    * ������
    */
    switch(mode) {
    /***********************************************
    * ������ �ɼ��¶�
    */
    case TEMP:
        error += sht_measure((u16*)&temp, &checksum, TEMP, step);
        return error;
    /***********************************************
    * ������ �ɼ�ʪ��
    */
    case HUMI:
        error += sht_measure((u16*)&humi, &checksum, HUMI, step);
        return error;
    /***********************************************
    * ������ ����ֵ
    */
    default: {
        //sht_read_statusreg(&j,&k);
        /***********************************************
        * ������ ת���ɸ���
        */
        float   ftemp       = (float)temp;
        float   fhumi       = (float)humi;
        /***********************************************
        * ������ ������ʪ��
        */
        calc_shtxx(&fhumi,&ftemp);
        /***********************************************
        * ������ ��ȫ�ֱ���
        */
        SHT_Data.Temp       = ftemp;
        SHT_Data.Humi       = fhumi;
        //SHT_Data.DewPoint   = calc_dewpoint(fhumi,ftemp);
        }
        return 0;
    }
}

/*******************************************************************************
* ��    �ƣ� SHT_GpioInit
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ���� ���ߣ� ������
* �������ڣ� 2015-08-27
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void SHT_GpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(I2C_SCL_RCC,ENABLE);
    RCC_APB2PeriphClockCmd(I2C_SDA_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin   = I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(I2C_SDA_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(I2C_SCL_GPIO, &GPIO_InitStructure);

    SET_SCL_OUTPUT();
    SET_SDA_OUTPUT();

    I2C_SET_SCL(HIGH);
    I2C_SET_SDA(HIGH);
}

/*******************************************************************************
 * �������ƣ� BSP_ShtxxInit
 *
 * ���������� SHT11��ʼ������(����IO���ų�ʼ���͸�λ����)
 *
 * ��    ���� NONE
 *
 * �� �� ֵ�� NONE
 ******************************************************************************/
void BSP_ShtxxInit(void)
{
    SHT_GpioInit();                                          //Initialize sht11
    sht_softreset();
}

/*******************************************************************************
*              end of file                                                    *
*******************************************************************************/
