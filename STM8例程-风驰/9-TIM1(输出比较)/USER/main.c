
/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：main.c
 * 描述    ：TIM1输出比较实验   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-5

  风驰STM8开发板硬件连接
  LED1-PD0
  TIM1输入捕获/输出比较通道1--PC1
  实验时，需要将PD0与PC1用杜邦线连接

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
	/* 设置外部高速时钟24M为主时钟 */ 
	SystemClock_Init(HSE_Clock);
	
	LED_Init();
	LED_Operation(LED_1, ON);

	Tim1_OC_Init();	/* 初始化TIM1输出比较引脚 */
	
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

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/

