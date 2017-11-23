/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                           (c) Copyright 2009-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*                 DC
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                          DEF_DISABLED


/****************************************************
* ������ ϵͳʹ�õ���ModBus�ӻ��ڵ�վ��
*/
#define ModbusNode                                  2u

/*
*********************************************************************************************************
*                                           ������������ȼ�
*                                            TASK PRIORITIES
*                �����������ȼ���uCOS-III�����ȼ�������ͬ��ʹ��ʱ��Ƭת��ѯ�����ͬ���ȼ������ִ��
*                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*                        !!!�������ȼ��������Ҫ�����ȼ���Ӱ������ϵͳ������!!!
*                           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*********************************************************************************************************
*/


#define  MB_OS_CFG_RX_TASK_PRIO     (OS_CFG_PRIO_MAX - 14)  // ModBus��������ȼ���û���õ�ModBus����Ҫ��ע��
#define  APP_TASK_START_PRIO        (OS_CFG_PRIO_MAX - 13)  
#define  APP_TASK_WIFI_PRIO         (OS_CFG_PRIO_MAX - 12) 
#define  APP_TASK_COMM_PRIO         (OS_CFG_PRIO_MAX - 12)  
#define  APP_TASK_SENSOR_PRIO       (OS_CFG_PRIO_MAX - 11)  
#define  APP_TASK_BEEP_PRIO         (OS_CFG_PRIO_MAX - 10)  
#define  APP_TASK_KEY_PRIO          (OS_CFG_PRIO_MAX -  9)  
#define  APP_TASK_CONTROL_PRIO      (OS_CFG_PRIO_MAX -  8)  

#define  APP_TASK_OSAL_PRIO         (OS_CFG_PRIO_MAX -  3)  // �����������ȼ������������޸ġ�

#define  APP_TASK_SAMPLE_PRIO       (OS_CFG_PRIO_MAX -  2)  // �����������ȼ������������޸ġ�
/*
*********************************************************************************************************
*                                           ���������ջ�Ĵ�С
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*                      ��ջ���ֽڴ�С = CPU���ݿ�� * 8 * size = 4 * 8 * size(�ֽ�)
*                           �磺APP_TASK_SAMPLE_STK_SIZE = 128u��ʵ��Ϊ 128u * 4 = 512���ֽڣ�
*                             ������IAR�����в鿴��ջ��С�����Ƿ���㡣
*********************************************************************************************************
*/

#define  MB_OS_CFG_RX_TASK_STK_SIZE                 512u        // ModBus�����ջ�Ĵ�С
#define  APP_TASK_START_STK_SIZE                    128u        // ����ʼ��������ջ
#define  APP_TASK_SAMPLE_STK_SIZE                    60u        // ���������ջ��С���û��������Լ���Ӧ������
#define  APP_TASK_WIFI_STK_SIZE                     128u
#define  APP_TASK_COMM_STK_SIZE                     256u
#define  APP_TASK_SENSOR_STK_SIZE                   256u
#define  APP_TASK_KEY_STK_SIZE                      128u
#define  APP_TASK_CONTROL_STK_SIZE                  128u
#define  APP_TASK_BEEP_STK_SIZE                     128u
#define  APP_TASK_OSAL_STK_SIZE                     512u


/*
*********************************************************************************************************
*                                    BSP CONFIGURATION: RS-232
*********************************************************************************************************
*/

#define  BSP_CFG_SER_COMM_SEL             			BSP_SER_COMM_UART_02
#define  BSP_CFG_TS_TMR_SEL                         2


/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0
#define  TRACE_LEVEL_OFF                            0
#define  TRACE_LEVEL_INFO                           1
#define  TRACE_LEVEL_DEBUG                          2
#endif

#define  APP_TRACE_LEVEL                            TRACE_LEVEL_INFO
#define  APP_TRACE                                  BSP_Ser_Printf

#define  APP_TRACE_INFO(x)            ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DEBUG(x)           ((APP_TRACE_LEVEL >= TRACE_LEVEL_DEBUG) ? (void)(APP_TRACE x) : (void)0)


#endif
