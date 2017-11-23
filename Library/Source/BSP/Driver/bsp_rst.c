/*******************************************************************************
 *   Filename:      bsp_rst.c
 *   Revised:       $Date: 2013-06-30
 *   Revision:      $V1.0
 *   Writer:        wumingshen.
 *
 *   Description:
 *
 *   Notes:         ϵͳ��������ģ��
 *
 *
 *   All copyrights reserved to wumingshen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

#define BSP_RST_MODULE_EN 1
#if BSP_RST_MODULE_EN > 0

/*******************************************************************************
 * ������ ��ģ���Ƿ����uCOSϵͳʹ��
 */
#define  UCOS_EN        DEF_ENABLED

//============================================================================//

/*******************************************************************************
* ��    �ƣ� BSP_SystemSoftReset
* ��    �ܣ� ϵͳ��������
* ��ڲ����� ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2014-08-18
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ��Cortex-M3Ȩ��ָ����ָ������SYSRESETREQ ����Ϊ��Ч������λ������ִ�и�λ���
*       ��������һ����ʱ���ڴ���ʱ�ڼ䣬��������Ȼ������Ӧ�ж����󡣵����ǵı���������Ҫ
*       �ô˴�ִ�е���Ϊֹ����Ҫ�����κ����������ˡ����ԣ�����ڷ�����λ����ǰ���Ȱ�
*       FAULTMASK ��λ ��
*******************************************************************************/
void  BSP_RST_SystemRst(void)
{
#if (UCOS_EN == ENABLE)
    #if OS_CRITICAL_METHOD == 3u               /* Allocate storage for CPU status register */
        OS_CPU_SR  cpu_sr = 0u;
        cpu_sr  = cpu_sr;
        cpu_sr  = 0u;
    #endif
    CPU_CRITICAL_ENTER();                      //�ر������ж�(cpu.h�ļ��ṩ�ú���)
#else
    __set_FAULTMASK(1);                        //�ر������ж�(core_cm3.h�ļ��ṩ�ú���)
#endif

    NVIC_SystemReset();                        // ������core_cm3.h�ļ��ṩ�ú�����

    /***************************************************
    * ������ ��ֹϵͳ���������������򿪿��Ź������ȴ�ϵͳ��λ
    */
    BSP_WDT_Init(BSP_WDT_MODE_ALL);
    while(1);
}

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif