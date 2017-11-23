/*******************************************************************************
*   Filename:       bsp_key.c
*   Revised:        All copyrights reserved to Roger.
*   Date:           2015-08-11
*   Revision:       v1.0
*   Writer:	     Roger-WY.
*
*   Description:    ��������ģ��
*                   ����ʶ�𳤰����̰���˫������ת�������Ĳ�����
*
*
*   Notes:
*
*   All copyrights reserved to Roger-WY
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <global.h>
#include <bsp_key.h>

#define BSP_KEY_MODULE_EN 1
#if BSP_KEY_MODULE_EN > 0
/*******************************************************************************
* CONSTANTS
*/
#define KEY_SPEED_FAST			5
#define KEY_SPEED_MIDDLE		200
#define KEY_SPEED_SLOW			600


StrKeyEvt Key = {
    NULL,                           // *pkey
    KEY_MENU_HOME,                  // menu
    1,                              // lock
    KEY_EVT_NULL,                   // evt
    0,                              // step
    0,                              // mode
    KEY_VAL_NULL,                   // val
    KEY_SPEED_SLOW,                 // cnt
    0,
    0,
    {0}
};

//============================================================================//



/*******************************************************************************
* ��    �ƣ� KEY_Init
* ��    �ܣ� �������ų�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
void KEY_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOB clocks */
    RCC_APB2PeriphClockCmd(KEY_PORT_RCC,ENABLE);

    /* Set PC.0 ~ PC.6 GPIO_Mode_IPU*/
    GPIO_InitStructure.GPIO_Pin     = KEY_GPIO_PWR
                                    | KEY_GPIO_NRELOAD
                                    | KEY_GPIO_NRESET;

    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_IPU;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

    /* Set PA.11 ~ PA.12 GPIO_Mode_IPU*/
//    GPIO_InitStructure.GPIO_Pin = KEY_GPIO_BMA    | KEY_GPIO_BMB;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*******************************************************************************
* ��    �ƣ� KEY_Init
* ��    �ܣ� �������ų�ʼ��
* ��ڲ����� key �����������ж�Ӧ�¼�
* ���ڲ����� 0���ް�����1���а�ť�¼���2����
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t KEY_GetKey( StrKeyEvt *key )
{
    static  INT16S  speed   = KEY_SPEED_SLOW;
    INT16U          keys;

    /***********************************************
    * �����������ת�����������ȼ���ߣ�
    */
//    if( KEY_GetEncodingKey(key) )
//        return TRUE;
    /***********************************************
    * ��������ť��ס
    */
    keys = KEY_KEYS;                                // ��ʼ����ֵ

    /***********************************************
    * �������м����»�ס
    */
    if ( ( keys & GPIO_ReadInputData(KEY_PORT) ) != keys ) {
//#if ( OSAL_EN == DEF_ENABLED )
//        static INT08U   flag    = 0;
//        flag    = ~flag;
//        if ( flag ) {
//            osal_start_timerRl( OS_TASK_ID_KEY,
//                                (1<<0),
//                                5);
//        } else {
//#else
        BSP_OS_TimeDly(5);                               // ��ʱһ��
//#endif
        if ( ( keys & GPIO_ReadInputData(KEY_PORT) ) != keys ) {        // ��Ȼ�м�����
            keys    = GPIO_ReadInputData(KEY_PORT);                     // ��ȡ��ֵ
            keys   &= KEY_KEYS;
            key->val= keys;
            /***********************************************
            * ����������
            */
            if ( ( key->evt == KEY_EVT_NULL ) ||
                ( key->evt == KEY_EVT_UP ) ) {
                    key->val	= keys;
                    key->evt	= KEY_EVT_DOWN;
                    key->cnt	= KEY_SPEED_SLOW;
                    speed       = KEY_SPEED_SLOW;
                    //UartPutString( (CHAR*)"\r\n����" );
                    return	KEY_STATUS_EVENT;
                }

            /***********************************************
            * ��������ס
            */
            if ( --key->cnt == 0 ) {
                speed   -= 10;
                if ( speed <= KEY_SPEED_FAST ) {
                    speed   = KEY_SPEED_FAST;
                }

                key->cnt = speed;
                //UartPutString( COMM_PORT_MSG,"\r\n��ס" );
                key->evt	= KEY_EVT_PRESSED;
                return KEY_STATUS_EVENT;
            }
            return KEY_STATUS_PRESSED;
        }
//#if ( OSAL_EN == DEF_ENABLED )
//        }
//#else
//#endif
    } else {
//#if ( OSAL_EN == DEF_ENABLED )
//        osal_start_timerRl( OS_TASK_ID_KEY,
//                            (1<<0),
//                            5);
//#else
        BSP_OS_TimeDly(5);                               // ��ʱһ��
//#endif
    }

    /***********************************************
    * �������ɿ�
    */
    if ( key->evt == KEY_EVT_DOWN || key->evt == KEY_EVT_PRESSED ) {
        key->evt	= KEY_EVT_UP;
        key->cnt	= KEY_SPEED_SLOW;
        //UartPutString( (CHAR*)"\r\n�ɿ�" );
        return KEY_STATUS_EVENT;
        /***********************************************
        * ������û�а���
        */
    } else {
        key->val	= KEY_VAL_NULL;
        key->evt	= KEY_EVT_NULL;
        key->cnt	= KEY_SPEED_SLOW;
        return KEY_STATUS_NULL;
    }
}


/*******************************************************************************
�о��˺����ӵı��뿪�س��򣬷����жϷ���ɨ�跨���޷��ܺõ�ȥ����
���������Ͽ����������ִ���ʽ������֤����ɿ��أ��ǳ��õر����˶������⡣
��������ת�������������������
______        ______        ______        ______
��תʱ:  A��   ___|      |______|      |______|      |______|

______        ______        ______        ___
B��   ______|      |______|      |______|      |______|

��λΪA��         01  11 10  00 01  11 10  00 01  11 10  00 01  11
______        ______        ______        ___
��תʱ:  A��   ______|      |______|      |______|      |______|
______        ______        ______        ______
B��   ___|      |______|      |______|      |______|
��λΪA��         10 11  01  00 10  11 01  00 10  11 01  00 10  11

A�����ĳ�˿��ϣ���Ӧ������Ϊ��if((REG32(P4SIN) & (1<<3)))  PhaseShift |= 0x01;
B�����ĳ�˿��ϣ���Ӧ������Ϊ��if((REG32(P1SIN) & (1<<2)))  PhaseShift |= 0x02;

PhaseShift��<1:0>��λ��ʾ��ǰAB�����״̬��<3:2>��λ��ʾ��һ��AB�����״̬
��תʱ��AB��仯˳��Ϊ   01->11->10->00->01    ��λΪA��
�����תʱ��PhaseShift�ĵ���λֵΪ 0111 1110 1000 0001
��תʱ��AB��仯˳��Ϊ   10->11->01->00->10    ��λΪA��
��˷�תʱ��PhaseShift�ĵ���λֵΪ 1011 1101 0100 0010
*******************************************************************************/
/***********************************************
* ��������λ����
*/
#define phase_no_move   0       // 0000 δת��
#define phase_dec      -1       // 0001 ��ת
#define phase_inc       1       // 0010 ��ת
#define phase_bad       2       // 0011 ������

/***********************************************
* ������������תΪ������תΪ�ӣ���ɵ�����һ��
*/
const INT8S table_Phase[16]=
{
    phase_no_move,    // 0000 δת��
    phase_dec,        // 0001 ��ת
    phase_inc,        // 0010 ��ת
    phase_bad,        // 0011 ������

    phase_inc,        // 0100 ��ת
    phase_no_move,    // 0101 δת��
    phase_bad,        // 0110 ������
    phase_dec,        // 0111 ��ת

    phase_dec,        // 1000 ��ת
    phase_bad,        // 1001 ������
    phase_no_move,    // 1010 δת��
    phase_inc,        // 1011 ��ת

    phase_bad,        // 1100 ������
    phase_inc,        // 1101 ��ת
    phase_dec,        // 1110 ��ת
    phase_no_move,    // 1111 δת��

};

/*******************************************************************************
* ��    �ƣ� KEY_GetEncodingKey
* ��    �ܣ� ��ȡ��ת�������ļ�ֵ
* ��ڲ����� key �����������ж�Ӧ�¼�
* ���ڲ����� 0���ް�����1���а�ť�¼���2����
* �������ߣ� Roger-WY.
* �������ڣ� 2015-06-25
* ��    �ģ�
* �޸����ڣ�
* ��    ע��
*******************************************************************************/
uint8_t KEY_GetEncodingKey ( StrKeyEvt *key )
{
    CPU_INT32U pin;

    static CPU_INT32U PhaseShift = 0;
    static CPU_INT32S PhaseCount = 0;

    /***********************************************
    * ������A�����ĳ�˿��ϣ���Ӧ������Ϊ
    */
    pin = GPIO_ReadInputDataBit(KEY_PORT_BM, KEY_GPIO_BMA);
    if (pin == 0)
        PhaseShift |= 0x01;

    /***********************************************
    * ������B�����ĳ�˿��ϣ���Ӧ������Ϊ
    */
    pin = GPIO_ReadInputDataBit(KEY_PORT_BM, KEY_GPIO_BMB);
    if (pin == 0)
        PhaseShift |= 0x02;


    /***********************************************
    * ������ȡ��4λ
    */
    PhaseShift &= 0x0f;

    /***********************************************
    * ����������PhaseShift�ĵ���λֵ����˱��ɵ�ת������
    */
    if(table_Phase[PhaseShift] == phase_inc)
        PhaseCount++;
    if(table_Phase[PhaseShift] == phase_dec)
        PhaseCount--;
    PhaseShift <<= 2;
    /***********************************************
    * ��������ͬһ�����ϵ�ת������Ϊ4��ʱ��
    *       ����Ϊ��һ����Ч��ת�����ﵽ��ȥ����Ŀ��
    */
    if(PhaseCount > 3) {
        PhaseCount -= 4;
        key->val    = KEY_INC;
        key->evt    = KEY_EVT_UP;
        return KEY_STATUS_EVENT;                    // phase_inc;
    } else if(PhaseCount < -3) {
        PhaseCount += 4;
        key->val    = KEY_DEC;
        key->evt    = KEY_EVT_UP;
        return KEY_STATUS_EVENT;                    // phase_dec;
    }

    return KEY_STATUS_NULL;                         // (INT8S)(phase_no_move);
}
/*******************************************************************************
* 				end of file
*******************************************************************************/
#endif