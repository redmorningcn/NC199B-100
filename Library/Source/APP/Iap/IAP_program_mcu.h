#ifndef	_IAP_pragram_mcu_h
#define	_IAP_pragram_mcu_h

/*******************************************************************************
 * TYPEDEFS
 */
typedef  struct   _stcIAPCtrl_
{
    char    buf[1024];      //数据区
    int     addr;           //数据地址
}stcIAPCtrl;

typedef  struct   _stcIAPPara_
{
    uint16  hardver;        //硬件版本
    uint16  softver;        //软件版本
    uint32  softsize;       //软件大小
    uint32  addr;           //当前地址
    uint32  framenum;       //帧序号
    uint16  code;           //指令码 01，标示有正确的可用
    uint16  crc16;
}stcIAPPara;


#endif