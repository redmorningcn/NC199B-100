/*******************************************************************************
 *   Filename:       bsp_pid.c
 *   Revised:        $Date: 2009-01-31	20:15 (Fri) $
 *   Revision:       $
 *	 Writer:		 WMing Shen.
 *
 *   Description:    PID�㷨
 *
 *   Notes:
 *
 *
 *   All copyrights reserved to WMing Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#include <includes.h>
#include <global.h>
#include <bsp_pid.h>

#define BSP_PID_MODULE_EN 1
#if BSP_PID_MODULE_EN > 0
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

/*******************************************************************************
 * GLOBAL VARIABLES
 */
StrPid          *pid;

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
 * ��    �ƣ� PID_Init
 * ��    �ܣ� PID_Init DESCRIPTION This function initializes the pointers in the
 *            _PID structure to the process
 *            variable and the setpoint. *Pv and *Sv are integer pointers.
 * ��ڲ�����
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PID_Init(StrPid *pid, int pv, int sv)
{
	pid->Pv 		= pv;
	pid->Sv 		= sv;
    pid->Kp         = 1;                            // ����ϵ��
    pid->Ki         = pid->Kp * pid->T / pid->Ti;   // ����ϵ��
    pid->Kd         = pid->Kp * pid->Td / pid->T;   // ΢��ϵ��
}

/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ� PID_Tune DESCRIPTION Sets the proportional gain (p_gain),
Integral gain (i_gain),
derivitive gain (d_gain), and the dead band (dead_band) of a pid control structure _PID.
//�趨PID���� �������� P,I,D,����
 * ��ڲ�����
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PID_Tune(StrPid        *pid,
              float         p_gain,
              float         i_gain,
              float         d_gain,
              float         Integral,
              CPU_INT16S    dead_band,
              CPU_INT16S    MaxPv,
              CPU_INT16S    MaxOut,
              CPU_INT16S    PidEnBand)
{
	pid->Kp 		= p_gain;
	pid->Ki 		= i_gain;
	pid->Kd 		= d_gain;
	pid->DeadBand 	= dead_band;
	pid->Integral	= Integral;
    pid->MaxPv      = MaxPv;
    pid->MaxOut     = MaxOut;
    pid->PidEnBand    = PidEnBand;
	pid->LastErr	= 0;
}
/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ� PID_SetInteral DESCRIPTION Set a new value for the Integral term of the pid equation.
//This is useful for setting the initial output of the pid controller at start up.
//�趨�����ʼֵ
 * ��ڲ�����
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PID_SetInteral(StrPid *pid,float new_Integral)
{
	pid->Integral	= new_Integral;
	pid->LastErr = 0;
}

/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ� PID_Bumpless DESCRIPTION Bumpless transfer algorithim.
//When suddenly changing setpoints, or when restarting the PID equation after an extended pause,
//the derivative of the equation can cause a bump in the
//controller output. This function will help smooth out that bump.
//The process value in *Pv should be the updated just before this function is used.
//PID_Bumpless ʵ�������л�
//��ͻȻ�ı��趨ֵʱ�������������󣬽������Ŷ�����������������ʵ��ƽ˳�Ŷ����ڵ��øú���֮ǰ��Ҫ�ȸ���PVֵ

 * ��ڲ�����
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void PID_Bumpless(StrPid *pid)
{
	pid->LastErr = (pid->Sv)-(pid->Pv);  //�趨ֵ�뷴��ֵƫ��
}
/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ� //PID_Calc DESCRIPTION Performs PID calculations for the _PID structure *a.
//This function uses the positional form of the pid equation, and incorporates an Integral windup prevention algorithim.
//Rectangular integration is used, so this function must be repeated on a consistent time basis for accurate control.
//RETURN VALUE The new output value for the pid loop. USAGE #include "control.h"
//������ʹ��λ��ʽPID���㷽ʽ�����Ҳ�ȡ�˻��ֱ����������� PID����
Kp                          ����ϵ��
Ki = Kp*T/Ti                ����ϵ��
Kd = Kp*Td/T                ΢��ϵ��
 * ��ڲ�����
 * ���ڲ����� -100% ~ 100%
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
float PID_Calc(StrPid *pid)
{
	float 	pterm;                                      // ������
    float   dterm;                                      // ΢����
    float   result;                                     // ������
    float   err;                                        // ƫ��ȡ��

    /*************************************************
    * ������ ���㵱ǰƫ��
    */
	pid->Err    = pid->Sv - pid->Pv;                    // ���㵱ǰƫ��
    err         = 0.0 - pid->Err;                       // ƫ��ȡ��

    if ( 0 == pid->Ki )
        pid->Integral   = 0.0;

    /*************************************************
    * ������ PID��������
    */
	if ( abs(pid->Err) > pid->DeadBand ) {                  // �ж��Ƿ��������
        /*************************************************
        * ������ ������
        */
		pterm   = pid->Kp * pid->Err;                       // ������ (0~100)
        /*************************************************
        * ������ ������
        */
        switch ( pid->OutPolarity ) {
        /*************************************************
        * ������ ������
        */
        case PID_POLARITY_POSITIVE:
            if ( pterm >= abs(pid->LoLimit) ) {             // ��������� > 100
                pid->Integral = 0.0;                        // �������Ϊ0
            } else {
                pid->Integral += pid->Ki * pid->Err;        // ������
            }
            break;
        /*************************************************
        * ������ ������
        */
        case PID_POLARITY_NEGATIVE:
            if ( pterm <= abs(pid->HiLimit)) {              // ��������� > 100
                pid->Integral = 0.0;                        // �������Ϊ0
            } else {
                pid->Integral += pid->Ki * pid->Err;        // ������
            }
            break;
        /*************************************************
        * ������ ˫����
        */
        case PID_POLARITY_UNIPOLAR:
            if ( fabs(pterm) >= abs(pid->LoLimit) ) {       // ��������� > 100
                pid->Integral = 0.0;                        // �������Ϊ0
            } else {
                pid->Integral += pid->Ki * pid->Err;        // ������
            }
            break;
        }

        /*************************************************
        * ������ ΢����
        */
		dterm   = pid->Kd * ( pid->Err - pid->LastErr );    // ΢����
        /*************************************************
        * ������ ������
        *        y = Kp [ e(t) + T/Ti|(e(t)*dt) + TD/T*de(t)/dt]
        */
		result 	= pterm                                     // ����������
                + pid->Integral                             // ���ֵ�����
                + dterm;                                    // ΢�ֵ�����
    /*************************************************
    * ������ ������Χ��������
    */
	} else {

	}

    /*************************************************
    * ������ �������
    */
    if ( err < pid->LoLimit ) {                         // �������PID��������ֵ
        pid->LastErr    = pid->Err;                             // �����ϴ�ƫ��
        return pid->MaxOut;                             // �������ֵ���
    } else if ( err > pid->HiLimit ) {                  // �������PID�ر�����ֵ
        if ( pid->LastErr > pid->Err ) {
            pid->Integral = 0.0;                            // �������Ϊ0
            pid->LastErr    = pid->Err;                             // �����ϴ�ƫ��
            return pid->MinOut;                             // ������Сֵ���
        } else {
        }
    }

    /*************************************************
    * ������ ���浱ǰƫ��
    */
	pid->LastErr    = pid->Err;                             // �����ϴ�ƫ��

    /*************************************************
    * ������ �������ֵ
    */
    switch ( pid->OutPolarity ) {
    /*************************************************
    * ������ ������
    */
    case PID_POLARITY_POSITIVE:
        result  = result / (float)abs(pid->LoLimit) * (float)pid->MaxOut;
        break;
    /*************************************************
    * ������ ������
    */
    case PID_POLARITY_NEGATIVE:
        result = result / (float)abs(pid->HiLimit) * (float)pid->MinOut;
        result  = 0.0 - abs(result);
        break;
    /*************************************************
    * ������ ˫����
    */
    case PID_POLARITY_UNIPOLAR:
        break;
    }

    /*************************************************
    * ������ �������
    */
    if ( result > pid->MaxOut ) { 				            // �������������100
        result = pid->MaxOut;                               // �����100
    } else if ( result < pid->MinOut ) { 				    // �������������100
        result = pid->MinOut;                               // �����100
    }
    /*************************************************
    * ������ ����
    */
	return (result);                                        // ���PIDֵ(0-100%)
}

/*******************************************************************************
 * ��    �ƣ�
 * ��    �ܣ�
 * ��ڲ�����
 * ���ڲ����� ��
 * �������ߣ� ������
 * �������ڣ� 2014-08-23
 * ��    �ģ�
 * �޸����ڣ�
 *******************************************************************************/
void BSP_PID_Main(void) {
	float 	result;
	int 	count	= 0;
    StrPid  HeatPid;
			pid 	= &HeatPid;

	printf("The values of Process point, Set point, P gain, I gain, D gain \n");
	printf("Enter the values of Process point\n");

    PID_Init(&HeatPid, 10.0, 50.0);             // �趨PV,SPֵ
//    PID_Tune(&HeatPid,                          // ���ڶ���
//             HeatPid.Kp,                        // ����ϵ��
//             HeatPid.Ki,                        // ����ϵ��
//             HeatPid.Kd,                        // ΢��ϵ��
//             HeatPid.DeadBand,                  // ����
//             HeatPid.Integral);                 // ����
    PID_SetInteral(&HeatPid, 0.0);              // ��ʼ��PID���ֵ
    PID_Bumpless(&HeatPid);                     // ��ȡ��ʼ�����

	while(count++<=1000) {
		result          = PID_Calc(&HeatPid);
        HeatPid.Pv     += result * 10.0 / 100.0;

		printf("%d: %f\n",  count, result);
		printf("\n%f%f%f%f",(float)HeatPid.Pv, (float)HeatPid.Sv, (float)HeatPid.Ki, (float)HeatPid.Kd);
	}
}




//////////////////////////////////////////////////////////

struct _pid{
	float Sv;				    //�����趨ֵ
	float Pv;			        //����ʵ��ֵ
	float Err;					//����ƫ��ֵ
	float LastErr;				//������һ��ƫ��ֵ
	float Kp,Ki,Kd;				//������������֡�΢��ϵ��
	float Integral;				//�������ֵ
    float DeadBand;
	float CtrlVal;  		    //�������������ִ�����ı�����
	float Max;
	float Min;
}pid2;

void PID_Init2(){
	printf("PID_init begin \n");
	pid2.Sv         = 0.0;
	pid2.Pv         = 0.0;
	pid2.Err        = 0.0;
	pid2.LastErr    = 0.0;
	pid2.CtrlVal    = 0.0;
	pid2.Integral   = 0.0;
	pid2.Kp         = 0.2;
	pid2.Ki         = 0.1;          //ע�⣬���ϼ�����ȣ�����Ӵ��˻��ֻ��ڵ�ֵ
	pid2.Kd         = 0.2;
    pid2.DeadBand   = 120;
	pid2.Max        = 120;
	pid2.Min        = 0;
	printf("PID_init end \n");
}

float PID_Realize(float Sv){
	int             IntegralEn;
	pid2.Sv	        = Sv;
	pid2.Err	    = pid2.Sv - pid2.Pv;

	if(pid2.Pv > pid2.Max) {                            //��ɫ��ɫ��ʾ�����ֱ��͵�ʵ��
		if(fabs(pid2.Err) > 100) {                      //��ɫ��עΪ���ַ������
			IntegralEn=0;
		} else {
			IntegralEn=1;
			if(pid2.Err<0) {
				pid2.Integral	+= pid2.Err;
			}
		}
	} else if (pid2.Pv < pid2.Max) {
		if(fabs(pid2.Err) > 100) {                      //���ַ������
			IntegralEn=0;
		} else {
			IntegralEn=1;
			if(pid2.Err > 0) {
				pid2.Integral	+= pid2.Err;
			}
		}
	} else {
		if(fabs(pid2.Err) > 100) {                      //���ַ������
			IntegralEn=0;
		} else {
			IntegralEn=1;
			pid2.Integral	+= pid2.Err;
		}
	}

	pid2.CtrlVal    =	pid2.Kp * pid2.Err				    +
						pid2.Ki * pid2.Integral * IntegralEn+
						pid2.Kd *(pid2.Err	  - pid2.LastErr);

	pid2.LastErr	= pid2.Err;
	pid2.Pv	        = pid2.CtrlVal * 1.0;

	return pid2.Pv;
}

int main2(){
    PID_Init2();
    int count=0;

    while(count<1000)
    {
        float Pv	= PID_Realize(100.0);
        printf("%f\n",Pv);
        count++;
    }
    return 0;
}
//////////////////////////////////////////////////////////
//PID�����㷨��C����ʵ��һ PID�㷨ԭ��
//	��������ڿ���һ������㷨��C����ʵ�����⣬������������û��һ�������ıȽ���ϵ��
//	���⡣   �����ܽ��˼��죬����һ��˼·�������ҡ�   �ڹ�ҵӦ����PID���������㷨
//	��Ӧ����㷺���㷨֮һ���ǵ�֮�����������㷨��   ����ܹ���������PID�㷨�����
//	��ʵ�ֹ��̣�����һ����з���Ա������Ӧ�����㹻Ӧ��һ���з������ˣ������ܿɹ���ǣ�
//	�������Ӵ��Ŀ����㷨���У�PID�����㷨������򵥣��������ַ���˼��Ŀ����㷨����
//	ν�����еľ��䡣�����δ���Ǹ��ӵģ�����Ķ��������Ǽ򵥵ģ���������򵥵ģ�����
//	ţ�ٵ���ѧ�����ɰɣ����밮��˹̹�����ܷ��̰ɣ��εȵļ򵥣��򵥵Ĳ���ԭʼ�ģ���
//	����Ҳ�������ģ��򵥵������ĳ̶ȡ��ȿ���PID�㷨��һ����ʽ��
//
//	PID�����̼򵥵��˲����ټ򵥵ĳ̶ȣ�ͨ������źſ��Ʊ���������������������Ǳ�����
//	���֡�΢���������ڵļӺ͡��������ǹ涨����tʱ�̣���
//	1.������Ϊrin(t);
//	2.�����Ϊrout(t);
//	3.ƫ����ΪErr(t)=rin(t)-rout(t);
//	pid�Ŀ��ƹ���Ϊ
//
//	���һ�������ʽ����Ҫ�����漸���������֣�Ϊ�˱�����⣬�ѿ��ƻ�������һ�£�
//	1.�涨�������������Ϊֱ��������ٵ�;
//	2.������rin(t)Ϊ���ת��Ԥ��ֵ;
//	3.�����rout(t)Ϊ���ת��ʵ��ֵ;
//	4.ִ����Ϊֱ�����;
//	5.������Ϊ������̣���������Ϊ10��;
//	6.ֱ���������PWM����ת���õ�λ ת/min ��ʾ;
//	���ѿ������½��ۣ�
//	1.������rin��t��Ϊ���ת��Ԥ��ֵ��ת/min��;
//	2. �����rout(t)Ϊ���ת��ʵ��ֵ��ת/min��;
//	3.ƫ����ΪԤ��ֵ��ʵ��ֵ֮�ת/min��;
//	��ô���¼���������ҪŪ�����
//	1.ͨ��PID����֮���U(t)��ʲôֵ�أ�
//	2.����ִ������ֱ�������ת��ת��Ӧ��Ϊ��ѹֵ��Ҳ����PWMռ�ձȣ���
//	3.��ôU(t)��PWM֮�������������ϵ�أ�
//	http://blog.21ic.com/user1/3407/archives/2006/33541.html��ƪ�����ϸ�����һ�ַ�
//	��������ÿ����ѹ��Ӧһ��ת�٣���ѹ��ת��֮��������Թ�ϵ�������ҿ������ַ�����
//	ǰ���ǰ�ֱ��������������Ϊ�����ˣ���ʵ������£�ֱ����������Ծ��Բ������Եģ�
//	����˵�ھֲ������������Եģ������Ϊʲô˵PID�����и���Χ�����⡣���忴һ��
//	http://articles.e-works.net.cn/component/article90249.htm��ƪ���¾Ϳ����˽��ˡ�
//	��������ʽ���е������֮ǰ����Ҫ���п���ϵͳ�����Ե����ת��֮����������ߣ�����
//	���ĵ��������˵������Ȼ���ٽ��бջ�������������ƪ��д���⣬��һƪ˵������ϵͳ��
//	��ɢ�����⡣��������ɢ������ص㽲��λ����PID��������PID���÷���C����ʵ�ֹ��̡�
//
////////////////////////////////////////////////////////////
//PID�����㷨��C����ʵ�ֶ� PID�㷨����ɢ��
//	��һ���У���������PID�㷨�Ļ�����ʽ����������ƹ��̵�ʵ������һ����Ҫ��˵����ͨ
//	����һ�ڵ��ܽᣬ�����Ѿ���������PID���ƵĹ��̡���һ�����ȼ�����һ�����ݲ���˵��
//	һ�¡�
//	1.˵��һ�·������Ƶ�ԭ��ͨ����һ�ڵĿ�ͼ���ѿ�����PID������ʵ�Ƕ�ƫ��Ŀ��ƹ���;
//	2.���ƫ��Ϊ0,��������ڲ������ã�ֻ�д���ƫ��ʱ���������ڲ������á�
//	3.���ֻ�����Ҫ���������������ν�������ϵͳ�ȶ������ֵ���趨ֵ֮��Ĳ�ֵ��
//	 ���ֻ���ʵ���Ͼ���ƫ���ۼƵĹ��̣����ۼƵ����ӵ�ԭ��ϵͳ���Ե���ϵͳ��ɵľ��
//	4.��΢���ź���Ӧ��ƫ���źŵı仯���ɣ�����˵�Ǳ仯���ƣ�����ƫ����Ϣ�ı仯����
//	 �����е��ڣ��Ӷ�������ϵͳ�Ŀ����ԡ�
//	���ˣ�����PID�Ļ���˵���Ͳ��䵽������潫��PID����ϵͳ��ɢ�����Ӷ������ڴ�����
//	��ʵ�֡����������״̬�Ĺ�ʽ����һ�£�
//
//	����������ΪT�����ڵ�KTʱ�̣�
//	Err(K)=rin(K)-rout(K);
//	���ֻ����üӺ͵���ʽ��ʾ����Err(K)+Err(K+1)+����;
//	΢�ֻ�����б�ʵ���ʽ��ʾ����[Err(K)-Err(K-1)]/T;
//	�Ӷ��γ�����PID��ɢ��ʾ��ʽ��
//	��u(K)�ɱ�ʾ��Ϊ��
//	����˵Kp��Ki��Kd���������ľ�����ʽ������������ɵ��Ƴ��ˣ������ʡʱ�䣬����
//	��ϸ��ʾ�ˡ�	��ʵ������Ϊֹ��PID�Ļ�����ɢ��ʾ��ʽ�Ѿ������ˡ�Ŀǰ�����ֱ���
//	��ʽ����λ����PID������һ�ֱ�����ʽΪ����ʽPID����U�������ʽ�������׵õ���
//	��ô��
//	�������ɢ��PID������ʽ��ʾ��ʽ���ɹ�ʽ���Կ���������ʽ�ı����ֻ��������ε�
//	ƫ����йأ������ʹ�������ϵͳ���ȶ��ԡ���Ҫע��������յ�������Ӧ��Ϊ
//	u(K)+��������ֵ;
//	PID����ɢ�����̻���˼·���������������ǽ���ɢ���Ĺ�ʽת����ΪC���ԣ��Ӷ�ʵ��΢
//	�������Ŀ������á�
//	PID�����㷨��C����ʵ�ֶ� PID�㷨����ɢ��   ��һ���У���������PID�㷨�Ļ�����ʽ��
//	��������ƹ��̵�ʵ������һ����Ҫ��˵����ͨ����һ�ڵ��ܽᣬ�����Ѿ���������PID
//	���ƵĹ��̡���һ�����ȼ�����һ�����ݲ���˵��һ�¡�
//	1.˵��һ�·������Ƶ�ԭ��ͨ����һ�ڵĿ�ͼ���ѿ�����PID������ʵ�Ƕ�ƫ��Ŀ��ƹ���;
//	2.���ƫ��Ϊ0,��������ڲ������ã�ֻ�д���ƫ��ʱ���������ڲ������á�
//	3.���ֻ�����Ҫ���������������ν�������ϵͳ�ȶ������ֵ���趨ֵ֮��Ĳ�ֵ��
//	  ���ֻ���ʵ���Ͼ���ƫ���ۼƵĹ��̣����ۼƵ����ӵ�ԭ��ϵͳ���Ե���ϵͳ��ɵľ��
//	4.��΢���ź���Ӧ��ƫ���źŵı仯���ɣ�����˵�Ǳ仯���ƣ�����ƫ����Ϣ�ı仯��
//	  �������е��ڣ��Ӷ�������ϵͳ�Ŀ����ԡ�
//	���ˣ�����PID�Ļ���˵���Ͳ��䵽������潫��PID����ϵͳ��ɢ�����Ӷ������ڴ���
//	����ʵ�֡����������״̬�Ĺ�ʽ����һ�£�
//
//	����������ΪT�����ڵ�KTʱ�̣�
//	Err(K)=rin(K)-rout(K);
//	���ֻ����üӺ͵���ʽ��ʾ����Err(K)+Err(K+1)+����;
//	΢�ֻ�����б�ʵ���ʽ��ʾ����[Err(K)-Err(K-1)]/T;
//	�Ӷ��γ�����PID��ɢ��ʾ��ʽ��
//	��u(K)�ɱ�ʾ��Ϊ��
//	����˵Kp��Ki��Kd���������ľ�����ʽ������������ɵ��Ƴ��ˣ������ʡʱ�䣬����
//	��ϸ��ʾ�ˡ���ʵ������Ϊֹ��PID�Ļ�����ɢ��ʾ��ʽ�Ѿ������ˡ�Ŀǰ�����ֱ�����ʽ
//	����λ����PID������һ�ֱ�����ʽΪ����ʽPID����U�������ʽ�������׵õ���
//	��ô��
//	�������ɢ��PID������ʽ��ʾ��ʽ���ɹ�ʽ���Կ���������ʽ�ı����ֻ��������ε�
//	ƫ����йأ������ʹ�������ϵͳ���ȶ��ԡ���Ҫע��������յ�������Ӧ��Ϊ
//	u(K)+��������ֵ;
//	PID����ɢ�����̻���˼·���������������ǽ���ɢ���Ĺ�ʽת����ΪC���ԣ��Ӷ�ʵ��
//	΢����	���Ŀ������á�
//
////////////////////////////////////////////////////////////
//PID�����㷨��C����ʵ���� λ����PID��C����ʵ��
//   ��һ�����Ѿ��������λ����PID��������PID����ѧ���ʽ����һ�ڣ��ص㽲��C���Դ���
//   ��ʵ�ֹ��̣��㷨��C����ʵ�ֹ��̾���һ���ԣ�ͨ��PID�㷨��C����ʵ�֣������Դ����ƣ�
//   ��������㷨��C����ʵ�֡�
//   ��һ��������PID�����ṹ�壬�������£�
//struct _pid{
//    float Sv;				//�����趨ֵ
//    float Pv;			//����ʵ��ֵ
//    float Err;					//����ƫ��ֵ
//    float LastErr;				//������һ��ƫ��ֵ
//    float Kp,Ki,Kd;				//������������֡�΢��ϵ��
//    float Result;				//�����ѹֵ������ִ�����ı�����
//    float Integral;				//�������ֵ
//}pid;
//	�����㷨������Ҫ�õ��Ĳ�����һ���ṹ����ͳһ���壬��������ʹ�á�
//	�ڶ�������ʼ���������������£�
//void PID_init(){
//    printf("PID_init begin \n");
//    pid.Sv=0.0;
//    pid.Pv=0.0;
//    pid.Err=0.0;
//    pid.LastErr=0.0;
//    pid.Result=0.0;
//    pid.Integral=0.0;
//    pid.Kp=0.2;
//    pid.Ki=0.015;
//    pid.Kd=0.2;
//    printf("PID_init end \n");
//}
//ͳһ��ʼ��������������Kp,Ki,Kd�������������Թ��̵��У�����Ҫ��Ŀ���Ч��������ͨ��
//������������ֱ�ӽ��е��ڡ�
//����������д�����㷨���������£�
//float PID_Realize(float Sv){
//    pid.Sv=Sv;
//    pid.Err=pid.Sv-pid.Pv;
//    pid.Integral+=pid.Err;
//    pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral				+
//						pid.Kd *(pid.Err	  - pid.LastErr);
//    pid.LastErr=pid.Err;
//    pid.Pv=pid.Result*1.0;
//    return pid.Pv;
//}
//ע�⣺����������������㷨ʵ����ʽ��û�п����������⣬û���趨�����ޣ�ֻ�ǶԹ�ʽ��
//һ��ֱ�ӵ�ʵ�֣�����Ľ��ܵ��л����𽥵ĶԴ˸Ľ���
//   ����Ϊֹ��PID�Ļ���ʵ�ֲ��־ͳ�������ˡ������ǲ��Դ��룺
//int main(){
//    printf("System begin \n");
//    PID_init();
//    int count=0;
//    while(count<1000)
//    {
//        float Sv=PID_Realize(200.0);
//        printf("%f\n",Sv);
//        count++;
//    }
//return 0;
//}
//
//PID�����㷨��C����ʵ���� ������PID��C����ʵ��
//   ��һ���н�������򵥵�λ����PID��ʵ���ֶΣ���һ����Ҫ��������ʽPID��ʵ�ַ�����
//   λ���ͺ�������PID����ѧ��ʽ��μ��ҵ�ϵ���ġ�PID�����㷨��C����ʵ�ֶ����еĽ��⡣
//   ʵ�ֹ�����Ȼ�Ƿ�Ϊ�����������ʼ��������ʵ�ֿ����㷨�������㷨�����ĸ����֣�
//   ��ϸ������μӡ�PID�����㷨��C����ʵ�������еĽ��⣬����ֱ�Ӹ��������ˡ�
//
///*
// * PID.c
// *
// *  Created on: 2011-11-7
// *      Author: wang
// */
//#include<stdio.h>
//#include<stdlib.h>
//
//struct _pid{
//    float Sv;            	//�����趨ֵ
//    float Pv;        	//����ʵ��ֵ
//    float Err;                	//����ƫ��ֵ
//    float Err_next;            	//������һ��ƫ��ֵ
//    float LastErr;            	//��������ǰ��ƫ��ֵ
//    float Kp,Ki,Kd;            	//������������֡�΢��ϵ��
//}pid;
//
//void PID_Init(){
//    pid.Sv=0.0;
//    pid.Pv=0.0;
//    pid.Err=0.0;
//    pid.LastErr=0.0;
//    pid.Err_next=0.0;
//    pid.Kp=0.2;
//    pid.Ki=0.015;
//    pid.Kd=0.2;
//}
//
//float PID_Realize(float Sv){
//    pid.Sv			= Sv;
//    pid.Err					= pid.Sv-pid.Pv;
//    float incrementSpeed	=
//						pid.Kp * (pid.Err	  - pid.Err_next) + \
//						pid.Ki * (pid.Err) 					+ \
//						pid.Kd * (pid.Err 	  - 2*pid.Err_next + pid.LastErr);
//    pid.Pv		   += incrementSpeed;
//    pid.LastErr			= pid.Err_next;
//    pid.Err_next 			= pid.Err;
//
//    return pid.Pv;
//}
//
//int main(){
//    PID_Init();
//    int count=0;
//
//    while(count<1000)
//    {
//        float Sv	= PID_Realize(200.0);
//        printf("%f\n",Sv);
//        count++;
//    }
//    return 0;
//}
//
//PID�����㷨��C����ʵ���� ���ַ����PID�����㷨C����ʵ��
//    ͨ����������ƪ���£��������Ѿ�Ū�����PID�����㷨�����ı�﷽����
//	����ͨPID�����У�������ֻ��ڵ�Ŀ�ģ���Ҫ��Ϊ�����������߿��ƾ��ȡ�
//	���������������������������趨ʱ����ʱ����ϵͳ����кܴ��ƫ�
//	�����PID����Ļ��ֻ��ۣ����¿���������ִ�л�������������������Χ��Ӧ����
//	��������	�Ӷ�����ϴ�ĳ������������𵴣����Ǿ��Բ�����ġ�Ϊ�˿˷���һ���⣬
//	�����˻��ַ���ĸ�������˼·�� �����������趨ֵƫ��ϴ�ʱ��ȡ����������;
//	���������ӽ�����ֵʱ��������ֿ��ƣ������������߾��ȡ������ʵ�ִ������£�
//    pid.Kp=0.2;
//    pid.Ki=0.04;
//    pid.Kd=0.2;  //��ʼ������
//
//	if(abs(pid.Err)>200) {
//		IntegralEn=0;
//    }else{
//		IntegralEn=1;
//		pid.Integral+=pid.Err;
//    }
//    pid.Result		= 	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn		+
//						pid.Kd *(pid.Err	  - pid.LastErr);    //�㷨����ʵ�ֹ���
//
//	�������ֵĴ���μ���PID�����㷨��C����ʵ�������еĽ��⣬����׸����ͬ���ɼ�1000
//	�������ᷢ�֣�ϵͳ����199���е�ʱ����ԭ��ʱ���1/2,ϵͳ�Ŀ����Եõ�����ߡ�
//
//PID�����㷨��C����ʵ���� �����ֱ��͵�PID�����㷨C����ʵ��
//	��ν�Ļ��ֱ���������ָ���ϵͳ����һ�������ƫ�PID��������������ڻ������õ�
//	�����ۼӶ��Ӵ󣬴Ӷ�����ִ�л����ﵽ����λ�ã������������U(k)��������ִ��
//	�����Ȳ����������󣬴�ʱ���������������������������з�Χ�����뱥������һ��ϵͳ
//	���ַ���ƫ�u(k)�𽥴ӱ������˳������뱥����Խ�����˳�������ʱ��Խ���������ʱ
//	���ִ�л�����Ȼͣ���ڼ���λ�ö�����ƫ��������������Ӧ�ĸı䣬��ʱϵͳ����
//	ʧ��һ������ɿ������ܶ񻯣����������Ϊ���ֱ�����������ʧ�����󡣷�ֹ���ֱ���
//	�ķ���֮һ���ǿ����ֱ��ͷ����÷�����˼·���ڼ���u(k)ʱ�������ж���һʱ�̵Ŀ�����
//	u(k-1)�Ƿ��Ѿ������˼��޷�Χ��
//	���u(k-1)>Max����ֻ�ۼӸ�ƫ��;
//	���u(k-1)<Min����ֻ�ۼ���ƫ�
//	�Ӷ������������ʱ��ͣ���ڱ�������ֱ���������룬�����Ŀ���ǰ�漸�ڵĽ��ܡ�
//struct _pid{
//	float Sv;				//�����趨ֵ
//	float Pv;			//����ʵ��ֵ
//	float Err;					//����ƫ��ֵ
//	float LastErr;				//������һ��ƫ��ֵ
//	float Kp,Ki,Kd;				//������������֡�΢��ϵ��
//	float Result;				//�����ѹֵ������ִ�����ı�����
//	float Integral;				//�������ֵ
//	float Max;
//	float Min;
//}pid;
//
//void PID_init(){
//	printf("PID_init begin \n");
//	pid.Sv=0.0;
//	pid.Pv=0.0;
//	pid.Err=0.0;
//	pid.LastErr=0.0;
//	pid.Result=0.0;
//	pid.Integral=0.0;
//	pid.Kp=0.2;
//	pid.Ki=0.1;       //ע�⣬���ϼ�����ȣ�����Ӵ��˻��ֻ��ڵ�ֵ
//	pid.Kd=0.2;
//	pid.Max=400;
//	pid.Min=-200;
//	printf("PID_init end \n");
//}
//
//float PID_Realize(float Sv){
//	int IntegralEn;
//	pid.Sv	= Sv;
//	pid.Err			= pid.Sv - pid.Pv;
//
//	if(pid.Pv > pid.Max) {         //��ɫ��ɫ��ʾ�����ֱ��͵�ʵ��
//		if(abs(pid.Err) > 200) {                   //��ɫ��עΪ���ַ������
//			IntegralEn=0;
//		} else {
//			IntegralEn=1;
//			if(pid.Err<0) {
//				pid.Integral	+= pid.Err;
//			}
//		}
//	} else if (pid.Pv < pid.Max) {
//		if(abs(pid.Err) > 200) {                   //���ַ������
//			IntegralEn=0;
//		} else {
//			IntegralEn=1;
//			if(pid.Err > 0) {
//				pid.Integral	+= pid.Err;
//			}
//		}
//	} else {
//		if(abs(pid.Err) > 200) {                   //���ַ������
//			IntegralEn=0;
//		} else {
//			IntegralEn=1;
//			pid.Integral	+= pid.Err;
//		}
//	}
//
//	pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn		+
//						pid.Kd *(pid.Err	  - pid.LastErr);
//
//	pid.LastErr	= pid.Err;
//	pid.Pv	= pid.Result * 1.0;
//
//	return pid.Pv;
//}
//���յĲ��Գ������������£��������ԵĿ���ϵͳ���ȶ�ʱ�����ǰ�������������˲��١�
//
//PID�����㷨��C����ʵ���� ���λ��ֵ�PID�����㷨C����ʵ��
//   �ȿ�һ�������㷨�Ļ��ֻ��ڹ�ʽ
//
//   ��ΪPID�����ɵĻ������������������Ϊ�˾�����С��Ӧ��߻��������㾫�ȣ�
//   Ϊ�˿��Խ����λ��ָ�Ϊ���λ��֣�����ʵ�ֵ����Ϊ��
//	pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn/2		+
//						pid.Kd *(pid.Err 	  - pid.LastErr);  //���λ���
//	����������μ���ϵ�н̳����еĽ���
//	���������ȶ�����Ϊ��199.999890���Ͻ̳����е�199.9999390���ԣ����Ƚ�һ����ߡ�
//
//
//PID�����㷨��C����ʵ�ְ� ����ֵ�PID�����㷨C����ʵ��
//   �����PID���Կ����ǻ��ַ����PID�㷨�ĸ�һ�����ʽ������ͨ��PID�����㷨�У�����
//   ����ϵ��ki�ǳ�����   �������������ƹ����У����������ǲ���ġ����ǣ�ϵͳ���ڻ���
//   ���Ҫ���ǣ�ϵͳƫ���ʱ����������Ӧ�ü���������ȫ�ޣ�   ����ƫ��Сʱ����Ӧ�ü�
//   ǿ������ϵ��ȡ���˻�����������������ֱ��ͣ�ȡС���ֲ��ܶ�ʱ�������������ˣ�
//   ����ϵͳ��ƫ���С�ı�����ٶ����б�Ҫ�ġ������PID�Ļ���˼�����跨�ı�������
//   �ۼ��ٶȣ�ʹ����ƫ���С���Ӧ��ƫ��Խ�󣬻���Խ��; ƫ��ԽС������Խ�졣
//   ���������ϵ��ǰ����һ������ֵIntegralEn��
//   ��abs(Err)<180ʱ��IntegralEn=1;
//   ��180<abs(Err)<200ʱ��IntegralEn=��200-abs(Err)��/20;
//   ��abs(Err)>200ʱ��IntegralEn=0;
//   ���յı������ڵı���ϵ��ֵΪki*IntegralEn;
//   ����PIDʵ�ִ������£�
//    pid.Kp=0.4;
//    pid.Ki=0.2;    //�����˻���ϵ��
//    pid.Kd=0.2;
//
//   float PID_Realize(float Sv){
//    float IntegralEn;
//    pid.Sv=Sv;
//    pid.Err=pid.Sv-pid.Pv;
//
//    if(abs(pid.Err)>200)                    //����ֹ���
//    {
//    IntegralEn=0.0;
//    }else if(abs(pid.Err)<180){
//    IntegralEn=1.0;
//    pid.Integral+=pid.Err;
//    }else{
//    IntegralEn=(200-abs(pid.Err))/20;
//    pid.Integral+=pid.Err;
//    }
//    pid.Result		=	pid.Kp * pid.Err				+
//						pid.Ki * pid.Integral * IntegralEn	+
//						pid.Kd * (pid.Err - pid.LastErr);
//
//    pid.LastErr=pid.Err;
//    pid.Pv=pid.Result*1.0;
//    return pid.Pv;
//}
//���ս�����Կ�����ϵͳ���ȶ��ٶȷǳ��죨���Գ���μ���ϵ�н̳�3����
//
//
//
//    pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral				+
//						pid.Kd *(pid.Err	  - pid.LastErr);
//    float incrementSpeed	=
//						pid.Kp * (pid.Err	  - pid.Err_next) + \
//						pid.Ki * (pid.Err) 					+ \
//						pid.Kd * (pid.Err 	  - 2*pid.Err_next + pid.LastErr);
//    pid.Result		= 	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn		+
//						pid.Kd *(pid.Err	  - pid.LastErr);    //�㷨����ʵ�ֹ���
//	pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn		+
//						pid.Kd *(pid.Err	  - pid.LastErr);
//	pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn/2		+
//						pid.Kd *(pid.Err 	  - pid.LastErr);  //���λ���
//    pid.Result		=	pid.Kp * pid.Err					+
//						pid.Ki * pid.Integral * IntegralEn		+
//						pid.Kd * (pid.Err 	  - pid.LastErr);
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif