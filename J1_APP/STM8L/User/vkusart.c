/******************************************************************************
 * �ļ�  ��vkusart.c
 * ����    ��STM8L����
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* �����Զ���ͷ�ļ� */
#include "vkusart.h"
#include "vkQueue.h"

/* �Զ��������� */

/* �Զ���� */
#define UART_SEND_RECV_TIMEOUT 1000		//���ͽ��ճ�ʱ
#define gCOM COM1						//ϵͳĬ��printf��ӡ����

/* ȫ�ֱ������� */
static vkUSART gUsart[3] = {0x00};

/*******************************************************************************
 * ����: vkUsart_Init
 * ����: USART��ʼ������
 * �β�: ����USARTx, ������BaudRate
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_Init(vkCOM com, uint32_t BaudRate)
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
	if(BaudRate!=9600 && BaudRate!=115200 && BaudRate!=38400)
	{
		return -1;
	}

	if(com == COM1)
	{
		gUsart[com].usart = USART1;
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

		/* USART1 Tx- Rx (PC3- PC2) */
		#if 0
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
		
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);		
		#endif

		/* USART1 Tx- Rx (PC3- PC2) remapping to (PA2- PA3) */
		#if 1
		/* GPIO��ӳ�� */
		//SYSCFG_REMAPDeInit();
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
		
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_2, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, ENABLE);
		#endif

		/* USART1 Tx- Rx (PC3- PC2) remapping to (PC5- PC6) */
		#if 0
		/* GPIO��ӳ�� */
		//SYSCFG_REMAPDeInit();
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_5, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_6, ENABLE);
		#endif

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(USART1_RX_TIM5_CC_IRQn, ITC_PriorityLevel_2);

		/* Configure DMA channel */
		gUsart[com].dma_enable = 0;
		gUsart[com].tx_channel = NULL;
		gUsart[com].rx_channel = NULL;
	}
	else if(com == COM2)
	{
		gUsart[com].usart = USART2;
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_4, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_3, ENABLE);

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(TIM2_CC_USART2_RX_IRQn, ITC_PriorityLevel_2);

		/* Configure DMA channel */
		gUsart[com].dma_enable = 0;
		gUsart[com].tx_channel = NULL;
		gUsart[com].rx_channel = NULL;
	}
	else if(com == COM3)
	{
		gUsart[com].usart = USART3;

		/* USART3 Tx- Rx (PE6- PE7) */
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOE, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);	

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, ENABLE);

		/* Configure Interupt Software Priority */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(TIM3_CC_USART3_RX_IRQn, ITC_PriorityLevel_2);

		/* Configure DMA channel */
		gUsart[com].dma_enable = 0;
		gUsart[com].tx_channel = NULL;
		gUsart[com].rx_channel = NULL;
	}
	else
	{
		return -1;
	}

	/* RESET USART */
  	USART_DeInit(gUsart[com].usart);

  	/* USART configuration */
  	USART_Init(gUsart[com].usart, BaudRate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));	
	
  	/* Enable the USART Receive interrupt: this interrupt is generated 
  	   when the USART receive data register is not empty */
  	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_RXNE);
 	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, ENABLE);

	/* Enable the USART Transmit complete interrupt: this interrupt is 
	   generated when the USART transmit Shift Register is empty */
	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_TC);
  	USART_ITConfig(gUsart[com].usart, USART_IT_TC, ENABLE);	

	/* Enable the USART IDLE line interrupt */
	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_IDLE);
  	USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, ENABLE);	

	/* Enable USART */
	USART_Cmd(gUsart[com].usart, ENABLE);

	/* Create Tx/Rx queue buffer */
	if(vkQUEUE_OK != vkQueueCreate(&gUsart[com].tx_queue,gUsart[com].tx_buffer,sizeof(char),UART_SEND_RECV_BUFSIZE))
	{
		return -1;
	}
	if(vkQUEUE_OK != vkQueueCreate(&gUsart[com].rx_queue,gUsart[com].rx_buffer,sizeof(char),UART_SEND_RECV_BUFSIZE))
	{
		return -1;
	}

	gUsart[com].tx_doing = 0;
	gUsart[com].rx_idle = 0;

	if(com == COM1)
	{
		if(gUsart[COM3].dma_enable == 0)
		{
		 	#if (UART1_DMA_ENABLE == 1u)
			gUsart[com].dma_enable = 1;
			gUsart[com].tx_channel = DMA1_Channel1;
			gUsart[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
			
			/* Intialize DMA channels */
			DMA_DeInit(gUsart[com].tx_channel); //Tx
			DMA_DeInit(gUsart[com].rx_channel); //Rx
			DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			
			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

			/* DMA_ITConfig */
			//ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
			//ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

			/* Disable RXNE and TC  */
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	
			
			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			/* Enable the USART Tx/Rx DMA channel */
			DMA_Cmd(gUsart[com].tx_channel, ENABLE);
			DMA_Cmd(gUsart[com].rx_channel, ENABLE);
			#endif
		}
	}
	else if(com == COM3)
	{
		if(gUsart[COM1].dma_enable == 0)
		{
		 	#if (UART3_DMA_ENABLE == 1u)
			gUsart[com].dma_enable = 1;
			gUsart[com].tx_channel = DMA1_Channel1;
			gUsart[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
			
			/* Intialize DMA channels */
			DMA_DeInit(gUsart[com].tx_channel); //Tx
			DMA_DeInit(gUsart[com].rx_channel); //Rx
			DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
			
			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

			/* DMA_ITConfig */
			//ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
			//ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

			/* Disable RXNE and TC  */
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	
			
			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			/* Enable the USART Tx/Rx DMA channel */
			DMA_Cmd(gUsart[com].tx_channel, ENABLE);
			DMA_Cmd(gUsart[com].rx_channel, ENABLE);
			#endif
		}		
	}
	else if(com == COM2)
	{
		#if (UART2_DMA_ENABLE==1u)
		gUsart[com].dma_enable = 1;
		gUsart[com].tx_channel = DMA1_Channel0;
		gUsart[com].rx_channel = DMA1_Channel3;

		/* Enable The DMA controller clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
		
		/* Intialize DMA channels */
		DMA_DeInit(gUsart[com].tx_channel); //Tx
		DMA_DeInit(gUsart[com].rx_channel); //Rx
		DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
		DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_buffer, (uint16_t)gUsart[com].usart->DR, UART_SEND_RECV_BUFSIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low, DMA_MemoryDataSize_Byte);
		
		/* Enable the USART Tx/Rx DMA requests */		
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

		/* DMA_ITConfig */
		//ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
		//ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

		/* Disable RXNE and TC  */
		USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
		USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	
		
		/* Global DMA Enable */
		DMA_GlobalCmd(ENABLE);
		
		/* Enable the USART Tx/Rx DMA channel */
		DMA_Cmd(gUsart[com].tx_channel, ENABLE);
		DMA_Cmd(gUsart[com].rx_channel, ENABLE);
			
		#endif		
	}
	
	return 0;
}

