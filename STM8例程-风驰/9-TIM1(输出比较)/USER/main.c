
/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��main.c
 * ����    ��TIM1����Ƚ�ʵ��   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 *�޸�ʱ�� ��2012-11-5

  ���STM8������Ӳ������
  LED1-PD0
  TIM1���벶��/����Ƚ�ͨ��1--PC1
  ʵ��ʱ����Ҫ��PD0��PC1�öŰ�������

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysclock.h"
#include "led.h"
#include "tim1_oc.h"


/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	/* �����ⲿ����ʱ��24MΪ��ʱ�� */ 
	SystemClock_Init(HSE_Clock);
	
	LED_Init();
	LED_Operation(LED_1, ON);

	Tim1_OC_Init();	/* ��ʼ��TIM1����Ƚ����� */
	
	enableInterrupts(); 

	while(1) {
	  
	}
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

