#ifndef  _Display_h_
#define  _Display_h_


/**-------------------------------------------------------
  * @������ LED_DIS_Config
  * @����   ��ʼ��LED�Ķ˿�
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
extern void LED_DIS_Config(void);

//-------------------------------------------------------------------
//��������:     Init7219 ()
//��    ��:     7219��ʼ��
//��ڲ���:     ��
//���ڲ���:     ��
//-------------------------------------------------------------------
extern void Init7219 (void);

//-----------------------------------------------------------------------------------
//��������:               DisplayNum ()
//��    ��:               ������ܽ�����������ʾ����
//��ڲ���:               number
//���ڲ���:               ��
//------------------------------------------------------------------------------------
extern void    DisplayNum (uint32_t Unmber);

//-----------------------------------------------------------------------------------
//��������:               DisplayNum_WithZero ()
//��    ��:               ������ܽ�����������ʾ����,��λ��0Ҳ��ʾ����
//��ڲ���:               number
//���ڲ���:               ��
//------------------------------------------------------------------------------------
extern void    DisplayNum_WithZero (uint32_t Unmber);

//-----------------------------------------------------------------------------
//��������:             MovDisplayChar ()
//��    ��:             ���������ʾ�ַ����Ѷ��ַ�
//��ڲ���:             character: �ַ�����
//���ڲ���:             ��
//��ʾ����:             -   E   H   L   P   Black
//���ݴ���:     	    A	B	C	D	E	F	  
//-----------------------------------------------------------------------------
extern void    MovDisplayChar (uint8_t Character);

//--------------------------------------------------------------------
//void          DisplayNumWithDoit (uint32 Unmber)
//��    ��:     ������ܽ�����������ʾ����
//��ڲ���:     number
//���ڲ���:     ��
//-------------------------------------------------------------------
extern void    DisplayNumWithDot (uint32_t Unmber);

//------------------------------------------------------
//��������:     DisplayString ()
//��    ��:     ���������ʾ�ַ�������
//��ڲ���:     String: �ַ�����
//���ڲ���:     ��
//��ʾ����:     String
//���ݴ���:	    ���ø�ʽ  DisplayString("ABCD");
//-------------------------------------------------------
extern void     DisplayString(char *String);
extern void     DisplayString_WithDot(char * String);

extern void     DisplayABC_2(uint_least8_t *String);

extern void     Dis_Test(void);

#endif

