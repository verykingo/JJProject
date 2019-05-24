/*************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：串口3通信     
 * 实验平台：iCreate STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-12-10
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysclock.h"

#include "uart3.h"

/* Private defines -----------------------------------------------------------*/
void Delay(u32 nCount);
extern u8 RxBuffer[RxBufferSize];
extern u8 UART_RX_NUM;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	u8 len ;
	
	SystemClock_Init(HSE_Clock);
	
	Uart3_Init();
	
	enableInterrupts(); 
	
	UART3_SendString("Serial Communication ---STM8 Development Board of FengChi Electron ",\
						sizeof("Serial Communication ---STM8 Development Board of FengChi Electron"));

	UART3_SendByte('\n');

	while (1)
	{
		if(UART_RX_NUM&0x80)
		{
			len=UART_RX_NUM&0x3f;	/*得到此次接收到的数据长度*/
			UART3_SendString("You sent the messages is:",sizeof("You sent the messages is"));
			UART3_SendString(RxBuffer,len);
			UART3_SendByte('\n');
			UART_RX_NUM=0;
		}
	}
}

void Delay(u32 nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
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
