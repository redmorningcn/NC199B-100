/*******************************************************************************
 *   Filename:       app_task_sen.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ˫��ѡ�� sen �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� Sen �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   ˫��ѡ�� SEN �� Ctrl + H, ��ѡ Match the case, Replace with
 *                   ������Ҫ�����֣���� Replace All
 *                   �� app_cfg.h ��ָ��������� ���ȼ�  �� APP_TASK_SEN_PRIO ��
 *                                            �� �����ջ�� APP_TASK_SEN_STK_SIZE ����С
 *                   �� app.h �������������     ���������� void  App_TaskSenCreate(void) ��
 *                                            �� ���Ź���־λ �� WDTFLAG_Sen ��
 *
 *   Notes:
 *     				E-mail: shenchangwei945@163.com
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#define  SNL_APP_SOURCE
#include <includes.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *app_task_sen__c = "$Id: $";
#endif

#define APP_TASK_SEN_EN     DEF_ENABLED
#if APP_TASK_SEN_EN == DEF_ENABLED
/*******************************************************************************
 * CONSTANTS
 */
#define CYCLE_TIME_TICKS            (OS_CFG_TICK_RATE_HZ * 2u)
#define CYCLE_SAMPLE_MSEC_TICKS     (OS_CFG_TICK_RATE_HZ / ADC_SAMPLE_CNTS_PER_SEC/2)
#define CYCLE_SAMPLE_SEC_TICKS      (OS_CFG_TICK_RATE_HZ * 5u)
#define CYCLE_SAMPLE_MIN_TICKS      (OS_CFG_TICK_RATE_HZ * 60u)
#define CYCLE_SAMPLE_TICKS          (OS_CFG_TICK_RATE_HZ * 2u)
#define CYCLE_UPDATE_DENSITY        (OS_CFG_TICK_RATE_HZ * 3u*60u)
#define CYCLE_UPDATE_TICKS          (OS_CFG_TICK_RATE_HZ * 30u)                 // �ܶȼ������

#define STOP_UPDATE_TICKS_CNT       (5 * 60 / (CYCLE_UPDATE_TICKS / OS_CFG_TICK_RATE_HZ))   // ͣ�����ʱ����

#define CYCLE_UPDATE_TICKS_CNT      (30 * 60 * CYCLE_UPDATE_TICKS / CYCLE_UPDATE_TICKS)     // ��ֹ���ʱ����

#define CYCLE_SEN_ERR_CHK           (OS_CFG_TICK_RATE_HZ * 2u)                  // ���������ϼ�⴫����
     
/***********************************************
* ������ �߶ȲΧ�޶�

��Ҫ�����������ٶȣ�m/s2��
����        9.788     ��/��^2��
�人        9.794     ��/��^2��
�Ϻ�        9.794     ��/��^2��
����        9.798     ��/��^2��
����        9.801     ��/��^2��
ŦԼ        9.803     ��/��^2��
Ī˹��      9.816     ��/��^2��
��������    9.832     ��/��^2��[6] 

��γ�Ⱥ�ƽ����������ٶȣ�m/s2��
γ��        �������ٶ�
0           9.78030
10          9.78186
20          9.78634
30          9.79321
40          9.80166
50          9.81066
60          9.81914
70          9.82606
80          9.83058
90          9.83218

��ͬ�߶ȵ��������ٶ� (m/s^2��
���� (km)   γ�ȣ��ȣ���	
            0       10      20      30      40      50      60      70      80      90��       
0           9.780   9.782   9.786   9.793   9.802   9.811   9.819   9.826   9.831   9.832��
4           9.768   9.770   9.774   9.781   9.789   9.798   9.807   9.814   9.818   9.820��
8           9.756   9.757   9.762   9.768   9.777   9.786   9.794   9.801   9.806   9.807��
12          9.743   9.745   9.749   9.756   9.765   9.774   9.782   9.789   9.794   9.795��
16          9.731   9.732   9.737   9.744   9.752   9.761   9.770   9.777   9.781   9.783��
20          9.719   9.720   9.725   9.732   9.740   9.749   9.757   9.764   9.769   9.770
��
ע����������߶Ȳ���,��ÿ��1km��g ����0.03%��
�������ٶ�g��ͬ��λ��֮��Ļ����ϵΪ���������ٶ�g = 9.81m/s^2;= 981cm/s^2; = 32.18ft/s^2;

ȫ���������������ٶȱ�
        ���                                      ����	   �����ٶ�       ��������ֵ
                                    g(m/s2)	                  g/1kg	       g/3kg	  g/6kg	     g/15kg	      g/30kg
*/
#define GRAVITY_01	                9.7986  	// ��ͷ	    -0.3981      -1.1943 	-2.3886    -11.9430 	-11.9430 
#define GRAVITY_02	                9.8015  	// ����	    -0.7045      -2.1135 	-4.2270    -10.5675 	-21.1350 
#define GRAVITY_03	                9.8048  	// ����	    -1.0413      -3.1239 	-6.2478    -15.6195 	-31.2390 
#define GRAVITY_04	                9.7915  	// ��ɳ	     0.3267       0.9801 	 1.9602 	 9.8010 	  9.8010 
#define GRAVITY_05	                9.7913  	// �ɶ�	     0.3267       0.9801 	 1.9602 	 4.9005 	  9.8010 
#define GRAVITY_06	                9.7914  	// ����	     0.3267       0.9801 	 1.9602 	 4.9005 	  9.8010 
#define GRAVITY_07	                9.8011  	// ����	    -0.6636      -1.9908 	-3.9816 	-9.9540 	-19.9080 
#define GRAVITY_08	                9.7833  	// ����	     0.6432    	  1.9296 	 3.8592 	 9.6480 	 19.2960 
#define GRAVITY_09	                9.7968  	// ����	     0.7963       2.3889 	 4.7778 	23.8890 	 23.8890 
#define GRAVITY_10	                9.8066  	// ������	-1.2251      -3.6753 	-7.3506    -18.3765 	-36.7530 
#define GRAVITY_11	                9.7936  	// ����   	 0.1020       0.3060 	 0.6120 	 1.5300 	  3.0600 
#define GRAVITY_12	                9.7863  	// ����   	 0.8474       2.5422 	 5.0844 	25.4220 	 25.4220 
#define GRAVITY_13	                9.7947  	// �Ϸ�   	 0.0204       0.0612 	 0.1224 	 0.3060 	  0.6120 
#define GRAVITY_14	                9.8048  	// ����   	-1.0413      -3.1239 	-6.2478    -15.6195 	-31.2390 
#define GRAVITY_15	                9.7988  	// ����   	-0.3981      -1.1943 	-2.3886 	-5.9715 	-11.9430 
#define GRAVITY_16	                9.7830  	// ����   	 1.1230       3.3690 	 6.7380 	16.8450 	 33.6900 
#define GRAVITY_17	                9.7799  	// ����   	 0.5513       1.6539 	 3.3078 	16.5390 	 16.5390 
#define GRAVITY_18	                9.7920  	// �ϲ�   	 0.2654       0.7962 	 1.5924 	 7.9620 	  7.9620 
#define GRAVITY_19	                9.7949  	// �Ͼ�   	-0.0306      -0.0918 	-0.1836 	-0.4590 	  0.9180 
#define GRAVITY_20	                9.7877  	// ����    	 0.7044       2.1132 	 4.2264 	10.5660 	 21.1320 
#define GRAVITY_21	                9.7985  	// �ൺ   	-0.3981      -1.1943 	-2.3886 	-5.9715 	-11.9430 
#define GRAVITY_22	                9.7964  	// �Ϻ�   	 0.0000       0.0000 	 0.0000 	 0.0000 	  0.0000 
#define GRAVITY_23	                9.8035  	// ����   	-0.9086      -2.7258 	-5.4516    -13.6290 	-27.2580 
#define GRAVITY_24	                9.7997  	// ʯ��ׯ	-0.5513      -1.6539 	-3.3078 	-8.2695 	-16.5390 
#define GRAVITY_25	                9.7970  	// ̫ԭ   	-0.2450      -0.7350 	-1.4700 	-3.6750 	 -7.3500 
#define GRAVITY_26	                9.8011  	// ���   	-0.6636      -1.9908 	-3.9816 	-9.9540 	-19.9080 
#define GRAVITY_27	                9.7936  	// �人   	 0.1020       0.3060 	 0.6120 	 1.5300 	  3.0600 
#define GRAVITY_28	                9.8015  	// ��³ľ�� -0.7248      -2.1744 	-4.3488    -21.7440 	-21.7440 
#define GRAVITY_29	                9.7944  	// ����   	 0.0204       0.0612 	 0.1224 	 0.3060 	  0.6120 
#define GRAVITY_30	                9.7911  	// ����   	 0.3267       0.9801 	 1.9602 	 9.8010 	  9.8010 
#define GRAVITY_31	                9.8000  	// �żҿ�	-0.5513      -1.6539 	-3.3078 	-8.2695 	-16.5390 
#define GRAVITY_32	                9.7966  	// ֣��   	-0.2041      -0.6123 	-1.2246 	-3.0615 	 -6.1230 


#define GRAVITY_CHANGSHA            GRAVITY_04  // ��ɳ
#define GRAVITY_WUHAN               GRAVITY_27  // �人
#define GRAVITY_CHENGDU             GRAVITY_05  // �ɶ�
#define GRAVITY_JINAN               GRAVITY_15  // ����
#define GRAVITY_BEIJING             GRAVITY_02  // ����
#define GRAVITY_SHENYANG            GRAVITY_23  // ����

#define GRAVITY_MIN                 9.768
#define GRAVITY_MAX                 9.832
//#define GRAVITY_DEF                 GRAVITY_CHANGSHA  // ��ɳ
//#define GRAVITY_DEF                 GRAVITY_WUHAN     // �人
//#define GRAVITY_DEF                 GRAVITY_CHENGDU   // �ɶ�
#define GRAVITY_DEF                 GRAVITY_JINAN     // ����
//#define GRAVITY_DEF                 GRAVITY_BEIJING   // ����
//#define GRAVITY_DEF                 GRAVITY_SHENYANG  // ����

/***********************************************
* ������ �����ܶȷ�Χ�޶�
*
�����ܶ� �༭
ͨ��������͵��ܶȷ�ΧΪ 0.83��0.855����ͬ�ͺŵ��ܶȲ�ͬ���磺
  0#�����ܶ� 0.835
+10#�����ܶ� 0.85
+20#�����ܶ� 0.87
-10#�����ܶ� 0.84
-20#�����ܶ� 0.83
-30#�����ܶ� 0.82
-35#�����ܶ� 0.82

ͨ�������ܶ��� 0.84 ���㣬
����һ�ֲ��ʹ�Լ�ۺ�1190����
������ �����ܶ� �ܶȷ�Χ 0.82��0.86
��    �� ���� 
Ӧ    �� ���� �ѻ�  �߻��ѻ��������ѻ�����ճ�ѻ�  
̼ԭ���� 10-22
*/
#define OIL_DENSITY_MIN             0.8200
#define OIL_DENSITY_MAX             0.8600
#define OIL_DENSITY_DEF             0.8500

/***********************************************
* ������ ˮ�ܶȷ�Χ�޶�
*/
#define WARTER_DENSITY_MIN          0.9500
#define WARTER_DENSITY_MAX          1.1000
#define WARTER_DENSITY_DEF          1.0000

/***********************************************
* ������ �߶ȲΧ�޶�
*/
#define DIFF_HIGHT_MIN              750
#define DIFF_HIGHT_MAX              850
#define DIFF_HIGHT_DEF              800

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

#if ( OSAL_EN == DEF_ENABLED )
#else
/***********************************************
* ������ ������ƿ飨TCB��
*/
static  OS_TCB   AppTaskSenTCB;

/***********************************************
* ������ �����ջ��STACKS��
*/
static  CPU_STK  AppTaskSenStk[ APP_TASK_SEN_STK_SIZE ];

#endif
/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if ( OSAL_EN == DEF_ENABLED )
#else
static  void    AppTaskSen           (void *p_arg);
#endif

float           App_fParaFilter     (float para, float def, float min, float max);
long            App_lParaFilter     (long para, long def, long min, long max);

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************/

/*******************************************************************************
 * ��    �ƣ� App_TaskSenCreate
 * ��    �ܣ� **���񴴽�
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� ���񴴽�������Ҫ��app.h�ļ�������
 *******************************************************************************/
void  App_TaskSenCreate(void)
{

#if ( OSAL_EN == DEF_ENABLED )
#else
    OS_ERR  err;

    /***********************************************
    * ������ ���񴴽�
    */
    OSTaskCreate((OS_TCB     *)&AppTaskSenTCB,                  // ������ƿ�  ����ǰ�ļ��ж��壩
                 (CPU_CHAR   *)"App Task Sen",                  // ��������
                 (OS_TASK_PTR ) AppTaskSen,                     // ������ָ�루��ǰ�ļ��ж��壩
                 (void       *) 0,                              // ����������
                 (OS_PRIO     ) APP_TASK_SEN_PRIO,              // �������ȼ�����ͬ�������ȼ�������ͬ��0 < ���ȼ� < OS_CFG_PRIO_MAX - 2��app_cfg.h�ж��壩
                 (CPU_STK    *)&AppTaskSenStk[0],               // ����ջ��
                 (CPU_STK_SIZE) APP_TASK_SEN_STK_SIZE / 10,     // ����ջ�������ֵ
                 (CPU_STK_SIZE) APP_TASK_SEN_STK_SIZE,          // ����ջ��С��CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)����app_cfg.h�ж��壩
                 (OS_MSG_QTY  ) 5u,                             // ���Է��͸�����������Ϣ��������
                 (OS_TICK     ) 0u,                             // ��ͬ���ȼ��������ѭʱ�䣨ms����0ΪĬ��
                 (void       *) 0,                              // ��һ��ָ����������һ��TCB��չ�û��ṩ�Ĵ洢��λ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK |           // �����ջ��������
                                OS_OPT_TASK_STK_CLR),           // ��������ʱ��ջ����
                 (OS_ERR     *)&err);                           // ָ���������ָ�룬���ڴ����������
#endif
}

