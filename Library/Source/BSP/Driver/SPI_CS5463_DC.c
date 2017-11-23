/******************** (C) COPYRIGHT 2013 www.armjishu.com  ********************
 * �ļ���  ��SPI_CS5463_DC.c
 * ����    ��ʵ��CS5463�ĵײ㺯��
 * ʵ��ƽ̨��STM32���ۿ�����
 * ��׼��  ��STM32F10x_StdPeriph_Driver V3.5.0
 * ����    ��zw
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "SPI_CS5463_DC.h"
#include "SZ_STM32F107VC_LIB.h"
#include "DELAY.h"
#include "POWER_MACRO.h"


#define SPI_CS5463_DC                   SPI1

#define SPI_CS5463_DC_CLK               RCC_APB2Periph_SPI1
#define SPI_CS5463_DC_SCK_PIN           GPIO_Pin_5              /* PA.05 */
#define SPI_CS5463_DC_SCK_GPIO_PORT     GPIOA
#define SPI_CS5463_DC_SCK_GPIO_CLK      RCC_APB2Periph_GPIOA

#define SPI_CS5463_DC_MISO_PIN          GPIO_Pin_6              /* PA.06 */
#define SPI_CS5463_DC_MISO_GPIO_PORT    GPIOA
#define SPI_CS5463_DC_MISO_GPIO_CLK     RCC_APB2Periph_GPIOA

#define SPI_CS5463_DC_MOSI_PIN          GPIO_Pin_7              /* PA.07 */
#define SPI_CS5463_DC_MOSI_GPIO_PORT    GPIOA
#define SPI_CS5463_DC_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOA

#define SPI_CS5463_DC_CS_PIN_NUM        4                       /* PA.04 */
#define SPI_CS5463_DC_CS_PIN            GPIO_Pin_4  
#define SPI_CS5463_DC_CS_GPIO_PORT      GPIOA
#define SPI_CS5463_DC_CS_GPIO_CLK       RCC_APB2Periph_GPIOA

#define SPI_RST_DC_PIN_NUM              0
#define SPI_RST_DC_PIN                  GPIO_Pin_0   		    /* PB.00 */
#define SPI_RST_DC_GPIO_PORT            GPIOB
#define SPI_RST_DC_GPIO_CLK             RCC_APB2Periph_GPIOB

#define E1_DC_PIN_NUM                   0
#define E1_DC_PIN                       GPIO_Pin_0
#define E1_DC_TIMER_PORT                GPIOA
#define E1_DC_TIMER_CLK                 RCC_APB2Periph_GPIOA
//#define E1_DC_TIMER_CLK                 RCC_APB1Periph_TIM5

#define E2_DC_PIN_NUM                   9
#define E2_DC_PIN                       GPIO_Pin_9
#define E2_DC_TIMER_PORT                GPIOE
#define E2_DC_GPIO_CLK                  RCC_APB2Periph_GPIOE


/*�Ĵ�����д*/

#define CONFIG_DATA_DC                  0x000001
#define STATUS_DATA_DC                  0x800000
#define INTMASK_DATA_DC                 0x000000
#define CTRL_DATA_DC                    0x000005
#define CYCLE_COUNT_DATA_DC             0x000FA0
#define PULSE_RATE_DATA_DC              0x00BE95
#define MODE_DATA_DC                    0x000000

CONFIGSFR   configsfr_DC;

/* Private macro -------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */

//#define SPI_CS5463_DC_CS_LOW()   SPI_SSOutputCmd(SPI1, ENABLE)		   //NSS���ʹ��
//#define SPI_CS5463_DC_CS_HIGH()  SPI_SSOutputCmd(SPI1, DISABLE)		   //NSS�����ֹ
#define SPI_CS5463_DC_CS_LOW()   (Periph_BB((uint32_t) & SPI_CS5463_DC_CS_GPIO_PORT->ODR, SPI_CS5463_DC_CS_PIN_NUM) = 0)
#define SPI_CS5463_DC_CS_HIGH()  (Periph_BB((uint32_t) & SPI_CS5463_DC_CS_GPIO_PORT->ODR, SPI_CS5463_DC_CS_PIN_NUM) = 1)

