//                           _ooOoo_
//                          o8888888o
//                          88" . "88
//                          (| -_- |)
//                          O\  =  /O
//                       ____/`---'\____
//                     .'  \\|     |//  `.
//                    /  \\|||  :  |||//  \
//                   /  _||||| -:- |||||-  \
//                   |   | \\\  -  /// |   |
//                   | \_|  ''\---/''  |   |
//                   \  .-\__  `-`  ___/-. /
//                 ___`. .'  /--.--\  `. . __
//              ."" '<  `.___\_<|>_/___.'  >'"".
//             | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//             \  \ `-.   \_ __\ /__ _/   .-` /  /
//        ======`-.____`-.___\_____/___.-`____.-'======
//                           `=---='
//
//
//        ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//                   ���汣��       ��������
//                   �����޷�       ��������

/*******************************************************************************
 *   Filename:      bsp_pm25.C
 *   Revised:       $Date: 2015-07-11
 *   Revision:      $V1.0
 *   Writer:        wumingshen.
 *
 *   Description:   PM2.5�ҳ���������ģ��
 *   Notes:         ����������һ���ҳ���������ͨ���ɼ�ADֵ����ȡPM2.5��ֵ
 *
 *
 *   All copyrights reserved to wumingshen.
 *
 *******************************************************************************/
#include "global.h"
#include "bsp_pm25.h"
#include "bsp_adc.h"

/***********************************************
* ������ PM2.5���ֵȼ���ֵ
*/
#define  PM2D5_LEV_0            (35 +0)// 50              //С��EXCELLENCE_VALֵ���ȼ�Ϊ��
#define  PM2D5_LEV_1            (75 +0)// 100             //С��FINE_VALֵ������EXCELLENCE_VALֵ���ȼ�Ϊ��
#define  PM2D5_LEV_2            (115+0)// 150             //С��FINE_VALֵ������EXCELLENCE_VALֵ���ȼ�Ϊ��
#define  PM2D5_LEV_3            (150+0)// 200             //С��FINE_VALֵ������EXCELLENCE_VALֵ���ȼ�Ϊ��

#define PM2D5_ADC_BUF_LEN       100    
#define PM2D5_UART_BUF_LEN      14
     
u8      PM2D5_UartBuf           [PM2D5_UART_BUF_LEN];
u16     PM2D5_AdcBuf            [PM2D5_ADC_BUF_LEN];

/*******************************************************************************
 * ��    �ƣ� Bsp_Pm25Init
 * ��    �ܣ� PM2.5������Ӳ����ʼ��
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-07-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void BSP_PM2D5Init(void)
{
    /***********************************************
    * ������ ��ʼ��PM2.5�������ڲ�LED �˿�����
    */
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(PM25LED_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = PM25LED_PIN ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PM25LED_PORT, &GPIO_InitStructure);

    PM25LEDOFF();

    /***********************************************
    * ������ ��ʼ���ɼ�PM2.5��������ADCͨ��
    */
    Bsp_ADC_Init();	          /* ��ʼ�� ADC1 ģ�� */

    /***********************************************
    * ������ ��ʼ��2������
    */
    BSP_UartCfg( 1, 2400 );
#if defined(PM2D5_UART_CHK)
    BSP_UartCfg( 0, 9600 );                       // ���ô�ʱLED���쳣
#endif
    
    /***********************************************
    * ������ һ��б�ʳ�ʼ��
    */
    char i  = 0;
    if (( Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i] >= Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] ) ||
        ( Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] >= Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] ) ) {
        Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]    = (INT16S)pm_x1;
        Ctrl.Para.dat.Pm2d5Chk.Value.y1[i]  = (INT16S)pm_y1;
        Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i]    = (INT16S)pm_x2;
        Ctrl.Para.dat.Pm2d5Chk.Value.y2[i]  = (INT16S)pm_y2;
        
        BSP_FlashWriteBytes((u32) 10*2, (u8 *)&Ctrl.Para.dat.Pm2d5Chk.Offset, (u16) 10*2);
        
        osal_start_timerEx( OS_TASK_ID_SEN,
                            OS_EVT_SEN_PM2D5_CHK,
                            OS_TICKS_PER_SEC * 120);
        BSP_OS_TimeDly(1000);
        BeepExt(3, 1500, 200);
    }    
    Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                        / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
    Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]);  

    /***********************************************
    * ������ ����б�ʳ�ʼ��
    */
    i   = 1;
    if (( Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i] >= Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] ) ||
        ( Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] >= Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] ) ) {
        Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]    = (INT16S)pm_X1;
        Ctrl.Para.dat.Pm2d5Chk.Value.y1[i]  = pm_Y1;
        Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i]    = (INT16S)pm_X2;
        Ctrl.Para.dat.Pm2d5Chk.Value.y2[i]  = pm_Y2;
       
        BSP_FlashWriteBytes((u32) 10*2, (u8 *)&Ctrl.Para.dat.Pm2d5Chk.Offset, (u16) 10*2);
        //
        //osal_start_timerEx( OS_TASK_ID_SENSOR,
        //                   OS_EVT_SEN_PM2D5_CHK,
        //                    OS_TICKS_PER_SEC * 30);
    }  
    Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                        / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
    Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i] + 39.5);  
}

/*******************************************************************************
 * ��    �ƣ� Bsp_GetPm25AdVal
 * ��    �ܣ� ��ȡPM2.5��������ADֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������
 * �������ڣ� 2015-07-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void BSP_PM2D5G1GetAdcVal(u16 *pdata)
{
    /***********************************************
    * ������ ��PM2.5�������ڲ�LED
    */
    PM25LEDON();
    /***********************************************
    * ������ ��ʱ280uS
    */
    //static u16 pm2d5_dly  = 280;
    Delay_Nus(275);

    /***********************************************
    * ������ ����ADת�����ߴ�ʱPM2.5��ADֵ
    */
    ADC1Convert_Begin();            /* ��ʼADת�� */
    Get_AD_AverageValue();
    *pdata =  ADC_AverageValue[0];
    //*pdata =  ADC_Value[0][0];
    ADC1Convert_Stop();             /* �ر�ADת�� */

    /***********************************************
    * ��������PM2.5�������ڲ�LED
    */
    PM25LEDOFF();
}

/*******************************************************************************
 * ��    �ƣ� BSP_PM2D5GetAdcValue
 * ��    �ܣ� 10ms��ʱ�ص�����
 * ��ڲ����� 
 * ���ڲ����� 
 * ���� ���ߣ� Roger-wumingshen
 * �������ڣ� 2015-10-19
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void BSP_PM2D5GetAdcValueCB(void)
{
    static u8 ctr = 0;
    /***********************************************
    * ������ע�⣺�ص�������ִ�е�ʱ�䲻�˹���
    */
    BSP_PM2D5G1GetAdcVal(&PM2D5_AdcBuf[ctr]);

    if ( ++ctr >= PM2D5_ADC_BUF_LEN ) {
        ctr    = 0;
    }
}

/*******************************************************************************
 * ��    �ƣ� BSP_PM2D5G1GetValue
 * ��    �ܣ� ��ȡPM2.5 1��ֵ
 * ��ڲ����� pm2d5  PM2.5��ֵ
 * ���ڲ����� ��Ⱦ�ȼ�
 * ���� ���ߣ� ������
 * �������ڣ� 2015-07-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
s16 BSP_PM2D5G1GetValue(void)
{
//#define FILTER_BUF_LEN  16
    u16             After[PM2D5_ADC_BUF_LEN];
    static u16      PM25ADDataBuff[FILTER_BUF_LEN]   = {0};          // 
    static  u8      FilterCtr   = 0;
    u32             sum         = 0;
    static float    x           = 0.0;
    static float    last        = 0.0;    
    
    float           k           = Ctrl.PM2D5.k[0];//(1.5*(y2-y1)/(x2-x1));     // 1.8125//400/(839.111 - 170.667) = 400/648.444=0.5984
    float           b           = Ctrl.PM2D5.b[0];//(y1 - k *x1 + 39.5);       // -0.5984*170.667=-102.127
    
    /***********************************************
    * ������ һ���˲�������
    */
    App_BubbleAscendingArray(PM2D5_AdcBuf, PM2D5_ADC_BUF_LEN, After);
    u16 *p  = After;
    
    for(u8 i = 20; i < 90;i++)
        sum += p[i];
    
    x = (float)(sum/70.0);
    /***********************************************
    * ������ �����˲���ƽ��
    */
    //if ( 0 == Ctrl.PM2D5.Type ) {                   // �����2�����������Զ����
    //    Ctrl.PM2D5.RxCtr++;
    //}
    
    PM25ADDataBuff  [FilterCtr]  = (s16)x;
    sum = 0;
    if ( ++FilterCtr >= FILTER_BUF_LEN )
        FilterCtr   = 0;
    /***********************************************
    * ������ ��һ���ϵ����ֵ�仯����ʱ�����ٵ���
    */
    if ( ( ( last == 0.0 )      && ( x != 0 ) ) ) {// || 
         //( abs( x - last ) > 150 ) && abs( x - last ) < 400 ) ) {
        last    = (uint16)x;
        for ( u8 i = 0; i < FILTER_BUF_LEN; i++ )
        PM25ADDataBuff[i]   = (s16)x;
    }
    
    App_BubbleAscendingArray(PM25ADDataBuff,FILTER_BUF_LEN,After);
        
    for(u8 i = 3; i < FILTER_BUF_LEN - 3; i++)
        sum += p[i];
    
    x = (float)(sum/( FILTER_BUF_LEN - 6 ));
    
    //for (u8 i = 0; i < FILTER_BUF_LEN; i++)
    //    sum   +=  PM25ADDataBuff[i];
    //x = (float)(sum/8.0); 

    /***********************************************
    * ������ �����˲���ƽ��
    */
    if ( last != 0.0 ) {
        last    = last * 0.7 + x * 0.3;
    } else {
        if ( x != 0 )
            last        = x;
    }
    x                   = last;    
    Ctrl.PM2D5.G1AdVal  = (s16)x;
    /***********************************************
    * ������ ���㵱ǰֵ
    */
    s16 y   = (s16)(k*x + b);
    
    return  (s16)y;//(u16)( (y > 0) ? y : 0 );
}