/*******************************************************************************
 * ����: vkUsart_Deinit
 * ����: USART��ʼ������
 * �β�: ����USARTx, ������BaudRate
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
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

		#if 0
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, DISABLE);
		#endif

		#if 1
		/* Disable USART1 Tx- Rx remapping to PA2- PA3 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, DISABLE);
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_2, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, DISABLE);
		#endif

		#if 0
		/* Disable USART1 Tx- Rx remapping to PC5- PC6 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, DISABLE);
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_5, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_6, DISABLE);
		#endif		
	}
	else if(com == COM2)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_4, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_3, DISABLE);
	}
	else if(com == COM3)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, DISABLE);		
	}
	else
	{
		return -1;
	}

  	/* Disable the USART Receive interrupt: this interrupt is generated 
  	   when the USART receive data register is not empty */
  	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_RXNE);
 	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);

	/* Disable the USART Transmit complete interrupt: this interrupt is 
	   generated when the USART transmit Shift Register is empty */
	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_TC);
  	USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	

	/* Disable USART */
	USART_Cmd(gUsart[com].usart, DISABLE);	

	/* RESET USART */
  	USART_DeInit(gUsart[com].usart);

	/* Free buffer memmory */
	if(NULL!=gUsart[com].tx_buffer)
	{
		free(gUsart[com].tx_buffer);
	}
	if(NULL!=gUsart[com].rx_buffer)
	{
		free(gUsart[com].rx_buffer);
	}

	/* Destory Tx/Rx queue buffer */
	if(vkQUEUE_OK != vkQueueDestory(&gUsart[com].tx_queue))
	{
		return -1;
	}
	if(vkQUEUE_OK != vkQueueDestory(&gUsart[com].rx_queue))
	{
		return -1;
	}

	/* Set be NULL */
	gUsart[com].usart = NULL;

	if(gUsart[com].dma_enable == 1)
	{
		/* Disable The DMA controller clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, DISABLE);
		
		/* Deintialize DMA channels */
		DMA_DeInit(gUsart[com].tx_channel); //Tx
		DMA_DeInit(gUsart[com].rx_channel); //Rx

		/* Disable the USART Tx/Rx DMA requests */		
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, DISABLE);
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, DISABLE);

		/* Global DMA Disable */
		DMA_GlobalCmd(DISABLE);
		
		/* Enable the USART Tx/Rx DMA channel */
		DMA_Cmd(gUsart[com].tx_channel, DISABLE);
		DMA_Cmd(gUsart[com].rx_channel, DISABLE);

		/* Configue NULL */
		gUsart[com].dma_enable = 0;
		gUsart[com].tx_channel = NULL;
		gUsart[com].rx_channel = NULL;
	}
	
	return 0;
}

