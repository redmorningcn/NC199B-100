#ifndef	_IAP_pragram_mcu_h
#define	_IAP_pragram_mcu_h

/*******************************************************************************
 * TYPEDEFS
 */
typedef  struct   _stcIAPCtrl_
{
    char    buf[1024];      //������
    int     addr;           //���ݵ�ַ
}stcIAPCtrl;

typedef  struct   _stcIAPPara_
{
    uint16  hardver;        //Ӳ���汾
    uint16  softver;        //����汾
    uint32  softsize;       //�����С
    uint32  addr;           //��ǰ��ַ
    uint32  framenum;       //֡���
    uint16  code;           //ָ���� 01����ʾ����ȷ�Ŀ���
    uint16  crc16;
}stcIAPPara;


#endif