/*******************************************************************************
 *   Filename:       app_calc.h
 *   Revised:        $Date: 2015-12-09$
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/
#ifndef	APP_CALC_H
#define	APP_CALC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
#endif
    
/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>

/*******************************************************************************
 * CONSTANTS
 */     
/***********************************************
* ������ ��λ
*/
#define Pa                  *1
#define mA                  *1
#define V                   *1
     
/***********************************************
* ������ �������ֶ���
*/
#define ADC_RES             65536.0
#define REF_VOL             3300.0
#define RES_SAMPLE          150.00
     
#define Rs                  RES_SAMPLE                  // ��������
#define Ioff               (4.0)                        // ������
#define Ifs                (20.0)                       // �����̵���
#define Voff               (Ioff * Rs)                  // ����ѹ
#define Vfs                (Ifs * Rs)                   // ��������ѹ
#define Vref               (REF_VOL)                    // �ο���ѹ
#define ADCres              ADC_RES                     // AD�ֱ���
#define ADCoff             (Voff / Vref * ADCres)       // ���ADֵ
#define ADCs               (Is * Rs / Vref * ADCres)    // �ɼ�ADֵ
#define Poff               (0.0Pa)                      // ���ѹǿ
#define Pfs                (10000.0Pa)                  // ������ѹǿ
#define P                  (ADCs/ADCres*Vref/Vfs*Pfs)   // ѹǿ
    
/***********************************************
* �������������ֶ���
*/
#define DIVS                5
#define STEP               ((Ifs-Ioff)/DIVS)
     
/***********************************************
* �������������ֶβ����
*/
#define ADC_TO_PRESSURE_TAB_LEN     (1+DIVS+2)
     
/*******************************************************************************
 * MACROS
 */
    
/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    float   Prassure;
    float   Adc;
} StrAdcToPressure;
    
extern StrAdcToPressure    ADC_TO_PRESSURE_TAB[];

typedef struct {
    float   I;
    float   Adc;
} StrAdcToI;
    
extern StrAdcToPressure    ADC_TO_PRESSURE_TAB[];
extern StrAdcToI           ADC_TO_I_TAB[];

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
 
/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *
 * ��������         ��������                ��ڲ���
 */
 float              App_AdcToPressure       (u16 adc, float *I, u16 nbr, u8 ch);
 float              App_HightToVolume       (float h);
 
/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif