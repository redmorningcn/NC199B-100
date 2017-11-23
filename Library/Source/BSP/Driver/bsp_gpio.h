/*******************************************************************************
 *   Revised:        $Date: 2017-10-15$
 *   Revision:       $
 *   Writer:	     redmorningcn.
 *
 *   Description:    ST MCU  GPIO 
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/


#ifndef  _BSP_GPIO_h_
#define  _BSP_GPIO_h_


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * INCLUDES
 */
//#include <includes.h>
#include <global.h>

#ifdef   BSP_GLOBALS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
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

/*******************************************************************************
 * GLOBAL VARIABLES
 */
 
/*******************************************************************************
 * LOCAL FUNCTIONS
 */


void	GPIO_SetOrClearValue(uint32_t GpioNum,uint8_t Val);
void	GPIO_PinselConfig(uint32_t GpioNum,uint8_t FuncNum);
uint8	GPIO_ReadIoVal(uint32_t GpioNum);




#endif
