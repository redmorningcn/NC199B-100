#ifndef	BSP_PID_H
#define	BSP_PID_H
/*******************************************************************************
 *   Filename:       bsp_pid.h
 *   Revised:        $Date: 2013-11-08	20:15 (Fri) $
 *   Revision:       $
 *	 Writer:		 Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *					QQ:276193028
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
 #include  <cpu.h>

/*******************************************************************************
 * CONSTANTS
 */
#define PID_POLARITY_BIPOLAR            0           // ������
#define PID_POLARITY_POSITIVE           0           // ������
#define PID_POLARITY_NEGATIVE           1           // ������
#define PID_POLARITY_UNIPOLAR           2           // ˫����

#define PID_TYPE_DIGITAL                0           // ����
#define PID_TYPE_ANALOG                 1           // ģ��
/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */
 /***********************************************
 * ������PID�������Ͷ���
 */
typedef struct _PID {
    // ������
	CPU_INT16S 		    Pv; 							// ������
	CPU_INT16S 		    Sv; 							// �趨ֵ

	float 		        Kp; 							// ����ϵ��
	float 		        Ki; 							// ����ϵ��
	float 		        Kd; 							// ΢��ϵ��
	float 		        Integral; 						// ����ֵ ���� ƫ���ۼ�ֵ

	CPU_INT16S 		    Err; 							// ƫ��ֵ
	CPU_INT16S 		    LastErr; 						// �ϴ�ƫ��ֵ

	CPU_INT08U 		    DeadBand;    					// ����
    CPU_INT08U          PidEnBand;                      // ��ʼPID��������ֵ

    // ������
    CPU_INT16S          MinPv;                          // ��������Сֵ
    CPU_INT16S          MaxPv;                          // ���������ֵ
    CPU_INT16S          HiLimit;                        // �趨ֵ��Сֵ
    CPU_INT16S          LoLimit;                        // �趨ֵ���ֵ
    CPU_INT16S          MinOut;                         // �����Сֵ��ռ�ձȻ�������ȣ�
    CPU_INT16S          MaxOut;                         // ������ֵ���������ڻ�������ȣ�
    CPU_INT16U          Ti;                             // ����ʱ��
    CPU_INT16U          Td;                             // ΢��ʱ��
    CPU_INT16U          T;                              // ��������

    CPU_BOOLEAN         InType              : 1;         // ��������ģ��������
    CPU_BOOLEAN         InPolarity          : 2;         // ģ�������뼫��
    CPU_BOOLEAN         OutType             : 1;         // ��������ģ�������
    CPU_BOOLEAN         OutPolarity         : 2;         // ģ�����������
    CPU_BOOLEAN         AlarmLFlag          : 1;         // PV�ͱ���
    CPU_BOOLEAN         AlarmHFlag          : 1;         // PV�߱���


    CPU_INT16S          Ctr;                             // �򿪼�����
    CPU_INT16S          OnDly;                           // �򿪼�����
    CPU_INT16S          OffDly;                          // �رռ�����

    CPU_INT08S          POut;                            // ���������-100 ~ 100��

} StrPid;

extern StrPid   *pid;
/*******************************************************************************
 * GLOBAL VARIABLES
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
void                PID_Init                (StrPid         *HeatPid,
                                             int            pv,
                                             int            sv);
void                PID_Tune                (StrPid         *pid,
                                             float          p_gain,
                                             float          i_gain,
                                             float          d_gain,
                                             float          integral,
                                             CPU_INT16S     dead_band,
                                             CPU_INT16S     MaxIn,
                                             CPU_INT16S     MaxOut,
                                             CPU_INT16S     PidBand);
void                PID_SetInteral          (StrPid         *pid,
                                             float          new_integral);
void                PID_Bumpless            (StrPid         *pid);
float               PID_Calc                (StrPid         *pid);
void                BSP_PID_Main            (void);
/*******************************************************************************
 * 				end of file
 *******************************************************************************/
#endif	/* GLOBLES_H */