/*******************************************************************************
 * ��    �ƣ� BSP_PM2D5G2GetValue
 * ��    �ܣ� ��ȡPM2.5 2W���������ͣ���ֵ
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-03-28
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
s16 BSP_PM2D5G2GetValue(void)
{
    static float    last    = 0.0;
    u32             sum     = 0;
    static float    x       = 0;
    
    u8  CheckVal            = 0;
    static u16 PM25ADDataBuff[8]   = {0};           // ��Ctrl.PM2D5��RxCtrλ����Ӧ
    u8  *pbuf   = PM2D5_UartBuf;
    
    /***********************************************
    * ������ ����Ƿ���2��������
    */
    if ( ++Ctrl.PM2D5.RxFaildCtr > 5 ) {            // 2����������ʱ���ĳ�1��
        Ctrl.PM2D5.RxFaildCtr   = 6;
        Ctrl.PM2D5.Type = 0;
        x   = 0;
        return 0;
    } else { 
        for(u8 i = 0;i < PM2D5_UART_BUF_LEN - 7; i++,pbuf++) {
            /***********************************************
            * ������ ��PM2.5������֡��ͷ�ͽ�β
            */
            if((pbuf[0] == 0xAA) && (pbuf[6] == 0xFF)) {
                CPU_SR  cpu_sr;
                CPU_CRITICAL_ENTER();
                CheckVal    = pbuf[1] + pbuf[2] + pbuf[3] + pbuf[4];
                if(CheckVal == pbuf[5]) {
                    x   = pbuf[1]*256 + pbuf[2];
                    PM25ADDataBuff[Ctrl.PM2D5.RxCtr++] = (u16)x;
                    CPU_CRITICAL_EXIT();
                    Ctrl.PM2D5.Type = 1;
                    break;
                }
                CPU_CRITICAL_EXIT();
            }
        }
        /***********************************************
        * ������ ��һ���ϵ����ֵ�仯����ʱ�����ٵ���
        */
        if ( ( ( last == 0.0 )      && ( x != 0 ) ) || 
             ( ( x - last ) > 200   || ( last - x ) > 200 ) ) {
            last    = (uint16)x;
            for ( uint8 i = 0; i < 8; i++ )
                PM25ADDataBuff[i]   = (u16)x;
        }
        /***********************************************
        * ������ 2��������δ��ʱ
        */
        for (u8 i = 0; i < 8; i++)
            sum   +=  PM25ADDataBuff[i];
        
        x = (u16)(sum/8);
    }
    
    /***********************************************
    * ������ 
    */
    float           K       = Ctrl.PM2D5.k[1];//((Y2-Y1)/(X2-X1));     // 1.8125//400/(839.111 - 170.667) = 400/648.444=0.5984
    float           B       = Ctrl.PM2D5.b[1];//(Y1 - K *X1 + 0);       // -0.5984*170.667=-102.127
  
    /***********************************************
    * ������ �˲�
    */
    if ( last != 0.0 ) {
        last    = last * 0.7 + x * 0.3;
    } else {
        if ( x != 0 )
            last        = x;
    }
    x                   = last;

    Ctrl.PM2D5.G2AdVal  = (s16)x;
    /***********************************************
    * ������ ����ʵ��ֵ
    */
    s16 y   = (s16)(K*x + B);
    /***********************************************
    * ������ ����
    */
    return  (s16)y;//(u16)( (y > 0) ? y : 0 );
}

