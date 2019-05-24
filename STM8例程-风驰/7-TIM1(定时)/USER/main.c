
/******************** (C) COPYRIGHT  风驰电子嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：定时器TIM1的操作    
 * 实验平台：风驰电子STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：ling_guansheng QQ:779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2011-12-20
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
	/* 设置外部24M晶振为系统主时钟 */
    SystemClock_Init(HSE_Clock);
	
	LED_Init();
	
	Tim1_Init();
	
	SetLedOFF(); /* 让所有灯灭 */
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

/******************* (C) COPYRIGHT 风驰电子嵌入式开发工作室 *****END OF FILE****/
