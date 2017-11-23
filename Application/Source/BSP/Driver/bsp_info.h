/*******************************************************************************
 *   Filename:       bsp_info.h
 *   Revised:        $Date: 2015-08-24	 $
 *   Revision:       $V1.0
 *	 Writer:		 Roger-WY.
 *
 *   Description:    ��ȡоƬΨһUIDģ������ͷ�ļ�
 *
 *   Notes:          ÿ��STM32оƬ�ڳ���ʱ������һ��ȫ��Ψһ��ID����IDΪ96λ�����ַ�ֲ����£�
 *                   Start Address : 0x1FFF F7E8
 *                   Address offset: 0��00 U_ID(00-31)
 *                   Address offset: 0��04 U_ID(32-63)
 *                   Address offset: 0��08 U_ID(64-96)
 *                   ��ID��Ӧ�ã�
 *
 *                   ��������Ϊ���к�(����USB�ַ����кŻ����������ն�Ӧ��)
 *                   ��������Ϊ���룬�ڱ�д����ʱ������Ψһ��ʶ������ӽ����㷨���ʹ�ã���ߴ���������洢���ڵİ�ȫ�ԡ�
 *                   �������������ȫ���Ƶ��Ծٹ���
 *
 *
 *   All copyrights reserved to Roger-WY.
 *
 *******************************************************************************/
#ifndef __BSP_INFO_H__
#define __BSP_INFO_H__

/*******************************************************************************
 * INCLUDES
 */
#include <global.h>

/*******************************************************************************
 * CONSTANTS
 */
#define APP_FLASH_DATA_ADDR          (0x08014800)                            // ����APP FLASH��3ҳ��ǰ1K
#define APP_FLASH_DATA_LEN           (STM_SECTOR_SIZE)                      // ����APP FLASH��3ҳ��ǰ1K

#define APP_CHECK_INFO_ADDR         (0x08014800 + STM_SECTOR_SIZE -32 )      // ����APP FLASH��3ҳ��ǰ1K
#define APP_REG_INFO_ADDR           (APP_CHECK_INFO_ADDR + 16)              // ����APP FLASH��3ҳ��ǰ1K
#define BOOT_CHECK_INFO_ADDR        0x08002000
#define APP_CTRL_ID_ADDR            (APP_CHECK_INFO_ADDR - 24)
#define APP_SENSOR_ID_ADDR          (APP_CHECK_INFO_ADDR - 48)

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
/***********************************************
* ������
*/
typedef union {
  uint8_t         buf1[12];
  uint16_t        buf2[6];
  uint32_t        buf3[3];
} UnionChipId;

/***********************************************
* ������
*/
#define CID_BUF_LEN         (8 * 3 + 2 + 1)
#define FLASH_BUF_LEN       (4 + 1 + 1)
#define DATE_BUF_LEN        (3 + 2 + 4 + 2 +1)
#define TIME_BUF_LEN        (2 + 2 + 2 + 2 +1)
#define VERSION_BUF_LEN     (9 +1)

typedef struct {
  UnionChipId   id;
  char          cid[CID_BUF_LEN];
  char          flash[FLASH_BUF_LEN];
  char          date[DATE_BUF_LEN];
  char          time[TIME_BUF_LEN];
  char          version[VERSION_BUF_LEN];
} StrChipInfo;

extern StrChipInfo  ChipInfo[3];
extern const u32    checkInfo[];


/*******************************************************************************
 * ������ �ⲿ��������
 */
uint8_t  BSP_GetChipID     (uint8_t devType, UnionChipId *pId );
uint8_t  BSP_GetChipInfo   (uint8_t devType, StrChipInfo *pInfo);

#endif
/*******************************************************************************
 * 				end of file
 *******************************************************************************/