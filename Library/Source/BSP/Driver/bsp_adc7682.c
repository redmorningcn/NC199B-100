/*******************************************************************************
*   Filename:       bsp_key.c
*   Revised:        All copyrights reserved to Roger.
*   Date:           2015-08-11
*   Revision:       v1.0
*   Writer:	        wumingshen.
*
*   Description:    ��������ģ��
*                   ����ʶ�𳤰����̰���˫������ת�������Ĳ�����
*
*
*   Notes:
*
*   All copyrights reserved to wumingshen
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <global.h>
#include <app_lib.h>

#include "bsp_adc7682.h"

#define BSP_KEY_MODULE_EN 1
#if BSP_KEY_MODULE_EN > 0
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */
/***********************************************
* ������ IO,ʱ�ӵȶ���
*/
#define SPI_SCK_PIN           GPIO_Pin_9             /* PC.09 */
#define SPI_SCK_PIN_NUM       9          
#define SPI_SCK_GPIO_PORT     GPIOC
#define SPI_SCK_GPIO_CLK      RCC_APB2Periph_GPIOC

#define SPI_MISO_PIN          GPIO_Pin_6             /* PA.06 */
#define SPI_MISO_PIN_NUM      6 
#define SPI_MISO_GPIO_PORT    GPIOA
#define SPI_MISO_GPIO_CLK     RCC_APB2Periph_GPIOA

#define SPI_MOSI_PIN          GPIO_Pin_7             /* PC.07 */
#define SPI_MOSI_PIN_NUM      7 
#define SPI_MOSI_GPIO_PORT    GPIOC
#define SPI_MOSI_GPIO_CLK     RCC_APB2Periph_GPIOC

#define SPI_CS_PIN            GPIO_Pin_8             /* PC.08 */
#define SPI_CS_PIN_NUM        8 
#define SPI_CS_GPIO_PORT      GPIOC
#define SPI_CS_GPIO_CLK       RCC_APB2Periph_GPIOC

/***********************************************
* ������ IO��������
*/
#define SPI_CS_LOW()     	  GPIO_ResetBits(SPI_CS_GPIO_PORT, SPI_CS_PIN)
#define SPI_CS_HIGH()    	  GPIO_SetBits(SPI_CS_GPIO_PORT, SPI_CS_PIN)

#define SPI_SCK_LOW()    	  GPIO_ResetBits(SPI_SCK_GPIO_PORT,SPI_SCK_PIN)
#define SPI_SCK_HIGH()   	  GPIO_SetBits(SPI_SCK_GPIO_PORT,SPI_SCK_PIN)

#define SPI_MISO_LOW()   	  GPIO_ResetBits(SPI_MISO_GPIO_PORT,SPI_MISO_PIN)
#define SPI_MISO_HIGH()  	  GPIO_SetBits(SPI_MISO_GPIO_PORT,SPI_MISO_PIN)

#define SPI_MOSI_LOW()        GPIO_ResetBits(SPI_MOSI_GPIO_PORT,SPI_MOSI_PIN)
#define SPI_MOSI_HIGH()       GPIO_SetBits(SPI_MOSI_GPIO_PORT,SPI_MOSI_PIN)

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

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

/*******************************************************************************
* ��    �ƣ� SPI_GpioInit
* ��    �ܣ� ADC GPIO��ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
static void SPI_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(SPI_CS_GPIO_CLK | SPI_MOSI_GPIO_CLK |
                         SPI_MISO_GPIO_CLK | SPI_SCK_GPIO_CLK, ENABLE);

    /*!< Configure SPI pins: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI pins: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;			//speed��mode�����޸ģ��������¸�ֵ
    GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI pins: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;          //GPIO_Mode_IPU;
    GPIO_Init(SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    /*!< Configure SPI pin: Chip CS pin */
    GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStructure);

    SPI_CS_HIGH();
}

/*******************************************************************************
* ��    �ƣ� SPI_SendByte
* ��    �ܣ� ͨ��SPI���߷���һ���ֽ�����(ͬʱ����һ���ֽ�����)
* ��ڲ����� Ҫд���һ���ֽ�����
* ���ڲ����� �ڷ�����ʱ��MISO�ź����Ͻ��յ�һ���ֽ�
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//static void SPI_SendByte(uint8_t byte)
//{
//    uint8_t i;
//    
//    /***********************************************
//    * ������ 
//    */
//    for(i=0;i<8;i++) {		
//		SPI_SCK_LOW();
//		Delay_Nus(10);
//		if(byte & 0x80)
//			SPI_MOSI_HIGH();
//		else
//			SPI_MOSI_LOW();
//		byte = byte << 1;
//		Delay_Nus(5);
//		SPI_SCK_HIGH();
//		Delay_Nus(10);
//	}
//}	

/*******************************************************************************
* ��    �ƣ� SPI_ReadByte
* ��    �ܣ� ��ȡһ���ֽڣ�δ�������Ͷ��������ʼ��ַ
* ��ڲ����� ��
* ���ڲ����� ��Ŀ���ȡ��һ���ֽ�
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//static uint8_t SPI_ReadByte(uint8_t Command)
//{
//    uint8_t i ,result = 0x00;
//	unsigned char SDI;
//    /***********************************************
//    * ������ 
//    */
//	for(i=0;i<8;i++) {
//		SPI_SCK_LOW();
//		Delay_Nus(2);
//		if (Command & 0x80) {
//			SPI_MOSI_HIGH();
//		} else {
//			SPI_MOSI_LOW();
//        }
//        
//		Command = Command << 1;
//		SPI_SCK_HIGH();
//		Delay_Nus(2);
//		SDI =  GPIO_ReadInputDataBit(SPI_MISO_GPIO_PORT, SPI_MISO_PIN);
//		result = result << 1;
//        
//		if(SDI)
//			result = result | 0x01;
//	}
//    
//	return result;
//}