/*******************************************************************************
 * ����: putchar
 * ����: ��ӡһ���ַ�
 * �β�: �ַ�c
 * ����: �ַ�/ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int putchar(int c)
{
	int send_timeout = UART_SEND_RECV_TIMEOUT;

	if(gUsart[gCOM].usart == NULL)
	{
		return -1;
	}

	/* Write a character to the USART */  	
  	USART_SendData8(gUsart[gCOM].usart, (uint8_t)c);

	/* Loop until the end of transmission or timeout */ 
  	while (USART_GetFlagStatus(gUsart[gCOM].usart, USART_FLAG_TC) == RESET && --send_timeout);

	/* timeout return -1 */
	if(send_timeout == 0)
	{
		return -1;
	}

	return c;
}

/*******************************************************************************
 * ����: getchar
 * ����: ��ȡһ���ַ�
 * �β�: ��
 * ����: �ַ�/ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int getchar(void)
{
	int c = -1;	
	int recv_timeout = UART_SEND_RECV_TIMEOUT;

	if(gUsart[gCOM].usart == NULL)
	{
		return -1;
	}

	/* Loop until the Read data register flag is SET or timeout */  
  	while (USART_GetFlagStatus(gUsart[gCOM].usart, USART_FLAG_RXNE) == RESET && --recv_timeout);

	/* timeout return -1 */
	if(recv_timeout == 0)
	{
		return -1;
	}

	c = (int)USART_ReceiveData8(gUsart[gCOM].usart);  

	return c;
}

/*
 * �����жϴ�����
 */
int vkUsart_Recv_Byte(vkCOM com)
{
	uint8_t c = USART_ReceiveData8(gUsart[com].usart); 

	/* ���ջ�����д���һ������ */
	if(gUsart[com].rx_idle == 1)
	{
		return -1;
	}

	if(vkQUEUE_OK != vkQueuePut(&gUsart[com].rx_queue, 0, &c))
	{
		return -1;
	}
	
	return 0;
}

/*
 * ���տ����жϴ�����
 */
int vkUsart_Recv_Line(vkCOM com)
{
	gUsart[com].rx_idle = 1;

	if(gUsart[com].dma_enable == 1)
	{
		DMA_Cmd(gUsart[com].rx_channel, DISABLE); 
	}
	
	return 0;
}

/*
 * �����жϴ�����
 */
int vkUsart_Send_Byte(vkCOM com)
{
	uint8_t c;

	/* ������������ݣ��ҷ������ݼĴ����ǿ� */
	if(vkQUEUE_OK != vkQueueGet(&gUsart[com].tx_queue, 0, &c))
	{
		gUsart[com].tx_doing = 0;		
		return -1;
	}

	/* Write a character to the USART */
	gUsart[com].tx_doing = 1;
  	USART_SendData8(gUsart[com].usart, (uint8_t)c);	
	
	return 0;
}

