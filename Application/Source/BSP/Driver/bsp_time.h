
/*******************************************************************************
 *   Filename:       bsp_time.h
 *   Revised:        $Date: 2014-05-27$
 *   Revision:       $
 *	 Writer:		 Roger-WY.
 *
 *   Description:
 *   Notes:
 *
 *
 *   All copyrights reserved to Roger-WY.
 *
 *******************************************************************************/

#ifndef	__BSP_TIME_H__
#define	__BSP_TIME_H__

/*******************************************************************************
 * INCLUDES
 */
#include  "includes.h"
#include  "global.h"
#include  <time.h>   /*����ANSI-C�ı�׼time.h*/


/*******************************************************************************
 * ������  ��tm_now�ṹ���ж���ʵʱʱ��
 */
extern struct tm   tm_now;


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
struct tm    TIME_ConvUnixToCalendar (time_t t);
time_t       TIME_ConvCalendarToUnix (struct tm t);
time_t       TIME_GetUnixTime        (void);
void         TIME_SetUnixTime        (time_t);


struct tm    TIME_GetCalendarTime    (void);          /*��ȡʵʱʱ��*/
void         TIME_SetCalendarTime    (struct tm t);   /*����ʵʱʱ��*/
void         BSP_TIME_Init           (void);          /*ʵʱʱ���ʼ��*/



/*******************************************************************************
 *              end of file                                                    *
 *******************************************************************************/
#endif