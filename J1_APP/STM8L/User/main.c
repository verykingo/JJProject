/**
  ******************************************************************************
  * @file    Project/main.c
  * @author  VeryKing.Inc Develop Team
  * @version V1.0.1
  * @date    01-June-2019
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2019 VeryKing.Inc</center></h2>
  *
  * Licensed under V&K License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm8l15x.h"
#include "sysclock.h"
#include "vktimetick.h"
#include "vksofttimer.h"
#include "vkprotocol.h"
#include "vkleds.h"
#include "vkusart.h"
#include "vkAT.h"
#include "vkADC.h"
#include "vkpluse.h"
#include "vkpower.h"

#if USE_UCOS_II == 1u
#include "ucos_ii.h"
#elif USE_ATOMTHREAD == 1u
#include "atom.h"
#elif USE_FREERTOS ==1u
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "list.h"
#include "timers.h"
#endif

#include "vkqueue.h"

/* �������ݿ��С�����ݿ���Ŀ*/
#define MAX_QUEUE_BUF_SIZE		1		
#define MAX_QUEUE_BUF_NUMS		1

/* ѭ������ */
vkQUEUE Queue;

/* �������ݴ洢�� */
//uint8_t QueueBuffer[MAX_QUEUE_BUF_SIZE*MAX_QUEUE_BUF_NUMS] = {0};

/* ����3���ͽ������ݻ��� */
//uint8_t COM3_TxBuffer[64] = {0};
//uint8_t COM3_RxBuffer[64] = {0};


#if USE_NO_RTOS == 1u
/* ��ʱ���ص� */
vkTIMER Timer_Callback0;
vkTIMER Timer_Callback1;
vkTIMER Timer_Callback2;
vkTIMER Timer_Callback3;
vkTIMER Timer_Callback4;
vkTIMER Timer_Callback5;

void Callback0(void * pdata)
{
	static uint32_t Callback0 = 0;

	Callback0++;

	vkLedsSet(LED0, Callback0%2);
	
	vkTimerInsert(&Timer_Callback0);
}

void Callback1(void * pdata)
{
	static uint32_t Callback1 = 0;

	Callback1++;

	vkLedsSet(LED1, Callback1%2);

	vkTimerInsert(&Timer_Callback1);
}

void Callback2(void * pdata)
{
	static uint32_t Callback2 = 0;

	Callback2++;

	vkTimerInsert(&Timer_Callback2);
}

void Callback3(void * pdata)
{
	static uint32_t Callback3 = 0;

	Callback3++;

	vkTimerInsert(&Timer_Callback3);
}

void Callback4(void * pdata)
{
	static uint32_t Callback4 = 0;

	Callback4++;

	vkTimerInsert(&Timer_Callback4);
}

void Callback5(void * pdata)
{
	static uint32_t Callback5 = 0;

	Callback5++;

	vkTimerInsert(&Timer_Callback5);
}

#elif USE_UCOS_II == 1u
/* ���������ջ��С */
#define  OS_IDLE_STK_SIZE                	100
#define  OS_TASK_0_STK_SIZE                	100
#define  OS_TASK_1_STK_SIZE                	100
#define  OS_TASK_2_STK_SIZE                	100

/* ��������ջ�ռ� */
OS_STK	IdleStack[OS_IDLE_STK_SIZE];
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* �����������ȼ� */
#define  OS_TASK_0_PRIO                    0
#define  OS_TASK_1_PRIO                    1
#define  OS_TASK_2_PRIO                    2

void Task0(void * pdata)
{  		
	static uint32_t task0_count = 0;
	while(1)
	{
		task0_count++;
		if(task0_count > 10000)
		{
			task0_count = 0;
		}	

		vkLedsSet(LED0, task0_count%2);
		OSTimeDlyHMSM(0,0,2,0 );		
	}	
}

void Task1(void * pdata)
{  	
	static uint32_t task1_count = 0;
	while(1)
	{
		task1_count++;
		if(task1_count > 10000)
		{
			task1_count = 0;
		}	

		vkLedsSet(LED1, task1_count%2);
		OSTimeDlyHMSM(0,0,3,0);
	}	
}

void Task2(void * pdata)
{  	
	static uint32_t task2_count = 0;
	while(1)
	{
		task2_count++;
		if(task2_count > 10000)
		{
			task2_count = 0;
		}	

		OSTimeDlyHMSM(0,0,5,0);
	}	
}
#elif USE_ATOMTHREAD == 1u
/* ���������ջ��С */
#define  OS_IDLE_STK_SIZE                	200
#define  OS_TASK_0_STK_SIZE                	200
#define  OS_TASK_1_STK_SIZE                	200
#define  OS_TASK_2_STK_SIZE                	200

