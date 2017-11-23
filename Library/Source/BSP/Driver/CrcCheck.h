
#ifndef  _CrcCheck_h_
#define  _CrcCheck_h_

//----------------------------------------------------------------------------
// ��    �ƣ�   uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len)
// ��    �ܣ�   ȡCRC16У���
// ��ڲ�����  
// ���ڲ�����   
//----------------------------------------------------------------------------
uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len);

unsigned short GetModBusCrc16(unsigned char *puchMsg,unsigned short  usDataLen);

unsigned short GetModBusCrc16Up(unsigned char *puchMsg,unsigned short  usDataLen);

//----------------------------------------------------------------------------
// ��    �ƣ�   uint8_t GetCheckSum(uint8_t  Buf[], uint32_t Len)
// ��    ��:    ȡУ���
// ��ڲ�����   Buf��������   Len�����鳤��
// ���ڲ�����   У���
//----------------------------------------------------------------------------
uint8_t GetCheckSum(uint8_t  Buf[], uint32_t Len);

//----------------------------------------------------------------------------
// ��    �ƣ�   uint16_t GetCheckSum16(uint8_t  Buf[], uint32_t Len)
// ��    ��:    ȡУ���
// ��ڲ�����   Buf��������   Len�����鳤��
// ���ڲ�����   У���
//----------------------------------------------------------------------------
uint16_t GetCheckSum16(uint8_t  Buf[], uint32_t Len);

extern uint16_t crc16(uint8_t *ptr, uint8_t len);

#endif


