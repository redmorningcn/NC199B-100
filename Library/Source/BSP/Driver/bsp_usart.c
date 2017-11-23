/*******************************************************************************
 *   Filename:       bsp_usart.c
 *   Revised:        All copyrights reserved to wumingshen.
 *   Revision:       v1.0
 *   Writer:	     wumingshen.
 *
 *   Description:    ��������ģ��
 *   Notes:          �жϳ���δ�Է��ͻ�������������.Ҳ����˵����ComSendBuf()����ʱ��
 *		             ������ٵķ��ͻ��������㣬���������ȷ���FIFO�����ݱ������
 *		             ��������һ���Ӧ�ó��򣬽����ͻ����������㹻���Ա�֤һ�����ͨ��֡�ܹ������¼��ɡ�
 *     				 E-mail:shenchangwei945@163.com
 *
 *******************************************************************************/
#include "includes.h"
#include "bsp_usart.h"

#define    UCOS_EN        DEF_ENABLED


/*******************************************************************************
 * ��    �ƣ� InitHardUart
 * ��    �ܣ� ����USARTӲ������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void BSP_UartCfg(u8 port_nbr,u32  baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    /***********************************************
    * ������ ����3���Žӵ��أ�����ʹ��
    */
    if ( port_nbr > 3 )
        return;
    
    switch ( port_nbr ) {
#if defined (Open_UART1)
    case 1:
	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
    ����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
    ���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��4�������ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	
    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    /*ע��: ��Ҫ�ڴ˴��򿪷����ж�
    �����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */

	/***********************************************
    * ������ CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
	*	     �����������1���ֽ��޷���ȷ���ͳ�ȥ�����⣺
	*	     �巢����ɱ�־��Transmission Complete flag
    */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */

#if  (UCOS_EN == DEF_ENABLED)
    /***********************************************
    * �������жϺ���ӳ�䴦����USART1���жϷ�������ڵ�ַӳ�䵽
    *       USARTx_RxTxISRHandler�ĺ�����ڵ�ַ�ϣ���ʹ�ܡ�
    */
    BSP_IntVectSet(BSP_INT_ID_USART1, USART1_IRQHandler);
    BSP_IntEn(BSP_INT_ID_USART1);

#else
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*�ڲ�ʹ��UCOSϵͳʱ����USART1_IRQHandler��������stm32_it.c�ļ��Ĵ����жϺ�����*/
#endif

        break;
#endif /*end of open_uart1*/

#if defined (Open_UART2)
    case 2:
	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��4�������ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
    ע��: ��Ҫ�ڴ˴��򿪷����ж�
    �����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */

	/***********************************************
    * ������ CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
	*	     �����������1���ֽ��޷���ȷ���ͳ�ȥ�����⣺
	*	     �巢����ɱ�־��Transmission Complete flag
    */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */

#if  (UCOS_EN == DEF_ENABLED)
    /***********************************************
    * �������жϺ���ӳ�䴦����USART1���жϷ�������ڵ�ַӳ�䵽
    *       USARTx_RxTxISRHandler�ĺ�����ڵ�ַ�ϣ���ʹ�ܡ�
    */
    BSP_IntVectSet(BSP_INT_ID_USART2, USART2_IRQHandler);
    BSP_IntEn(BSP_INT_ID_USART2);

#else
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*�ڲ�ʹ��UCOSϵͳʱ����USART2_IRQHandler��������stm32_it.c�ļ��Ĵ����жϺ�����*/
#endif

        break;
#endif /*end of open_uart2*/
#if defined (Open_UART3)

    case 3:
	/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;//GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    //    return ;
	/* ��4�������ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    ע��: ��Ҫ�ڴ˴��򿪷����ж�
    �����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */

	/***********************************************
    * ������ CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
	*	     �����������1���ֽ��޷���ȷ���ͳ�ȥ�����⣺
	*	     �巢����ɱ�־��Transmission Complete flag
    */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */

#if  (UCOS_EN == DEF_ENABLED)
    /***********************************************
    * �������жϺ���ӳ�䴦����USART1���жϷ�������ڵ�ַӳ�䵽
    *       USARTx_RxTxISRHandler�ĺ�����ڵ�ַ�ϣ���ʹ�ܡ�
    */
    BSP_IntVectSet(BSP_INT_ID_USART3, USART3_IRQHandler);
    BSP_IntEn(BSP_INT_ID_USART3);

#else
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*�ڲ�ʹ��UCOSϵͳʱ����USART3_IRQHandler��������stm32_it.c�ļ��Ĵ����жϺ�����*/
#endif

    break;
#endif /*end of open_uart3*/
    }
}

