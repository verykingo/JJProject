/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� ***************
 * �ļ���  ��main.c
 * ����    ������ͨ�ų���     
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    ��ling_guansheng QQ:779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-12-22
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
	/* �����ⲿ24M����Ϊϵͳ��ʱ�� */ 
	SystemClock_Init(HSE_Clock);
	
	Uart1_Init();
	enableInterrupts();	/* �������ж� */
	
	printf("%s\r\n", "UART1 ʵ��------");
	printf("Ӳ��ƽ̨��%s\r\n", "���STM8������");
        
	UART1_printf("This is a UART1 Demo \r\n" );
	UART1_printf("("__DATE__ " - " __TIME__ ") \r\n");
	
	while (1)
	{
	  	;	/* �ȴ������ж� */
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

/*************** (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/