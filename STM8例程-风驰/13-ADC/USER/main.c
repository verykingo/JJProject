/****************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 *****************
 * 文件名  ：main.c
 * 描述    ：ADC模数转换    
 * 实验平台：风驰iCreate STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ:779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-11-18
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "uart1.h"
#include "adc.h"
#include "sysclock.h"
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
  
	/* 设置外部24MHz晶振为系统时钟 */
	SystemClock_Init(HSE_Clock);
	
	/* 初始化ADC */
	ADC_Init();
	
	Uart1_Init();
	
	while (1)
	{
	  	/* 读取并打印AD转换值 */
		MultipleChannelGetADValue();
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
