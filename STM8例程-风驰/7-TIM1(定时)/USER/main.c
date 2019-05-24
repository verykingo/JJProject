
/******************** (C) COPYRIGHT  ��۵���Ƕ��ʽ���������� ********************
 * �ļ���  ��main.c
 * ����    ����ʱ��TIM1�Ĳ���    
 * ʵ��ƽ̨����۵���STM8������
 * ��汾  ��V2.1.0
 * ����    ��ling_guansheng QQ:779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2011-12-20
**********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysclock.h"
#include "led.h"
#include "tim1.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	/* �����ⲿ24M����Ϊϵͳ��ʱ�� */
    SystemClock_Init(HSE_Clock);
	
	LED_Init();
	
	Tim1_Init();
	
	SetLedOFF(); /* �����е��� */
	enableInterrupts(); 
	
	while (1)
	{
		;
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

/******************* (C) COPYRIGHT ��۵���Ƕ��ʽ���������� *****END OF FILE****/
