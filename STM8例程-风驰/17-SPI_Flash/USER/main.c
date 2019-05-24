/****************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� *****************
 * �ļ���  ��main.c
 * ����    ��SPI_Flash ��д   
 * ʵ��ƽ̨��iCreate STM8������
 * ��汾  ��V2.1.0
 * ����    ��ling_guansheng  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-11-20
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "sysclock.h"
#include "uart1.h"
#include "spi_flash.h"

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer))

/* Private variables ---------------------------------------------------------*/
u8 Tx_Buffer[] = "STM8S SPI TEST:Can communication with an W25X16 SPI FLASH from NO.18 block to \
NO.31 block,ǰ��0~17block�ѹ̻����Ŀ��ͼƬ��\r\n���iCreateǶ��ʽ����������\r\n2013-2-28";
u8 Rx_Buffer[BufferSize];
u16 FLASH_ID ;
u32 pAddr=0;
extern bool Flag_receive;
extern u8 Res;
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void main(void)
{
	/*�����ⲿ24M����Ϊ��ʱ��*/ 
	SystemClock_Init(HSE_Clock);
	
	SPI_FLASH_Init();	/* SPI��ʼ��*/
	Uart1_Init();
	enableInterrupts();
	FLASH_ID = SPI_FLASH_ReadID();	/* FlashоƬID��ȡ */
	printf("W25X16-ID:0x%X\r\n",FLASH_ID);
	printf("W25X16-MANUFACTURER ID:0x%X\r\n",FLASH_ID >> 8);
	printf("W25X16-DEVICE ID:0x%X\r\n",(u8)FLASH_ID);
	printf("���ڲ��� flash...\r\n");
	/* ����FLASH */
	SPI_FLASH_BlockErase(0);
	printf("�������...\r\n");
	/* ��FLASHд������ */
	SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	
	/* ��FLASH������ */
	SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
	
	UART1_SendString(Rx_Buffer,BufferSize);

	while (1){
          if(Flag_receive==TRUE){
            W25X16_SPI_FLASH_WriteOneByte(&Res,pAddr++,1);
            Flag_receive=FALSE;
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

/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/
