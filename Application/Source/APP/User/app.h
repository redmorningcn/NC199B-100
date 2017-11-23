/*******************************************************************************
 *   Filename:       app.h
 *   Revised:        $Date: 2015-03-11$
 *   Revision:       $
 *	 Writer:		 wumingshen.
 *
 *   Description:
 *   Notes:
 *
 *     				E-mail:shenchangwei945@163.com
 *
 *   All copyrights reserved to wumingshen.
 *
 *******************************************************************************/

#ifndef	APP_H
#define	APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
//#include <includes.h>
#include <app_ctrl.h>
#include <app_lib.h>
    
#ifdef   APP_GLOBALS
#define  APP_EXT
#else
#define  APP_EXT  extern
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
 * TYPEDEFS
 */

/***********************************************
* ������ ��ʱ���ⲿ��������
*/
extern  OS_TMR          OSTmr0;                   // ��ʱ��
extern  OS_SEM          SemBeepOn;
/***********************************************
* ������ ��ʱ���ص������ⲿ��������
*/
extern void  OSTmr0_callback(OS_TMR *ptmr,void *p_arg);

/***********************************************
* ������ ���Ź���־���ⲿ��������
*/
extern OS_FLAG_GRP      WdtFlagGRP;             //���Ź���־��
extern OS_FLAGS         WdtFlags;

extern OS_FLAG_GRP      BeepFlagGrp;            //��������־��
extern OS_FLAGS         BeepFlags;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
/***********************************************************
* ������ ���Ź���־��λ˵��:
*        ÿ���������һ�����Ź���־λ���Է���������쳣������
*        ����ֻд��4����־λ����������ӡ�
*        ��־����32λ��һ����־���������32����־λ
*/
#define WDT_FLAG_WIFI               (1<<0)    // 0X00000001
#define WDT_FLAG_COMM               (1<<0)    // 0X00000001
#define WDT_FLAG_SEN                (1<<1)    // 0X00000002
#define WDT_FLAG_KEY                (1<<2)    // 0X00000004
#define WDT_FLAG_CTRL               (1<<3)    // 0X00000008
#define WDT_FLAG_BEEP               (1<<4)    // 0X00000010
#define WDT_FLAG_OSAL               (1<<5)    // 0X00000020
#define WDT_FLAG_DISP               (1<<6)    // 0X00000040

/***********************************************************
* ������ ������־λ
*/
#define BEEP_FLAG_PWR_ON            (1<<0)    // 0X00000001
#define BEEP_FLAG_PWR_OFF           (1<<1)    // 0X00000002
#define BEEP_FLAG_KEY_EVT           (1<<2)    // 0X00000004
#define BEEP_FLAG_LOCK_EVT          (1<<3)    // 0X00000008

/***********************************************************
* ������ ���ػ�״̬
*/
#define SYS_PWR_OFF                 0
#define SYS_PWR_ON                  1
#define SYS_WIFI_CONFIG             2

/***********************************************************
* ������ WIFI״̬
*/
#define WIFI_STS_OK                 0
#define WIFI_STS_CONFIG             1
#define WIFI_STS_CONFIG_OK          2
#define WIFI_STS_CONFIG_ERR         3

/***********************************************************
* ������ LED��ɫ����
*/
#define LED_CH_R                    2
#define LED_CH_G                    3
#define LED_CH_B                    1

/***********************************************************
* ������ LED��ɫ����
*/
#define PWM_OUT_NEGTIVE             DEF_DISABLED

#define FAN_DUTY_STOP               0.0
#define FAN_DUTY_SCALE_MIN          500.0
#define FAN_DUTY_SCALE_0            680.0
#define FAN_DUTY_SCALE_1            720.0
#define FAN_DUTY_SCALE_2            740.0
#define FAN_DUTY_SCALE_3            780.0
#define FAN_DUTY_SCALE_4            820.0
#define FAN_DUTY_SCALE_5            820.0
#define FAN_DUTY_SCALE_MAX          900.0
//#endif
/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
/******************************************************
* ������ ���񴴽�������ÿ�������Ĵ����������ڴ˴�����
*/
void    App_TaskSampleCreate        (void);
void    App_TaskWifiCreate          (void);
void    App_TaskCommCreate          (void);
void    App_TaskSensorCreate        (void);
void    App_TaskKeyCreate           (void);
void    App_TaskControlCreate       (void);
void    App_TaskBeepCreate          (void);
void    App_TaskOsalCreate          (void);
void    App_TaskDispCreate          (void);

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