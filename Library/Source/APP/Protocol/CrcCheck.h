
#ifndef  _CrcCheck_h_
#define  _CrcCheck_h_

#include <global.h>

//----------------------------------------------------------------------------
// ��    �ƣ�   uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len)
// ��    �ܣ�   ȡCRC16У���
// ��ڲ�����  
// ���ڲ�����   
//----------------------------------------------------------------------------
extern uint8_t GetCheckSum(uint8_t *Buf, uint32_t Len);

extern	uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len);
    
extern	uint16_t GetModBusCrc16(uint8_t *puchMsg,uint16_t  usDataLen);    

extern	uint16_t GetModBusCrc16Up(unsigned char *puchMsg,unsigned short  usDataLen);
#endif


