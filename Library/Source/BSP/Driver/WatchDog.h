#ifndef		__WATCHGDOG_H__
#define		__WATCHGDOG_H__


/**-------------------------------------------------------
  * @������ IO-PORT_Config
  * @����   ��ʼ��SPI_ISD�Ķ˿�
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
extern void WDG_Init(void);


extern void FeedDog_LOW(void);


extern void FeedDog_HIGH(void);

#endif

