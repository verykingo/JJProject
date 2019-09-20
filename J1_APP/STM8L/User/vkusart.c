/******************************************************************************
 * 文件  ：vkusart.c
 * 描述    ：STM8L串口
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* 包含自定义头文件 */
#include "vkusart.h"

/* 自定义新类型 */

/* 自定义宏 */
#define UART_SEND_RECV_TIMEOUT 1000		//发送接收超时

/* 全局变量定义 */
static vkUSART Usart_Global[3] = {0x00};

/* 标准库printf串口 */
static USART_TypeDef* Usart_Printf = NULL;

/*******************************************************************************
 * 名称: vkUsart_Init
 * 功能: USART初始化操作
 * 形参: 串口USARTx, 波特率BaudRate
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/
int vkUsart_Init(vkCOM com, uint32_t baudrate, uint8_t* tx_buffer, uint8_t tx_bufsize, uint8_t* rx_buffer, uint8_t rx_bufsize)
{
	/* USART configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Receive and transmit enabled
	- USART Clock enabled
	*/

	/* USART check */
	if(com!=COM1 && com!=COM2 && com!=COM3)
	{
		return -1;
	}	
	
	/* BaudRate check */
	if(baudrate!=9600 && baudrate!=115200 && baudrate!=38400)
	{
		return -1;
	}

	/* TX/RX Buffer Check */
	if(tx_buffer==NULL || rx_buffer==NULL || tx_bufsize==0 || rx_bufsize==0)
	{
		return -1;
	}

	if(com == COM1)
	{
		Usart_Global[com].usart = USART1;
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

		/* USART1 Tx- Rx (PC3- PC2) remapping to (PA2- PA3) */
		#if 1
		/* GPIO重映射 */
		//SYSCFG_REMAPDeInit();
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
		
		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_2, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, ENABLE);
		#endif

		/* USART1 Tx- Rx (PC3- PC2) */
		#if 0
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
		
		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);		
		#endif		

		/* USART1 Tx- Rx (PC3- PC2) remapping to (PC5- PC6) */
		#if 0
		/* GPIO重映射 */
		//SYSCFG_REMAPDeInit();
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);

		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_5, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_6, ENABLE);
		#endif

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(USART1_RX_TIM5_CC_IRQn, ITC_PriorityLevel_2);
	}
	else if(com == COM2)
	{
		Usart_Global[com].usart = USART2;
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);

		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_4, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_3, ENABLE);

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(TIM2_CC_USART2_RX_IRQn, ITC_PriorityLevel_2);
	}
	else if(com == COM3)
	{
		Usart_Global[com].usart = USART3;

		/* USART3 Tx- Rx (PE6- PE7) */
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOE, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);	

		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, ENABLE);

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(TIM3_CC_USART3_RX_IRQn, ITC_PriorityLevel_2);
	}
	else
	{
		return -1;
	}

	/* Disable USART */
	USART_Cmd(Usart_Global[com].usart, DISABLE);

	/* RESET USART */
  	USART_DeInit(Usart_Global[com].usart);

  	/* USART configuration */
  	USART_Init(Usart_Global[com].usart, baudrate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));	

	/* 如果串口非标准库函数prinft串口，配置中断方式 */
	if(Usart_Global[com].usart != Usart_Printf)
	{
	  	/* Enable the USART Receive interrupt: this interrupt is generated 
	  	   when the USART receive data register is not empty */
	 	USART_ITConfig(Usart_Global[com].usart, USART_IT_RXNE, ENABLE);

		/* Enable the USART Transmit complete interrupt: this interrupt is 
		   generated when the USART transmit Shift Register is empty */
		USART_ClearITPendingBit(Usart_Global[com].usart, USART_IT_TC);
	  	USART_ITConfig(Usart_Global[com].usart, USART_IT_TC, ENABLE);	

		/* Enable the USART IDLE line interrupt */
	  	USART_ITConfig(Usart_Global[com].usart, USART_IT_IDLE, ENABLE);	
	}
	
	/* Enable USART */
	USART_Cmd(Usart_Global[com].usart, ENABLE);

	/* Set Zero */
	Usart_Global[com].tx_buffer = tx_buffer;
	Usart_Global[com].rx_buffer = rx_buffer;
	Usart_Global[com].tx_bufsize = tx_bufsize;
	Usart_Global[com].rx_bufsize = rx_bufsize;
	Usart_Global[com].tx_index = 0;
	Usart_Global[com].rx_index = 0;	
	Usart_Global[com].tx_count = 0;
	Usart_Global[com].rx_count = 0;	
	Usart_Global[com].tx_doing = 0;
	Usart_Global[com].rx_idle = 0;

	/* Configure DMA channel */
	Usart_Global[com].dma_enable = 0;
	Usart_Global[com].tx_channel = NULL;
	Usart_Global[com].rx_channel = NULL;

	/* DMA Configure */
	/* 串口非标准库函数prinft串口，才需要配置DMA方式 */
	if(com == COM1 && Usart_Global[com].usart != Usart_Printf)
	{
		if(Usart_Global[COM3].dma_enable == 0)
		{
		 	#if (UART1_DMA_ENABLE == 1u)

			/* Disable RXNE and TC  */
			USART_ITConfig(Usart_Global[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(Usart_Global[com].usart, USART_IT_TC, DISABLE);	

			/* 开始配置DMA设置 */
			Usart_Global[com].dma_enable = 1;
			Usart_Global[com].tx_channel = DMA1_Channel1;
			Usart_Global[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

			/* Disable the USART Tx/Rx DMA channel */
			DMA_Cmd(Usart_Global[com].tx_channel, DISABLE);
			DMA_Cmd(Usart_Global[com].rx_channel, DISABLE);

			/* Deintialize DMA channels */
			DMA_DeInit(Usart_Global[com].tx_channel);
			DMA_DeInit(Usart_Global[com].rx_channel);
			
			/* Intialize DMA channels */
			DMA_Init(Usart_Global[com].tx_channel, (uint32_t)Usart_Global[com].tx_buffer, (uint16_t)Usart_Global[com].usart->DR, Usart_Global[com].tx_bufsize, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			DMA_Init(Usart_Global[com].rx_channel, (uint32_t)Usart_Global[com].rx_buffer, (uint16_t)Usart_Global[com].usart->DR, Usart_Global[com].rx_bufsize, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			
			/* DMA_ITConfig */
			DMA_ClearFlag(DMA1_FLAG_TC1);			
			DMA_ITConfig(Usart_Global[com].tx_channel, DMA_ITx_TC, ENABLE);

			/* DMA_ITConfig */
			ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
			ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_RX, ENABLE);

			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			#endif
		}
	}
	else if(com == COM3 && Usart_Global[com].usart != Usart_Printf)
	{
		if(Usart_Global[COM1].dma_enable == 0)
		{
		 	#if (UART3_DMA_ENABLE == 1u)

			/* Disable RXNE and TC  */
			USART_ITConfig(Usart_Global[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(Usart_Global[com].usart, USART_IT_TC, DISABLE);	
			
			/* 开始配置DMA设置 */
			Usart_Global[com].dma_enable = 1;
			Usart_Global[com].tx_channel = DMA1_Channel1;
			Usart_Global[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

			/* Disable the USART Tx/Rx DMA channel */
			DMA_Cmd(Usart_Global[com].tx_channel, DISABLE);
			DMA_Cmd(Usart_Global[com].rx_channel, DISABLE);

			/* Deintialize DMA channels */
			DMA_DeInit(Usart_Global[com].tx_channel);
			DMA_DeInit(Usart_Global[com].rx_channel);
			
			/* Intialize DMA channels */
			DMA_Init(Usart_Global[com].tx_channel, (uint32_t)Usart_Global[com].tx_buffer, (uint16_t)&Usart_Global[com].usart->DR, Usart_Global[com].tx_bufsize, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			DMA_Init(Usart_Global[com].rx_channel, (uint32_t)Usart_Global[com].rx_buffer, (uint16_t)&Usart_Global[com].usart->DR, Usart_Global[com].rx_bufsize, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			
			/* DMA_ITConfig */
			DMA_ClearFlag(DMA1_FLAG_TC1);			
			DMA_ITConfig(Usart_Global[com].tx_channel, DMA_ITx_TC, ENABLE);
			
			ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
			ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_RX, ENABLE);

			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			#endif
		}		
	}
	else if(com == COM2 && Usart_Global[com].usart != Usart_Printf)
	{
		#if (UART2_DMA_ENABLE==1u)
		/* Disable RXNE and TC  */
		USART_ITConfig(Usart_Global[com].usart, USART_IT_RXNE, DISABLE);
		USART_ITConfig(Usart_Global[com].usart, USART_IT_TC, DISABLE);			

		/* 开始配置DMA设置 */
		Usart_Global[com].dma_enable = 1;
		Usart_Global[com].tx_channel = DMA1_Channel0;
		Usart_Global[com].rx_channel = DMA1_Channel3;

		/* Enable The DMA controller clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

		/* Disable the USART Tx/Rx DMA channel */
		DMA_Cmd(Usart_Global[com].tx_channel, DISABLE);
		DMA_Cmd(Usart_Global[com].rx_channel, DISABLE);

		/* Deintialize DMA channels */
		DMA_DeInit(Usart_Global[com].tx_channel);
		DMA_DeInit(Usart_Global[com].rx_channel);	
		
		/* Intialize DMA channels */
		DMA_Init(Usart_Global[com].tx_channel, (uint32_t)Usart_Global[com].tx_buffer, (uint16_t)Usart_Global[com].usart->DR, Usart_Global[com].tx_bufsize, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
		DMA_Init(Usart_Global[com].rx_channel, (uint32_t)Usart_Global[com].rx_buffer, (uint16_t)Usart_Global[com].usart->DR, Usart_Global[com].rx_bufsize, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);

		/* DMA_ITConfig */
		DMA_ClearFlag(DMA1_FLAG_TC0);			
		DMA_ITConfig(Usart_Global[com].tx_channel, DMA_ITx_TC, ENABLE);

		/* DMA_ITConfig */
		ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

		/* Enable the USART Tx/Rx DMA requests */		
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_TX, ENABLE);
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_RX, ENABLE);

		/* Global DMA Enable */
		DMA_GlobalCmd(ENABLE);			
		#endif		
	}
	
	return 0;
}

/*******************************************************************************
 * 名称: vkUsart_Deinit
 * 功能: USART初始化操作
 * 形参: 串口USARTx, 波特率BaudRate
 * 返回: 成功0，失败-1
 * 说明: 无 
 ******************************************************************************/
int vkUsart_Deinit(vkCOM com)
{
	/* USART check */
	if(com!=COM1 && com!=COM2 && com!=COM3)
	{
		return -1;
	}	
	
	if(com == COM1)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);

		#if 1
		/* Disable USART1 Tx- Rx remapping to PA2- PA3 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, DISABLE);
		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_2, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, DISABLE);
		#endif
		
		#if 0
		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, DISABLE);
		#endif

		#if 0
		/* Disable USART1 Tx- Rx remapping to PC5- PC6 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, DISABLE);
		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_5, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_6, DISABLE);
		#endif		
	}
	else if(com == COM2)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);

		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_4, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_3, DISABLE);
	}
	else if(com == COM3)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);

		/* 是否使用内部上拉电阻 */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, DISABLE);		
	}
	else
	{
		return -1;
	}

  	/* Disable the USART Receive interrupt: this interrupt is generated 
  	   when the USART receive data register is not empty */
 	USART_ITConfig(Usart_Global[com].usart, USART_IT_RXNE, DISABLE);

	/* Disable the USART Transmit complete interrupt: this interrupt is 
	   generated when the USART transmit Shift Register is empty */
	USART_ClearITPendingBit(Usart_Global[com].usart, USART_IT_TC);
  	USART_ITConfig(Usart_Global[com].usart, USART_IT_TC, DISABLE);
	
	/* Disable the USART IDLE line interrupt */
  	USART_ITConfig(Usart_Global[com].usart, USART_IT_IDLE, DISABLE);	

	/* Disable USART */
	USART_Cmd(Usart_Global[com].usart, DISABLE);	

	/* RESET USART */
  	USART_DeInit(Usart_Global[com].usart);

	/* Set be NULL */
	Usart_Global[com].usart = NULL;

	if(Usart_Global[com].dma_enable == 1)
	{
		/* Disable The DMA controller clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, DISABLE);
		
		/* Deintialize DMA channels */
		DMA_DeInit(Usart_Global[com].tx_channel); //Tx
		DMA_DeInit(Usart_Global[com].rx_channel); //Rx

		/* Disable the USART Tx/Rx DMA requests */		
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_TX, DISABLE);
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_RX, DISABLE);
	
		/* Disable the USART Tx/Rx DMA requests */		
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_TX, DISABLE);
		USART_DMACmd(Usart_Global[com].usart, USART_DMAReq_RX, DISABLE);

		/* Configue NULL */
		Usart_Global[com].dma_enable = 0;
		Usart_Global[com].tx_channel = NULL;
		Usart_Global[com].rx_channel = NULL;
	}
	
	return 0;
}

