/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
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
#include "stm8l15x_it.h"
#include "vkusart.h"
#include "vksofttimer.h"
#include "vkpluse.h"
#include "vkleds.h"

/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */
	
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval 
  * None
*/
INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval 
  * None
*/
#if USE_UCOS_II == 1u
/*
  Trap中断服务程序_interrupt_1()，在RTOS/uCOS-II-2.9/Port/os_cpu_a.s汇编语言实现。
*/
#elif USE_NO_RTOS == 1u
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
#endif
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler,1)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler,2)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{	
		#if UART1_DMA_ENABLE == 1u
		//vkUsart_DMA_Send_Line_Over_Handler(COM1);
		#elif UART3_DMA_ENABLE == 1u
		//vkUsart_DMA_Send_Line_Over_Handler(COM3);
		#endif 
		
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
	
    if(DMA_GetITStatus(DMA1_IT_TC0) != RESET)
	{
		//vkUsart_DMA_Send_Line_Over_Handler(COM2);
		DMA_ClearITPendingBit(DMA1_IT_TC0);
	}
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler,3)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler,5)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler,7)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
#if TLY_INT_ENABLE == 1u
	if(EXTI_GetITStatus(EXTI_IT_Pin1) != RESET)
	{
		/* 陀螺仪中断，每5m秒进入一次中断 */
		//vkTly_Interrupt_Update_Callback((void *)0);
		
		/* 清除中断标志 */
		EXTI_ClearITPendingBit(EXTI_IT_Pin1);
	}
#endif

}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    
	if(EXTI_GetITStatus(EXTI_IT_Pin2) != RESET)
	{
 		/* 开机按键中断回调函数 */
		//vkPower_Interrupt_Callback();

		/* 清除中断标志 */
		EXTI_ClearITPendingBit(EXTI_IT_Pin2);
	}
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

	if(EXTI_GetITStatus(EXTI_IT_Pin4) != RESET)
	{
		/* 电池充电头插拔回调函数 */
		//vkBattery_Charge_Callback();

		/* 清除中断标志 */
		EXTI_ClearITPendingBit(EXTI_IT_Pin4);
	}
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler,13)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler,16)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler,17)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler,18)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    /* 串口发送一个字节数据完成 */
    if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		//vkUsart_INT_Send_Byte_Over_Handler(COM2);
		USART_ClearITPendingBit(USART2, USART_IT_TC);
    }
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler,20)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	/* 串口收到一个字节数据 */
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{		
		//vkUsart_INT_Recv_Byte_Over_Handler(COM2);
	}
	
	/* 串口空闲，收到一行最后一个字节数据 */
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData8(USART2);
		//vkUsart_Recv_Line_Handler(COM2);		
	}
}

/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler,21)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	/* 串口发送一个字节数据完成 */
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
		//vkUsart_INT_Send_Byte_Over_Handler(COM3);
		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler,22)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

	/* 串口收到一个字节数据 */
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {    	
		//vkUsart_INT_Recv_Byte_Over_Handler(COM3);
    }

	/* 串口空闲，收到一行最后一个字节数据 */
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData8(USART3);
		//vkUsart_Recv_Line_Handler(COM3);		
	}
}

/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine. is "_interrupt_25"
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	/* 软件定时器Tick */
	if(TIM1_GetITStatus(TIM1_IT_Update) != RESET)
	{
		//vkTimerTick();
		TIM1_ClearITPendingBit(TIM1_IT_Update); 	
	}
}

/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler,24)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
#if USE_UCOS_II == 1u
	/*
	  TIM4中断服务程序_interrupt_27()，在RTOS/uCOS-II-2.9/Port/os_cpu_a.s汇编语言实现。
	*/
#elif USE_ATOMTHREAD == 1u
	/*
	  TIM4中断服务程序_interrupt_27()，在RTOS/atomthreads-1.3/ports/stm8/atomport.c实现。
	*/
#elif USE_NO_RTOS == 1u
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler,25)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    /* 实时操作系统Tick */
	if(TIM4_GetITStatus(TIM4_IT_Update) != RESET)
	{		
		TIM4_ClearITPendingBit(TIM4_IT_Update);
	}
}
#endif

/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler,26)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */		
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler,27)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
   	/* 串口发送一个字节数据完成 */
    if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
    	//vkUsart_INT_Send_Byte_Over_Handler(COM1);

		USART_ClearITPendingBit(USART1, USART_IT_TC);
    }
	
	if(TIM5_GetITStatus(TIM5_IT_Update) != RESET)
	{	
		/* 脉冲 */
		//vkPluseTick();
		TIM5_ClearITPendingBit(TIM5_IT_Update);
	}
}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

	/* 串口收到一个字节数据 */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {    	
		//vkUsart_INT_Recv_Byte_Over_Handler(COM1);
    }

	/* 串口空闲，收到一行最后一个字节数据 */
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData8(USART1);
		//vkUsart_Recv_Line_Handler(COM1);		
	}
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler,29)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