int vkUsart_QUE_Send(vkCOM com, uint8_t *buf, int size)
{
	int ret = 0;

	/* �������ݷ��뻺����� */
	for(int i=0; i<size; i++)
	{
		if(vkQUEUE_OK != vkQueuePut(&gUsart[com].tx_queue, 0, (uint8_t *)buf+i))
		{
			break;
		}

		ret++;
	}

	/* ������������� */
	if(vkQUEUE_NULL != vkQueueEmpty(&gUsart[com].tx_queue) && gUsart[com].tx_doing == 0)
	{
		vkUsart_Send_Byte(com);
	}	
	
	return ret;
}

int vkUsart_QUE_Recv(vkCOM com, uint8_t *buf, int size)
{
	int ret = 0;

	/* ���ջ�����в�����һ������ */
	if(gUsart[com].rx_idle == 0)
	{
		return -1;
	}

	/* �ӻ�������ж������� */
	for(int i=0; i<size; i++)
	{
		if(vkQUEUE_OK != vkQueueGet(&gUsart[com].rx_queue, 0, (uint8_t *)buf+i))
		{
			break;
		}

		ret++;		
	}

	gUsart[com].rx_idle = 0;

	return ret;
}

/*******************************************************************************
 * ����: vkUsart_DMA_Send
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: -1��ʱ���ɹ������ַ�����С
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_DMA_Send(vkCOM com, uint8_t *buf, int size)
{	
	int ret = 0;
	int send_timeout = UART_SEND_RECV_TIMEOUT;

	/* Tx channel remaining data && not timeout */
	while(DMA_GetCurrDataCounter(gUsart[com].tx_channel) && --send_timeout);

	/* if timeout, send failed */
	if(send_timeout == 0)
	{
		return -1;
	}

	/* Copy data to Tx Channel */
	if(gUsart[com].tx_buffer != NULL)
	{
		ret = (size<UART_SEND_RECV_BUFSIZE)?size:UART_SEND_RECV_BUFSIZE;
		
		memcpy(gUsart[com].tx_buffer, buf, ret);
	}

	DMA_Cmd(gUsart[com].tx_channel, DISABLE); 
    DMA_SetCurrDataCounter(gUsart[com].tx_channel, ret); 
    DMA_Cmd(gUsart[com].tx_channel, ENABLE);	

	return ret;
}

/*******************************************************************************
 * ����: vkUsart_DMA_Recv
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: �ɹ������ַ�����С
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_DMA_Recv(vkCOM com, uint8_t *buf, int size)
{
	int ret = 0;
	
	/* ���ջ�����в�����һ������ */
	if(gUsart[com].rx_idle == 0)
	{
		return -1;
	}

	/* ��ȡ�������ݴ�С */
	ret = DMA_GetCurrDataCounter(gUsart[com].rx_channel);

	ret = (ret<size)?ret:size;
	
	/* Copy data From Rx Channel */
	if(gUsart[com].rx_buffer != NULL)
	{
		memcpy(buf, gUsart[com].rx_buffer, ret);
	}
	
	DMA_Cmd(gUsart[com].rx_channel, DISABLE); 
    DMA_SetCurrDataCounter(gUsart[com].rx_channel, UART_SEND_RECV_BUFSIZE); 
    DMA_Cmd(gUsart[com].rx_channel, ENABLE);

	gUsart[com].rx_idle = 0;

	return ret;	
}

/*******************************************************************************
 * ����: vkUsart_Send
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: �ɹ������ַ�����С
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_Send(vkCOM com, uint8_t *buf, int size)
{
	if(gUsart[com].dma_enable == 1)
	{
		return vkUsart_DMA_Send(com, buf, size);
	}
	else
	{
		return vkUsart_QUE_Send(com, buf, size);
	}
}

/*******************************************************************************
 * ����: vkUsart_Recv
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: �ɹ������ַ�����С
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_Recv(vkCOM com, uint8_t *buf, int size)
{
	if(gUsart[com].dma_enable == 1)
	{
		return vkUsart_DMA_Recv(com, buf, size);
	}
	else
	{
		return vkUsart_QUE_Recv(com, buf, size);
	}	
}


