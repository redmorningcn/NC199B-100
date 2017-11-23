
#ifndef  _CrcCheck_h_
#define  _CrcCheck_h_

#include <global.h>

//----------------------------------------------------------------------------
// 名    称：   uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len)
// 功    能：   取CRC16校验和
// 入口参数：  
// 出口参数：   
//----------------------------------------------------------------------------
extern uint8_t GetCheckSum(uint8_t *Buf, uint32_t Len);

extern	uint16_t GetCrc16Check(uint8_t *Buf, uint16_t Len);
    
extern	uint16_t GetModBusCrc16(uint8_t *puchMsg,uint16_t  usDataLen);    

extern	uint16_t GetModBusCrc16Up(unsigned char *puchMsg,unsigned short  usDataLen);
#endif


