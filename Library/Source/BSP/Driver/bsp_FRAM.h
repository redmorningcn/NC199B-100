/*******************************************************************************
 *   Filename:       bsp_FRAM.h
 *   Revised:        $Date: 2017-05-15$
 *   Revision:       $
 *   Writer:	     redmorningcn.
 *
 *   Description:
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/


#ifndef  _FRAM_h_
#define  _FRAM_h_


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
//#include <includes.h>
#include <global.h>
#include <bsp_IIC.h>
#include "App_ctrl.h"


#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif
    
/*******************************************************************************
 * CONSTANTS
 */
        

/*******************************************************************************
 * MACROS
 */
   //�������
#define		FRAM_SIZE               		8192		//1024*8�ռ�����
#define		FRAM_START_ADDR	        		0x0000	   	//�������ʼ��ַ
#define		FRAM_DIVICE_ADDR     			0xa0	     	//������豸��ַ
#define		FRAM_END_ADDR	      			(FRAM_START_ADDR + FRAM_SIZE)	        
														//����Ľ�����ַ
//�����洢��ַ
#define		FRAM_RECNUM_MGR_ADDR		    4			                                            //���ݼ�¼����洢��ַ
#define		FRAM_PRODUCT_INFO_ADDR		    (FRAM_RECNUM_MGR_ADDR   + sizeof(stcRecNumMgr))		//��ƷID��ַ
#define		FRAM_REC_ADDR				    (FRAM_PRODUCT_INFO_ADDR + sizeof(stcProductInfo))		//��ǰ���ݼ�¼�洢��ַ
#define		FRAM_AIR_PARA_ADDR			    (FRAM_REC_ADDR          + sizeof(stcFlshRec))			//�����������
#define		FRAM_RUN_PARA_ADDR			    (FRAM_AIR_PARA_ADDR     + sizeof(stcAirPara))			//ϵͳ���в���
//#define		FRAM_CALC_MODEL_ADDR		(FRAM_RUN_PARA_ADDR     + sizeof(stcRunPara))			//��������ģ��
//////�����洢��ַ
//#define		FRAM_RECNUM_MGR_ADDR		4			//���ݼ�¼����洢��ַ
//#define		FRAM_PRODUCT_INFO_ADDR		20			//��ƷID��ַ
//#define		FRAM_REC_ADDR				32			//��ǰ���ݼ�¼�洢��ַ
//#define		FRAM_OIL_PARA_ADDR			160			//�����������
//#define		FRAM_RUN_PARA_ADDR			172			//ϵͳ���в���
//#define		FRAM_CALC_MODEL_ADDR		180			//��������ģ��    
//    
    
#define		OIL_BOX_MODEL_ADDR			    2000		//����ģ�͵�ַ	

//�����ʶ
#define		FRAM_ERR_EVT					33			//�����쳣
 
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



//------------------------------------------------------------------------
//  ��  �� ��void void InitI2CIO(void)
//  ��  �� ������ SDA SCL Ϊ��ͨIO�� ����ģ�����߲���
// ��ڲ�������
// ���ڲ�������
//------------------------------------------------------------------------
extern	void InitI2CIO(void);

//-------------------------------------------------------------------------------------------------------
//��������:         WriteFM24CL16()
//��    ��:         ��FM24CL16ָ����ַ����д����
//��ڲ���:         ADDR:       ������ַ    ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ���
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 WriteFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ; 

//-------------------------------------------------------------------------------------------------------
//��������:         ReadFM24CL16()
//��    ��:         ��ȡFM24CL16ָ����ַ��������
//��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ��� 
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 ReadFM24CL64(uint16  Addr, uint8 *DataBuf, uint32 DataBufLen) ;

//-------------------------------------------------------------------------------------------------------
//��������:         TestFM24CL64()
//��    ��:         ��ȡFM24CL16ָ����ַ��������
//��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
//                  *DataBuf:   ���ݻ��� 
//                  Len:        ���ݳ���
//���ڲ���:         ��
//˵����            
//--------------------------------------------------------------------------------------------------------
extern	uint8 TestFM24CL64(void)  ;

uint8 FRAM_StoreRecNumMgr(stcRecNumMgr  *SRecNumMgr);
uint8 FRAM_ReadRecNumMgr(stcRecNumMgr  *SRecNumMgr);

uint8 FRAM_StoreProductInfo(stcProductInfo  *sProductInfo);
uint8 FRAM_ReadProductInfo(stcProductInfo  *sProductInfo);

uint8 FRAM_StoreRunPara(stcRunPara  *sRunPara);
uint8 FRAM_ReadRunPara(stcRunPara  *sRunPara);

uint8 FRAM_ReadCurRecord(stcFlshRec  *sFlshRec);

uint8 FRAM_ReadAirPara(stcAirPara  *sAir);


#endif
