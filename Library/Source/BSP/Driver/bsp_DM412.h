/*******************************************************************************
 *   Revised:        $Date: 2017-11-15$
 *   Revision:       $
 *   Writer:	     redmorningcn.
 *   Description:    
 *   Notes:
 *     				E-mail:redmorningcn@qq.com
 *
 *   All copyrights reserved to redmorningcn.
 *
 *******************************************************************************/
#ifndef  _BSP_DM412_h_
#define  _BSP_DM412_h_

/* Define to prevent recursive inclusion -------------------------------------*/
//#include    <bsp_gpio.h>

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

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *
 * 返回类型         函数名称                入口参数
 */

/**-------------------------------------------------------
* @函数名  bsp_DM412Init
* @功能    初始化LED1的端口
author:    redmorningcn
data:      2017-11-13
***------------------------------------------------------*/
void bsp_DM412Init(void);

//---------------------------------------------------------------------------------
//功    能:  对led进行颜色设置
//入口参数:  需要连续发送三次，再发送一次装载指令，才能将数据下载至DM412           
//author:    redmorningcn，颜色为RGB，
//data:      2017-11-13
//----------------------------------------------------------------------------------
void    SetLedColor(uint8 lenuum,uint16  red,uint16  green,uint16  blue);



#endif


