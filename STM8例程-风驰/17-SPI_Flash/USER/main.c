/****************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 *****************
 * 文件名  ：main.c
 * 描述    ：SPI_Flash 读写   
 * 实验平台：iCreate STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-11-20
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
NO.31 block,前面0~17block已固化中文库和图片库\r\n风驰iCreate嵌入式开发工作室\r\n2013-2-28";
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
	/*设置外部24M晶振为主时钟*/ 
	SystemClock_Init(HSE_Clock);
	
	SPI_FLASH_Init();	/* SPI初始化*/
	Uart1_Init();
	enableInterrupts();
	FLASH_ID = SPI_FLASH_ReadID();	/* Flash芯片ID读取 */
	printf("W25X16-ID:0x%X\r\n",FLASH_ID);
	printf("W25X16-MANUFACTURER ID:0x%X\r\n",FLASH_ID >> 8);
	printf("W25X16-DEVICE ID:0x%X\r\n",(u8)FLASH_ID);
	printf("正在擦除 flash...\r\n");
	/* 擦除FLASH */
	SPI_FLASH_BlockErase(0);
	printf("擦除完毕...\r\n");
	/* 向FLASH写入数据 */
	SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	
	/* 从FLASH读数据 */
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

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/
