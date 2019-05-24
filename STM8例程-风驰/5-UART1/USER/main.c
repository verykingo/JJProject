/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************
 * 文件名  ：main.c
 * 描述    ：串口通信程序     
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：ling_guansheng QQ:779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-12-22
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysclock.h"
#include "uart1.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	/* 设置外部24M晶振为系统主时钟 */ 
	SystemClock_Init(HSE_Clock);
	
	Uart1_Init();
	enableInterrupts();	/* 开启总中断 */
	
	printf("%s\r\n", "UART1 实验------");
	printf("硬件平台：%s\r\n", "风驰STM8开发板");
        
	UART1_printf("This is a UART1 Demo \r\n" );
	UART1_printf("("__DATE__ " - " __TIME__ ") \r\n");
	
	while (1)
	{
	  	;	/* 等待接收中断 */
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

/*************** (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/