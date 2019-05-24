/*************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：uCOS-V290 多任务测试程序   
 * 实验平台：风驰 iCreate STM8开发板 V2
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 修改时间 ：2012-12-28
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Private defines -----------------------------------------------------------*/
/* 定义任务堆栈大小 */
#define  OS_TASK_0_STK_SIZE                200
#define  OS_TASK_1_STK_SIZE                200
#define  OS_TASK_2_STK_SIZE                200

/* 定义任务优先级 */
#define  OS_TASK_0_PRIO                    0
#define  OS_TASK_1_PRIO                    1
#define  OS_TASK_2_PRIO                    2

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* 定义任务栈空间 */
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* LED灯闪烁任务1--LED1每隔1s闪烁 */
void LedTask1( void * pdata )
{
  	
  	Tim1_Init();	/* 初始化系统时钟节拍 */
	
	while(1)
	{
		LED_Operation(LED_1, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		LED_Operation(LED_1, OFF);
		OSTimeDlyHMSM(0,0,1,0 );
	}
	
}

/* LED灯闪烁任务2--LED2每隔2s闪烁 */
void LedTask2(void * pdata )
{
  
	while(1)
	{
		LED_Operation(LED_2, ON);
		OSTimeDlyHMSM(0,0,2,0 );
		LED_Operation(LED_2, OFF);
		OSTimeDlyHMSM(0,0,2,0 );
	}
	
}

/* LED灯闪烁任务3--LED3每隔3s闪烁 */
void LedTask3(void * pdata)
{
	while(1)
	{
		LED_Operation(LED_3, ON);
		OSTimeDlyHMSM(0,0,3,0 );
		LED_Operation(LED_3, OFF);
		OSTimeDlyHMSM(0,0,3,0);
	}	
}


int main()
{
	__disable_interrupt();
	   
	SystemClock_Init(HSE_Clock);
	LED_Init();
	SetLedOFF();
	__enable_interrupt();
	
	OSInit();	/* 初始化uCOS系统 */
	
	/* 创建任务 */
	OSTaskCreate(LedTask1, (void *)OS_TASK_0_STK_SIZE, &Task0Stack[OS_TASK_0_STK_SIZE-1], OS_TASK_0_PRIO );
	OSTaskCreate(LedTask2, (void *)OS_TASK_1_STK_SIZE, &Task1Stack[OS_TASK_1_STK_SIZE-1], OS_TASK_1_PRIO );
	OSTaskCreate(LedTask3, (void *)OS_TASK_2_STK_SIZE, &Task2Stack[OS_TASK_2_STK_SIZE-1], OS_TASK_2_PRIO );
	
	OSStart();	/* 启动uCOS系统 */
	
	return	0;
}





#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/