/*******************************************************************************
* ��    �ƣ� SPI_SendCommand
* ��    �ܣ� ͨ��SPI���߷���һ���ֽ�����
* ��ڲ����� Ҫд���һ���ֽ�����
* ���ڲ����� ��
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
//static void SPI_SendCommand(uint8_t Command)
//{
//	SPI_CS_LOW();
//	Delay_Nus(5);
//	SPI_SendByte(Command);
//	SPI_CS_HIGH();
//	Delay_Nus(5);
//}

/*******************************************************************************
* ��    �ƣ� AD7682_spi
* ��    �ܣ� reg�Ĵ�������,dataת������
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
static uint16_t AD7682_spi(uint8_t ch)
{
	uint16_t    reg     = 0;
	uint16_t    data    = 0;
	uint8_t     i       = 0;
	u8          SDI     = 0;
	
    /***********************************************
    * ������ 
    */
	SPI_CS_LOW();
	Delay_Nus(2);
	SPI_CS_HIGH();
	Delay_Nus(2);
	SPI_CS_LOW();
    
    /***********************************************
    * ������ 7682���üĴ���
    *        13  12   11   10   9   8   7   6  5   4   3   2   1   0
             CFG INCC INCC INCC INx INx INx BW REF REF REF SEQ SEQ RB
    */
    reg = (1 <<13)                                  // 1    = ���ǼĴ���������
        | (6 <<10)                                  // 110  = �����ԣ�INx��COM=GND��0.1VΪ�ο�
        | (ch<<7)                                   // 1XX  = �ɼ�ͨ��ѡ��
        | (0 <<6)                                   // 0    = 1/4����62.5K����1   = ȫ����
        | (2 <<3)                                   // 010  = �ⲿ��׼���¶ȴ�����ʹ��
        | (0 <<1)                                   // 00   = ����������
        | (1 <<0);                                  // 1    = ���ض���������
    //reg = b1 110 100 0 010 00 1
	//reg = (ch<<7)|0x3811;
	reg <<=2;                                       //�Ĵ���ֻ�õ���14λ������Ҫ���Ƴ�����λ	
	Delay_Nus(2);
	
    /***********************************************
    * ������ 
    */
	for(i = 0; i < 16; i++) {
		if(reg & 0x8000)   
            SPI_MOSI_HIGH();
		else               
            SPI_MOSI_LOW();
		
		Delay_Nus(2);
		SPI_SCK_LOW();
		Delay_Nus(2);
		reg <<=1;
		SPI_SCK_HIGH();
		data <<=1;
		Delay_Nus(1);
		SDI = GPIO_ReadInputDataBit(SPI_MISO_GPIO_PORT,  SPI_MISO_PIN); 
		if(SDI)      
			data = data | 0x01;	
	}
    
    /***********************************************
    * ������ 
    */
	Delay_Nus(2);
	SPI_SCK_LOW();
	Delay_Nus(2);
	SPI_CS_HIGH();
	return (data);
}

/*******************************************************************************
* ��    �ƣ� BSP_ADCInit
* ��    �ܣ� ADC7682��ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void BSP_ADCInit( void )
{
	SPI_GpioInit(); 
    
	SPI_CS_HIGH();
	SPI_MISO_HIGH();
	SPI_SCK_HIGH();
    
	Delay_Nus(30);
	AD7682_spi(0);
	AD7682_spi(0); 
}


/*******************************************************************************
* ��    �ƣ� Get_ADC_10times_Value
* ��    �ܣ� ���� ch ѡ��ͨ��  ǰ���βɼ���Ч�� �ɼ�10�Σ�ȥ�����ֵ����Сֵ
* ��ڲ����� ch - ͨ����ţ�0~3��
* ���ڲ����� ��
* �������ߣ� wumingshen.
* �������ڣ� 2015-12-07
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
 uint16_t BSP_ADCGetValue(uint8_t ch)
{
#define SAMPLE_TIMES            100
#define FILTER_LEN              10
#if (FILTER_LEN * 2 + 1) > SAMPLE_TIMES
#error  "FILTER_LEN ̫�󣬹��˲���̫��"
#endif
	uint16_t      x;    
	long       sum             = 0;
    
    uint16_t      buf[SAMPLE_TIMES];
    uint16_t      After[SAMPLE_TIMES];
    /***********************************************
    * ������ ��������
    */
    if ( ch > 3 )
        return  0;
    
    /***********************************************
    * ������ ����ǰǰһ�β���ֵ 
    */
	AD7682_spi(ch);
	
    /***********************************************
    * ������ ��ʼ�ɼ�
    */
	for(u8 i = 0; i < SAMPLE_TIMES; i++) {
		buf[i]      = AD7682_spi(ch);
	}
    
    /***********************************************
    * ������ һ���˲�������
    */
    App_BubbleAscendingArray(buf, SAMPLE_TIMES, After);
    u16 *p  = After;
    
    /***********************************************
    * ������ һ���˲�������
    */
    for(u8 i = FILTER_LEN; i < SAMPLE_TIMES - FILTER_LEN;i++) {
        sum += p[i];
    }
    
    x = (uint16_t)(sum/(float)(SAMPLE_TIMES - FILTER_LEN*2));
    
	return (x);
}

/*******************************************************************************
* 				                end of file                                    *
*******************************************************************************/
#endif