/*******************************************************************************
*   Filename:       bsp_eeprom.h
*   Revised:        All copyrights reserved to Roger.
*   Date:           2014-08-18
*   Revision:       v1.0
*   Writer:	        wumingshen.
*
*   Description:    AT24Cxxxϵ��EEPROM����ģ��ͷ�ļ�
*
*
*   Notes:
*
*******************************************************************************/
#ifndef	__BSP_EEPROM_H__
#define	__BSP_EEPROM_H__


/*********************************************************************
 * INCLUDES
 */
#include  <global.h>
#include  <includes.h>

//============================================================================//
//              EEPROM�����ͺŵ�ѡ���ʹ���������ã�����Ӳ���޸ģ�            //
//============================================================================//
/***********************************************
* ������ �궨�������ʹ�õ�AT24xxϵ�еľ����ͺ�
*/
#define AT_DEV_TYPE             AT24C64         // �����ͺ�
#define AT_DEV_NO_SEL_PIN       DEF_DISABLED     // �Ƿ��е�ַѡ������

/***********************************************
* ������ �궨��AT24xxϵ��һ���ֽڵĴ�С
*/
#define AT_BYTE_SIZE            8

/***********************************************
* ������ �궨��AT24xxϵ��EEPROM������
*/
#define AT24C01                 (1 * 1024)
#define AT24C02                 (2 * 1024)
#define AT24C04                 (4 * 1024)
#define AT24C08                 (8 * 1024)
#define AT24C16                 (16 * 1024)
#define AT24C32                 (32 * 1024)
#define AT24C64                 (64 * 1024)
#define AT24C128                (128 * 1024)
#define AT24C256                (256 * 1024)
#define AT24C512                (512 * 1024)
#define AT24C1024               (1024 * 1024)

/***********************************************
* ������ ���û�ж����豸���ͣ�Ĭ��ΪAT24C02
*/
#ifndef AT_DEV_TYPE
#define AT_DEV_TYPE             AT24C02
#endif


/***********************************************
* ������ ����AT24xxϵ�в�ͬ�ͺŵ�ҳ��
*/
#if     AT_DEV_TYPE == AT24C01
#define AT_MAX_BYTES            ( AT24C01 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                16u                             // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C02
#define AT_MAX_BYTES            ( AT24C02 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                32u                             // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C04
#define AT_MAX_BYTES            ( AT24C04 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                32u                             // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C08
#define AT_MAX_BYTES            ( AT24C08 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                64u                             // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C16
#define AT_MAX_BYTES            (AT24C16 / AT_BYTE_SIZE )       // ���ֽ���
#define AT_PAGES                128u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C32
#define AT_MAX_BYTES            (AT24C32 / AT_BYTE_SIZE )       // ���ֽ���
#define AT_PAGES                128u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C64
#define AT_MAX_BYTES            (AT24C64 / AT_BYTE_SIZE )       // ���ֽ���
#define AT_PAGES                256u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C128
#define AT_MAX_BYTES            (AT24C128 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                128u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C256
#define AT_MAX_BYTES            (AT24C256 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                256u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C512
#define AT_MAX_BYTES            (AT24C512 / AT_BYTE_SIZE )      // ���ֽ���
#define AT_PAGES                512u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#elif   AT_DEV_TYPE == AT24C1024
#define AT_MAX_BYTES            (AT24C1024 / AT_BYTE_SIZE )     // ���ֽ���
#define AT_PAGES                512u                            // ҳ��
#define AT_PAGE_SIZE            ( AT_MAX_BYTES / AT_PAGES )     // һҳ�ֽ���
#endif

/***********************************************
* ������ �궨��AT24xxϵ�е�I2C��ַ
*/
#define BSP_EEP_I2C_ADDR        0xA0


/*********************************************************************
 * GLOBAL FUNCTIONS
 */
BOOL     BSP_EEP_Init              (void);
void    BSP_EEP_Main              (void);

uint8_t  BSP_EEP_WriteByte         (uint16_t suba, uint8_t byte );
uint16_t BSP_EEP_WriteBytes        (uint16_t suba, uint8_t *buf, uint16_t len);
uint8_t  BSP_EEP_QuickReadByte     (uint8_t *dat);
uint8_t  BSP_EEP_ReadByte          (uint16_t suba, uint8_t *byte);
uint16_t BSP_EEP_ReadBytes         (uint16_t suba, uint8_t *buf, uint16_t len);
uint8_t  BSP_EEP_WriteByteChk      (uint16_t suba, uint8_t byte );

uint8_t  BSP_EEP_WriteINT16U       (uint16_t suba, uint16_t dat );
uint8_t  BSP_EEP_WriteINT32U       (uint16_t suba, uint32_t dat );

uint8_t  BSP_EEP_ReadINT16U        (uint16_t suba, uint16_t *dat );
uint8_t  BSP_EEP_ReadINT32U        (uint16_t suba, uint32_t *dat );
void    BSP_I2CSetPort              (INT08U port);
#endif
/*********************************************************************
 * 				end of file
 *********************************************************************/