#define SPI_RST_DC_LOW()         GPIO_ResetBits(SPI_RST_DC_GPIO_PORT, SPI_RST_DC_PIN)
#define SPI_RST_DC_HIGH()        GPIO_SetBits(SPI_RST_DC_GPIO_PORT, SPI_RST_DC_PIN)



/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**-----------------------------------------------------------------
  * @������ SPI_DC_INIT
  * @����   ��ʼ����CS5463�ӿڵ���������
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_DC_INIT(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable SPI1 and GPIO clocks */
    /*!< SPI_CS5463_DC_CS_GPIO, SPI_CS5463_DC_MOSI_GPIO, 
       SPI_CS5463_DC_MISO_GPIO, SPI_CS5463_DC_DETECT_GPIO 
       and SPI_CS5463_DC_SCK_GPIO Periph clock enable */
    RCC_APB2PeriphClockCmd(SPI_CS5463_DC_CS_GPIO_CLK | SPI_CS5463_DC_MOSI_GPIO_CLK |
                         SPI_CS5463_DC_MISO_GPIO_CLK | SPI_CS5463_DC_SCK_GPIO_CLK, ENABLE);

    /*!< SPI_CS5463_AC Periph clock enable */
    RCC_APB2PeriphClockCmd(SPI_CS5463_DC_CLK, ENABLE);
    
    /*!< AFIO Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /*!< Configure SPI_CS5463_AC pins: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_CS5463_DC_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_CS5463_DC_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_CS5463_DC pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI_CS5463_DC_MOSI_PIN;			//speed��mode�����޸ģ��������¸�ֵ
    GPIO_Init(SPI_CS5463_DC_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_CS5463_DC pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_CS5463_DC_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;//
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(SPI_CS5463_DC_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI_CS5463_AC_CS_PIN pin: CS5463 Chip CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_CS5463_DC_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI_CS5463_DC_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure SPI_CS5463_AC pins: RST */
    RCC_APB2PeriphClockCmd(SPI_RST_DC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SPI_RST_DC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI_RST_DC_GPIO_PORT, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(E1_DC_TIMER_CLK | E2_DC_GPIO_CLK , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = E1_DC_PIN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(E1_DC_TIMER_PORT , &GPIO_InitStructure);
//	GPIO_Init(E2_DC_TIMER_PORT , &GPIO_InitStructure);          //E1��E2Ϊͬһ��PORT���˴�����

    /* Deselect the FLASH: Chip Select high */
    SPI_CS5463_DC_CS_HIGH();

	/* SPI1 configuration */
    // data input on the DIO pin is sampled on the rising edge of the CLK. 
    // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		 //����SSIΪ1����SPIΪ��ģʽ��
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;			 //CPOL = 1,ʱ�Ӽ���Ϊ1����������������
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		 //CPHA = 1,��λΪ1����2����������
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			 //����SSMΪ1������������豸����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

//	SPI_SSOutputCmd(SPI1, ENABLE);      //ʹ��SPI1��NSS���
    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);

//  SPI_CS5463_DC_SOFT_RESET();
}

/**-----------------------------------------------------------------
  * @������ CS5463_DC_INIT
  * @����   CS5463��ʼ��
  *         
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void CS5463_DC_INIT(void)
{
	uint32_t STATUS_VALUE = 0;
	SPI_RST_DC_LOW();
	SPI_RST_DC_LOW();
	SPI_RST_DC_LOW();
	Delay(1);
	SPI_RST_DC_HIGH() ;
	SPI_RST_DC_HIGH();
	SPI_RST_DC_HIGH();
	Delay(1000);

//	SPI_CS5463_DC_CS_LOW();

  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(SYNC1);
	SPI_CS5463_DC_SendByte(SYNC1);
	SPI_CS5463_DC_SendByte(SYNC1);
  	SPI_CS5463_DC_SendByte(SYNC0);

	SPI_CS5463_DC_WriteRegister(CONFIG , CONFIG_DATA_DC);

/*   ����Ϊ150V �� 1A�����У׼��оƬ�������ŶԵ�ֱ�Ӷ̽�  */
	SPI_CS5463_DC_WriteRegister(IG , 0x488C31);
	SPI_CS5463_DC_WriteRegister(VG , 0x682E94);
	SPI_CS5463_DC_WriteRegister(IDCOFF , 0xFE396B);
	SPI_CS5463_DC_WriteRegister(VDCOFF , 0x072C2E);
	SPI_CS5463_DC_WriteRegister(IACOFF , 0xFFFC8A);
	SPI_CS5463_DC_WriteRegister(VACOFF , 0xFFF800);


/*   ����Ϊ187.5V �� 1.2A�����У׼����ֻ�̽��������߶Ե�  */
//	SPI_CS5463_DC_WriteRegister(IDCOFF , 0xFE4F47);
//	SPI_CS5463_DC_WriteRegister(VDCOFF , 0x075074);
//	SPI_CS5463_DC_WriteRegister(IACOFF , 0xFFF100);
//	SPI_CS5463_DC_WriteRegister(VACOFF , 0xFFF100);
//	SPI_CS5463_DC_WriteRegister(IG , 0x3C764F);
//	SPI_CS5463_DC_WriteRegister(VG , 0x531018);

	STATUS_VALUE = SPI_CS5463_DC_ReadRegister_INT32(STATUS);		    //��ȡ״̬�Ĵ���
	SPI_CS5463_DC_WriteRegister(STATUS , STATUS_VALUE);					//д״̬�Ĵ���

	SPI_CS5463_DC_WriteRegister(INTMASK , INTMASK_DATA_DC);				//�����ж�

	SPI_CS5463_DC_WriteRegister(CTRL , CTRL_DATA_DC);		  			//д���ƼĴ���

	SPI_CS5463_DC_WriteRegister(CYCLE_COUNT , CYCLE_COUNT_DATA_DC);		//A/Dת��������

	SPI_CS5463_DC_WriteRegister(PULSE_RATE , PULSE_RATE_DATA_DC);			//������������

	SPI_CS5463_DC_WriteRegister(PAGE , PAGE_1);						//д�Ĵ���1ҳ
	SPI_CS5463_DC_WriteRegister(PULSE_WIDTH , 0x000002);			//���������ȣ�10ms
	SPI_CS5463_DC_WriteRegister(PAGE , PAGE_0);						//�ָ��ԼĴ���0ҳ����

	STATUS_VALUE = SPI_CS5463_DC_ReadRegister_INT32(STATUS);		    //��ȡ״̬�Ĵ���
	SPI_CS5463_DC_WriteRegister(STATUS , STATUS_VALUE);					//д״̬�Ĵ���

	SPI_CS5463_DC_SendByte(CONTINUOUS_CYCLE);				            //����������������

    /* Deselect the CS5463: Chip Select high */
    SPI_CS5463_DC_CS_HIGH();
	Delay(10);
}