/*******************************************************************************
 * 名称: putchar
 * 功能: 打印一个字符
 * 形参: 字符c
 * 返回: 字符/失败-1
 * 说明: 无 
 ******************************************************************************/
int putchar(int c)
{
	int send_timeout = UART_SEND_RECV_TIMEOUT;

	if(Usart_Printf == NULL)
	{
		return -1;
	}

	/* Write a character to the USART */  	
  	USART_SendData8(Usart_Printf, (uint8_t)c);

	/* Loop until the end of transmission or timeout */ 
  	while (USART_GetFlagStatus(Usart_Printf, USART_FLAG_TC) == RESET && --send_timeout);

	/* timeout return -1 */
	if(send_timeout == 0)
	{
		return -1;
	}

	return c;
}

/*******************************************************************************
 * 名称: getchar
 * 功能: 获取一个字符
 * 形参: 无
 * 返回: 字符/失败-1
 * 说明: 无 
 ******************************************************************************/
int getchar(void)
{
	int c = -1;	
	int recv_timeout = UART_SEND_RECV_TIMEOUT;
	
	if(Usart_Printf == NULL)
	{
		return -1;
	}

	/* Loop until the Read data register flag is SET or timeout */  
  	while (USART_GetFlagStatus(Usart_Printf, USART_FLAG_RXNE) == RESET && --recv_timeout);

	/* timeout return -1 */
	if(recv_timeout == 0)
	{
		return -1;
	}

	c = (int)USART_ReceiveData8(Usart_Printf);  

	return c;
}

