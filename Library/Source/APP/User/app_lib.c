/*******************************************************************************
 *   Filename:       app_lib.c
 *   Revised:        All copyrights reserved to Wuming Shen.
 *   Date:           2014-07-05
 *   Revision:       v1.0
 *   Writer:	     Wuming Shen.
 *
 *   Description:    ˫��ѡ�� wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� WDT �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ����������ȼ� �� �����ջ��С
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <app.h>
#include <app_lib.h>
#include <app_ctrl.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_lib__c = "$Id: $";
#endif

#define APP_TEMP_EN     DEF_ENABLED
#if APP_TEMP_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS            (OS_TICKS_PER_SEC * 1)

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
     
/*******************************************************************************
 * LOCAL VARIABLES
 */
/***********************************************
* ������ NTC���Ա�
*/

/*******************************************************************************
 * GLOBAL VARIABLES
 */
extern          stcCtrlDCB              Ctrl;

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

/*******************************************************************************/

/*******************************************************************************
* ��    �ƣ� SWAP_INT16U
* ��    �ܣ� ģ���htons ���� ntohs�����ϵͳ֧�ֽ�����Ŀ�ֱ���滻��ϵͳ����
* ��ڲ����� value
* ���ڲ����� ���Ĺ��ֽ����short��ֵ
* ������ �ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
INT16U SWAP_INT16U(INT16U value)                      
{
	int16_t		tmp_value;
	uint8_t		*index_1, *index_2;

	index_1 = (uint8_t *)&tmp_value;
	index_2 = (uint8_t *)&value;

	*index_1 = *(index_2+1);
	*(index_1+1) = *index_2;

	return tmp_value;
}
/*******************************************************************************
 * ��    �ƣ� App_SetParaToEep
 * ��    �ܣ�
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL App_SetParaToEep(INT16U addr, INT08U *pbuf, INT16U len)
{
    INT08U    retrys    = 3;
    INT08U    ret;
    INT08U    *pc;
    INT08U    *pb;
//    /***********************************************
//    * ������ ��Ĭ�ϲ���д��EEPROM��
//    */
//    do {
//        pb  = (INT08U   *)pbuf;
//        pc  = (INT08U   *)&EepRdBuf;
//        
//        WdtReset();
//        if ( len == BSP_EEP_WriteBytes( addr, pb, len ) ) {
//            if ( len == BSP_EEP_ReadBytes( addr, pc, len ) ) {
//                ret     = 1;
//                for ( INT16U i = 0; i < len; i++ ) {
//                    if ( *pb++ != *pc++ ) {
//                        ret = 0;
//                        break;
//                    }
//                }
//                if ( ret )
//                    break;
//            }
//        }
//    } while ( --retrys );
//    
//    if ( !ret )
//        return FALSE;
//    
    return TRUE;
}

/*******************************************************************************
 * ��    �ƣ� App_SetParaToEeprom()
 * ��    �ܣ� Ӧ�ò����洢��EERPOM
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ�
 * �������ڣ�
 * ��    �ģ� ������
 * �޸����ڣ� 2014-06-25
 *******************************************************************************/
BOOL App_SetParaToEeprom(void)
{
//#if (STORAGE_TYPE == INTNER_FLASH )
//    Ctrl.Para.dat.RestoreSetting    = 0xA5;
//    //BSP_FlashWrite((INT32U  ) PARAM_START_ADDR + 0,
//    //          (INT16U *)Ctrl.Para.buf2,128);
//    BSP_FlashWriteBytes((u32) 0*2, (u8 *)Ctrl.Para.buf1, (u16) 128*2);
//#elif (STORAGE_TYPE == INTNER_EEPROM )
//#elif (STORAGE_TYPE == EXTERN_EEPROM )
//    INT08U    ret;
//    INT08U   *pb;
//    
//    pb    = (INT08U       *)&EepWrBuf;
//    
//    /***********************************************
//    * ������ ��Ĭ�ϲ���д��EEPROM��
//    */
//    pb    = (INT08U   *)Ctrl.Para.buf1;
//    ret   = App_SetParaToEep( 0, pb, 255 );
//    
//    if ( !ret )
//        return FALSE;
//    
//    /***********************************************
//    * ������ ��ʼ�����ݱ�־д��EEPROM��
//    */
////    if(!BSP_EEP_WriteByteChk(255, 0xA5)) {
////        return FALSE;
////    }
//    
//#endif
//    return TRUE;
}