/**-----------------------------------------------------------------
  * @������ CS5463_DC_Adjust
  * @����   CS5463У׼
  *         
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void CS5463_DC_Adjust(void)						//5463У׼
{
	uint32_t i;
	float x;
	uint8_t mode;
    SPI_CS5463_DC_SendByte(SYNC1);
	SPI_CS5463_DC_SendByte(SYNC1);
	SPI_CS5463_DC_SendByte(SYNC1);
	SPI_CS5463_DC_SendByte(SYNC0);   										
 	SPI_CS5463_DC_ClearDrdy();    
 	SPI_CS5463_DC_WaitDrdy();  
    SPI_CS5463_DC_SendCommand(STOP); 									//����POWER_HALT ����
   
    mode = IACGAINC;
    //����  ����ƫ��У׼��
 	SPI_CS5463_DC_ClearDrdy();    
    switch(mode)
    {
    case IDCOFFC:														//ֱ�����У׼
	 	SPI_CS5463_DC_ClearDrdy();        								 
    	SPI_CS5463_DC_WriteRegister(IDCOFF , 0x000000);				    //��ѹ����У׼�Ĵ���**
		SPI_CS5463_DC_SendCommand(IDCOFFC);								//дУ׼����Ĵ���
		for(i=0;i<100000000;i++);
    	SPI_CS5463_DC_WaitDrdy();     									//�ȴ�У׼��� 										
    	configsfr_DC.Idcoff = SPI_CS5463_DC_ReadRegister_INT32(IDCOFF)|0xAA000000;	//��Idcoff �Ĵ���
    	printf("\r\n ֱ������ƫ�� = %x\n", configsfr_DC.Idcoff);
		break;
    case VDCOFFC:														//ֱ�����У׼
	 	SPI_CS5463_DC_ClearDrdy();    
        SPI_CS5463_DC_WriteRegister(VDCOFF , 0x000000); 				//��ѹ����У׼�Ĵ���** 
   		SPI_CS5463_DC_SendCommand(VDCOFFC);								//дУ׼����Ĵ���
    	for(i=0;i<100000000;i++);
		SPI_CS5463_DC_WaitDrdy();     									//�ȴ�У׼���
    	configsfr_DC.Vdcoff = SPI_CS5463_DC_ReadRegister_INT32(VDCOFF)|0xAA000000;	//��Idcoff �Ĵ���
    	printf("\r\n ֱ����ѹƫ�� = %x\n", configsfr_DC.Vdcoff);
		break;
    case IACOFFC:														//�������У׼
	 	SPI_CS5463_DC_ClearDrdy();    
        SPI_CS5463_DC_WriteRegister(IACOFF , 0x000000); 				//��ѹ����У׼�Ĵ���** 
  		SPI_CS5463_DC_SendCommand(IACOFFC);								//дУ׼����Ĵ���
    	for(i=0;i<100000000;i++);
		SPI_CS5463_DC_WaitDrdy();     									//�ȴ�У׼���
		configsfr_DC.Iacoff = SPI_CS5463_DC_ReadRegister_INT32(IACOFF)|0xAA000000;	//��Iacoff �Ĵ���
    	printf("\r\n ��������ƫ�� = %x\n", configsfr_DC.Iacoff);
		break;
    case VACOFFC:														//�������У׼
		SPI_CS5463_DC_ClearDrdy();    
        SPI_CS5463_DC_WriteRegister(VACOFF , 0x000000); 				//��ѹ����У׼�Ĵ���** 
    	SPI_CS5463_DC_SendCommand(VACOFFC);								//дУ׼����Ĵ���
    	for(i=0;i<100000000;i++);
		SPI_CS5463_DC_WaitDrdy();     									//�ȴ�У׼���
		configsfr_DC.Vacoff = SPI_CS5463_DC_ReadRegister_INT32(VACOFF)|0xAA000000; //��Idcoff �Ĵ���
    	printf("\r\n ������ѹƫ�� = %x\n", configsfr_DC.Vacoff);
		break;
	case VACGAINC:														//��ѹ����У׼
		SPI_CS5463_DC_ClearDrdy();    
      	SPI_CS5463_DC_WriteRegister(VG , 0x400000); 					//��ѹ����У׼�Ĵ���** 
		SPI_CS5463_DC_SendCommand(VACGAINC);								//дУ׼����Ĵ���
    	for(i=0;i<50000000;i++);
		SPI_CS5463_DC_WaitDrdy();     									  //�ȴ�У׼���
		configsfr_DC.Vgain = SPI_CS5463_DC_ReadRegister_INT32(VG)|0xAA000000; //��Igain �Ĵ���
//    	x = SPI_CS5463_DC_Read_VIgain_FLOAT(VG);
		SPI_CS5463_DC_ClearDrdy();
		SPI_CS5463_DC_SendByte(CONTINUOUS_CYCLE);
		SPI_CS5463_DC_WaitDrdy();
		x = SPI_CS5463_DC_Read_VIrms_FLOAT(VRMS);
		printf("\r\n ������ѹ���� = %x\n", configsfr_DC.Vgain);
		printf("\r\n ������ѹ��Чֵ = %f\n", x);
		break;
	case IACGAINC:														//��������У׼
		SPI_CS5463_DC_ClearDrdy();    
	    SPI_CS5463_DC_WriteRegister(IG , 0x400000); 					//��������У׼�Ĵ���** 
    	SPI_CS5463_DC_SendCommand(IACGAINC);								//дУ׼����Ĵ���
    	for(i=0;i<50000000;i++);
		SPI_CS5463_DC_WaitDrdy();     									  //�ȴ�У׼���
		configsfr_DC.Igain = SPI_CS5463_DC_ReadRegister_INT32(IG)|0xAA000000; //��Igain �Ĵ���
//    	x = SPI_CS5463_DC_Read_VIgain_FLOAT(VG);
		SPI_CS5463_DC_ClearDrdy();
		SPI_CS5463_DC_SendByte(CONTINUOUS_CYCLE);
		SPI_CS5463_DC_WaitDrdy();
		x = SPI_CS5463_DC_Read_VIrms_FLOAT(IRMS);
		printf("\r\n ������������ = %x\n", configsfr_DC.Igain);
		printf("\r\n ����������Чֵ = %f\n", x);
		break;   
    }
	CS5463_DC_INIT();			 												//��������
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_AC_SendByte
  * @����   ͨ��SPI���߷���һ���ֽ�����(ͬʱ����һ���ֽ�����)
  *         Sends a byte through the SPI interface and return the byte
  *         received from the SPI bus.
  * @����   Ҫд���һ���ֽ�����
  * @����ֵ �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ���ֽ�
***----------------------------------------------------------------*/
uint8_t SPI_CS5463_DC_SendByte(uint8_t byte)
{
    SPI_CS5463_DC_CS_LOW();
//	Delay(10);
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, byte);

    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

