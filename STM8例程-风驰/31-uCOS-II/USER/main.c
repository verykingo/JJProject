/*************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ********************
 * �ļ���  ��main.c
 * ����    ��uCOS-V290 ��������Գ���   
 * ʵ��ƽ̨����� iCreate STM8������ V2
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �޸�ʱ�� ��2012-12-28
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Private defines -----------------------------------------------------------*/
/* ���������ջ��С */
#define  OS_TASK_0_STK_SIZE                200
#define  OS_TASK_1_STK_SIZE                200
#define  OS_TASK_2_STK_SIZE                200

/* �����������ȼ� */
#define  OS_TASK_0_PRIO                    0
#define  OS_TASK_1_PRIO                    1
#define  OS_TASK_2_PRIO                    2

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* ��������ջ�ռ� */
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];

/* LED����˸����1--LED1ÿ��1s��˸ */
void LedTask1( void * pdata )
{
  	
  	Tim1_Init();	/* ��ʼ��ϵͳʱ�ӽ��� */
	
	while(1)
	{
		LED_Operation(LED_1, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		LED_Operation(LED_1, OFF);
		OSTimeDlyHMSM(0,0,1,0 );
	}
	
}

/* LED����˸����2--LED2ÿ��2s��˸ */
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

/* LED����˸����3--LED3ÿ��3s��˸ */
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
	
	OSInit();	/* ��ʼ��uCOSϵͳ */
	
	/* �������� */
	OSTaskCreate(LedTask1, (void *)OS_TASK_0_STK_SIZE, &Task0Stack[OS_TASK_0_STK_SIZE-1], OS_TASK_0_PRIO );
	OSTaskCreate(LedTask2, (void *)OS_TASK_1_STK_SIZE, &Task1Stack[OS_TASK_1_STK_SIZE-1], OS_TASK_1_PRIO );
	OSTaskCreate(LedTask3, (void *)OS_TASK_2_STK_SIZE, &Task2Stack[OS_TASK_2_STK_SIZE-1], OS_TASK_2_PRIO );
	
	OSStart();	/* ����uCOSϵͳ */
	
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

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/
