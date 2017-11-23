/*******************************************************************************
 *   Filename:       app_calc.c
 *   Revised:        All copyrights reserved to Wuming Shen.
 *   Date:           2015-12-09
 *   Revision:       v1.0
 *   Writer:	     Wuming Shen.
 *
 *   Description:    ˫��ѡ�� wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Wdt �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� WDT �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ����������ȼ� �� �����ջ��С
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <app.h>
#include <app_lib.h>
#include <app_ctrl.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_calc__c = "$Id: $";
#endif

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
     
/*******************************************************************************
 * LOCAL VARIABLES
 */
#define ADC_CODE_0         (ADCres * (Ioff - 1 * STEP) * Rs / Vref )
#define ADC_CODE_1         (ADCres * (Ioff + 0 * STEP) * Rs / Vref )
#define ADC_CODE_2         (ADCres * (Ioff + 1 * STEP) * Rs / Vref )
#define ADC_CODE_3         (ADCres * (Ioff + 2 * STEP) * Rs / Vref )
#define ADC_CODE_4         (ADCres * (Ioff + 3 * STEP) * Rs / Vref )
#define ADC_CODE_5         (ADCres * (Ioff + 4 * STEP) * Rs / Vref )
#define ADC_CODE_6         (ADCres * (Ioff + 5 * STEP) * Rs / Vref )
#define ADC_CODE_7         (ADCres )

/*******************************************************************************
 * GLOBAL VARIABLES
 */
StrAdcToPressure    ADC_TO_PRESSURE_TAB[ADC_TO_PRESSURE_TAB_LEN]   = { 
    {-2000 ,    ADC_CODE_0 },     //{2384,  -2000}
    { 0    ,    ADC_CODE_1 },     //{11916, 0},                                // 0Pa
    { 2000 ,    ADC_CODE_2 },     //{21448, 2000},                             // 2000Pa
    { 4000 ,    ADC_CODE_3 },     //{30981, 4000},                             // 4000Pa
    { 6000 ,    ADC_CODE_4 },     //{40513, 6000},                             // 6000Pa
    { 8000 ,    ADC_CODE_5 },     //{50046, 8000},                             // 8000Pa
    { 10000,    ADC_CODE_6 },     //{59578, 10000}                             // 10000Pa
    { 11000,    ADC_CODE_7 }      //{65535, 11000}                             // 10000Pa
};

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

 /*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_AdcToI
 * ��    �ܣ� ��ADCֵת����ѹǿֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-12-09
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float App_AdcToI(u16 adc, u8 ch)
{
    float         I       = 0.0;
    double   k,b;     
    /***********************************************
    * ������ ��������С��Ϊб�ʼ����
    */
    if ( (Ctrl.Para.dat.Check[ch].K == 0) || (Ctrl.Para.dat.Check[ch].B == 0) ) {
        k   = (double)(Ctrl.Para.dat.Check[ch].I2 - Ctrl.Para.dat.Check[ch].I1)
            / (double)(Ctrl.Para.dat.Check[ch].Adc2 - Ctrl.Para.dat.Check[ch].Adc1);
        b   = (double)(Ctrl.Para.dat.Check[ch].I1
            - k * (double)Ctrl.Para.dat.Check[ch].Adc1);
        Ctrl.Para.dat.Check[ch].K   = k;
        Ctrl.Para.dat.Check[ch].B   = b;
    }
    /***********************************************
    * ������ ��ȡб�ʣ����㵱ǰѹǿ
    */
    I       = (double)Ctrl.Para.dat.Check[ch].K * (double)adc 
            + (double)Ctrl.Para.dat.Check[ch].B;    // �Ŵ�1��
        
    return I;
}