/*******************************************************************************
 * ��    �ƣ� BSP_PM2D5GetValue
 * ��    �ܣ� 
 * ��ڲ����� 
 * ���ڲ����� 
 * ���� ���ߣ� wumingshen
 * �������ڣ� 2015-10-19
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
s16 BSP_PM2D5GetValue(u8 type)
{
    /***********************************************
    * ������ һ����������ȡ
    */
    if ( type == 0 ) 
        return BSP_PM2D5G1GetValue();
    /***********************************************
    * ������ ������������ȡ
    */
    else
        return BSP_PM2D5G2GetValue();
}

/*******************************************************************************
 * ��    �ƣ� BSP_PM2D5GetLevel
 * ��    �ܣ� ��ȡPM2.5����������ʵֵ
 * ��ڲ����� pm2d5  PM2.5��ֵ
 * ���ڲ����� ��Ⱦ�ȼ�
 * ���� ���ߣ� ������
 * �������ڣ� 2015-07-20
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
uint8_t BSP_PM2D5GetLevel(u16 pm2d5)
{
    /***********************************************
    * ������ 
    */
    typedef struct _PM2D5_LEVEL {
        u32     retlevel    : 2;
        u32     lastlevel   : 2;
        u32     lastvalue   :12;
        u32     chgctr      : 6;
        u32     rsv         :10;
    } StrPM2D5Level;
    
    static StrPM2D5Level    pm25Level;
    u16                     pm2d5cmpL;
    u16                     pm2d5cmpH;
    
    /***********************************************
    * ������ 
    */
#define DL                  5
#define DH                  2
#define CHG_CTR             30
    
    /***********************************************
    * ������ PM2.5�ȼ��� ��
    */
    if(pm2d5 <=  PM2D5_LEV_0) {
        pm25Level.retlevel  = 0;
        pm2d5cmpL           = PM2D5_LEV_0;
        pm2d5cmpH           = PM2D5_LEV_0;
    /***********************************************
    * ������ PM2.5�ȼ��� ����
    */
    } else if( pm2d5 <= PM2D5_LEV_1 ) {
        pm25Level.retlevel  = 1;
        pm2d5cmpL           = PM2D5_LEV_0;
        pm2d5cmpH           = PM2D5_LEV_1;
    /***********************************************
    * ������ PM2.5�ȼ��� �С�
    */
    } else if( pm2d5 <= PM2D5_LEV_2 ) {
        pm25Level.retlevel  = 1;
        pm2d5cmpL           = PM2D5_LEV_1;
        pm2d5cmpH           = PM2D5_LEV_2;
    /***********************************************
    * ������ PM2.5�ȼ��� �
    */
    } else {
        pm25Level.retlevel  = 2;//3;
        pm2d5cmpL           = PM2D5_LEV_2;
        pm2d5cmpH           = PM2D5_LEV_3;
    }
    
    /***********************************************
    * ������ ����ȼ�����
    */
    if ( pm25Level.retlevel > pm25Level.lastlevel ) {
        if ( pm2d5 < pm2d5cmpL + DL ) {                  // ��ǰֵС������+��ֵ
            pm25Level.chgctr    = 0;
            pm25Level.retlevel  = pm25Level.lastlevel;
        } else {
            if ( (pm25Level.chgctr < CHG_CTR) && 
                 (pm25Level.chgctr++ < CHG_CTR ) ) {   // �������5��
                pm25Level.retlevel  = pm25Level.lastlevel;
            }
        }
    /***********************************************
    * ������ ����ȼ��½�
    */
    } else if ( pm25Level.retlevel < pm25Level.lastlevel ) {
        if ( pm2d5 > pm2d5cmpH - DH ) {                  // ��ǰֵ��������-��ֵ
            pm25Level.chgctr    = 0;
            pm25Level.retlevel  = pm25Level.lastlevel;
        } else {
            if ( (pm25Level.chgctr < CHG_CTR) &&
                 (pm25Level.chgctr++ < CHG_CTR ) ) {    // �������5��
                pm25Level.retlevel  = pm25Level.lastlevel;
            }
        }
    /***********************************************
    * ������ ����ȼ�����
    */
    } else {
        pm25Level.chgctr    = 0;
    }
    
    /***********************************************
    * ������ 
    */
    pm25Level.lastlevel   = pm25Level.retlevel;
    pm25Level.lastvalue   = pm2d5;
    
    /***********************************************
    * ������ 
    */
    return(pm25Level.retlevel);
}

