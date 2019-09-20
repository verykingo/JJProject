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
#include "vkUsart.h"
#include "vkAT.h"

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

/* 队列数据块大小，数据库数目*/
#define MAX_QUEUE_BUF_SIZE		1		
#define MAX_QUEUE_BUF_NUMS		1

/* 循环队列 */
vkQUEUE Queue;

/* 队列数据存储区 */
uint8_t QueueBuffer[MAX_QUEUE_BUF_SIZE*MAX_QUEUE_BUF_NUMS] = {0};

/* 串口3发送接收数据缓存 */
uint8_t COM3_TxBuffer[64] = {0};
uint8_t COM3_RxBuffer[64] = {0};


#if USE_NO_RTOS == 1u
/* 定时器回调 */
vkTIMER Timer_Callback0;
vkTIMER Timer_Callback1;
vkTIMER Timer_Callback2;

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

#elif USE_UCOS_II == 1u
/* 定义任务堆栈大小 */
#define  OS_IDLE_STK_SIZE                	100
#define  OS_TASK_0_STK_SIZE                	100
#define  OS_TASK_1_STK_SIZE                	100
#define  OS_TASK_2_STK_SIZE                	100

/* 定义任务栈空间 */
OS_STK	IdleStack[OS_IDLE_STK_SIZE];
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* 定义任务优先级 */
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
/* 定义任务堆栈大小 */
#define  OS_IDLE_STK_SIZE                	200
#define  OS_TASK_0_STK_SIZE                	200
#define  OS_TASK_1_STK_SIZE                	200
#define  OS_TASK_2_STK_SIZE                	200

/* 定义任务栈空间 */
OS_STK	IdleStack[OS_IDLE_STK_SIZE];
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* 定义任务优先级 */
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

	if(-1 == vkUsart_Init(COM3,115200,COM3_TxBuffer,(sizeof(COM3_TxBuffer)/sizeof(COM3_TxBuffer[0])),COM3_RxBuffer,(sizeof(COM3_RxBuffer)/sizeof(COM3_RxBuffer[0]))))
	{
		printf("failed!\r\n");
	}

	/* TimeTick Init for RTOS tick*/
	vkTimeTick_Init(TIME1);

	/* TimeTick Init for softtimer tick*/
	vkTimeTick_Init(TIME2);

	/* TimeTick Init for pluse tick*/
	vkTimeTick_Init(TIME5);

	/* Create a queue buffer */
	vkQueueCreate(&Queue,QueueBuffer,MAX_QUEUE_BUF_SIZE,MAX_QUEUE_BUF_NUMS);

	/* After Finish All Init, Enable Interrupt */
	enableInterrupts();	

	vkUsart_Send(COM3 ,"Hello STM8L!\r\n", 14);
	vkTimerDelayMS(100);
	
	vkAT_TerminalStart(COM3);
	//vkAT_CommunicationStart(COM3);
	
#if USE_NO_RTOS == 1u
	Timer_Callback0.timer_name 	= (void *)&Timer_Callback0;
	Timer_Callback0.cb_func		= Callback0;
	Timer_Callback0.cb_data		= NULL;
	Timer_Callback0.cb_ticks 	= vkSS_TO_TICKS(2);
	vkTimerInsert(&Timer_Callback0);

	Timer_Callback1.timer_name	= (void *)&Timer_Callback1;
	Timer_Callback1.cb_func 	= Callback1;
	Timer_Callback1.cb_data 	= NULL;
	Timer_Callback1.cb_ticks	= vkSS_TO_TICKS(3);
	vkTimerInsert(&Timer_Callback1);

	Timer_Callback2.timer_name	= (void *)&Timer_Callback2;
	Timer_Callback2.cb_func 	= Callback2;
	Timer_Callback2.cb_data 	= NULL;
	Timer_Callback2.cb_ticks	= vkSS_TO_TICKS(5);
	vkTimerInsert(&Timer_Callback2);

	while(1)
	{};
	
#elif USE_UCOS_II == 1u
	/* 初始化uCOS系统 */
	OSInit();	

	/* 创建任务 */
	OSTaskCreate(Task0, (void *)NULL, &Task0Stack[OS_TASK_0_STK_SIZE-1], OS_TASK_0_PRIO );
	OSTaskCreate(Task1, (void *)NULL, &Task1Stack[OS_TASK_1_STK_SIZE-1], OS_TASK_1_PRIO );
	OSTaskCreate(Task2, (void *)NULL, &Task2Stack[OS_TASK_2_STK_SIZE-1], OS_TASK_2_PRIO );

	OSTaskNameSet(OS_TASK_0_PRIO, (INT8U *)(void *)"uC/OS-II Task0", NULL);
	OSTaskNameSet(OS_TASK_1_PRIO, (INT8U *)(void *)"uC/OS-II Task1", NULL);
	OSTaskNameSet(OS_TASK_2_PRIO, (INT8U *)(void *)"uC/OS-II Task2", NULL);

	/* 启动uCOS系统 */
	OSStart();

#elif USE_ATOMTHREAD == 1u
	/* 初始化AtomThread系统 */
	atomOSInit(&IdleStack[OS_IDLE_STK_SIZE-1], OS_IDLE_STK_SIZE);

	/* 创建任务 */
	atomThreadCreate(&Task0_TCB, OS_TASK_0_PRIO, Task0, 0, &Task0Stack[OS_TASK_0_STK_SIZE - 1], OS_TASK_0_STK_SIZE);
	atomThreadCreate(&Task1_TCB, OS_TASK_1_PRIO, Task1, 0, &Task1Stack[OS_TASK_1_STK_SIZE - 1], OS_TASK_1_STK_SIZE);
	atomThreadCreate(&Task2_TCB, OS_TASK_2_PRIO, Task2, 0, &Task2Stack[OS_TASK_2_STK_SIZE - 1], OS_TASK_2_STK_SIZE);

	/* 启动AtomThread系统 */
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