/* ��������ջ�ռ� */
OS_STK	IdleStack[OS_IDLE_STK_SIZE];
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* �����������ȼ� */
#define  OS_TASK_0_PRIO                    0
#define  OS_TASK_1_PRIO                    1
#define  OS_TASK_2_PRIO                    2

/* ATOM TCB */
static ATOM_TCB Task0_TCB;
static ATOM_TCB Task1_TCB;
static ATOM_TCB Task2_TCB;

void Task0(uint32_t pdata)
{  	
	static uint32_t task0_count = 0;
	while(1)
	{
		task0_count++;
		if(task0_count > 10000)
		{
			task0_count = 0;
		}	

		vkLedsSet(LED0, task0_count%2);
		atomTimerDelay(2000);
	}	
}

void Task1(uint32_t pdata)
{  	
	static uint32_t task1_count = 0;
	while(1)
	{
		task1_count++;
		if(task1_count > 10000)
		{
			task1_count = 0;
		}	

		vkLedsSet(LED1, task1_count%2);
		atomTimerDelay(3000);
	}	
}

void Task2(uint32_t pdata)
{  	
	static uint32_t task3_count = 0;
	while(1)
	{
		task3_count++;
		if(task3_count > 10000)
		{
			task3_count = 0;
		}	

		atomTimerDelay(5000);
	}	
}

#elif USE_FREERTOS == 1u

void Task0(uint32_t pdata)
{  	
	static uint32_t task0_count = 0;
	while(1)
	{
		task0_count++;
		if(task0_count > 10000)
		{
			task0_count = 0;
		}	

		vkLedsSet(LED0, task0_count%2);
		vTaskDelay(2000);
	}

	vTaskDelete(NULL);
}

void Task1(uint32_t pdata)
{  	
	static uint32_t task1_count = 0;
	while(1)
	{
		task1_count++;
		if(task1_count > 10000)
		{
			task1_count = 0;
		}	

		vkLedsSet(LED1, task1_count%2);
		vTaskDelay(3000);
	}	

	vTaskDelete(NULL);
}

void Task2(uint32_t pdata)
{  	
	static uint32_t task3_count = 0;
	while(1)
	{
		task3_count++;
		if(task3_count > 10000)
		{
			task3_count = 0;
		}	

		vTaskDelay(5000);
	}

	vTaskDelete(NULL);
}

#endif

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	/* At Begin, Disable Interrupt */
	disableInterrupts();
	
	/* System CLK Init */
	SystemClock_Init();

	/* Leds Init */
	vkLeds_Init();

	/* ADC Init */
	vkADC_Init();
	
	//if(-1 == vkUsart_Init(COM3,115200,COM3_TxBuffer,(sizeof(COM3_TxBuffer)/sizeof(COM3_TxBuffer[0])),COM3_RxBuffer,(sizeof(COM3_RxBuffer)/sizeof(COM3_RxBuffer[0]))))
	{
		printf("failed!\r\n");
	}
	
	/* TimeTick Init for softtimer tick*/
	vkTimeTick_Init(TIME1);

	/* TimeTick Init for RTOS 1ms/tick */
	vkTimeTick_Init(TIME4);

	/* TimeTick Init for pluse tick ���ȼ����=3 */
	vkTimeTick_Init(TIME5);
	
	/* �缫�����ʼ�� */
	//vkPluse_Init();

	/* ģ���Դ��ʼ�� */
	//vkPower_Init();
	//vkPowerBleSet(1);
	//vkPowerTlySet(1);
	//vkPowerDJASet(1);
	//vkPowerDJBSet(1);
	
	/* Create a queue buffer */
	//vkQueueCreate(&Queue,QueueBuffer,MAX_QUEUE_BUF_SIZE,MAX_QUEUE_BUF_NUMS);

	/* After Finish All Init, Enable Interrupt */
	enableInterrupts();

	/* ���ԣ�ѭ��ģʽ������1����1.5�� */
	//vkPluseSetMode(PLUSE0, PLUSE_MODE_CONTINUE);
	//vkPluseSetExpt(PLUSE0, 20);
	//vkPluseSetTime(PLUSE0, 2, 3);
	//vkPluseStart(PLUSE0);

	vkUsart_Send(COM3 ,"Hello STM8L!\r\n", 14);
	vkTimerDelayMS(100);
	
	vkAT_TerminalStart(COM3);
	vkAT_CommunicationStart(COM3);
	
#if USE_NO_RTOS == 1u
#if 1
	Timer_Callback0.timer_name 	= (void *)&Timer_Callback0;
	Timer_Callback0.cb_func		= Callback0;
	Timer_Callback0.cb_data		= NULL;
	Timer_Callback0.cb_ticks 	= vkMS_TO_TICKS(100);
	vkTimerInsert(&Timer_Callback0);

	Timer_Callback1.timer_name	= (void *)&Timer_Callback1;
	Timer_Callback1.cb_func 	= Callback1;
	Timer_Callback1.cb_data 	= NULL;
	Timer_Callback1.cb_ticks	= vkMS_TO_TICKS(200);
	vkTimerInsert(&Timer_Callback1);

	Timer_Callback2.timer_name	= (void *)&Timer_Callback2;
	Timer_Callback2.cb_func 	= Callback2;
	Timer_Callback2.cb_data 	= NULL;
	Timer_Callback2.cb_ticks	= vkMS_TO_TICKS(300);
	vkTimerInsert(&Timer_Callback2);

	Timer_Callback3.timer_name	= (void *)&Timer_Callback3;
	Timer_Callback3.cb_func 	= Callback3;
	Timer_Callback3.cb_data 	= NULL;
	Timer_Callback3.cb_ticks	= vkMS_TO_TICKS(400);
	vkTimerInsert(&Timer_Callback3);

	Timer_Callback4.timer_name	= (void *)&Timer_Callback4;
	Timer_Callback4.cb_func 	= Callback4;
	Timer_Callback4.cb_data 	= NULL;
	Timer_Callback4.cb_ticks	= vkMS_TO_TICKS(500);
	vkTimerInsert(&Timer_Callback4);

	Timer_Callback5.timer_name	= (void *)&Timer_Callback5;
	Timer_Callback5.cb_func 	= Callback5;
	Timer_Callback5.cb_data 	= NULL;
	Timer_Callback5.cb_ticks	= vkMS_TO_TICKS(600);
	vkTimerInsert(&Timer_Callback5);
#endif
	while(1)
	{};
	
#elif USE_UCOS_II == 1u
	/* ��ʼ��uCOSϵͳ */
	OSInit();	

	/* �������� */
	OSTaskCreate(Task0, (void *)NULL, &Task0Stack[OS_TASK_0_STK_SIZE-1], OS_TASK_0_PRIO );
	OSTaskCreate(Task1, (void *)NULL, &Task1Stack[OS_TASK_1_STK_SIZE-1], OS_TASK_1_PRIO );
	OSTaskCreate(Task2, (void *)NULL, &Task2Stack[OS_TASK_2_STK_SIZE-1], OS_TASK_2_PRIO );

	OSTaskNameSet(OS_TASK_0_PRIO, (INT8U *)(void *)"uC/OS-II Task0", NULL);
	OSTaskNameSet(OS_TASK_1_PRIO, (INT8U *)(void *)"uC/OS-II Task1", NULL);
	OSTaskNameSet(OS_TASK_2_PRIO, (INT8U *)(void *)"uC/OS-II Task2", NULL);

	/* ����uCOSϵͳ */
	OSStart();

#elif USE_ATOMTHREAD == 1u
	/* ��ʼ��AtomThreadϵͳ */
	atomOSInit(&IdleStack[OS_IDLE_STK_SIZE-1], OS_IDLE_STK_SIZE);

	/* �������� */
	atomThreadCreate(&Task0_TCB, OS_TASK_0_PRIO, Task0, 0, &Task0Stack[OS_TASK_0_STK_SIZE - 1], OS_TASK_0_STK_SIZE);
	atomThreadCreate(&Task1_TCB, OS_TASK_1_PRIO, Task1, 0, &Task1Stack[OS_TASK_1_STK_SIZE - 1], OS_TASK_1_STK_SIZE);
	atomThreadCreate(&Task2_TCB, OS_TASK_2_PRIO, Task2, 0, &Task2Stack[OS_TASK_2_STK_SIZE - 1], OS_TASK_2_STK_SIZE);

	/* ����AtomThreadϵͳ */
	atomOSStart();
#elif USE_FREERTOS == 1u
	
	xTaskCreate((TaskFunction_t)Task0, ((const char*)"Task0"), 1024, NULL, tskIDLE_PRIORITY + 3 , NULL);
	xTaskCreate((TaskFunction_t)Task1, ((const char*)"Task1"), 1024, NULL, tskIDLE_PRIORITY + 3 , NULL);
	xTaskCreate((TaskFunction_t)Task2, ((const char*)"Task2"), 1024, NULL, tskIDLE_PRIORITY + 3 , NULL);

	/*Enable Schedule, Start Kernel*/
	vTaskStartScheduler();
	
#endif

}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT VeryKing.Inc *****END OF FILE****/