/*******************************************************************************
 * ��    �ƣ� AppTaskSen
 * ��    �ܣ� ��������
 * ��ڲ����� p_arg - �����񴴽���������
 * ���ڲ����� ��
 * ���� ���ߣ� wumingshen.
 * �������ڣ� 2015-02-05
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/

#if ( OSAL_EN == DEF_ENABLED )
osalEvt  TaskSenEvtProcess(INT8U task_id, osalEvt task_event)
#else
static  void  AppTaskSen (void *p_arg)
#endif
{
    OS_ERR      err;
    
    /***********************************************
    * ������ �������Ź���־��λ
    */
    OS_FlagPost ((OS_FLAG_GRP *)&WdtFlagGRP,
                 (OS_FLAGS     ) WDT_FLAG_SEN,
                 (OS_OPT       ) OS_OPT_POST_FLAG_SET,
                 (CPU_TS       ) 0,
                 (OS_ERR      *) &err);    
  
    /***************************************************************************
    *                               ���ݲɼ�
    ***************************************************************************/
    
    /***************************************************************************
    * ������ һ�������˲���100MS/2����һ��
    */
    if( task_event & OS_EVT_SEN_MSEC ) {
        static u8   step    = 0;
        static u8   ctrL    = 0;
        static u8   ctrH    = 0;
        
        static u16  ctr1   = 0;
        static u16  ctr2   = 0;
        
        /***********************************************
        * ������ �ɼ��ߵ�ֵ
        */
        if ( step )  {
            __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
            double  AdcValue    = (double)BSP_ADCGetValue( 2 );
                        
            if ( pSen->AdcValue != 0 ) {
                pSen->AdcValue           = (u16)((double)pSen->AdcValue * 0.500 + (double)AdcValue * 0.500);
            } else {
                pSen->AdcValue           = (u16)AdcValue;
            }
            
            pSen->AdcFilter[ctrH] = pSen->AdcValue; 
            if ( ++ctrH >= ADC_SAMPLE_CNTS_PER_SEC ) {
                ctrH    = 0;
                /***********************************************
                * ������ ����������¼�
                */
                osal_set_event( OS_TASK_ID_SEN, OS_EVT_SEN_SEC);
            }
            
            Ctrl.Sen.Filter[0][ctr2]   = pSen->AdcValue;
            
            if ( ++ctr1 >= ADC_SAMPLE_CNT ) {
                ctr1   = 0;
                if ( ++ctr2 >= ADC_SAMPLE_CNTS_DEEPTH ) {
                    ctr2   = 0;
                }
            }
            /***********************************************
            * ������ �ɼ��͵�ֵ
            */
        } else {
            __StrSen    *pSen   = &Ctrl.Sen.L;              // ָ��͵㴫����
            double  AdcValue     = (double)BSP_ADCGetValue( 3 );
            pSen->AdcValue      = (u16)((double)pSen->AdcValue * 0.500 + (double)AdcValue * 0.5000);
            pSen->AdcFilter[ctrL] = pSen->AdcValue; 
            if ( ++ctrL >= ADC_SAMPLE_CNTS_PER_SEC ) {
                ctrL    = 0;  
            }
            
            Ctrl.Sen.Filter[1][ctr2]   = pSen->AdcValue;
        }
        
        step    = ~step;                                    // �л��ɼ�ͨ��
        /***********************************************
        * ������ 
        */
        
        return ( task_event ^ OS_EVT_SEN_MSEC );
    }
    
    /***************************************************************************
    * ������ �����˲���1�����˲�һ��
    */
    if( task_event & OS_EVT_SEN_SEC ) {
        static u8   ctr = 0;
        u16         *After[ADC_SAMPLE_CNTS_PER_SEC];
        __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
        
        for ( u8 sen = 0; sen < 2; sen++, pSen++ ) {
            /***********************************************
            * ������ �ߵ�λ����������
            */ 
            double  AdcValue    = 
                (double)App_GetFilterValue( (u16 *)&pSen->AdcFilter[0], 
                                           (u16 *)&After[0], 
                                           (u16  ) ADC_SAMPLE_CNTS_PER_SEC, 
                                           (u16  ) ADC_SAMPLE_CNTS_PER_SEC - 4);
            if ( pSen->AdcValueSec != 0 )
                pSen->AdcValueSec           = (u16)((double)pSen->AdcValueSec * 0.500 + (double)AdcValue * 0.500);
            else
                pSen->AdcValueSec           = (u16)AdcValue;
            pSen->AdcFilterSec[ctr]  = pSen->AdcValueSec;
        }
        
        if ( ++ctr  >= ADC_SAMPLE_SECS ) {
            ctr = 0;
            /***********************************************
            * ������ ������������¼�
            */
            osal_set_event( OS_TASK_ID_SEN, OS_EVT_SEN_MIN);                
        }  
        
        return ( task_event ^ OS_EVT_SEN_SEC );
    }
    
    /***************************************************************************
    * ������ ���ļ��˲���1�����˲�һ��
    */
    if( task_event & OS_EVT_SEN_MIN ) {
        u16         *After[ADC_SAMPLE_SECS];
        __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
        
#define AVER_TIME   30
#if AVER_TIME > ADC_SAMPLE_CNTS_MAX
#undef AVER_TIME
#define AVER_TIME   ADC_SAMPLE_CNTS_MAX
#endif
        
        /***********************************************
        * ������ �ߵ�λ����������
        */ 
        for( char sen = 0; sen < 2; sen++, pSen++ ) {
            u32 sum     = 0;
            u16 cnt     = 0;
            double  AdcValue    = 
                (double)App_GetFilterValue( (u16 *)&pSen->AdcFilterSec[0], 
                                            (u16 *)&After[0], 
                                            (u16  ) ADC_SAMPLE_SECS, 
                                            (u16  ) ADC_SAMPLE_SECS - 0);
            if ( pSen->AdcValueMin != 0 )
                pSen->AdcValueMin           = (u16)((double)pSen->AdcValueMin * 0.500 + (double)AdcValue * 0.500);
            else
                pSen->AdcValueMin           = (u16)AdcValue;
            // ����ǰֵд�뻺����
            pSen->AdcFilterMin[pSen->Head]  = (u16)AdcValue;
            /***********************************************
            * ������ ͷָ����ǰ�棨������δ����
            */
            if ( pSen->Head > pSen->Tail ) {
                for ( int i  = pSen->Tail; 
                     ( i <= pSen->Head ) && ( cnt <= AVER_TIME ); 
                     i++, cnt++ ) {
                    sum    += pSen->AdcFilterMin[i];
                }
                pSen->AdcValueAll = sum / cnt; 
                /***********************************************
                * ������ ͷָ���ں��棨������������
                */
            } else if ( pSen->Head < pSen->Tail ) {
                for ( int i = 0; 
                    i < AVER_TIME; 
                    i++, cnt++ ) {
                    sum    += pSen->AdcFilterMin[i];
                }
                pSen->AdcValueAll = sum / cnt; 
                /***********************************************
                * ������ ͷβָ����ȣ�û����
                */
            } else {
                pSen->AdcValueAll = 0;
            }
            
            if ( ++pSen->Head >= ADC_SAMPLE_CNTS_MAX ) {
                pSen->Head = 0; 
                if ( pSen->Tail >= pSen->Head )
                    pSen->Tail++; 
                
                if ( pSen->Tail == 0 )
                    pSen->Tail++;
            } else {
                if ( pSen->Tail >= pSen->Head )
                    pSen->Tail++; 
            }
            
            if ( pSen->Tail >= ADC_SAMPLE_CNTS_MAX )
                pSen->Tail = 0;
        }            
        
        return ( task_event ^ OS_EVT_SEN_MIN );
    }
    
    /***************************************************************************
    * ������ ����˲���1�����˲�һ��
    */
    if( task_event & OS_EVT_SEN_DEAL ) {
        __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
        
        for ( u8 i  = 0; i < 2; i++, pSen++ ) {
            double  AdcValue  = 
           (double) App_GetFilterValue( (u16 *)&Ctrl.Sen.Filter[i][0], 
                                        NULL, 
                                       (u16  ) ADC_SAMPLE_CNTS_DEEPTH, 
                                       (u16  ) ADC_SAMPLE_CNTS_DEEPTH - 0);            
                pSen->AdcValueSample     = (u16  )AdcValue;
        }
        return ( task_event ^ OS_EVT_SEN_DEAL );
    }
    
    /***************************************************************************
    *                               ���ݼ���
    ***************************************************************************/
    
    /***************************************************************************
    * ������ ���ݼ���
    */
    if( task_event & OS_EVT_SEN_TICKS ) {
        //osal_set_event( OS_TASK_ID_SEN, OS_EVT_SEN_MIN);
        /***********************************************
        * ������ ��ֵ����
        *        ����������
        *        �������裺 150R/1%/25ppm
        *        4~20MA     FS
        *        < 4MA      ���ϣ���·
        *        > 20MA     ���ϣ���·��
        *        4MA        = 0KP
        *        20MA       = 10KP
        *        �궨ѹ���� 0��   2��  4��  6��  8��  10
        *        �궨ADC��  
        * 
        *        Rs     = 150/1%/25ppm                  // ��������
        *        Ioff   = 4mA;                          // ������
        *        Ifs    = 20mA;                         // �����̵���
        *        Voff   = Ioff * Rs;                    // ����ѹ
        *        Vfs    = Ifs * Rs;                     // ��������ѹ
        *        Vref   = 3.3V;                         // �ο���ѹ
        *        ADCres = 65536;                        // AD�ֱ���
        *        ADCoff = Voff / Vref * ADCres;         // ���ADֵ
        *        ADCs   = Is * Rs / Vref * ADCres;      // �ɼ�ADֵ
        *        Poff   = 0Pa;                          // ���ѹǿ
        *        Pfs    = 10000Pa;                      // ������ѹǿ
        *        P      = ADCs/ADCres*Vref/Vfs*Pfs;     // ѹǿ
        *        h      = P/��g;                        // �߶�       
        *      ��h      = hL - hH;                      // �߶Ȳ�            
        *        y      = kx + b;                       // y:�߶ȣ�xADCֵ��k:б�ʣ�b:ƫ��  
        *        x1     = (ADCoff) 
        *        y1     = (Poff)      
        *        x2     = (ADCfs)  
        *        y2     = (Pfs)    
        *        k      = (y2 - y1)/(x2 - x1))
        *        b      = (y1 - k * x1)
        *        y      = kx + b; 
        */ 
        __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
        
        /***********************************************
        * ������ 1��������ѹǿ�硢��ѹ����
        */
        for ( u8 sen = 0; sen < 2; sen++, pSen++ ) {
            /***********************************************
            * ������ ������ѹǿ���� 4~20mA��0~10000pa
            */
            pSen->Pressure              = App_AdcToPressure( pSen->AdcValue, 
                                                            (float *)&(pSen->Current), 
                                                            ADC_TO_PRESSURE_TAB_LEN, sen );
            pSen->PressureSec           = App_AdcToPressure( pSen->AdcValueSec, 
                                                            (float *)&(pSen->CurrentSec), 
                                                            ADC_TO_PRESSURE_TAB_LEN, sen );
            pSen->PressureMin           = App_AdcToPressure( pSen->AdcValueMin, 
                                                            (float *)&(pSen->CurrentMin), 
                                                            ADC_TO_PRESSURE_TAB_LEN, sen );
            pSen->PressureAll           = App_AdcToPressure( pSen->AdcValueAll, 
                                                            (float *)&(pSen->CurrentAll), 
                                                            ADC_TO_PRESSURE_TAB_LEN, sen );
            pSen->PressureSample        = App_AdcToPressure( pSen->AdcValueSample, 
                                                            (float *)&(pSen->CurrentSample), 
                                                            ADC_TO_PRESSURE_TAB_LEN, sen );
            
            /***********************************************
            * ������ ��ѹ����
            */
            pSen->Voltage               = (INT16S)(REF_VOL * (double)pSen->AdcValue        / ADC_RES);
            pSen->VoltageSec            = (INT16S)(REF_VOL * (double)pSen->AdcValueSec     / ADC_RES);
            pSen->VoltageMin            = (INT16S)(REF_VOL * (double)pSen->AdcValueMin     / ADC_RES);
            pSen->VoltageAll            = (INT16S)(REF_VOL * (double)pSen->AdcValueAll     / ADC_RES);
            pSen->VoltageSample         = (INT16S)(REF_VOL * (double)pSen->AdcValueSample  / ADC_RES);
        }
        
        /***********************************************
        * ������ ѹ�����
        */
        Ctrl.Sen.C.DiffPressure         = Ctrl.Sen.L.Pressure         - Ctrl.Sen.H.Pressure;
        Ctrl.Sen.C.DiffPressureSec      = Ctrl.Sen.L.PressureSec      - Ctrl.Sen.H.PressureSec;
        Ctrl.Sen.C.DiffPressureMin      = Ctrl.Sen.L.PressureMin      - Ctrl.Sen.H.PressureMin;
        Ctrl.Sen.C.DiffPressureAll      = Ctrl.Sen.L.PressureAll      - Ctrl.Sen.H.PressureAll;
        Ctrl.Sen.C.DiffPressureSample   = Ctrl.Sen.L.PressureSample   - Ctrl.Sen.H.PressureSample;
        
        /***********************************************
        * ������ �ܶ�ѡ��
        */
        static float LastDensity    = 0.0;
        if ( LastDensity == 0.0 )
            LastDensity = Ctrl.Para.dat.LastDensity;
        
        switch ( Ctrl.Para.dat.Sel.udat.DensitySel ) {
        case 0:                                         // ѡ�������ܶ�
            {
            float   temp    = Ctrl.Para.dat.LastDensity - LastDensity;
            if ( temp != 0 ) {
                temp *= 0.8;
                if ( temp > 0.002 )
                    temp    = 0.002;
                else if ( temp < -0.002 )
                    temp    = -0.002;
                
                Ctrl.Sen.C.Density          = Ctrl.Para.dat.SetDensity + temp;
            } else {
                Ctrl.Sen.C.Density          = Ctrl.Para.dat.SetDensity;
            }
            }
            break;
        case 1:                                         // ѡ������ܶ�
            Ctrl.Sen.C.Density          = Ctrl.Para.dat.LastDensity;
            break;
        default:
            Ctrl.Sen.C.Density          = Ctrl.Para.dat.SetDensity;
            break;
        }
        
        /***********************************************
        * ������ ��������ѡ��
        */
        switch ( Ctrl.Para.dat.Sel.udat.GravitySel ) {
        case 0:                                         // ѡ�������������ٶ�
            Ctrl.Sen.C.Gravity          = Ctrl.Para.dat.SetGravity;
            break;
        case 1:                                         // ѡ��û���������ٶ�
            Ctrl.Sen.C.Gravity          = Ctrl.Para.dat.LastGravity;
            break;
        default:
            Ctrl.Sen.C.Gravity          = Ctrl.Para.dat.SetGravity;
            break;
        }
        
        /***********************************************
        * ������ 2���ܶȼ��� 0.xxxx��/�������ף�10000����
        *        h1      = P1/(d*g);
        *        h2      = P2/(d*g);
        *        h1 - h2 = (P1 - P2)/(d*g);
        *        dh      = Ctrl.Sen.C.CalcDiffHight;
        *        P1      = Ctrl.Sen.L.Pressure;
        *        P2      = Ctrl.Sen.H.Pressure;
        */
        Ctrl.Sen.C.CalcDensity              = (float)(10.000 * (double)(Ctrl.Sen.C.DiffPressure)      / (double)((double)Ctrl.Sen.Para.dat.ChkedDiffHight * Ctrl.Sen.C.Gravity));
        Ctrl.Sen.C.CalcDensitySec           = (float)(10.000 * (double)(Ctrl.Sen.C.DiffPressureSec)   / (double)((double)Ctrl.Sen.Para.dat.ChkedDiffHight * Ctrl.Sen.C.Gravity));
        Ctrl.Sen.C.CalcDensityMin           = (float)(10.000 * (double)(Ctrl.Sen.C.DiffPressureMin)   / (double)((double)Ctrl.Sen.Para.dat.ChkedDiffHight * Ctrl.Sen.C.Gravity));
        Ctrl.Sen.C.CalcDensityAll           = (float)(10.000 * (double)(Ctrl.Sen.C.DiffPressureAll)   / (double)((double)Ctrl.Sen.Para.dat.ChkedDiffHight * Ctrl.Sen.C.Gravity));            
        Ctrl.Sen.C.CalcDensitySample        = (float)(10.000 * (double)(Ctrl.Sen.C.DiffPressureSample)/ (double)((double)Ctrl.Sen.Para.dat.ChkedDiffHight * Ctrl.Sen.C.Gravity));            
        
        /***********************************************
        * ������ 3���߶ȼ��� xxx.x���ף�10����
        *        h = P/��g; 
        */
        pSen                            = &Ctrl.Sen.H;              // ָ��ߵ㴫����
        
        for ( u8 sen = 0; sen < 2; sen++, pSen++ ) {
            pSen->Hight                 = (INT16S)((double)pSen->Pressure       * 10.0 / ( (double)Ctrl.Sen.C.Density * Ctrl.Sen.C.Gravity ) + Ctrl.Sen.C.OffsetHight);
            pSen->HightSec              = (INT16S)((double)pSen->PressureSec    * 10.0 / ( (double)Ctrl.Sen.C.Density * Ctrl.Sen.C.Gravity ) + Ctrl.Sen.C.OffsetHight);
            pSen->HightMin              = (INT16S)((double)pSen->PressureMin    * 10.0 / ( (double)Ctrl.Sen.C.Density * Ctrl.Sen.C.Gravity ) + Ctrl.Sen.C.OffsetHight);
            pSen->HightAll              = (INT16S)((double)pSen->PressureAll    * 10.0 / ( (double)Ctrl.Sen.C.Density * Ctrl.Sen.C.Gravity ) + Ctrl.Sen.C.OffsetHight);
            pSen->HightSample           = (INT16S)((double)pSen->PressureSample * 10.0 / ( (double)Ctrl.Sen.C.Density * Ctrl.Sen.C.Gravity ) + Ctrl.Sen.C.OffsetHight);
        }
        
        Ctrl.Sen.C.CalcHight            = (INT16S)((double)Ctrl.Sen.L.Pressure        * (double)Ctrl.Sen.Para.dat.ChkedDiffHight / (double)( Ctrl.Sen.C.DiffPressure       ) + Ctrl.Sen.C.OffsetHight);
        Ctrl.Sen.C.CalcHightSec         = (INT16S)((double)Ctrl.Sen.L.PressureSec     * (double)Ctrl.Sen.Para.dat.ChkedDiffHight / (double)( Ctrl.Sen.C.DiffPressureSec    ) + Ctrl.Sen.C.OffsetHight);
        Ctrl.Sen.C.CalcHightMin         = (INT16S)((double)Ctrl.Sen.L.PressureMin     * (double)Ctrl.Sen.Para.dat.ChkedDiffHight / (double)( Ctrl.Sen.C.DiffPressureMin    ) + Ctrl.Sen.C.OffsetHight);
        Ctrl.Sen.C.CalcHightAll         = (INT16S)((double)Ctrl.Sen.L.PressureAll     * (double)Ctrl.Sen.Para.dat.ChkedDiffHight / (double)( Ctrl.Sen.C.DiffPressureAll    ) + Ctrl.Sen.C.OffsetHight);
        Ctrl.Sen.C.CalcHightSample      = (INT16S)((double)Ctrl.Sen.L.PressureSample  * (double)Ctrl.Sen.Para.dat.ChkedDiffHight / (double)( Ctrl.Sen.C.DiffPressureSample ) + Ctrl.Sen.C.OffsetHight);
       
        Ctrl.Sen.C.AverHight            = (INT16S)(((float)Ctrl.Sen.L.Hight       + (float)Ctrl.Sen.H.Hight       + Ctrl.Sen.Para.dat.ChkedDiffHight) / 2);
        Ctrl.Sen.C.AverHightSec         = (INT16S)(((float)Ctrl.Sen.L.HightSec    + (float)Ctrl.Sen.H.HightSec    + Ctrl.Sen.Para.dat.ChkedDiffHight) / 2);
        Ctrl.Sen.C.AverHightMin         = (INT16S)(((float)Ctrl.Sen.L.HightMin    + (float)Ctrl.Sen.H.HightMin    + Ctrl.Sen.Para.dat.ChkedDiffHight) / 2);
        Ctrl.Sen.C.AverHightAll         = (INT16S)(((float)Ctrl.Sen.L.HightAll    + (float)Ctrl.Sen.H.HightAll    + Ctrl.Sen.Para.dat.ChkedDiffHight) / 2);
        Ctrl.Sen.C.AverHightSample      = (INT16S)(((float)Ctrl.Sen.L.HightSample + (float)Ctrl.Sen.H.HightSample + Ctrl.Sen.Para.dat.ChkedDiffHight) / 2);
        
           
        INT16S  *ph;
        
        if ( Ctrl.Para.dat.Sel.udat.FilterSel > 4 )
            Ctrl.Para.dat.Sel.udat.FilterSel = 0;
        
        INT08U  SenSel  = Ctrl.Para.dat.Sel.udat.SensorSel;
        /***********************************************
        * ������ ������ѡ��:0�͡�1�ߡ�2ƽ����3����
        */
        again:
        switch ( SenSel ) {            
        /***********************************************
        * ������ ������ѡ��0��
        */
        case 0:
            ph                          = (INT16S *)(&Ctrl.Sen.L.HightSample - Ctrl.Para.dat.Sel.udat.FilterSel);
            Ctrl.Sen.C.Hight            = *ph;
            break;
        /***********************************************
        * ������ ������ѡ��1��
        */
        case 1:
            ph                          = (INT16S *)(&Ctrl.Sen.H.HightSample - Ctrl.Para.dat.Sel.udat.FilterSel);
            Ctrl.Sen.C.Hight            = (INT16S)(*ph + Ctrl.Sen.Para.dat.ChkedDiffHight);
            break;
        /***********************************************
        * ������ ������ѡ��2ƽ��
        */
        case 2:
            ph                          = (INT16S *)(&Ctrl.Sen.C.AverHightSample - Ctrl.Para.dat.Sel.udat.FilterSel);
            Ctrl.Sen.C.Hight            = *ph;
            break;
        /***********************************************
        * ������ ������ѡ��3����
        */
        case 3:
            /***********************************************
            * ������ ��λ�������й���
            */
            if (Ctrl.Para.dat.SenSts.Udat.Err.HSenBrokenErr || 
                Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection ||
                Ctrl.Para.dat.SenSts.Udat.Err.HSenShortErr ) {
                /***********************************************
                * ������ ��λ�������й���
                */
                if (Ctrl.Para.dat.SenSts.Udat.Err.LSenBrokenErr || 
                    Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection ||
                    Ctrl.Para.dat.SenSts.Udat.Err.LSenShortErr ) {
                   //ph          = NULL;
                /***********************************************
                * ������ ��λ�������޹��ϣ�ѡ���λ������
                */
                } else {
                    SenSel      = 0;
                    goto again;
                }
            /***********************************************
            * ������ ��λ�������й���
            */
            } else if (Ctrl.Para.dat.SenSts.Udat.Err.LSenBrokenErr || 
                Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection ||
                Ctrl.Para.dat.SenSts.Udat.Err.LSenShortErr ) {
                if (Ctrl.Para.dat.SenSts.Udat.Err.HSenBrokenErr || 
                    Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection ||
                    Ctrl.Para.dat.SenSts.Udat.Err.HSenShortErr ) {
                   //ph          = NULL;
                /***********************************************
                * ������ ��λ�������޹��ϣ�ѡ���λ������
                */
                } else {
                    SenSel      = 1;
                    goto again;
                }
            /***********************************************
            * ������ �����������޹���
            */
            } else {
                /***********************************************
                * ������ �ж������Ƿ�����
                */
                //if (  ) {
                
                /***********************************************
                * ������ ����������ѡ��ƽ��
                */
                //} else {
                if ( abs( (int)(Ctrl.Sen.C.CalcDiffHight - Ctrl.Sen.Para.dat.ChkedDiffHight)) > 100 ) {
                    SenSel  = 0;
                } else {
                    SenSel  = 2;
                }
                    goto again;
                //}
            }
            
            Ctrl.Sen.C.Hight            = 0;
            break;
        /***********************************************
        * ������ ������ѡ��:0������ֵѡ���
        */
        default: 
            Ctrl.Para.dat.Sel.udat.SensorSel    = 0;
            break;
        }
        
        /***********************************************
        * ������ 4���߶Ȳ���� 0~1000x0.1mm(���������߶Ȳ�
        *        ��d = dL - dH
        */
        Ctrl.Sen.C.CalcDiffHight        = (INT16S)(100 * Ctrl.Sen.C.DiffPressure       / Ctrl.Sen.Para.dat.TestDensity   / Ctrl.Sen.Para.dat.TestGravity);//Ctrl.Sen.L.Hight      - Ctrl.Sen.H.Hight;
        Ctrl.Sen.C.CalcDiffHightSec     = (INT16S)(100 * Ctrl.Sen.C.DiffPressureSec    / Ctrl.Sen.Para.dat.TestDensity   / Ctrl.Sen.Para.dat.TestGravity);//Ctrl.Sen.L.HightSec   - Ctrl.Sen.H.HightSec;
        Ctrl.Sen.C.CalcDiffHightMin     = (INT16S)(100 * Ctrl.Sen.C.DiffPressureMin    / Ctrl.Sen.Para.dat.TestDensity   / Ctrl.Sen.Para.dat.TestGravity);//Ctrl.Sen.L.HightMin   - Ctrl.Sen.H.HightMin;
        Ctrl.Sen.C.CalcDiffHightAll     = (INT16S)(100 * Ctrl.Sen.C.DiffPressureAll    / Ctrl.Sen.Para.dat.TestDensity   / Ctrl.Sen.Para.dat.TestGravity);//Ctrl.Sen.L.HightAll   - Ctrl.Sen.H.HightAll;
        Ctrl.Sen.C.CalcDiffHightSample  = (INT16S)(100 * Ctrl.Sen.C.DiffPressureSample / Ctrl.Sen.Para.dat.TestDensity   / Ctrl.Sen.Para.dat.TestGravity);//Ctrl.Sen.L.HightSample- Ctrl.Sen.H.HightSample;
        
        /***********************************************
        * ������ 5��������� 
        *        V = h*s
        */
        Ctrl.Sen.C.Volume   = App_HightToVolume(Ctrl.Sen.C.Hight) ;
        
        /***********************************************
        * ������ 6���������� 
        *        m = ��V
        */
        Ctrl.Sen.C.Weight   = Ctrl.Sen.C.Density * Ctrl.Sen.C.Volume;
        if ( Ctrl.Sen.C.Weight < 0 )
            Ctrl.Sen.C.Weight   = 0;
        
        /***********************************************
        * ������  
        *        Kp = ��gh/��P
        */
        Ctrl.Sen.C.Kp       = Ctrl.Sen.Para.dat.TestDensity
                            * Ctrl.Sen.Para.dat.TestGravity
                            * Ctrl.Sen.Para.dat.ChkedDiffHight
                            / Ctrl.Sen.C.DiffPressureSample
                            / 10.0;
        
        /***********************************************
        * ������ �ٶȲ�Ϊ��ʱ��ֹͣ�����ܶȶ�ʱ��
        */
        return ( task_event ^ OS_EVT_SEN_TICKS );
    }
    
    /***************************************************************************
    *                               �ܶȲ���
    ***************************************************************************/
    
    /***************************************************************************
    * ������ �����ܶ�ֵ
    */
    if( task_event & OS_EVT_SEN_UPDATE_DENSITY ) {
        /***********************************************
        * ������ �ܶȻ�ȡ״̬��
        *        0 �ϵ������
        *        1 ͣ��
        *        2 ͣ��->����
        *        3 ����
        *        4 ����->ͣ��
        *        5 ͣ��
        */
        switch ( Ctrl.Sen.DensityStep ) {
        /***********************************************
        * ������ �ϵ������
        */
        case 0:
            if ( Ctrl.Comm.Rd.Data.Speed == 0 ) {           // �ϵ���ٶ�Ϊ��
                Ctrl.Sen.DensitySts        |= 1 << 0;       // ����һ���ܶ�ֵ
                Ctrl.Sen.DensityStep        = 1;            // ת��30���Ӽ��
            } else {                                        // �ϵ���ٶȲ�Ϊ��
                if ( Ctrl.Para.dat.LastDensity == OIL_DENSITY_DEF )
                    Ctrl.Sen.DensitySts    |= 1 << 0;       // ����һ���ܶ�ֵ
                else
                    Ctrl.Sen.DensitySts     = 0;            // �򲻸����ܶ�
                Ctrl.Sen.DensityStep        = 2;            // ת��ͣ��5���Ӽ��
            }
            // ����5�����ض�ʱ��
            osal_start_timerRl( OS_TASK_ID_SEN, OS_EVT_SEN_UPDATE_DENSITY , CYCLE_UPDATE_TICKS);
            break;
        /***********************************************
        * ������ ͣ����30���Ӽ��
        */
        case 1:
            if ( Ctrl.Comm.Rd.Data.Speed == 0 ) {           // ͣ��������ٶ�Ϊ�㣬��û�п���
                if ( ++Ctrl.Sen.DensityCtr > CYCLE_UPDATE_TICKS_CNT ) { // ��30���Ӻ�
                    Ctrl.Sen.DensityCtr = 0;                // ���30���Ӽ����������¼���
                    Ctrl.Sen.DensitySts|= 1 << 0;           // ����һ���ܶ�ֵ
                }
            } else {                                        // ���ͣ������������������ٶ�
                Ctrl.Sen.DensityCtr     = 0;                // ���5���Ӽ�����
                Ctrl.Sen.DensityStep    = 2;                // ת��5����ͣ�����
            }
            break;
        /***********************************************
        * ������ ͣ����5���Ӽ��
        */
        case 2:
            if ( Ctrl.Comm.Rd.Data.Speed == 0 ) {           // ���ͣ��
                if ( ++Ctrl.Sen.DensityCtr > STOP_UPDATE_TICKS_CNT ) {         // ��ʱ5���ӵ�
                    Ctrl.Sen.DensityCtr     = 0;            // ���������
                    Ctrl.Sen.DensitySts    |= 1 << 0;       // ����һ���ܶ�ֵ
                    Ctrl.Sen.DensityStep    = 1;            // ת��ͣ��30���Ӽ��
                }
            } else {                                        // �������������
                Ctrl.Sen.DensityCtr = 0;                    // ���������
            }
            break;
        /***********************************************
        * ������ 
        */
        default:
            break;
        }
        
        /***********************************************
        * ������ Һλ���,���ݲ�ͬ��Һλ�����ܶ�
        */
        static char     flag    = 0; 
        float  Pressure         = Ctrl.Sen.L.PressureSample;
#define DIFF_P  250
        if ( Pressure < 1000 - DIFF_P  ) {
            if ( flag != 1 ) {
                flag    = 1;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 1000 + DIFF_P ) && ( Pressure < 2000 - DIFF_P ) ) {
            if ( flag != 2 ) {
                flag    = 2;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 2000 + DIFF_P ) && ( Pressure < 3000 - DIFF_P ) ) {
            if ( flag != 3 ) {
                flag    = 3;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 3000 + DIFF_P ) && ( Pressure < 4000 - DIFF_P ) ) {
            if ( flag != 4 ) {
                flag    = 4;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 4000 + DIFF_P ) && ( Pressure < 5000 - DIFF_P ) ) {
            if ( flag != 5 ) {
                flag    = 5;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 5000 + DIFF_P ) && ( Pressure < 6000 - DIFF_P ) ) {
            if ( flag != 6 ) {
                flag    = 6;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 6000 + DIFF_P ) && ( Pressure < 7000 - DIFF_P ) ) {
            if ( flag != 7 ) {
                flag    = 7;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 7000 + DIFF_P ) && ( Pressure < 8000 - DIFF_P ) ) {
            if ( flag != 8 ) {
                flag    = 8;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 8000 + DIFF_P ) && ( Pressure < 9000 - DIFF_P ) ) {
            if ( flag != 9 ) {
                flag    = 9;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( ( Pressure  > 9000 + DIFF_P ) && ( Pressure < 10000 - DIFF_P ) ) {
            if ( flag != 10 ) {
                flag    = 10;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } else if ( Pressure  > 10050  ) {
            if ( flag != 11 ) {
                flag    = 11;
                Ctrl.Sen.DensitySts |= 1 << 1;
            }
        } 
        
        static float   LastPressure     = 0;
        static float   LastDensity      = 0.0;
        
        if ( LastDensity == 0.0 )
            LastDensity = Ctrl.Sen.C.CalcDensity;
        
        if ( LastPressure == 0 )  {
            LastPressure    = Ctrl.Sen.L.PressureSec;
            goto next;
        }
        /***********************************************
        * ������ ����һ���ܶ�ֵ���������Ա��´��������ϵ���
        */
        if ( ( Ctrl.Sen.DensitySts ) &&                                         // ���ܶȸ����¼�
             ( fabs( LastPressure - Ctrl.Sen.L.PressureSec ) < 3 ) &&           // ѹ���ı仯���ʲ��ܳ���10Pas
             ( Ctrl.Sen.H.PressureMin > 500 ) &&                                // ��λ��������ѹǿ > 100Pa
             ( !Ctrl.Para.dat.SenSts.Udat.Test ) ) {                            // ���ڲ���ģʽ
            /***********************************************
            * ������ ���㵱ǰ�ܶ�
            */ 
            float   CalcDensity   = 0.0; 
            INT08U   nbr           = 0;
            
            if ( ( (Ctrl.Sen.C.CalcDensityMin > OIL_DENSITY_MIN) && (Ctrl.Sen.C.CalcDensityMin  < OIL_DENSITY_MAX) ) || 
                 ( (Ctrl.Sen.C.CalcDensityMin > WARTER_DENSITY_MIN) && (Ctrl.Sen.C.CalcDensityMin  < WARTER_DENSITY_MAX) ) ) {
                CalcDensity +=  Ctrl.Sen.C.CalcDensityMin;
                nbr++;
            }
            
            if ( ( (Ctrl.Sen.C.CalcDensityAll > OIL_DENSITY_MIN) && (Ctrl.Sen.C.CalcDensityAll  < OIL_DENSITY_MAX) ) || 
                 ( (Ctrl.Sen.C.CalcDensityAll > WARTER_DENSITY_MIN) && (Ctrl.Sen.C.CalcDensityAll  < WARTER_DENSITY_MAX) ) ) {
                CalcDensity +=  Ctrl.Sen.C.CalcDensityAll;
                nbr++;
            }
            
            CalcDensity /= nbr;
            
            if ( ( (CalcDensity > OIL_DENSITY_MIN) && (CalcDensity  < OIL_DENSITY_MAX) ) || 
                 ( (CalcDensity > WARTER_DENSITY_MIN) && (CalcDensity  < WARTER_DENSITY_MAX) ) ) {
                /***********************************************
                * ������ 
                */ 
                nbr     = (INT08U)Ctrl.Sen.Para.dat.DiffPNbr;
                /***********************************************
                * ������ У׼��ʽΪ1��2��ʱ��Ӷβ���
                */ 
                if ( (( Ctrl.Sen.Para.dat.SenSts.udat.ChkType == 1 ) ||
                      ( Ctrl.Sen.Para.dat.SenSts.udat.ChkType == 2 ) ) && 
                     ( ( nbr > 1 ) && ( nbr <= 10 ) ) ) {
                       
                    INT08U    i       = (INT08U)(Ctrl.Sen.L.PressureMin/Ctrl.Sen.C.CalcDensity/(1000*(10/nbr)) );
                    i                 = ( i > 9 ) ? 9 : i;
                    float     k       = Ctrl.Sen.Para.dat.DiffP[i];
                      
                    /***********************************************
                    * ������ ���ϵ���Ƿ�Ϸ������Ϸ��򲻳�ϵ��
                    */ 
                    if ( ( k > 0.9 ) && ( k < 1.1 ) )
                        CalcDensity       *= k; 
                }
                
                /***********************************************
                * ������ ��⵱ǰ�ܶȵı仯��Χ���ϴ��ܶȱ仯̫��ʱ�����²���һ��
                */ 
                if ( fabs(LastDensity - CalcDensity) < 0.003 ) {
                    /***********************************************
                    * ������ �������״̬λ
                    */
                    Ctrl.Sen.DensitySts = 0;
                    /***********************************************
                    * ������ 
                    */ 
                    Ctrl.Para.dat.LastDensity  = CalcDensity;
                    /***********************************************
                    * ������ ���浱ǰ32λ�ܶ�ֵ
                    */ 
                    BSP_EEP_WriteBytes(2, (INT08U *)&Ctrl.Para.buf1[2],4);
                }
                
                LastDensity = CalcDensity;
            } else {
                /***********************************************
                * ������ 
                */ 
                Ctrl.Sen.DensityCtr     = 0;                    // ���5���Ӽ�����
                Ctrl.Sen.DensityStep    = 2;                    // ת��5����ͣ�����
            }
        }
    next:
        /***********************************************
        * ������ �����ϴ�ѹ��ֵ
        */ 
        LastPressure    = Ctrl.Sen.L.PressureSec;
        
        return ( task_event ^ OS_EVT_SEN_UPDATE_DENSITY );
    }
    
    /***************************************************************************
    *                               ���ϼ��
    ***************************************************************************/
    
    /***************************************************************************
    * ������ ���������ϼ�⣺2�����˲�һ��
    */
    if( task_event & OS_EVT_SEN_ERR_CHK ) {
        __StrSen    *pSen   = &Ctrl.Sen.H;              // ָ��ߵ㴫����
         
        if ( pSen->CurrentSec < 0.1 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.HSenBrokenErr         = TRUE;
            Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection     = FALSE;
        } else if ( pSen->Hight < 20 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection     = TRUE;
        } else if ( pSen->CurrentSec >= 22.0 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.HSenShortErr          = TRUE;
            Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection     = FALSE;
        } else { 
            Ctrl.Para.dat.SenSts.Udat.Err.HSenBrokenErr         = FALSE;
            Ctrl.Para.dat.SenSts.Udat.Err.HSenExitDetection     = FALSE;
            Ctrl.Para.dat.SenSts.Udat.Err.HSenShortErr          = FALSE;
        }
        
        pSen   = &Ctrl.Sen.L;                           // ָ��͵㴫����
         
        if ( pSen->CurrentSec < 0.1 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.LSenBrokenErr         = TRUE;
            Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection     = FALSE;
        } else if ( pSen->Hight < 0.0 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection     = TRUE;
        } else if ( pSen->CurrentSec >= 22.0 ) {
            Ctrl.Para.dat.SenSts.Udat.Err.LSenShortErr          = TRUE;
            Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection     = FALSE;
        } else { 
            Ctrl.Para.dat.SenSts.Udat.Err.LSenBrokenErr         = FALSE;
            Ctrl.Para.dat.SenSts.Udat.Err.LSenExitDetection     = FALSE;
            Ctrl.Para.dat.SenSts.Udat.Err.LSenShortErr          = FALSE;
        }
        
        //osal_start_timerEx( OS_TASK_ID_SEN, OS_EVT_SEN_ERR_CHK ,        CYCLE_SEN_ERR_CHK);  

        return ( task_event ^ OS_EVT_SEN_ERR_CHK );
    }
    return 0;
}

/*******************************************************************************
 * ��    �ƣ� App_fParaFilter
 * ��    �ܣ� �����������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2016-04-20
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float App_fParaFilter(float para, float def, float min, float max)
{
    if ( para < min ) 
        para   = def;
    else if ( para > max )
        para   = def;
    else if ( (INT16U)para == 0XFFFF ) {
        para   = def;
    }
    
    return para;
}

/*******************************************************************************
 * ��    �ƣ� App_lParaFilter
 * ��    �ܣ� ���Ͳ�������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2016-04-20
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
long App_lParaFilter(long para, long def, long min, long max)
{
    if ( para < min ) 
        para   = def;
    else if ( para > max )
        para   = def; 
    
    return para;
}
/*******************************************************************************
 * ��    �ƣ� APP_SenInit
 * ��    �ܣ� �����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� wumingshen.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void TaskInitSen(void)
{
    /***********************************************
    * ������ ��ʼ��ADC
    */
    BSP_ADCInit();
    /***********************************************
    * ������ �ϵ���Ҫ��ʼ���Ĳ���
    */
    // ��ȡ�ϴ��ܶ�
    Ctrl.Para.dat.LastDensity       = (float)App_fParaFilter(Ctrl.Para.dat.LastDensity,
                                                             OIL_DENSITY_DEF,
                                                             OIL_DENSITY_MIN,
                                                             OIL_DENSITY_MAX);
    Ctrl.Para.dat.SetDensity        = (float)App_fParaFilter(Ctrl.Para.dat.SetDensity,
                                                             OIL_DENSITY_DEF,
                                                             OIL_DENSITY_MIN,
                                                             OIL_DENSITY_MAX);
    Ctrl.Para.dat.LastGravity       = (float)App_fParaFilter(Ctrl.Para.dat.LastGravity,
                                                             GRAVITY_DEF,
                                                             GRAVITY_MIN,
                                                             GRAVITY_MAX);
    Ctrl.Para.dat.SetGravity        = (float)App_fParaFilter(Ctrl.Para.dat.SetGravity,
                                                             GRAVITY_DEF,
                                                             GRAVITY_MIN,
                                                             GRAVITY_MAX); 
    
    // ��ȡƫ��ֵ
    Ctrl.Sen.C.OffsetHight          = (INT16S)App_fParaFilter(Ctrl.Para.dat.Offset,0,0,1000);
    if ( fabs(Ctrl.Sen.Para.dat.Offset) < 50.0 )
        Ctrl.Sen.C.OffsetHight     += (INT16S)Ctrl.Sen.Para.dat.Offset;
    
    // ��ȡ��������װ�߶Ȳ�
    Ctrl.Sen.Para.dat.ChkedDiffHight= (float)App_fParaFilter(Ctrl.Sen.Para.dat.ChkedDiffHight,
                                                             DIFF_HIGHT_DEF,
                                                             DIFF_HIGHT_MIN,
                                                             DIFF_HIGHT_MAX);
    
    //Ctrl.Sen.C.ChkedDiffHight       = (INT16S)Ctrl.Sen.Para.dat.ChkedDiffHight;
    //Ctrl.Sen.C.ChkedDensity         = (INT16S)Ctrl.Sen.Para.dat.ChkedDiffHight;
    
    // ���Ա�־λ����
    Ctrl.Para.dat.SenSts.Udat.Test      = 0;
    
    // ��λ��־λ����
    Ctrl.Para.dat.SenSts.Udat.RstFlag   = 0; 
    
    // ��ʾ��ʽ�ָ�Ĭ��
    //Ctrl.Para.dat.Sel.udat.DispSel      = 0; 
    
    // ������У׼��ʽѡ��
    if ( Ctrl.Sen.Para.dat.SenSts.udat.ChkType > 4 )
        Ctrl.Sen.Para.dat.SenSts.udat.ChkType   = 0;
    
    // ������ѡ��ʽ������
    Ctrl.Para.dat.Sel.udat.SensorSel            = 3; 
    
    // �����ݻ���ѡ��
    //Ctrl.VTab   = &OilTab.Tab[0];
    Ctrl.VTab   = &Ctrl.Para.dat.Vtab;
    
    /***********************************************
    * ������ ��·��У׼���ʼ��
    */
    double   k,b;
    INT08U   saveFlag;
    StrIToPSenChk   i2pDef      = {
        0.0,                                        // ѹ��1  PaΪ��λ
        10000.0,                                    // ѹ��2
        4.0000,                                     // ����1  uAΪ��λ
        20.0000,                                    // ����2
        625.0,                                      // б��
        -2500,                                      // ƫ��
    };
    StrAdcToIBoardChk a2iDef    = {
        4.0000,                                     // ����1  uAΪ��λ
        20.0000,                                    // ����2
        11915.6,                                    // ADC1
        59578.2,                                    // ADC2
        3.356929752048776e-4,                       // б��
        1.678464876024388e-5,                       // ƫ��
    };
    
    for ( char ch = 0; ch < 2; ch++ ) {
        saveFlag    = 0;
        
        /***********************************************
        * ������ ���У׼���ͣ�ѹǿ���ѹǿ����������
        */ 
        switch ( Ctrl.Sen.Para.dat.SenSts.udat.ChkType ) {
        /***********************************************
        * ������  ��У׼
        */
        case 0:
        /***********************************************
        * ������  ѹǿ��У׼
        */
        case 1:
            Ctrl.Para.dat.SenChk[ch]    = i2pDef;
            break;
        /***********************************************
        * ������  ѹǿ��+����������У׼
        */
        case 2:
            if ( Ctrl.Sen.Para.dat.PINbr == 2 ) {
                Ctrl.Para.dat.SenChk[ch].P1 = Ctrl.Sen.Para.dat.PI[ch][0][0];
                Ctrl.Para.dat.SenChk[ch].I1 = Ctrl.Sen.Para.dat.PI[ch][0][1];
                Ctrl.Para.dat.SenChk[ch].P2 = Ctrl.Sen.Para.dat.PI[ch][1][0];
                Ctrl.Para.dat.SenChk[ch].I2 = Ctrl.Sen.Para.dat.PI[ch][1][1];
            }
            break;
        /***********************************************
        * ������  ����������У׼
        */
        case 3:
        case 4:
            if ( Ctrl.Sen.Para.dat.PINbr == 2 ) {
                Ctrl.Para.dat.SenChk[ch].P1 = Ctrl.Sen.Para.dat.PI[ch][0][0];
                Ctrl.Para.dat.SenChk[ch].I1 = Ctrl.Sen.Para.dat.PI[ch][0][1];
                Ctrl.Para.dat.SenChk[ch].P2 = Ctrl.Sen.Para.dat.PI[ch][1][0];
                Ctrl.Para.dat.SenChk[ch].I2 = Ctrl.Sen.Para.dat.PI[ch][1][1];
            }
            break;
        default:
            break;
        }
        
        /***********************************************
        * ������ �������ģ���б�ʣ���������С��Ϊб�ʼ����
        */        
        if ( ( Ctrl.Para.dat.SenChk[ch].I2 > Ctrl.Para.dat.SenChk[ch].I1 ) &&
             ( Ctrl.Para.dat.SenChk[ch].P2 > Ctrl.Para.dat.SenChk[ch].P1 ) ) {
            k   = (double)(Ctrl.Para.dat.SenChk[ch].P2 - Ctrl.Para.dat.SenChk[ch].P1)
                / (double)(Ctrl.Para.dat.SenChk[ch].I2 - Ctrl.Para.dat.SenChk[ch].I1);
            b   = (double)(Ctrl.Para.dat.SenChk[ch].P1 )
                - (double)Ctrl.Para.dat.SenChk[ch].I1 * k;
         } else {
            saveFlag    = 1;
            k   = ( 10000.0 - 0.0 ) / ( 20.0000 - 4.0000 ); // (y2-y1)/(x2-x1)
            b   = 0.0 - k * 4.0000;                         // y1-k*x1
         } 
        /***********************************************
        * ������ ����ģ��Ĭ�ϱ�׼ֵ
        */
        if ( saveFlag || (fabs( k - 625 ) > 20 ) ) {
            saveFlag    = 0;
            Ctrl.Para.dat.SenChk[ch]    = i2pDef;
            // ��Ĭ�ϲ���д��EEPROM��
            INT08U *pb    = (INT08U   *)&Ctrl.Para.dat.SenChk[ch];
            App_SetParaToEep( 68 + ch * 24, pb, sizeof(StrAdcToIBoardChk) );
        } else {        
            Ctrl.Para.dat.SenChk[ch].K   = k;
            Ctrl.Para.dat.SenChk[ch].B   = b; 
        }
        
        /***********************************************
        * ������ ������ʾģ��б�ʣ���������С��Ϊб�ʼ����
        */
        if ( ( Ctrl.Para.dat.Check[ch].I2 > Ctrl.Para.dat.Check[ch].I1 ) &&
             ( Ctrl.Para.dat.Check[ch].Adc2 > Ctrl.Para.dat.Check[ch].Adc1 ) ) {
            k   = (double)(Ctrl.Para.dat.Check[ch].I2 - Ctrl.Para.dat.Check[ch].I1)
                / (double)(Ctrl.Para.dat.Check[ch].Adc2 - Ctrl.Para.dat.Check[ch].Adc1);
            b   = (double)(Ctrl.Para.dat.Check[ch].I1 )
                - (double)Ctrl.Para.dat.Check[ch].Adc1 * k;
        } else {
            saveFlag    = 1;
            k   = ( 20.0000 - 4.0000 ) / ( 59578.2 - 11915.6 ); // (y2-y1)/(x2-x1)
            b   = 4.0000 - k * 11915.6;                         // y1-k*x1
        } 
        
        /***********************************************
        * ������ Ĭ�ϱ�׼ֵ
        */
        if ( saveFlag || ( fabs( k - a2iDef.K > 0.000005 ) ) ) {
            saveFlag    = 0;
            Ctrl.Para.dat.Check[ch]     = a2iDef;
            // ��Ĭ�ϲ���д��EEPROM��
            INT08U *pb    = (INT08U   *)&Ctrl.Para.dat.Check[0];
            App_SetParaToEep( 20 + ch * 24, pb, sizeof(StrAdcToIBoardChk) );
        } else {
            Ctrl.Para.dat.Check[ch].K   = k;
            Ctrl.Para.dat.Check[ch].B   = b; 
        }
    }
       
    /***********************************************
    * ������ �ڿ��Ź���־��ע�᱾����Ŀ��Ź���־
    */
    WdtFlags |= WDT_FLAG_SEN;
    /*************************************************
    * �����������¼���ѯ
    */
#if ( OSAL_EN == DEF_ENABLED )
    osal_start_timerRl( OS_TASK_ID_SEN, OS_EVT_SEN_TICKS,           CYCLE_TIME_TICKS);
    osal_start_timerRl( OS_TASK_ID_SEN, OS_EVT_SEN_MSEC ,           CYCLE_SAMPLE_MSEC_TICKS);
    osal_start_timerRl( OS_TASK_ID_SEN, OS_EVT_SEN_DEAL ,           CYCLE_SAMPLE_TICKS);
    osal_start_timerEx( OS_TASK_ID_SEN, OS_EVT_SEN_UPDATE_DENSITY , CYCLE_UPDATE_DENSITY);  
    osal_start_timerRl( OS_TASK_ID_SEN, OS_EVT_SEN_ERR_CHK ,        CYCLE_SEN_ERR_CHK);  
#else
#endif
}

/*******************************************************************************
 * 				                    end of file                                *
 *******************************************************************************/
#endif