/*******************************************************************************
 * ��    �ƣ� UartIRQ
 * ��    �ܣ� ���жϷ��������ã�ͨ�ô����жϴ�������
 * ��ڲ����� _pUart: ���ڽṹ��ָ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
#if defined(PM2D5_UART_CHK)
u8  Uart1RxBuf[11];
u8  Uart1RxCtr  = 0;
#endif
static void  USARTx_RxTxISRHandler (USART_TypeDef* USARTx)
{
    uint8_t t_receivedata = 0;
    /***********************************************
    * �����������ж�
    */
    if (SET == USART_GetFlagStatus(USARTx, USART_FLAG_RXNE)) {
        /***********************************************
        * ����������һ���ֽ�
        */    
#if defined(PM2D5_UART_CHK)
        Uart1RxBuf[Uart1RxCtr++]   = USART_ReceiveData(USARTx);
        if ( Uart1RxCtr > 10 )
            Uart1RxCtr = 0;
        
        osal_start_timerEx( OS_TASK_ID_SENSOR,
                           OS_EVT_SEN_UART_TIMEOUT,
                            20);
#else
        t_receivedata   = USART_ReceiveData(USARTx);
        t_receivedata   = t_receivedata; 
#endif
        USART_ClearITPendingBit(USARTx, USART_IT_RXNE);         /* Clear the USARTx receive interrupt.                  */
    }

    /***********************************************
    * �����������ж�
    */
    if (SET == USART_GetFlagStatus(USARTx, USART_FLAG_TC)) {


        USART_ClearITPendingBit(USARTx, USART_IT_TC);           /*������ڷ����жϱ�־λ*/
        USART_ITConfig(USARTx, USART_IT_TC, DISABLE);           /*�رմ��ڷ����ж�*/
    }
}

/*******************************************************************************
 * ��    �ƣ� USART1_IRQHandler
 * ��    �ܣ� USART1�жϷ������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART1_IRQHandler(void)
{
	USARTx_RxTxISRHandler (USART1);
}

/*******************************************************************************
 * ��    �ƣ� USART2_IRQHandler
 * ��    �ܣ� USART2�жϷ������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART2_IRQHandler(void)
{
	USARTx_RxTxISRHandler (USART2);
//     static u16   recvcont = 0;

//    /***********************************************
//    * �����������ж�
//    */
//    if (SET == USART_GetFlagStatus(USART2, USART_FLAG_RXNE)) {
//         extern u8 PM2D5_UartBuf[];
//         PM2D5_UartBuf[recvcont] = USART_ReceiveData(USART2);
//         if(++recvcont >= 14) {
//            recvcont = 0;
//            Ctrl.PM2D5.RxFaildCtr   = 0;
//         }
//         USART_ClearITPendingBit(USART2, USART_IT_RXNE);                /* Clear the USART2 receive interrupt.                  */
//     }
//
//    /***********************************************
//    * �����������ж�
//    */
//    if (SET == USART_GetFlagStatus(USART2, USART_FLAG_TC)) {
//        USART_ITConfig(USART2, USART_IT_TC, DISABLE);
//        USART_ClearITPendingBit(USART2, USART_IT_TC);                   /* Clear the USART2 receive interrupt.                */
//    }
}

/*******************************************************************************
 * ��    �ƣ� USART3_IRQHandler
 * ��    �ܣ� USART3�жϷ������
 * ��ڲ����� ��
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
//extern UART_HandleTypeDef  				Ctrl.Wifi.UartHandle;
//extern Device_ReadTypeDef               Device_ReadStruct;
void USART3_IRQHandler(void)
{
	USARTx_RxTxISRHandler (USART3);
//    uint8_t 	value;
//    OS_ERR      err;
//    
//    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
//		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
//		value = USART_ReceiveData(USART3);
//        /***********************************************
//        * ������ ����WIFI���ڽ��ճ�ʱ��־λ
//        */
//        //OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
//        //            ( OS_FLAGS      )WIFI_EVT_FLAG_TIMEOUT,
//        //            ( OS_OPT        )OS_OPT_POST_FLAG_SET,
//        //            ( OS_ERR       *)&err); 
//        Ctrl.Wifi.UartHandle.UART_RecvFlag         = 1;
//        Ctrl.Wifi.UartHandle.UART_RecvTimeoutCtr   = 0;
//        /***********************************************
//        * ������ ׼����ȡһ������
//        */
//		if(Ctrl.Wifi.UartHandle.Package_Flag != 0 ) {
//            if ( Ctrl.Wifi.UartHandle.UART_Count >= Max_UartBuf ){
//                Ctrl.Wifi.UartHandle.UART_Count    = 0;
//                Ctrl.Wifi.UartHandle.Package_Flag  = 0;
//            }
//		} else {
//            /***********************************************
//            * ������ �жϵ�һ���ֽ��ǲ�������֡ͷ0XFF
//            */
//			if(Ctrl.Wifi.UartHandle.UART_Flag1 ==0) {
//				if(value == 0xff) {
//                    Ctrl.Wifi.UartHandle.UART_Count = 0;
//                    Ctrl.Wifi.UartHandle.UART_Buf[Ctrl.Wifi.UartHandle.UART_Count]=value;
//                    Ctrl.Wifi.UartHandle.UART_Count++;
//                    Ctrl.Wifi.UartHandle.UART_Flag1 = 1;
//				}
//				return ;
//                /***********************************************
//                * ������ �жϵڶ����ֽ��ǲ���0XFF
//                */
//			} else if(Ctrl.Wifi.UartHandle.UART_Flag2 ==0) {
//                Ctrl.Wifi.UartHandle.UART_Buf[Ctrl.Wifi.UartHandle.UART_Count]=value;
//                Ctrl.Wifi.UartHandle.UART_Count++;
//                /***********************************************
//                * ������ �ڶ����ֽ���0XFF��������һ������
//                */
//                if(Ctrl.Wifi.UartHandle.UART_Buf[1] == 0xff) {
//                    Ctrl.Wifi.UartHandle.UART_Flag2 = 1;
//                    /***********************************************
//                    * ������ �ڶ����ֽڲ���0XFF���¿�ʼ��������
//                    */
//                } else {
//                    Ctrl.Wifi.UartHandle.UART_Flag1 = 0;
//                }
//                return ;
//                /***********************************************
//                * ������ ����һ�����ֽ���ȷʱ������֮����ֽ�
//                */
//			} else {
//                /***********************************************
//                * �����������ڽ��ܻ�������Ž�������
//                */
//				Ctrl.Wifi.UartHandle.UART_Buf[Ctrl.Wifi.UartHandle.UART_Count] = value;
//                /***********************************************
//                * �����������ڽ��ܻ�������Ž�������
//                */
//				if(Ctrl.Wifi.UartHandle.UART_Count >=4 && Ctrl.Wifi.UartHandle.UART_Buf[Ctrl.Wifi.UartHandle.UART_Count] == 0x55 && \
//                    Ctrl.Wifi.UartHandle.UART_Buf[Ctrl.Wifi.UartHandle.UART_Count - 1] == 0xFF){
//                        //������
//                    } else {
//                        Ctrl.Wifi.UartHandle.UART_Count++;
//                    }
//                
//				if(Ctrl.Wifi.UartHandle.UART_Count == 0x04) {
//					Ctrl.Wifi.UartHandle.UART_Cmd_len = Ctrl.Wifi.UartHandle.UART_Buf[2]*256+  Ctrl.Wifi.UartHandle.UART_Buf[3];
//                    //============================================//
//                    /***********************************************
//                    * �������öδ������м���,�յ��ĳ��ȴ���ʱ�����½�������
//                    */
//                    if((Ctrl.Wifi.UartHandle.UART_Cmd_len >= 255) || (Ctrl.Wifi.UartHandle.UART_Cmd_len  == 0)) {
//                        Ctrl.Wifi.UartHandle.UART_Cmd_len = 0;
//                        Ctrl.Wifi.UartHandle.UART_Flag1 = 0;
//                        Ctrl.Wifi.UartHandle.UART_Flag2 = 0;
//                        return;
//                    }
//                    //============================================//
//				}
//				if(Ctrl.Wifi.UartHandle.UART_Count ==  (Ctrl.Wifi.UartHandle.UART_Cmd_len + 4)) {
//					//OSSemPost ((OS_SEM  *)&SemUsartPackageFlag,
//                    //           (OS_OPT   )OS_OPT_POST_1,
//                    //           (OS_ERR  *)&err);
//                    
//                    /***********************************************
//                    * ������ ����WIFI���ڽ��ձ�־λ
//                    */
//                    OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
//                               ( OS_FLAGS      )WIFI_EVT_FLAG_RECV,
//                               ( OS_OPT        )OS_OPT_POST_FLAG_SET,
//                               ( OS_ERR       *)&err); 
//                    /***********************************************
//                    * ������ ����WIFI���ڽ��ճ�ʱ��־λ
//                    */
//                    //OSFlagPost( ( OS_FLAG_GRP  *)&Ctrl.Os.WifiEvtFlagGrp,
//                    //            ( OS_FLAGS      )WIFI_EVT_FLAG_TIMEOUT,
//                    //            ( OS_OPT        )OS_OPT_POST_FLAG_CLR,
//                    //            ( OS_ERR       *)&err); 
//                    
//                    
//                    Ctrl.Wifi.UartHandle.UART_RecvFlag = 0;
//                    Ctrl.Wifi.UartHandle.Package_Flag  = 1;
//					Ctrl.Wifi.UartHandle.UART_Flag1    = 0;
//					Ctrl.Wifi.UartHandle.UART_Flag2    = 0;
//                    //printf("Package_Flag =1 \r\n");
//				}
//			}
//		}
//    }
}

/*******************************************************************************
 * ��    �ƣ� USART_OPen_INT
 * ��    �ܣ� ��ĳ�����ڵ��ж�
 * ��ڲ����� USART_TypeDef* USARTx ���ں�
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART_OPen_INT(USART_TypeDef* USARTx)
{
    USART_ClearITPendingBit(USARTx,USART_IT_RXNE);
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

}

/*******************************************************************************
 * ��    �ƣ� USART_Close_INT
 * ��    �ܣ� �ر�ĳ�����ڵ��ж�
 * ��ڲ����� USART_TypeDef* USARTx ���ں�
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART_Close_INT(USART_TypeDef* USARTx)
{
    USART_ITConfig(USARTx, USART_IT_RXNE, DISABLE);

}

/*******************************************************************************
 * ��    �ƣ� USART_Send_Char
 * ��    �ܣ� ��ѯ������һ���ֽ�
 * ��ڲ����� USART_TypeDef* USARTx ���ں� ��data ����Ҫ���͵��ֽ�
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART_Send_Char(USART_TypeDef* USARTx,unsigned char data)
{
    USART_ClearFlag(USARTx,USART_FLAG_TC);
    USART_SendData(USARTx,data);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);     //�ȴ��������
}

/*******************************************************************************
 * ��    �ƣ� USART_Send_String
 * ��    �ܣ� ��ѯ������һ���ַ���
 * ��ڲ����� USART_TypeDef* USARTx ���ں� ��unsigned char *buffer����Ҫ���͵��ַ���
 *            unsigned int len �� �ַ�������
 * ���ڲ����� ��
 * ���� ���ߣ� ������.
 * �������ڣ� 2015-06-25
 * ��    �ģ�
 * �޸����ڣ�
 * ��    ע��
 *******************************************************************************/
void USART_WriteBytes(USART_TypeDef* USARTx,unsigned char *buffer,unsigned int len)
{
    if ( len ) {
        while(len--) {
            USART_Send_Char(USARTx,*buffer);
            buffer++;
        }
    } else {        
        while(*buffer) {
            USART_Send_Char(USARTx,*buffer);
            buffer++;
        }
    }
}