/*******************************************************************************
 * ��    �ƣ� App_AdcToPressure
 * ��    �ܣ� ��ADCֵת����ѹǿֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-12-09
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float App_IToPressure(float I, u8 ch)
{
    float  p       = 0;
    
    INT08U  nbr     = (INT08U)Ctrl.Sen.Para.dat.PINbr;
    INT08U  iMax    = nbr - 1;
    /***********************************************
    * ������ ���ֶδ���
    */
    if ( ( nbr > 2 ) && ( nbr < 7  ) ) {
        static char i   = 1;
        /***********************************************
        * ������ ��ǰֵ <= ��Сֵ
        */
        if ( I <= Ctrl.Sen.Para.dat.PI[ch][0][1] ) {
            i   = 1;
        /***********************************************
        * ������ ��ǰֵ >= ���ֵ
        */
        } else if ( I >= Ctrl.Sen.Para.dat.PI[ch][iMax][1] ) {
            i   = iMax;
        /***********************************************
        * ������ ��Сֵ < ��ǰֵ < ���ֵ
        */
        } else { 
            /***********************************************
            * ������ ����Ѱ�ҵ�ֵ >= ��ǰֵ >= �ϴβ���ֵ
            */
            if ( I >= Ctrl.Sen.Para.dat.PI[ch][i][1] ) {
                while( I >= Ctrl.Sen.Para.dat.PI[ch][++i][1] );
            /***********************************************
            * ������ ����Ѱ�ҵ�ֵ <= ��ǰֵ <= �ϴβ���ֵ
            */
            } else if ( I <= Ctrl.Sen.Para.dat.PI[ch][i][1] ) {
                while( I <= Ctrl.Sen.Para.dat.PI[ch][--i][1] );
            }
        }
        /***********************************************
        * ������ ��ֹ�������
        */        
        if ( i < 1 ) {
            i   = 1;
        } else if ( i > iMax ) {
            i   = iMax;
        }
        
        /***********************************************
        * ������
        */ 
        double   k   = (double)(Ctrl.Sen.Para.dat.PI[ch][i][0] - Ctrl.Sen.Para.dat.PI[ch][i-1][0])
                      / (double)(Ctrl.Sen.Para.dat.PI[ch][i][1] - Ctrl.Sen.Para.dat.PI[ch][i-1][1]);
        double   b   = (double)(Ctrl.Sen.Para.dat.PI[ch][i-1][0] )
                      - k * (double)Ctrl.Sen.Para.dat.PI[ch][i-1][1];
        /***********************************************
        * ������ ��ȡб�ʣ����㵱ǰѹǿ
        */
        p       = (float)(k * I + b);
    } else {
        /***********************************************
        * ������ ��������С��Ϊб�ʼ����
        */
        if ( ( Ctrl.Para.dat.SenChk[ch].K == 0 ) || ( Ctrl.Para.dat.SenChk[ch].B == 0 ) ) {
            double   k   = (double)(Ctrl.Para.dat.SenChk[ch].P2 - Ctrl.Para.dat.SenChk[ch].P1)
                          / (double)(Ctrl.Para.dat.SenChk[ch].I2 - Ctrl.Para.dat.SenChk[ch].I1);
            double   b   = (double)(Ctrl.Para.dat.SenChk[ch].P1
                          - k * (double)Ctrl.Para.dat.SenChk[ch].I1);
            Ctrl.Para.dat.SenChk[ch].K   = (float)k;
            Ctrl.Para.dat.SenChk[ch].B   = (float)b;
        }
        /***********************************************
        * ������ ��ȡб�ʣ����㵱ǰѹǿ
        */
        p       = (float)(((double)Ctrl.Para.dat.SenChk[ch].K * (double)I 
                + (double)Ctrl.Para.dat.SenChk[ch].B));    // �Ŵ�1��    
    }
    
    return p;  
}

/*******************************************************************************
 * ��    �ƣ� App_AdcToPressure
 * ��    �ܣ� ��ADCֵת����ѹǿֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-12-09
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float App_AdcToPressure(u16 adc, float *I, u16 nbr, u8 ch)
{
    /***********************************************
    * ������ У׼�ɼ��壬��֤�ã�������ò�����ȥ��
    */
    if ( Ctrl.Para.dat.Sel.udat.CalcSel == 0 ) {
                    *I        = App_AdcToI(adc,ch);
        float      Pressure = App_IToPressure(*I,ch);
        return Pressure;
    } else {
                    *I        = App_AdcToI(adc,ch);
        /***********************************************
        * ������ 
        */
        float       p       = 0;
        double   k,b;
        /***********************************************
        * ������ ��������С��Ϊб�ʼ����
        */        
        //k   = (double)(Ctrl.Para.dat.Check[ch].P2 - Ctrl.Para.dat.Check[ch].P1)
        //    / (double)(Ctrl.Para.dat.Check[ch].Adc2 - Ctrl.Para.dat.Check[ch].Adc1);
        //b   = (double)(Ctrl.Para.dat.Check[ch].P1 )
        //    - (double)Ctrl.Para.dat.Check[ch].Adc1 * k;
        
        k   = (double)(10000 - 0)
            / (double)(59578 - 11916);
        b   = (double)(0 )
            - k * (double)11916;
        /***********************************************
        * ������ ��ȡб�ʣ����㵱ǰѹǿ
        */
        p       = (float)((k * (double)adc + b));    // �Ŵ�1��
            
        return p;  
    }
}

