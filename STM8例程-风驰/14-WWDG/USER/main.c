
/****************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 *****************
 * 文件名  ：main.c
 * 描述    ：窗口看门狗   
 * 实验平台：iCreate STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：ling_guansheng  QQ:779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-11-25
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "led.h"
#include "wwdg.h"

/* Private defines -----------------------------------------------------------*/
//#define LED_INTERVAL	0xfff		/* 设置流水灯的时间间隔 */
#define LED_INTERVAL	0xffff

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
  	/* 不初始化时钟的话系统默认时钟为内部16MHz的8分频，也就是2MHz */

	LED_Init();
	
	Test_WWDGReset();
	
	WWDG_Conf(); 
	
	while (1)
	{
		Refresh_WWDG_Window();	/* 重新加载计数器 */
		
		LED_ShowOneToOne(LED_INTERVAL);
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