void BSP_PM2D5Chk(u8 type, s16 value )
{
    char i  = 0;
    
    /***********************************************
    * ������ �ָ�Ĭ��
    */
    if ( type == PM2D5_CHK_TYPE_DEF ) {
        Ctrl.Wifi.Wr.Offset = 0;
        if ( Ctrl.PM2D5.Type    == 0 ) {
            i  = 0;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]    = (INT16S)pm_x1;
            Ctrl.Para.dat.Pm2d5Chk.Value.y1[i]  = (INT16S)pm_y1;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i]    = (INT16S)pm_x2;
            Ctrl.Para.dat.Pm2d5Chk.Value.y2[i]  = (INT16S)pm_y2;
                
            Ctrl.Para.dat.Pm2d5Chk.Offset.b[i]       = Ctrl.Wifi.Wr.Offset;
            Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                                / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
            Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]);  
        } else {
            i   = 1;    
            Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]    = (INT16S)pm_X1;
            Ctrl.Para.dat.Pm2d5Chk.Value.y1[i]  = (INT16S)pm_Y1;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i]    = (INT16S)pm_X2;
            Ctrl.Para.dat.Pm2d5Chk.Value.y2[i]  = (INT16S)pm_Y2;
           
            Ctrl.Para.dat.Pm2d5Chk.Offset.b[i]       = Ctrl.Wifi.Wr.Offset;
            Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                                / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
            Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i] + 39.5);  
        }
    /***********************************************
    * ������ У׼�͵�
    */
    } else if ( type == PM2D5_CHK_TYPE_K_Y1 ) {
        /***********************************************
        * ������ �޸�PM2.5�궨��ֵ
        */
        if ( Ctrl.PM2D5.Type    == 0 ) {
            Ctrl.Para.dat.Pm2d5Chk.Value.y1[0]    = value;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x1[0]      = Ctrl.PM2D5.G1AdVal;
        } else {
            Ctrl.Para.dat.Pm2d5Chk.Value.y1[1]    = value;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x1[1]      = Ctrl.PM2D5.G2AdVal;
        } 
            
        i  = Ctrl.PM2D5.Type;
        Ctrl.Wifi.Wr.Offset = 0;
        Ctrl.Para.dat.Pm2d5Chk.Offset.b[i]       = Ctrl.Wifi.Wr.Offset;
        Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                            / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
        Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]); 

    /***********************************************
    * ������ У׼�ߵ�
    */
    } else if ( type == PM2D5_CHK_TYPE_K_Y2 ) {
        if ( Ctrl.PM2D5.Type    == 0 ) {
            Ctrl.Para.dat.Pm2d5Chk.Value.y2[0]    = value;
            Ctrl.Para.dat.Pm2d5Chk.Adc.x2[0]      = Ctrl.PM2D5.G1AdVal;
        } else {
            Ctrl.Para.dat.Pm2d5Chk.Value.y2[1]    = value; 
            Ctrl.Para.dat.Pm2d5Chk.Adc.x2[1]      = Ctrl.PM2D5.G2AdVal;
        }
            
        i  = Ctrl.PM2D5.Type;
        Ctrl.Wifi.Wr.Offset = 0;
        Ctrl.Para.dat.Pm2d5Chk.Offset.b[i]       = Ctrl.Wifi.Wr.Offset;
        Ctrl.PM2D5.k[i]     = ((float)(Ctrl.Para.dat.Pm2d5Chk.Value.y2[i] - Ctrl.Para.dat.Pm2d5Chk.Value.y1[i])
                            / (float)(Ctrl.Para.dat.Pm2d5Chk.Adc.x2[i] - Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]));
        Ctrl.PM2D5.b[i]     = (Ctrl.Para.dat.Pm2d5Chk.Value.y1[i] - Ctrl.PM2D5.k[i] *Ctrl.Para.dat.Pm2d5Chk.Adc.x1[i]);  
    /***********************************************
    * ������ У׼ƫ��
    */
    } else if ( type == PM2D5_CHK_TYPE_B_OFFSET ) {
        Ctrl.Wifi.Wr.Offset = value - Ctrl.PM2D5.Val;
        Ctrl.Para.dat.Pm2d5Chk.Offset.b[Ctrl.PM2D5.Type]       = Ctrl.Wifi.Wr.Offset;
    /***********************************************
    * ������ У׼ƫ��
    */
    } else if ( type == PM2D5_CHK_TYPE_OFFSET ) {
        Ctrl.Wifi.Wr.Offset = value;
        Ctrl.Para.dat.Pm2d5Chk.Offset.b[Ctrl.PM2D5.Type]       = Ctrl.Wifi.Wr.Offset;
    }  
        
    BSP_FlashWriteBytes((u32) 10*2, (u8 *)&Ctrl.Para.dat.Pm2d5Chk.Offset, (u16) 10*2);
}