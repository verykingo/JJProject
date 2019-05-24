/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm8l15x.h"
#include "sysclock.h"

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
	/* At Begin, Disable Interrupt */
	disableInterrupts();
	
	/* System CLK Init */
	SystemClock_Init();

	/* After Finish All Init, Enable Interrupt */
	enableInterrupts();	

}

void GotoApplication()
{
	asm("LDW X, SP");	//栈寄存器的值赋给X寄存器
	asm("LD  A, $FF");	//A寄存器赋值0xFF   
	asm("LD  XL, A");	//
	asm("LDW SP, X");	//应用程序分配一个新栈
	asm("JPF $9000");	//跳转到0x009000地址
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * RAM空间: 0x8000~0x17FFF，中断向量表: 0x8000~0x8080
  * 使用4K空间存放bootloader: 0x8000~0x8FFF
  * 应用程序存放空间: 0x9000~0x17FFF
  * 中断向量表重映射: 0x9000~0x9080
  * Bootloader
  */
/* interrupt vetor redirected app addres is 0x9000 bld size is 4KB*/
__root const long reintvec[]@".intvec"={
	0x82008080,0x82009004,0x82009008,0x8200900c,
	0x82009010,0x82009014,0x82009018,0x8200901c,
	0x82009020,0x82009024,0x82009028,0x8200902c,
	0x82009030,0x82009034,0x82009038,0x8200903c,
	0x82009040,0x82009044,0x82009048,0x8200904c,
	0x82009050,0x82009054,0x82009058,0x8200905c,
	0x82009060,0x82009064,0x82009068,0x8200906c,
	0x82009070,0x82009074,0x82009078,0x8200907c,
	};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/