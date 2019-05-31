
/****************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� *****************
 * �ļ���  ��main.c
 * ����    �����ڿ��Ź�   
 * ʵ��ƽ̨��iCreate STM8������
 * ��汾  ��V2.1.0
 * ����    ��ling_guansheng  QQ:779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-11-25
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "led.h"
#include "wwdg.h"

/* Private defines -----------------------------------------------------------*/
//#define LED_INTERVAL	0xfff		/* ������ˮ�Ƶ�ʱ���� */
#define LED_INTERVAL	0xffff

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
  	/* ����ʼ��ʱ�ӵĻ�ϵͳĬ��ʱ��Ϊ�ڲ�16MHz��8��Ƶ��Ҳ����2MHz */

	LED_Init();
	
	Test_WWDGReset();
	
	WWDG_Conf(); 
	
	while (1)
	{
		Refresh_WWDG_Window();	/* ���¼��ؼ����� */
		
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

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/
