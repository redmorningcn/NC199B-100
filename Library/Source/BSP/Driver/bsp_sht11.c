/*******************************************************************************
 *   Filename:       bsp_sht11.C
 *   Revised:        All copyrights reserved to Roger-WY.
 *   Revision:       v1.0
 *   Writer:	     Roger-WY.
 *
 *   Description:    STH11��ʪ�ȴ���������ģ��
 *                   ������ͬ������ ��SHT10/SHT20��
 *   Notes:
 *
 *     				 E-mail:261313062@qq.com
 *
 *******************************************************************************/

#include "bsp_sht11.h"
#include "includes.h"

/******************************************************************************/
GPIO_InitTypeDef    GPIO_InitStructure;



/************************* Sensirion Provide **********************************/

void  s_transstart(void);
void  s_connectionreset(void);
char  s_write_byte(unsigned char value);
char  s_read_byte(unsigned char ack);
char  s_softreset(void);
char  s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum);
char  s_write_statusreg(unsigned char *p_value);
char  s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void  calc_sth11(float *p_humidity ,float *p_temperature);
float calc_dewpoint(float h,float t);


/*******************************************************************************
 * ��    �ƣ� SHT_Delay
 * ��    �ܣ� SHT11��������ʱ t*10(us)
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע�� �ú���ֻ���ڱ��ļ��е���
 *******************************************************************************/
static void SHT_Delay(unsigned char t)
{
    unsigned int tmp;
    while(t--)
    {
        tmp = 90;
        while(tmp--);
    }
}

/*******************************************************************************
 * ��    �ƣ� Bsp_ShtInit
 * ��    �ܣ� SHT11��ʼ������(����IO���ų�ʼ���͸�λ����)
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void Bsp_ShtInit(void)
{
    /***********************************************
    * ������ SHTxxʹ�õ�IO��ʼ��
    */
    SHT_INIT();
    /***********************************************
    * ������ ��λ����SHTxx
    */
    s_connectionreset();
}

/*******************************************************************************
 * ��    �ƣ� s_write_byte
 * ��    �ܣ� ��ʼ���δ�ʱ�������������ʱ��
 * ��ڲ����� ��SHT11д1���ֽڲ����SHT11�Ƿ�Ӧ��
 * ���ڲ����� error=1����SHT11δӦ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_write_byte(unsigned char value)
{
    unsigned char i,error=0;

    SHT_DATA_OUPUT(); // DATA����Ϊ���

    /* �Ӹߵ�����λ���� */
    for (i=0x80;i>0;i/=2) {                                   // ��λ����
        if(i & value) {
            SHT_DATA_H();
        } else {
            SHT_DATA_L();
        }
        SHT_CLK_H();
        SHT_Delay(5);
        SHT_CLK_L();
        SHT_Delay(5);
    }
    SHT_DATA_H();                                             // �ͷ�DATA��
    SHT_CLK_H();                                              // ��9��SCK
    SHT_Delay(5);
    SHT_DATA_INPUT();
    error = GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT); // ���Ӧ�� (SHT11������DATA��ΪӦ��)
    SHT_Delay(5);
    SHT_CLK_L();
    return error;                                             // error=1����SHT11δӦ��
}

/*******************************************************************************
 * ��    �ƣ� s_read_byte
 * ��    �ܣ� ��SHT11��1���ֽڲ����������ack=1ʱ����Ӧ��
 * ��ڲ����� ack  Ӧ���־
 * ���ڲ����� error=1����SHT11δӦ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_read_byte(unsigned char ack)
{
    unsigned char i,val=0;

    SHT_DATA_OUPUT();                                       // DATA����Ϊ���
    SHT_DATA_H();                                           // �ͷ�DATA��
    SHT_DATA_INPUT();

    /* �Ӹߵ�����λ��ȡ */
    for (i=0x80;i>0;i/=2) {                                 // ��λ����
       SHT_CLK_H();
        if (GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT)) {
            val=(val | i);
        }
        SHT_CLK_L();
    }
    SHT_DATA_OUPUT();         // DATA����Ϊ���
    if(ack) {
        SHT_DATA_L();           // ��ack=1(����Ҫʹ��Ӧ��)ʱ����DATA��
    } else {
        SHT_DATA_H();
    }
    SHT_CLK_H();                                            // ��9��SCK
    SHT_Delay(5);
    SHT_CLK_L();
    SHT_DATA_H();                                           // �ͷ�DATA��
    return val;
}

/*******************************************************************************
 * ��    �ƣ� s_transstart
 * ��    �ܣ� ����һ��"��������"����
 *                 _____         ________
 *           DATA:      |_______|
 *                     ___     ___
 *           SCK : ___|   |___|   |______
 *
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void s_transstart(void)
{
    SHT_DATA_OUPUT();   // DATA����Ϊ���

    SHT_DATA_H();
    SHT_CLK_L();   // ��ʼ״̬

    SHT_Delay(10);
    SHT_CLK_H();

    SHT_Delay(10);
    SHT_DATA_L();

    SHT_Delay(10);
    SHT_CLK_L();

    SHT_Delay(30);
    SHT_CLK_H();

    SHT_Delay(10);
    SHT_DATA_H();

    SHT_Delay(10);
    SHT_CLK_L();

    SHT_Delay(10);
    SHT_DATA_L();

    SHT_Delay(10);
    SHT_Delay(10);
}

/*******************************************************************************
 * ��    �ƣ� s_connectionreset
 * ��    �ܣ� ͨ�Ÿ�λ
 *                 _____________________________________________________         ________
 *           DATA:                                                      |_______|
 *                    _    _    _    _    _    _    _    _    _        ___     ___
 *           SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
 *
 * ��ڲ����� ��
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void s_connectionreset(void)
{
    unsigned char i;

    SHT_DATA_OUPUT(); // DATA����Ϊ���
    SHT_DATA_H();
    SHT_CLK_L();   // ��ʼ״̬
    /* 9��SCK ����*/
    for(i=0;i<9;i++)
    {
        SHT_CLK_H();
        SHT_Delay(10);
        SHT_CLK_L();
        SHT_Delay(10);
    }
}

/*******************************************************************************
 * ��    �ƣ� s_softreset
 * ��    �ܣ� �����λ
 * ��ڲ����� ��
 * ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_softreset(void)
{
    unsigned char error=0;

    s_connectionreset();                                // ͨ�Ÿ�λ
    error+=s_write_byte(SHTCMD_RESET);                         // ����"��λ"�����SHT11
    return error;                                       // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
 * ��    �ƣ� s_read_statusreg
 * ��    �ܣ� ��ȡ״̬�Ĵ�����У���
 * ��ڲ����� p_value      ״̬�Ĵ�����ֵ
 *            p_checksum   У���
 * ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)
{
    unsigned char error=0;

    s_transstart();                                     // ����һ��"��������"����
    error=s_write_byte(SHTCMD_READ_STATUES);                   // ����"��״̬�Ĵ���"����
    *p_value=s_read_byte(ACK);                          // ��״̬�Ĵ���
    *p_checksum=s_read_byte(noACK);                     // ��У���

    return error;                                       // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
 * ��    �ƣ� s_write_statusreg
 * ��    �ܣ� д״̬�Ĵ���
 * ��ڲ����� p_value      ״̬�Ĵ�����ֵ
 * ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_write_statusreg(unsigned char *p_value)
{
    unsigned char error=0;

    s_transstart();                                     // ����һ��"��������"����
    error += s_write_byte(SHTCMD_WRITE_STATUS);         // ����"д״̬�Ĵ���"����
    error += s_write_byte(*p_value);                    // д״̬�Ĵ���

    return error;                                       // error=1��ʾSHT11δ��Ӧ
}

/*******************************************************************************
 * ��    �ƣ� s_measure
 * ��    �ܣ� ����һ�β���(���ʪ�Ȼ��¶�)
 * ��ڲ����� p_value      ����ֵ
 *            checksum     У���
 *            mode         TEMP��ʾ�����¶Ȳ���
 *                         HUMI��ʾ�������ʪ�Ȳ���
 * ���ڲ����� ����ֵΪ1��ʾSHT11δ��Ӧ
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
{
    OS_ERR err;
    uint16_t error = 0;
    uint16_t i;

    s_transstart();                         // ����һ��"��������"����
    switch(mode)                            // �����������mode����һ����Ӧ�Ĳ���
    {
        case TEMP: error += s_write_byte(SHTCMD_MEASURE_TEMP); break;
        case HUMI: error += s_write_byte(SHTCMD_MEASURE_HUMI); break;
        default  : break;
    }
    /***********************************************
    * ������ ����д���ֱ�ӷ���
    */
    if(error != 0) {
        return error;
    }

//    for (i=0;i<200;i++)  {
//        //�˴�Ӧ��Ҫ����ʱ
//        __NOP();__NOP();__NOP();__NOP();__NOP();
//        if(!GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT)) // �ȴ�SHT11��ɲ���
//            break;
//    }
//    if(GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT)) {
//        error += 1;
//    }                                      // ��������

    for(i = 0; i < 30; i ++) {
       OSTimeDly(20,OS_OPT_TIME_DLY,&err);
       if(!GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT)) // �ȴ�SHT11��ɲ���
           break;
    }
    if(GPIO_ReadInputDataBit(SHT_PORT,GPIO_Pin_SHT_DAT)) {
        error+=1;
    }
    /*
    * Note:�ڴ����ݶ�ȡ�����ֽڣ����ݴ洢�迼�Ǵ�С�˸�ʽ�������
    */
    *(p_value+1) = s_read_byte(ACK);                         // ����1���ֽ� (MSB)
    *(p_value)   = s_read_byte(ACK);                         // ����2���ֽ� (LSB)
    *p_checksum  = s_read_byte(noACK);                       // ��У���

    return error;
}

/*******************************************************************************
 * ��    �ƣ� calc_sth11
 * ��    �ܣ� ��ʼ���δ�ʱ�������������ʱ��
 * ��ڲ����� p_humidity      SHT11�ɼ��������ʪ��ֵ(������������ת�����ò�������ʵ����������ֵ)
 *            p_temperature   SHT11�ɼ������¶�ֵ(������������ת�����ò�������ʵ����������ֵ)
 * ���ڲ����� ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void calc_sth11(float *p_humidity ,float *p_temperature)
{
    const float C1=-4.0;                                     // 12λ
    const float C2=+0.0405;                                  // 12 Bit
    const float C3=-0.0000028;                               // 12 Bit
    const float T1=+0.01;                                    // 14λ 5V
    const float T2=+0.00008;                                 // 14λ 5V

    float rh=*p_humidity;                                  // ���ʪ�Ȳɼ�ֵ 12λ
    float t=*p_temperature;                                // �¶Ȳɼ�ֵ 14λ
    float rh_lin;                                          // ���ʪ�ȵķ����Բ���
    float rh_true;                                         // ���ʪ��������ֵ
    float t_C;                                             // �¶�������ֵ

    t_C=t*0.01 - 39.60;                                    // �����¶�������ֵ
    rh_lin=C3*rh*rh + C2*rh + C1;                      // �������ʪ�ȵķ����Բ���
    rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;                    // �������ʪ��������ֵ

    /* ��������������ʪ��������ֵ����Χ��ض� */
    if(rh_true>100) rh_true=100;
    if(rh_true<0.1) rh_true=0.1;

    *p_temperature = t_C;                                  // �����¶�������ֵ
    *p_humidity = rh_true;                                 // �������ʪ��������ֵ
}

/*******************************************************************************
 * ��    �ƣ� calc_dewpoint
 * ��    �ܣ� ����¶��
 * ��ڲ����� h      ���ʪ��������ֵ   t      �¶�������ֵ
 * ���ڲ����� ¶��ֵ
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
float calc_dewpoint(float h,float t)
{
    float logEx,dew_point;

    logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
    dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
    return dew_point;
}

/*******************************************************************************
 * ��    �ƣ� Bsp_ShtMeasureOnce
 * ��    �ܣ� SHT11����һ�β���
 * ��ڲ����� SHT_DATA_TYPE *t_shtval  SHT��������ʪ��ֵ
 * ���ڲ����� 1�������ɹ���0����ʧ��
 * �������ߣ� Roger-WY.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
u8 Bsp_ShtMeasureOnce(SHT_DATA_TYPE *t_shtval)
{
    float dew_point;  // ��¶�� (����)
    value humi_val,temp_val;
    unsigned char error,checksum;
    unsigned char j,k;

    int retrytime = 3;
    /***********************************************
    * ������ ��������ʱ�����ԵĴ���
    */
    while(retrytime--) {
        humi_val.i = 0;
        temp_val.i = 0;
        error=0;
        error += s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity
        error += s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature
        s_read_statusreg(&j,&k);
        /***********************************************
        * ������ ���������������Ӳ���
        */
        if(error!=0) {
           s_connectionreset();                   //in case of an error: connection SHTCMD_RESET
        }  else {
            humi_val.f=(float)humi_val.i;                   //converts integer to float
            temp_val.f=(float)temp_val.i;                   //converts integer to float
            calc_sth11(&humi_val.f,&temp_val.f);            //calculate humidity, temperature
            dew_point =calc_dewpoint(humi_val.f,temp_val.f); //calculate dew point

            //������ֵΪ��ʵֵ
            t_shtval->fDewpoint   = dew_point;
            t_shtval->fTemp       = temp_val.f;
            t_shtval->fHumi       = humi_val.f;
            //������ֵ��Ϊʵ����ֵ�Ŵ�10�������ֵ
            t_shtval->DewPoint    = ((int)(dew_point*10));
            t_shtval->Temperature = ((int)(temp_val.f*10));
            t_shtval->Humidity    = ((int)(humi_val.f*10));
            return 1;
        }
    }
    return 0;
}





