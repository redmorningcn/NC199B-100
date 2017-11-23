#include    "stm32f10x.h"
#include	"I2C_CLK.h"
#include    <stdio.h>
#include    "DELAY.h"
#include	"FM24CL64.h"


#include  <global.h>
#include  <includes.h>


//�������
#define		FRAM_SIZE               8192						         	//1024*8�ռ�����
#define		FRAM_START_ADDR	        0x0000	                                //�������ʼ��ַ
#define		FRAM_DIVICE_ADDR        0xa0	                                //������豸��ַ
#define		FRAM_END_ADDR	      	(FRAM_START_ADDR + FRAM_SIZE)	

//#define		FRMA_PROTECT			P121
//
////------------------------------------------------------------------------
////  ��  �� ��void WriteSCL( uint8 temp )
////  ��  �� ������ SCL
//// ��ڲ�������
//// ���ڲ�������
////------------------------------------------------------------------------
//void WriteFRAM_PROTECT(uint8 temp)
//{
//    IO1DIR_OUT(FRMA_PROTECT);    
//    IO1PIN_W(temp,FRMA_PROTECT);
//}

#define  UCOS_EN            DEF_ENABLED

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
static uint8_t EEP_WaitEvent(void)
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
static void EEP_SendEvent(void)
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

void EEP_OS_Init(void)
{
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
}

////-------------------------------------------------------------------------------------------------------
////��������:         WriteFM24CL16()
////��    ��:         ��FM24CL16ָ����ַ����д����
////��ڲ���:         ADDR:       ������ַ    ��ַ��0 ~ sizeof(FM24CL16)
////                  *DataBuf:   ���ݻ���
////                  Len:        ���ݳ���
////���ڲ���:         ��
////˵����            
////--------------------------------------------------------------------------------------------------------
//uint8_t WriteFM24CL64(uint16_t  Addr, uint8_t *DataBuf, uint16_t DataBufLen)            
//{
//    uint32_t  i = 0;
//    uint8_t   SlaveAddr;                                      //�ӻ���ַ
//    uint16_t  AddrTemp = 0;                                   //��ַ����
//    
////    WriteFRAM_PROTECT(0);									//��д����
//    
//    EEP_WaitEvent();                                        // ���ź���
//    
//    AddrTemp = Addr;
//    AddrTemp += FRAM_START_ADDR;                            //�����ַ
//
//    if( (uint16_t)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
//    {
//        EEP_SendEvent();                                    // �ͷ��ź���
//        return  0;                                          //��ַδ����
//    }
//        
//    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //��FM24CL64�豸д��ַ
//    
//    I2C_Start();                                             //����IIC����
//    
//    I2C_SendByte(SlaveAddr);                            //дFM24CL64��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
// 
//    I2C_SendByte(Addr >> 8);                            //дFM24CL64��λ��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    
//    I2C_SendByte( Addr );                               //дFM24CL64��λ��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��     
//  
//    for(i = 0; i < DataBufLen; i++)                         //��������
//    {
//        I2C_SendByte(DataBuf[i]);                       //д����
//        while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    }
//    
//    I2C_Stop();                                              //ֹͣIIC����   
//    
//    EEP_SendEvent();                                    // �ͷ��ź���
//    return  1;                                           //��ȷ
//}
//
////-------------------------------------------------------------------------------------------------------
////��������:         ReadFM24CL16()
////��    ��:         ��ȡFM24CL16ָ����ַ��������
////��ڲ���:         ADDR:       ������ַ   ��ַ��0 ~ sizeof(FM24CL16)
////                  *DataBuf:   ���ݻ��� 
////                  Len:        ���ݳ���
////���ڲ���:         ��
////˵����            
////--------------------------------------------------------------------------------------------------------
//uint8_t ReadFM24CL64(uint16_t  Addr, uint8_t *DataBuf, uint16_t DataBufLen)            
//{
//    uint32_t  i = 0;
//    uint8_t   SlaveAddr;                                      //�ӻ���ַ
//    uint16_t  AddrTemp;                                       //��ַ����
//    
//    //	WriteFRAM_PROTECT(0);									//��д����
//    
//    EEP_WaitEvent();                                        // ���ź���
//    
//    AddrTemp = Addr;
//    
//    AddrTemp += FRAM_START_ADDR;                            //�����ַ
//    
//    if( (uint16_t)AddrTemp + DataBufLen > FRAM_END_ADDR ) 
//    {
//        EEP_SendEvent();                                // �ͷ��ź���
//        return  0;                                      //��ַδ����
//    }
//    
//    SlaveAddr = FRAM_DIVICE_ADDR & (~(0x01));               //��FM24CL64�豸д��ַ
//    
//    I2C_Start();                                             //����IIC����
//    
//    I2C_SendByte(SlaveAddr);                            //дFM24CL64�豸д��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    
//    I2C_SendByte(Addr >> 8 );                           //д���ݸ�λ��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    
//    I2C_SendByte( Addr );                               //д���ݵ�λ��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    
//    SlaveAddr = (FRAM_DIVICE_ADDR)|(0x01);                  //��FM24CL64�豸����ַ
//    
//    I2C_Start();                                             //����IIC����
//    
//    I2C_SendByte(SlaveAddr);                            //дFM24CL16��ַ
//	while(I2C_WaitAck());			  	   //�ȴ����豸Ӧ��
//    
//    for(i = 0; i < (DataBufLen -1); i++)                    //��������
//    {
//        DataBuf[i] = I2C_ReadByte();                     //д����
//        I2C_Ack();                                     //����Ӧ��ӻ�
//    }
//    DataBuf[i] = I2C_ReadByte();                         //д����
//    
//    I2C_NoAck();                                       //����Ӧ��ӻ�
//    
//    I2C_Stop();
//    
//    EEP_SendEvent();                                    // �ͷ��ź���
//    return  1;                                           //��ȷ
//}

void TO_Arayy(uint8_t *buf , uint32_t NUM)
{
	buf[0] = NUM & 0xFF;			  
	buf[1] = (NUM >> 8) & 0xFF;	   //���ֽ���ǰ�����ֽ��ں�
	buf[2] = (NUM >> 16) & 0xFF;
	buf[3] = (NUM >> 24) & 0xFF;
}