/*
 * 一个字节数据接收中断处理函数
 */
int vkUsart_Recv_Byte(vkCOM com)
{
	int ret = 0;
	
	/* 一帧数据接收未完成，持续取数据 */
	if(Usart_Global[com].rx_idle == 0 && Usart_Global[com].rx_count < Usart_Global[com].rx_bufsize)
	{
		Usart_Global[com].rx_buffer[Usart_Global[com].rx_index++] = USART_ReceiveData8(Usart_Global[com].usart);
		Usart_Global[com].rx_count++;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

/*
 * 接收空闲中断处理函数
 */
int vkUsart_Recv_Line(vkCOM com)
{
	/* 关IDLE中断 */
	USART_ITConfig(Usart_Global[com].usart, USART_IT_IDLE, DISABLE);	
	Usart_Global[com].rx_idle = 1;
	
	if(Usart_Global[com].dma_enable == 1)
	{
		DMA_Cmd(Usart_Global[com].rx_channel, DISABLE);
	}
	
	return 0;
}

/*
 * 发送中断处理函数
 */
int vkUsart_Send_Byte(vkCOM com)
{
	/* 逐字节发送数据 */
	if(Usart_Global[com].tx_index < Usart_Global[com].tx_count)
	{
		/* Write a character to the USART */	
	  	USART_SendData8(Usart_Global[com].usart, Usart_Global[com].tx_buffer[Usart_Global[com].tx_index++]);		
	}
	/* 数据发送完成 */
	else
	{		
		Usart_Global[com].tx_doing = 0;
	}	
	return 0;
}

int vkUsart_INT_Send(vkCOM com, uint8_t *buf, int size)
{
	int ret = -1;	

	/* 发送缓存没有正在发送数据 */
	if(Usart_Global[com].tx_doing == 0)
	{
		Usart_Global[com].tx_doing = 1;
		
		ret = (size>Usart_Global[com].tx_bufsize)?Usart_Global[com].tx_bufsize:size;
		
		memcpy(Usart_Global[com].tx_buffer, buf, ret);

		Usart_Global[com].tx_count = ret;
		Usart_Global[com].tx_index = 0;

		vkUsart_Send_Byte(com);
	}
	
	return ret;
}

int vkUsart_INT_Recv(vkCOM com, uint8_t *buf, int size)
{
	int ret = -1;

	/* 接收缓存收到一帧数据 */
	if(Usart_Global[com].rx_idle == 1)
	{
		ret = (size>Usart_Global[com].rx_count)?Usart_Global[com].rx_count:size;
		
		memcpy(buf, Usart_Global[com].rx_buffer, ret);
		
		Usart_Global[com].rx_index = 0;	
		Usart_Global[com].rx_count = 0;

		/* 开IDLE中断 */
		Usart_Global[com].rx_idle = 0;	
		USART_ITConfig(Usart_Global[com].usart, USART_IT_IDLE, ENABLE);		
	}
	
	return ret;
}

/*******************************************************************************
 * 名称: vkUsart_DMA_Send
 * 功能: 发送字符串
 * 形参: 字符串指针buf,字符串大小size
 * 返回: -1超时，成功发送字符串大小
 * 说明: 无 
 ******************************************************************************/
int vkUsart_DMA_Send(vkCOM com, uint8_t *buf, int size)
{	
	int ret = -1;
	
	if(Usart_Global[com].tx_doing == 0)
	{
		Usart_Global[com].tx_doing = 1;
		
		/* Copy data to Tx Channel */
		ret = (size<Usart_Global[com].tx_bufsize)?size:Usart_Global[com].tx_bufsize;
		memcpy(Usart_Global[com].tx_buffer, buf, ret);

		DMA_Cmd(Usart_Global[com].tx_channel, DISABLE); 
	    DMA_SetCurrDataCounter(Usart_Global[com].tx_channel, ret); 
	    DMA_Cmd(Usart_Global[com].tx_channel, ENABLE);		
	}

	return ret;
}

/*******************************************************************************
 * 名称: vkUsart_DMA_Recv
 * 功能: 接收字符串
 * 形参: 字符串指针buf,字符串大小size
 * 返回: 成功接收字符串大小
 * 说明: 无 
 ******************************************************************************/
int vkUsart_DMA_Recv(vkCOM com, uint8_t *buf, int size)
{
	int ret = -1;
	
	/* 接收缓存收到一帧数据 */
	if(Usart_Global[com].rx_idle == 1)
	{
		/* 获取接收数据大小 */
		int len = Usart_Global[com].rx_bufsize-DMA_GetCurrDataCounter(Usart_Global[com].rx_channel);
		ret = (len<size)?len:size;
		memcpy(buf, Usart_Global[com].rx_buffer, ret);

		/* 准备接收数据 */
		DMA_Cmd(Usart_Global[com].rx_channel, DISABLE); 
		DMA_SetCurrDataCounter(Usart_Global[com].rx_channel, Usart_Global[com].rx_bufsize); 
    	DMA_Cmd(Usart_Global[com].rx_channel, ENABLE);

		/* 开IDLE中断 */
		Usart_Global[com].rx_idle = 0;	
		USART_ITConfig(Usart_Global[com].usart, USART_IT_IDLE, ENABLE);		
	}
	
	return ret;	
}

/*
 * 接收空闲中断处理函数
 */
int vkUsart_Send_Over(vkCOM com)
{
	Usart_Global[com].tx_doing = 0;

	return 0;
}

/*******************************************************************************
 * 名称: vkUsart_Send
 * 功能: 发送字符串
 * 形参: 字符串指针buf,字符串大小size
 * 返回: 成功发送字符串大小
 * 说明: 无 
 ******************************************************************************/
int vkUsart_Send(vkCOM com, uint8_t *buf, int size)
{
	/* 参数检查 */
	if(com != COM1 && com != COM2 && com != COM3)
	{
		return -1;
	}

	if(buf == NULL || size <= 0)
	{	
		return -1;
	}

	if(Usart_Global[com].dma_enable == 1)
	{
		return vkUsart_DMA_Send(com, buf, size);
	}
	else
	{
		return vkUsart_INT_Send(com, buf, size);
	}
}

/*******************************************************************************
 * 名称: vkUsart_Recv
 * 功能: 接收字符串
 * 形参: 字符串指针buf,字符串大小size
 * 返回: 成功接收字符串大小
 * 说明: 无 
 ******************************************************************************/
int vkUsart_Recv(vkCOM com, uint8_t *buf, int size)
{
	/* 参数检查 */
	if(com != COM1 && com != COM2 && com != COM3)
	{
		return -1;
	}

	if(buf == NULL || size <= 0)
	{	
		return -1;
	}

	if(Usart_Global[com].dma_enable == 1)
	{
		return vkUsart_DMA_Recv(com, buf, size);
	}
	else
	{
		return vkUsart_INT_Recv(com, buf, size);
	}	
}


