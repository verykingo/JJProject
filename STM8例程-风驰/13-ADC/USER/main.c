/****************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� *****************
 * �ļ���  ��main.c
 * ����    ��ADCģ��ת��    
 * ʵ��ƽ̨�����iCreate STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ:779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-11-18
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
  
	/* �����ⲿ24MHz����Ϊϵͳʱ�� */
	SystemClock_Init(HSE_Clock);
	
	/* ��ʼ��ADC */
	ADC_Init();
	
	Uart1_Init();
	
	while (1)
	{
	  	/* ��ȡ����ӡADת��ֵ */
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

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/