/*******************************************************************************
 * ��    �ƣ� App_AdcToPressure
 * ��    �ܣ� ��ADCֵת����ѹǿֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-12-09
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float App_HightToVolume(float h)
{
    float  v       = 0;
    
    INT08U  nbr     = (INT08U)Ctrl.VTab->Tab.Cnts;
    INT08U  iMax    = nbr - 1;
    /***********************************************
    * ������ ���ֶδ���
    */
    if ( ( nbr > 1 ) && ( nbr < 50  ) ) {
        char i   = 0;
        /***********************************************
        * ������ ��ǰֵ <= ��Сֵ
        */
        if ( h <= Ctrl.VTab->Tab.Point[0].Hight ) {
            i   = 0;
        /***********************************************
        * ������ ��ǰֵ >= ���ֵ
        */
        } else if ( h >= Ctrl.VTab->Tab.Point[iMax].Hight ) {
            i   = iMax;
        /***********************************************
        * ������ ��Сֵ < ��ǰֵ < ���ֵ
        */
        } else { 
            /***********************************************
            * ������ ����Ѱ�ҵ�ֵ > ��ǰֵ > �ϴβ���ֵ
            */
            if ( h >= Ctrl.VTab->Tab.Point[i].Hight ) {
                while( h >= Ctrl.VTab->Tab.Point[++i].Hight );
            /***********************************************
            * ������ ����Ѱ�ҵ�ֵ < ��ǰֵ < �ϴβ���ֵ
            */
            } else if ( h <= Ctrl.VTab->Tab.Point[i].Hight ) {
                while( h <= Ctrl.VTab->Tab.Point[--i].Hight );
            }
        }
        /***********************************************
        * ������ ��ֹ�������
        */        
        if ( i < 1 ) {
            i   = 1;
        } else if ( i > iMax ) {
            i   = iMax;
        }
        
        /***********************************************
        * ������ 
        */  
        double   k   = (double)(Ctrl.VTab->Tab.Point[i].Volume - Ctrl.VTab->Tab.Point[i-1].Volume)
                      / (double)(Ctrl.VTab->Tab.Point[i].Hight - Ctrl.VTab->Tab.Point[i-1].Hight);
        double   b   = (double)(Ctrl.VTab->Tab.Point[i-1].Volume )
                      - k * (double)Ctrl.VTab->Tab.Point[i-1].Hight;
        /***********************************************
        * ������ ��ȡб�ʣ����㵱ǰѹǿ
        */
        v       = (float)(k * h + b);
        if ( v < 0 )
            v   = 0;
    } else {
        v   = 0;
        /***********************************************
        * ������ ��������С��Ϊб�ʼ����
        *
        if ( Ctrl.Para.dat.SenChk[ch].K == 0 ) {
            double   k   = (double)(Ctrl.Para.dat.SenChk[ch].P2 - Ctrl.Para.dat.SenChk[ch].P1)
                / (double)(Ctrl.Para.dat.SenChk[ch].I2 - Ctrl.Para.dat.SenChk[ch].I1);
            double   b   = (double)(Ctrl.Para.dat.SenChk[ch].P1
                                    - k * (double)Ctrl.Para.dat.SenChk[ch].I1);
            Ctrl.Para.dat.SenChk[ch].K   = k;
            Ctrl.Para.dat.SenChk[ch].B   = b;
        }
        *//***********************************************
        * ������ ��ȡб�ʣ����㵱ǰѹǿ
        */
        //v       = (float)(((double)Ctrl.Para.dat.SenChk[ch].K * (double)I + (double)Ctrl.Para.dat.SenChk[ch].B));    // �Ŵ�1��    
    }
    
    return v;  
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
