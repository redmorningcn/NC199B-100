/*******************************************************************************
 *   Filename:       bsp_dataconversion.h
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ��������ת������ ͷ�ļ�
 *
 *
 *
 *   Notes:
 *     				 E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/
#ifndef __BSP_DATACONVERSION_H__
#define __BSP_DATACONVERSION_H__

#include "includes.h"
#include "global.h"

/***********************************************
* ������ ��������
*/
BYTE Bcd2Hex(BYTE val);
BYTE Hex2Bcd(BYTE val);

u8 * UINTDatToStrings(u16 temp);
int ChartoInt(u8*chr,u8 lenth);


#endif