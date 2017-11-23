/*******************************************************************************
 *   Filename:       bsp_filteralgorithm.c
 *   Revised:        All copyrights reserved to Roger.
 *   Date:           2015-05-11
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    �˲��㷨
 *
 *
 *   Notes:
 *
 *******************************************************************************/
#include "bsp_filteralgorithm.h"

//============================================================================//

/*******************************************************************************
* ��    �ƣ� App_ModbusQPost
* ��    �ܣ� ʱ��ƽ��ƽ���˲�
* ��ڲ����� filter������һ��ʱ���ڵ�datֵ   dat:��ǰ����ֵ
* ���ڲ����� ����ƽ��ƽ������֮���datֵ
* ���� ���ߣ� ������
* �������ڣ� 2015-05-30
* ��    �ģ�
* �޸����ڣ�
* ��    ע�� ȡһ��ʱ�������е�datֵ������ʱ������������ݵ�ƽ��ֵ
*            ���磺StrDataFilter    TempFilter = {8,0,{0}}; 8����ȡ���������ǰ���ٸ�ֵ����ƽ��
*******************************************************************************/
INT16U App_GetTimeMeanFiler(StrDataFilter *filter, INT16S dat)
{
    filter->Buf[filter->Idx]  = dat;

    if ( ++filter->Idx >= filter->Nbr ) {
        filter->Idx = 0;
    } else {

    }
    double  sum = 0.0;
    INT16U  nbr = 0;

    for ( int i = 0; i < filter->Nbr; i++ ) {
        if ( filter->Buf[i] > 0 ) {
            sum += filter->Buf[i];
            nbr++;
        }
    }
    if (nbr)
        sum /= nbr;
    else
        sum  = dat;

    return  (INT16U)sum;
}


/*******************************************************************************
* ��    �ƣ� App_BubbleAscendingArray
* ��    �ܣ� ð�ݷ���������
* ��ڲ����� buf�� ��Ҫ���������   bufsize:����Ĵ�С
* ���ڲ����� ����֮�������
* ���� ���ߣ� ������
* �������ڣ� 2015-05-30
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void App_BubbleAscendingArray(INT16U *needbuf,INT16U bufsize,INT16U *afterbuf)
{
    INT16U i, j;
    INT16U usTemp;

    for(i = 0; i < bufsize; i++) {
      afterbuf[i] = *(needbuf+i);
    }
    for(i = 0; i < bufsize - 1; i++) {
        for(j = i + 1; j < bufsize; j++) {
            if(afterbuf[i] > afterbuf[j]) {
                usTemp = afterbuf[i];
                afterbuf[i] = afterbuf[j];
                afterbuf[j] = usTemp;
            }
        }
    }
}

/*******************************************************************************
* ��    �ƣ� App_BubbleAscendingArray
* ��    �ܣ� ð�ݷ���������
* ��ڲ����� buf�� ��Ҫ���������   bufsize:����Ĵ�С
* ���ڲ����� ����֮�������
* ���� ���ߣ� ������
* �������ڣ� 2015-05-30
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
INT16U App_GetFilterValue(INT16U *needbuf, INT16U *afterbuf, INT16U bufsize, INT16U needsize)
{
    INT16U  i, j;
    INT16U  usTemp;          
    u16     cutsize     = 0;
    INT32U  sum         = 0;  
    u16     cnt         = 0;
    
    /***********************************************
    * ������ ����
    */
    if ( bufsize == 0 ) {
        return 0;
    }  
        
    if ( 0 == needsize ) {
        return 0;
    }

    /***********************************************
    * ������ ����ɾ��
    */
    if ( bufsize == needsize ) {
        for(u16 i = cutsize; i < bufsize - cutsize;i++) {
            if ( needbuf[i] != 0 ) {
                sum += needbuf[i];
                cnt++;
            }
        }
        
        return (INT16U)(sum/cnt);            // ����ƽ��ֵ
    }
    
    /***********************************************
    * ������ 
    */
    for(i = 0; i < bufsize; i++) {
      afterbuf[i] = *(needbuf+i);
    }
    
    /***********************************************
    * ������ ����
    */
    for(i = 0; i < bufsize - 1; i++) {
        for(j = i + 1; j < bufsize; j++) {
            if(afterbuf[i] > afterbuf[j]) {
                usTemp = afterbuf[i];
                afterbuf[i] = afterbuf[j];
                afterbuf[j] = usTemp;
            }
        }
    }    
    
    /***********************************************
    * ������ ��ȡɾ������
    */
    cutsize     = (bufsize - needsize) / 2;
    /***********************************************
    * ������ ���
    */
    for(u8 i = cutsize; i < bufsize - cutsize;i++) {
        sum += afterbuf[i];
        cnt++;
    }
    
    return (INT16U)(sum/(cnt));                     // ����ƽ��ֵ
}

/******************************************************************************/
//                              end of file                                   //
/******************************************************************************/