//    SPI_CS5463_DC_CS_HIGH();
		/* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);	
//	Delay(10);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_ReadByte
  * @����   ��ȡCS5463��һ���ֽڣ�δ�������Ͷ��������ʼ��ַ
  * @����   ��
  * @����ֵ ��CS5463��ȡ��һ���ֽ�
***----------------------------------------------------------------*/
uint8_t SPI_CS5463_DC_ReadByte(void)
{
    return (SPI_CS5463_DC_SendByte(Dummy_Byte));
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_SendCommand
  * @����   ͨ��SPI���߷���һ���ֽ�����
  *         Sends a byte through the SPI interface
  * @����   Ҫд���һ���ֽ�����
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_SendCommand(uint8_t Command)
{
	SPI_CS5463_DC_CS_LOW();
	Delay(5);
	SPI_CS5463_DC_SendByte(Command);
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Write
  * @����   ��CS5463��д���ⳤ������
  * @����   *buf ���飬len ���鳤��(���ֽ�Ϊ��С��λ)
***----------------------------------------------------------------*/
void SPI_CS5463_DC_Write(uint8_t *buf , uint8_t len)
{
	uint8_t i;
	/* Select the CS5463: Chip Select low */
	SPI_CS5463_DC_CS_LOW();
	Delay(2);
	buf[0]|=0x40;
	for(i=0;i<len;i++)
		SPI_CS5463_DC_SendByte(buf[i]);
	Delay(1);
	SPI_CS5463_DC_CS_HIGH();
	Delay(1);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_WriteRegister
  * @����   дCS5463�ļĴ���
  * @����   CommandΪ���DATAΪ����
***----------------------------------------------------------------*/
void SPI_CS5463_DC_WriteRegister(uint8_t Command , uint32_t DATA)
{
	uint8_t buf[4];
	/* Select the CS5463: Chip Select low */
	buf[0] = Command;
	buf[1] = (DATA >> 16) & 0xFF;
	buf[2] = (DATA >> 8) & 0xFF;
	buf[3] = DATA & 0xFF;
	SPI_CS5463_DC_Write(buf , 4);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Read
  * @����   ��ȡCS5463�ļĴ���
  * @����   Command�����ֽ�����,bufΪ��ȡ��������
***----------------------------------------------------------------*/
void SPI_CS5463_DC_Read(uint8_t Command , uint8_t *buf)
{
	uint8_t i;
  /* Select the CS5463: Chip Select low */
    SPI_CS5463_DC_CS_LOW();
	Delay(2);
  /* Send  instruction */
    SPI_CS5463_DC_SendByte(Command);
	for(i=1;i<4;i++)
		buf[i] = SPI_CS5463_DC_SendByte(0xFE);
	buf[0] = Command;
	Delay(1);
	SPI_CS5463_DC_CS_HIGH();
	Delay(2);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_ReadRegister_INT32
  * @����   ��ȡCS5463�ļĴ������õ�1��3�ֽ�����
  * @����   Command�����ֽ�����
  * @����   Data��3�ֽ�����
***----------------------------------------------------------------*/
uint32_t SPI_CS5463_DC_ReadRegister_INT32(uint8_t Command)
{
	uint32_t Data = 0;
	uint8_t Data0 = 0,Data1 = 0,Data2 = 0;
  /* Select the CS5463: Chip Select low */
  	SPI_CS5463_DC_CS_LOW();
	Delay(1);
  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(Command);

	Data0 = SPI_CS5463_DC_SendByte(0xFE);
	Data1 = SPI_CS5463_DC_SendByte(0xFE);
	Data2 = SPI_CS5463_DC_SendByte(0xFE);
	Data = Data0<<16 | Data1<<8 | Data2;
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
	return Data;
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Read_Temp_FLOAT
  * @����   ��ȡCS5463�ļĴ������õ�1������������
  * @����   Command�����ֽ�����
  * @����   Data������������
  * @��ע   ���صĵ��ֽ�Ϊ-2^7,2^6��2^-16
***----------------------------------------------------------------*/
float SPI_CS5463_DC_Read_Temp_FLOAT(uint8_t Command)
{
	float Data = 0;
	uint8_t Data0 = 0,Data1 = 0,Data2 = 0;
  /* Select the CS5463: Chip Select low */
  	SPI_CS5463_DC_CS_LOW();
	Delay(1);
  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(Command);
	
	Data0 = SPI_CS5463_DC_SendByte(0xFE);
	Data1 = SPI_CS5463_DC_SendByte(0xFE);
	Data2 = SPI_CS5463_DC_SendByte(0xFE);
	Data = ((uint8_t)((Data0>>7) & 0x01))*(-128) + (Data0 & 0x7F) + (float)Data1/256 + (float)Data2/65536;
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
	return Data;
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Read_VIrms_FLOAT
  * @����   ��ȡCS5463�ĵ�ѹ/������Чֵ���õ�1������������
  * @����   Command�����ֽ�����
  * @����   Data������������				  
  * @��ע   ���صĵ��ֽ�Ϊ2^-1��2^-24
***----------------------------------------------------------------*/
float SPI_CS5463_DC_Read_VIrms_FLOAT(uint8_t Command)
{
	float Data = 0;
	uint8_t Data0 = 0,Data1 = 0,Data2 = 0;
  /* Select the CS5463: Chip Select low */
  	SPI_CS5463_DC_CS_LOW();
	Delay(1);
  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(Command);
	
	Data0 = SPI_CS5463_DC_SendByte(0xFE);
	Data1 = SPI_CS5463_DC_SendByte(0xFE);
	Data2 = SPI_CS5463_DC_SendByte(0xFE);
	Data = (float)(Data0)/256 + (float)Data1/256/256 + (float)Data2/256/256/256;
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
	return Data;
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Read_VIgain_FLOAT
  * @����   ��ȡCS5463�ĵ�ѹ/�������棬�õ�1������������
  * @����   Command�����ֽ�����
  * @����   Data������������
  * @��ע   ���صĵ��ֽ�Ϊ2^1��2^0 + 2^-1��2^-22 (2^1��2^-22)
***----------------------------------------------------------------*/
float SPI_CS5463_DC_Read_VIgain_FLOAT(uint8_t Command)
{
	float Data = 0;
	uint8_t Data0 = 0,Data1 = 0,Data2 = 0;
  /* Select the CS5463: Chip Select low */
  	SPI_CS5463_DC_CS_LOW();
	Delay(1);
  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(Command);
	
	Data0 = SPI_CS5463_DC_SendByte(0xFE);
	Data1 = SPI_CS5463_DC_SendByte(0xFE);
	Data2 = SPI_CS5463_DC_SendByte(0xFE);
	Data = ((Data0>>6)&0x03)+(float)(Data0&0x3F)/64 + (float)Data1/64/256 + (float)Data2/64/256/256;
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
	return Data;
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_Read_Else_FLOAT
  * @����   ��ȡCS5463�ĵ�ѹ����DCƫ�ƣ��õ�1������������
  * @����   Command�����ֽ�����
  * @����   Data������������
  * @��ע   ���صĵ��ֽ�Ϊ2^-1��2^-23���ټ������λ��ֵ-2^0
***----------------------------------------------------------------*/
float SPI_CS5463_DC_Read_Else_FLOAT(uint8_t Command)
{
	float Data = 0;
	uint8_t Data0 = 0,Data1 = 0,Data2 = 0;
  /* Select the CS5463: Chip Select low */
  	SPI_CS5463_DC_CS_LOW();
	Delay(1);
  /* Send  instruction */
  	SPI_CS5463_DC_SendByte(Command);
	
	Data0 = SPI_CS5463_DC_SendByte(0xFE);
	Data1 = SPI_CS5463_DC_SendByte(0xFE);
	Data2 = SPI_CS5463_DC_SendByte(0xFE);
	Data = (float)((Data0>>7)&0x01)*(-1)+(float)(Data0&0x7F)/128 + (float)Data1/32768 + (float)Data2/256/32768;
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
	return Data;
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_ClearDrdy
  * @����   CS5463����������ݾ���״̬λ
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_ClearDrdy(void)
{
	SPI_CS5463_DC_WriteRegister(STATUS , 0xFFFFFF);
}

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_AC_WaitDrdy
  * @����   CS5463�ȴ����ݾ���״̬λ
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_WaitDrdy(void)
{
	while (SPI_CS5463_DC_ReadRegister_INT32(STATUS) & 0x800000 == 0); 	
}


/**-----------------------------------------------------------------
  * @������ SPI_SPI_CS5463_DC_SLEEP
  * @����   CS5463����˯��ģʽ
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_SLEEP(void)   
{ 
	/* Select the CS5463: Chip Select low */
	SPI_CS5463_DC_CS_LOW();
	//  Delay(10);
	/* Send "WAIT" instruction */
	SPI_CS5463_DC_SendByte(SLEEP);
	
	/* Deselect the CS5463: Chip Select high */
	SPI_CS5463_DC_CS_HIGH();
	//  Delay(10);
}   

/**-----------------------------------------------------------------
  * @������ SPI_SPI_CS5463_DC_Wait
  * @����   CS5463����ȴ�ģʽ
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_WAIT(void)   
{ 
	/* Select the CS5463: Chip Select low */
	SPI_CS5463_DC_CS_LOW();
	Delay(5);
	/* Send "WAIT" instruction */
	SPI_CS5463_DC_SendByte(WAIT);
	
	/* Deselect the CS5463: Chip Select high */
	SPI_CS5463_DC_CS_HIGH();
	Delay(5);
}   

/**-----------------------------------------------------------------
  * @������ SPI_CS5463_DC_SOFT_RESET
  * @����   �����λCS5463
  * @����   ��
  * @����ֵ ��
***----------------------------------------------------------------*/
void SPI_CS5463_DC_SOFT_RESET(void)   
{
  /* Select the CS5463: Chip Select low */
  SPI_CS5463_DC_CS_LOW();
  Delay(100);
  /* Send "SOFT_RESET" instruction */
  SPI_CS5463_DC_SendByte(SOFT_RESET);

  /* Deselect the CS5463: Chip Select high */
  SPI_CS5463_DC_CS_HIGH();
  Delay(100);
}   

/******************* (C) COPYRIGHT 2010 www.armjishu.com *****END OF FILE****/
