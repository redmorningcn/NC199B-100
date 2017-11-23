/*******************************************************************************
 *   Filename:       bsp_dataconversion.h
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    ��������ת������ ͷ�ļ�
 *
 *
 *
 *   Notes:
 *     				 E-mail:261313062@qq.com
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