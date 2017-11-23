/*******************************************************************************
 *   Filename:       bsp_info.c
 *   Revised:        $Date: 2015-08-24	 $
 *   Revision:       $V1.0
 *	 Writer:		 wumingshen.
 *
 *   Description:    ��ȡоƬΨһUIDģ������
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
 *   All copyrights reserved to wumingshen.
 *
 *******************************************************************************/

#define BSP_INFO_EN 1
#if BSP_INFO_EN > 0

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <bsp_info.h>
#include <app.h>


/*******************************************************************************
 * ������ оƬUID�洢����
 */
StrChipInfo ChipInfo[3];

//============================================================================//

/*******************************************************************************
* ��    �ƣ� BSP_GetChipID
* ��    �ܣ� ��ȡоƬ��UID
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� 96λ��ID��stm32Ψһ��ݱ�ʶ��������8bit��16bit��32bit��ȡ�ṩ�˴�˺�С�����ֱ�ʾ������
*            ID�Ŵ��������洢��ģ���ϵͳ�洢���򣬿���ͨ��JTAG/SWD����CPU��ȡ��
*            ����������оƬʶ����Ϣ�ڳ���ʱ��д���û��̼������ⲿ�豸���Զ�ȡ����ǩ����
*            �����Զ�ƥ�䲻ͬ���õ�STM32F10xxx΢��������
*            ��ƷΨһ����ݱ�ʶ�����ã�
*          �� ������Ϊ���к�(����USB�ַ����кŻ����������ն�Ӧ��)��
*          �� ������Ϊ���룬�ڱ�д����ʱ������Ψһ��ʶ������ӽ����㷨���ʹ�ã���ߴ���������洢���ڵİ�ȫ�ԣ�
*          �� �����������ȫ���Ƶ��Ծٹ��̣�
*            96λ�Ĳ�ƷΨһ��ݱ�ʶ���ṩ�Ĳο����������һ��STM32΢��������
*            ���κ�����¶���Ψһ�ġ��û��ں�������£��������޸������ݱ�ʶ��
*            �����û���ͬ���÷����������ֽ�(8λ)Ϊ��λ��ȡ��
*            Ҳ�����԰���(16λ)����ȫ��(32λ)��ȡ���ٺ٣�Ҫע����С��ģʽӴ~~~
*******************************************************************************/
uint8_t BSP_GetChipID( uint8_t devType, UnionChipId *pId )
{
    switch ( devType ) {
        case 0:
            pId->buf3[0] = *(vu32 *)(0X1FFFF7F0);            // ���ֽ�
            pId->buf3[1] = *(vu32 *)(0X1FFFF7EC);            //
            pId->buf3[2] = *(vu32 *)(0X1FFFF7E8);            // ���ֽ�
        return TRUE;
        break;
        case 1:
        case 2:
        break;
    }
    return FALSE;
}


/*******************************************************************************
* ��    �ƣ� BSP_GetChipInfo
* ��    �ܣ� ��ȡоƬ�����Ϣ
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t BSP_GetChipInfo(uint8_t devType, StrChipInfo *pInfo)
{
    //INT16U STM32_FLASH_SIZE;

    pInfo->cid[CID_BUF_LEN-1] = 0;
    pInfo->flash[FLASH_BUF_LEN-1] = 0;
    pInfo->date[DATE_BUF_LEN-1] = 0;
    pInfo->time[TIME_BUF_LEN-1] = 0;
    pInfo->version[VERSION_BUF_LEN-1] = 0;

    if ( !BSP_GetChipID(devType, &pInfo->id) ){
      return FALSE;
    }
    //STM32_FLASH_SIZE= *(INT16U*)(0x1FFFF7E0);           //���������Ĵ���
//    usprintf(pInfo->cid,        "%08X%08X%08X",         pInfo->id.buf3[0],
//                                                        pInfo->id.buf3[1],
//                                                        pInfo->id.buf3[2]);
//    usprintf(pInfo->flash,     "%dK",                   STM32_FLASH_SIZE);
//    usprintf(pInfo->date,      ""__DATE__"");
//    usprintf(pInfo->time,      ""__TIME__"");
//    usprintf(pInfo->version,   "V%d.%d.%d",             __STM32F10X_STDPERIPH_VERSION_MAIN,              // ���ʹ�ù̼���汾��
//                                                        __STM32F10X_STDPERIPH_VERSION_SUB1,
//                                                        __STM32F10X_STDPERIPH_VERSION_SUB2);
    pInfo->cid[CID_BUF_LEN-1] = 0;
    pInfo->flash[FLASH_BUF_LEN-1] = 0;
    pInfo->date[DATE_BUF_LEN-1] = 0;
    pInfo->time[TIME_BUF_LEN-1] = 0;
    pInfo->version[VERSION_BUF_LEN-1] = 0;

    return TRUE;
}
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif