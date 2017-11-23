/*******************************************************************************
 *   Filename:       osal_timer.h
 *   Revised:        $Date: 2013-04-17 $
 *   Revision:       $
 *   Writer:         Wuming Shen.
 *
 *   Description:
 *
 *   Notes:
 *
 *
 *   All copyrights reserved to Wuming Shen.
 *
 *******************************************************************************/
#ifndef BSP_HAL_H
#define	BSP_HAL_H

#ifdef	__cplusplus
extern "C" {
#endif
/*******************************************************************************
 * INCLUDES
 */
#include <osal.h>

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * MACROS
 */
typedef struct {
    INT08U  Hour;
    INT08U  Min;
    INT08U  Sec;
    INT16U  Msec;
} TimerStr;
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
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
void        HAL_ProcessPoll         ( void );
void        HAL_StartTimer          ( TimerStr *timer, INT08U time );
void        HAL_StopTimer           ( TimerStr *timer );

void        HAL_HTimerPoll          (void);
void        HAL_MTimerPoll          (void);
void        HAL_STimerPoll          (void);

/*******************************************************************************
 * 				     end of file                               *
 *******************************************************************************/
#ifdef	__cplusplus
}
#endif
#endif	/* BSP_HAL_H */