/*******************************************************************************
 * ��    �ƣ� App_SetParaToEeprom()
 * ��    �ܣ� Ӧ�ò����洢��EERPOM
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ�
 * �������ڣ�
 * ��    �ģ� ������
 * �޸����ڣ� 2014-06-25
 *******************************************************************************/
BOOL App_SetParaToSensor(INT08U dev, INT32U addr, INT08U *dat, INT16U len)
{    
//    INT08U   *pb    = dat;
//    /***********************************************
//    * ������ ��������
//    */
//    if ( dat == NULL ) { 
//        if ( dev == 2) { 
//            //len     = 256;
//            if ( addr < 256 ) {
//                addr    = 0;
//                pb    = (INT08U   *)Ctrl.Para.buf1;
//            } else if ( addr < 512 ) {
//                addr    = 256;
//                pb    = (INT08U   *)Ctrl.Tab.buf1;
//            }                
//        } else { 
//            addr    = 0;
//            //len     = 256;
//            Ctrl.Sen.Para.dat.ReadFlag    = 0xA5;
//            pb    = (INT08U   *)Ctrl.Sen.Para.buf1;
//        }
//    }
//    
//    /***********************************************
//    * ������ �����ڲ�FLASH��
//    */
//#if (STORAGE_TYPE == INTNER_FLASH )
//    BSP_FlashWriteBytes((u32) addr, (u8 *)pb, (u16) len);
//    /***********************************************
//    * ������ �����ڲ�EEPROM��
//    */
//#elif (STORAGE_TYPE == INTNER_EEPROM )
//    /***********************************************
//    * ������ �����ⲿEEPROM��
//    */
//#elif (STORAGE_TYPE == EXTERN_EEPROM )
//    
    INT08U    ret   = FALSE;
//    
//    BSP_I2CSetPort(dev);                              // �л���I2C1
//    
//    /***********************************************
//    * ������ ��Ĭ�ϲ���д��EEPROM��
//    */
//    ret   = App_SetParaToEep( addr, pb, len );
//    
//    if ( !ret ) {
//        goto exit;
//    }
//    
//    ret = TRUE;
//#endif
//exit:
//    BSP_I2CSetPort(2);                              // �л���I2C1
    return ret;
}

extern  const StrParaRange MbsParaRange[];
/*******************************************************************************
 * ��    �ƣ� App_GetParaFromEeprom()
 * ��    �ܣ� Ӧ�ò�����ȡ��RAM
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ�
 * �������ڣ�
 * ��    �ģ� ������
 * �޸����ڣ� 2014-06-25
 *******************************************************************************/
BOOL App_GetParaFromEeprom(void)
{    
//#if (STORAGE_TYPE == INTNER_FLASH )    
//    INT08U   *pb = (INT08U   *)&EepWrBuf;
//    //BSP_FlashRead((INT32U  ) PARAM_START_ADDR + 0,
//    //          (INT16U *)pu,127);
//    BSP_FlashReadBytes((u32) 0*2, (u8 *)pb, (u16) 127*2);
//#elif (STORAGE_TYPE == INTNER_EEPROM )
//#elif (STORAGE_TYPE == EXTERN_EEPROM )
//    INT08U   *pb = (INT08U   *)&EepWrBuf;
//    /***********************************************
//    * ������ �Ѿ����ù��ˣ����״γ�ʼ�������ȡ������RAM
//    */
//    /***********************************************
//    * ������ ��ȡMODBUS����
//    */
//    
//    INT08U  retrys  = 5;
//    INT16U  len = 254;
//    do {
//        WdtReset();
////        if ( len == BSP_EEP_ReadBytes(0,(INT08U *)pb, len) ) {            
////            for ( INT08U i = 0; i < len; i++, pb++ ) {
////                Ctrl.Para.buf1[i] = *pb;
////            }
////            return TRUE;
////            //break;
////        }
//#ifdef APP_LIBRARY
//        Delay_Nms( 10 );
//#else
//        BSP_OS_TimeDly(10);
//#endif
//    } while( --retrys );
//    
//    if ( !retrys )
//        return FALSE;
//#endif
//    /***********************************************
//    * ������ ��������������MODBUS����
//    */
//    StrParaRange *prag    = Ctrl.Prge;
//    
//#if ( PARA_TYPE == PARA_TYPE_INT16U )
//    INT16U  *pu = (INT16U*)pb;
//    for ( INT08U i = 0; i < 127; i++, pu++,prag++ ) {
//        if ( ( (INT16U)*pu >= prag->Min ) &&
//            ( (INT16U)*pu <= prag->Max ) ) {
//            Ctrl.Para.buf2[i] = *pu;
//        }
//    }
//#elif ( PARA_TYPE == PARA_TYPE_INT16S )
//    INT16S  *ps = (INT16S*)pb;
//    for ( INT08U i = 0; i < 127; i++, ps++,prag++ ) {
//        if ( ( (INT16S)*ps >= prag->Min ) &&
//            ( (INT16S)*ps <= prag->Max ) ) {
//            Ctrl.Para.buf2[i] = *ps;
//        }
//    }
//#elif ( PARA_TYPE == PARA_TYPE_INT08U )
//    for ( INT08U i = 0; i < 254; i++, pb++,prag++ ) {
//        if ( ( (INT08U)*pb >= prag->Min ) &&
//            ( (INT08U)*pb <= prag->Max ) ) {
//            Ctrl.Para.buf1[i] = *pb;
//        }
//    }
//#else
//    for ( INT08U i = 0; i < 254; i++, pb++,prag++ ) {
//        if ( ( (INT08S)*pb >= prag->Min ) &&
//            ( (INT08S)*pb <= prag->Max ) ) {
//            Ctrl.Para.buf1[i] = *pb;
//        }
//    }
//#endif
//    return TRUE;
}

/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ�
 * ��ڲ�����
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2014-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
BOOL App_PraInit(void)
{
    extern const StrCtrlPara    MbsParaAtrIdx;
    
//    Ctrl.Prge               = (StrParaRange *)MbsParaRange;
//    //Ctrl.Key                = (StrKeyEvt *)&Key;
//    Ctrl.Para.dat.idx       = &MbsParaAtrIdx;
//    BSP_I2CSetPort(1);
//#if (STORAGE_TYPE == INTNER_FLASH ) 
//    BSP_FlashReadBytes((u32) 0*2, (u8 *)EepWrBuf.buf1, (u16) 128*2);
//    INT16U dat = EepWrBuf.buf2[127];    
//#elif (STORAGE_TYPE == INTNER_EEPROM )
//#elif (STORAGE_TYPE == EXTERN_EEPROM )
//    INT08U  dat     = 0;
//    INT08U  retrys  = 10;
//    BSP_EEP_Init();
//    
//    //if ( FALSE == BSP_EEP_Init())
//    //    goto exit;                                 // ��ʼ���洢��
//    /***********************************************
//    * ��������ȡ��ʼ����ַ�ϵ�ֵ
//    */
//    do {
//        WdtReset();
//        if ( TRUE == BSP_EEP_ReadByte(255, &dat))
//            break;
//#ifdef APP_LIBRARY
//        Delay_Nms( 10 );
//#else
//        BSP_OS_TimeDly(10);
//#endif
//    } while( --retrys );
//#endif
//    WdtReset();
//    /***********************************************
//    * ��������Ĭ��ֵ�Ƚ�
//    */
//    if(dat == 0xA5) {
//        dat = App_GetParaFromEeprom();                  // �Ѿ���ʼ������ȡ����
//        Ctrl.Sys.FirstPwrFlag       = FALSE;
//        /***********************************************
//        * ������û�г�ʼ��������Ĭ��ֵ
//        */
//    } else { 
//        dat = App_SetParaToEeprom();                    // δ��ʼ��������Ĭ�ϲ���
//        Ctrl.Sys.FirstPwrFlag       = TRUE;
//    }
//           
//    WdtReset();
//    /***********************************************
//    * ������ �л�����ʾģ��洢��
//    */
//    //BSP_I2CSetPort(2);                              // �л���I2C1
//    return (BOOL)dat;
}

/*******************************************************************************
 * 				                    ��ʱ����                                   *
 *******************************************************************************/
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
OPTIMIZE_NONE void  Delay_Nus( INT32U  dly )
{
    /***********************************************
    * ��������ʱ����ʼ��
    *       72000000��ѭ��  = 6.9999999583333333333333333333333��
    *       һ��ѭ�� = 6.9999999583333333333333333333333 / 72000000
    *       = 97.222221643518518518518518518519 ns
    *       ~= 0.1 us
    *       ��ʱ1us  dly  = 9
    *       dly = 0ʱ��ѭ��һ��
    *
    while(dly--);
    *//***********************************************
    * ��������ʱ����ʼ��
    *       1000000��ѭ��  = 1027.7778611111111111111111111111ms
    *       ƽ��һ��ѭ�� = 1.02777786111111111111111111111us
    *       dly = 1ʱ��= 1.1111111111111111111111111111111us
    *       dly = 2ʱ  = 2.1388888888888888888888888888889us
    *       dly ÿ����1 ����-1.0277777777777777777777777777778us
    */
    INT32U  i;

    while(dly--) {
        for(i=0; i<7; i++);             // clk = 72000000 1027.7778611111111111111111111111
    }
}

/*******************************************************************************
* ��    �ƣ� Delay_Nms()
* ��    �ܣ� ��ʱ���1ms
* ��ڲ����� dly		��ʱ������ֵԽ����ʱԽ��
* ���ڲ����� ��
* ���� ���ߣ� ������
* �������ڣ� 2009-01-03
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
OPTIMIZE_NONE void  Delay_Nms( INT32U  dly )
{
#if UCOS_II_EN > 0
    OSTimeDly(dly);
#else
    CPU_INT32U  hclk_freq;
    CPU_INT32U  cnts;
    INT32U      i;

    hclk_freq   = BSP_CPU_ClkFreq();            // hclk_freq = 72000000ʱdly 1000 �ӽ�1��
    cnts        = hclk_freq / 9000;
    //WdtReset();

    for( ; dly > 0; dly-- ) {
        for(i=0; i<cnts; i++);
    }
#endif
}

/*******************************************************************************
* ��    �ƣ�Delay_Ns()
* ��    �ܣ���ʱ���1s
* ��ڲ�����dly		��ʱ������ֵԽ����ʱԽ��
* ���ڲ�������
* ���� ���ߣ�������
* �������ڣ�2009-01-03
* ��    �ģ�
* �޸����ڣ�
*******************************************************************************/
OPTIMIZE_NONE void  Delay_Ns( INT32U  dly )
{
#if UCOS_II_EN > 0
    OSTimeDlyHMSM(dly/3600,dly/60%60,dly%3600,0);
#else
    for( ; dly > 0; dly--) {
        //WdtReset();
        Delay_Nms( 950 );
    }
#endif
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif