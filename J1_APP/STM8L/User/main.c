/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
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
#include "timetick.h"

#if USE_UCOS_II == 1u
#include "ucos_ii.h"
#elif USE_ATOMTHREAD == 1u
#include "atom.h"
#endif

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


#if USE_UCOS_II == 1u
void Task0( void * pdata )
{  	
	static uint32_t task1 = 0;
	while(1)
	{
		task1++;
		if(task1 > 10000)
		{
			task1 = 0;
		}	

		OSTimeDlyHMSM(0,0,1,0 );
		
		task1++;
	}	
}

void Task1( void * pdata )
{  	
	static uint32_t task2 = 0;
	while(1)
	{
		task2++;
		if(task2 > 10000)
		{
			task2 = 0;
		}	

		OSTimeDlyHMSM(0,0,1,0 );
		
		task2++;
	}	
}

void Task2( void * pdata )
{  	
	static uint32_t task3 = 0;
	while(1)
	{
		task3++;
		if(task3 > 10000)
		{
			task3 = 0;
		}	

		OSTimeDlyHMSM(0,0,1,0 );

		task3++;
	}	
}
#elif USE_ATOMTHREAD == 1u
/* ATOM TCB */
static ATOM_TCB Task0_TCB;
static ATOM_TCB Task1_TCB;
static ATOM_TCB Task2_TCB;

void Task0( uint32_t pdata )
{  	
	static uint32_t task1 = 0;
	while(1)
	{
		task1++;
		if(task1 > 10000)
		{
			task1 = 0;
		}	

		atomTimerDelay(100 );

		task1++;
	}	
}

void Task1( uint32_t pdata )
{  	
	static uint32_t task2 = 0;
	while(1)
	{
		task2++;
		if(task2 > 10000)
		{
			task2 = 0;
		}	

		atomTimerDelay(200 );

		task2++;
	}	
}

void Task2( uint32_t pdata )
{  	
	static uint32_t task3 = 0;
	while(1)
	{
		task3++;
		if(task3 > 10000)
		{
			task3 = 0;
		}	

		atomTimerDelay(300 );

		task3++;
	}	
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

	/* TimeTick Init */
	TimeTick_Init();

	/* After Finish All Init, Enable Interrupt */
	enableInterrupts();	

#if USE_UCOS_II == 1u

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
