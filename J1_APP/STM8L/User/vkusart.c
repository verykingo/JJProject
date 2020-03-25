/******************************************************************************
 * �ļ�  ��vkusart.c
 * ����    ��STM8L���ڣ�֧�ַ��ͽ��ջ��棬֧�ֲ�ѯ��ʽ���жϷ�ʽ��DMA��ʽ���ͽ���
           ���ݡ�֧��ϵͳprintf������
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

/* �Զ��������� */

/* �Զ���� */
#define USART_SEND_RECV_TIMEOUT 1000		//���ͽ��ճ�ʱ

/* ȫ�ֱ������� */
static uint8_t Usart1_TxBuffer[USART1_BUFFER_TX_SIZE] = {0};
static uint8_t Usart1_RxBuffer[USART1_BUFFER_RX_SIZE] = {0};

//static uint8_t Usart2_TxBuffer[USART2_BUFFER_SIZE] = {0};
//static uint8_t Usart2_RxBuffer[USART2_BUFFER_SIZE] = {0};

static uint8_t Usart3_TxBuffer[USART3_BUFFER_SIZE] = {0};
static uint8_t Usart3_RxBuffer[USART3_BUFFER_SIZE] = {0};

static vkUSART gUsart[3] = {0x00};

/* ��׼��printf���ڣ�����putchar/getchar�������������жϺ�DMA��ʹ�ò�ѯ��ʽ */
static USART_TypeDef* Usart_Printf = USART3;

/*******************************************************************************
 * ����: vkUsart_Init
 * ����: USART��ʼ������
 * �β�: ����com, ������baudrate
 * ����: �ɹ�0��ʧ��-1
 * ˵��: 1.֧�ֲ�����9600/115200/38400��
         2.֧���жϷ�ʽ��DMA��ʽ���ͺͽ������ݣ�
 ******************************************************************************/
int vkUsart_Init(vkCOM com, uint32_t baudrate, uint8_t* tx_fbuf, uint8_t tx_fsize, uint8_t* rx_fbuf, uint8_t rx_fsize)
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
	if(com>=COMMAX)
	{
		printf("%s Param Error.\r\n", __FUNCTION__);
		
		return -1;		
	}	
	
	/* BaudRate check */
	if(baudrate!=9600 && baudrate!=115200 && baudrate!=230400)
	{	
		printf("%s Baudrate Error.\r\n", __FUNCTION__);
		
		return -1;
	}

	/* TX/RX frame Check */
	if(tx_fbuf==NULL || rx_fbuf==NULL || tx_fsize==0 || rx_fsize==0)
	{	
		printf("%s Frame Error.\r\n", __FUNCTION__);
		
		return -1;
	}
	if(tx_fsize > USARTX_UFRAME_SIZE || rx_fsize > USARTX_UFRAME_SIZE)
	{
		printf("%s Framesize Error.\r\n", __FUNCTION__);
		
		return -1;
	}

	if(com == COM1)
	{		
		printf("%s COM%d Using Tx[PC3] Rx[PC2]\r\n", __FUNCTION__,(int)com);
		
		gUsart[com].usart = USART1;
		
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

		/* 48���� USART1 Tx- Rx (PC3- PC2) remapping to (PA2- PA3) */
		#if 0
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

		/* 64���� USART1 Tx- Rx (PC3- PC2) */
		#if 1
		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_High_Fast);
		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);
		
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);		
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
		//ITC_SetSoftwarePriority(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_2);
		//ITC_SetSoftwarePriority(USART1_RX_TIM5_CC_IRQn, ITC_PriorityLevel_2);

		gUsart[com].tx_frame = 0;
		gUsart[com].tx_buffer.locked = 0;
		gUsart[com].rx_buffer.locked = 0;
		
		gUsart[com].tx_buffer.buffer = Usart1_TxBuffer;
		gUsart[com].rx_buffer.buffer = Usart1_RxBuffer;

		gUsart[com].tx_buffer.frame_size = tx_fsize;
		gUsart[com].rx_buffer.frame_size = rx_fsize;

		gUsart[com].tx_buffer.frame_nums = (USART1_BUFFER_TX_SIZE/tx_fsize<255)?(USART1_BUFFER_TX_SIZE/tx_fsize):255;
		gUsart[com].rx_buffer.frame_nums = (USART1_BUFFER_RX_SIZE/rx_fsize<255)?(USART1_BUFFER_RX_SIZE/rx_fsize):255;

		gUsart[com].tx_buffer.frame_stored = 0;
		gUsart[com].rx_buffer.frame_stored = 0;

		gUsart[com].tx_buffer.index_insert = 0;
		gUsart[com].rx_buffer.index_insert = 0;
		
		gUsart[com].tx_buffer.index_delete = 0;
		gUsart[com].rx_buffer.index_delete = 0;
	}
	else if(com == COM2)
	{
	#if 0		
		printf("%s COM%d Using Tx[PE4] Rx[PE3]\r\n", __FUNCTION__,(int)com);

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

		gUsart[com].tx_frame = 0;
		gUsart[com].tx_buffer.locked = 0;
		gUsart[com].rx_buffer.locked = 0;
		
		gUsart[com].tx_buffer.buffer = Usart2_TxBuffer;
		gUsart[com].rx_buffer.buffer = Usart2_RxBuffer;

		gUsart[com].tx_buffer.frame_size = tx_bufsize;
		gUsart[com].rx_buffer.frame_size = rx_bufsize;

		gUsart[com].tx_buffer.frame_nums = (USART2_BUFFER_SIZE/tx_bufsize<255)?(USART2_BUFFER_SIZE/tx_bufsize):255;
		gUsart[com].rx_buffer.frame_nums = (USART2_BUFFER_SIZE/rx_bufsize<255)?(USART2_BUFFER_SIZE/rx_bufsize):255;

		gUsart[com].tx_buffer.frame_stored = 0;
		gUsart[com].rx_buffer.frame_stored = 0;

		gUsart[com].tx_buffer.index_insert = 0;
		gUsart[com].rx_buffer.index_insert = 0;
		
		gUsart[com].tx_buffer.index_delete = 0;
		gUsart[com].rx_buffer.index_delete = 0;

	#endif
	}
	else if(com == COM3)
	{
		printf("%s COM%d Using Tx[PG1] Rx[PG0]\r\n", __FUNCTION__,(int)com);
	
		gUsart[com].usart = USART3;

		/* 48���� USART3 Tx- Rx (PE6- PE7) */
		#if 0
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOE, GPIO_Pin_6, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_In_PU_No_IT);	

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, ENABLE);
		#endif

		/* 64���� USART3 Tx- Rx (PG1- PG0) */
		#if 1
		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);	

		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOG, GPIO_Pin_1, ENABLE);
		GPIO_ExternalPullUpConfig(GPIOG, GPIO_Pin_0, ENABLE);
		#endif		
		/* Configure Interupt Software Priority */
		//ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQn, ITC_PriorityLevel_2);
		//ITC_SetSoftwarePriority(TIM3_CC_USART3_RX_IRQn, ITC_PriorityLevel_2);
		
		gUsart[com].tx_frame = 0;
		gUsart[com].tx_buffer.locked = 0;
		gUsart[com].rx_buffer.locked = 0;

		gUsart[com].tx_buffer.buffer = Usart3_TxBuffer;
		gUsart[com].rx_buffer.buffer = Usart3_RxBuffer;

		gUsart[com].tx_buffer.frame_size = tx_fsize;
		gUsart[com].rx_buffer.frame_size = rx_fsize;

		gUsart[com].tx_buffer.frame_nums = (USART3_BUFFER_SIZE/tx_fsize<255)?(USART3_BUFFER_SIZE/tx_fsize):255;
		gUsart[com].rx_buffer.frame_nums = (USART3_BUFFER_SIZE/rx_fsize<255)?(USART3_BUFFER_SIZE/rx_fsize):255;

		gUsart[com].tx_buffer.frame_stored = 0;
		gUsart[com].rx_buffer.frame_stored = 0;

		gUsart[com].tx_buffer.index_insert = 0;
		gUsart[com].rx_buffer.index_insert = 0;
		
		gUsart[com].tx_buffer.index_delete = 0;
		gUsart[com].rx_buffer.index_delete = 0;
	}
	else
	{		
		printf("%s COM%d Failed!\r\n", __FUNCTION__,(int)com);
		return -1;
	}

	/* Disable USART */
	USART_Cmd(gUsart[com].usart, DISABLE);

	/* RESET USART */
  	USART_DeInit(gUsart[com].usart);

  	/* USART configuration */
  	USART_Init(gUsart[com].usart, baudrate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));	

	/* ������ڷǱ�׼�⺯��prinft���ڣ������жϷ�ʽ */
	if(gUsart[com].usart != Usart_Printf)
	{		
		printf("%s COM%d Using INT Mode\r\n", __FUNCTION__,(int)com);
		
	  	/* Enable the USART Receive interrupt: this interrupt is generated 
	  	   when the USART receive data register is not empty */
	 	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, ENABLE);

		/* Enable the USART Transmit complete interrupt: this interrupt is 
		   generated when the USART transmit Shift Register is empty */
		USART_ClearITPendingBit(gUsart[com].usart, USART_IT_TC);
	  	USART_ITConfig(gUsart[com].usart, USART_IT_TC, ENABLE);	

		/* Enable the USART IDLE line interrupt */
		//USART_ClearITPendingBit(gUsart[com].usart, USART_IT_IDLE);
	  	USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, ENABLE);	
	}
	else
	{		
		printf("%s COM%d Using BaudRate %d00, as Default printf()\r\n", __FUNCTION__,(int)com,(int)(baudrate/100));
	}
	
	/* Enable USART */
	USART_Cmd(gUsart[com].usart, ENABLE);

	/* Set Zero */
	gUsart[com].tx_data = tx_fbuf;
	gUsart[com].rx_data = rx_fbuf;
	gUsart[com].tx_size = tx_fsize;
	gUsart[com].rx_size = rx_fsize;
	gUsart[com].tx_index = 0;
	gUsart[com].rx_index = 0;	
	gUsart[com].tx_count = 0;
	gUsart[com].rx_count = 0;	
	gUsart[com].tx_doing = 0;
	gUsart[com].rx_idle = 0;

	/* Configure DMA channel */
	gUsart[com].dma_enable = 0;
	gUsart[com].tx_channel = NULL;
	gUsart[com].rx_channel = NULL;

	/* DMA Configure */
	/* ���ڷǱ�׼�⺯��prinft���ڣ�����Ҫ����DMA��ʽ */
	if(com == COM1 && gUsart[com].usart != Usart_Printf)
	{
		if(gUsart[COM3].dma_enable == 0)
		{
		 	#if (UART1_DMA_ENABLE == 1u)
			
			printf("%s COM%d Using DMA Mode\r\n", __FUNCTION__,(int)com);

			/* Disable RXNE and TC  */
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	
			
			/* ��ʼ����DMA���� */
			gUsart[com].dma_enable = 1;
			gUsart[com].tx_channel = DMA1_Channel1;
			gUsart[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

			/* Disable the USART Tx/Rx DMA channel */
			DMA_Cmd(gUsart[com].tx_channel, DISABLE);
			DMA_Cmd(gUsart[com].rx_channel, DISABLE);

			/* Deintialize DMA channels */
			DMA_DeInit(gUsart[com].tx_channel);
			DMA_DeInit(gUsart[com].rx_channel);
			
			/* Intialize DMA channels */
			DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_data, (uint16_t)&gUsart[com].usart->DR, gUsart[com].tx_size, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_data, (uint16_t)&gUsart[com].usart->DR, gUsart[com].rx_size, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			
			/* DMA_ITConfig */
			DMA_ClearFlag(DMA1_FLAG_TC1);			
			DMA_ITConfig(gUsart[com].tx_channel, DMA_ITx_TC, ENABLE);
			
			ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);	//����ͨ�������ȼ�
			ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); //����ͨ�������ȼ�

			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			#endif
		}
	}
	else if(com == COM3 && gUsart[com].usart != Usart_Printf)
	{
		if(gUsart[COM1].dma_enable == 0)
		{
		 	#if (UART3_DMA_ENABLE == 1u)
			
			printf("%s COM%d Using DMA Mode\r\n", __FUNCTION__,(int)com);

			/* Disable RXNE and TC  */
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
			USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);	
			
			/* ��ʼ����DMA���� */
			gUsart[com].dma_enable = 1;
			gUsart[com].tx_channel = DMA1_Channel1;
			gUsart[com].rx_channel = DMA1_Channel2;

			/* Enable The DMA controller clock */
			CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

			/* Disable the USART Tx/Rx DMA channel */
			DMA_Cmd(gUsart[com].tx_channel, DISABLE);
			DMA_Cmd(gUsart[com].rx_channel, DISABLE);

			/* Deintialize DMA channels */
			DMA_DeInit(gUsart[com].tx_channel);
			DMA_DeInit(gUsart[com].rx_channel);
			
			/* Intialize DMA channels */
			DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_data, (uint16_t)&gUsart[com].usart->DR, gUsart[com].tx_size, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_data, (uint16_t)&gUsart[com].usart->DR, gUsart[com].rx_size, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
			
			/* DMA_ITConfig */
			DMA_ClearFlag(DMA1_FLAG_TC1);			
			DMA_ITConfig(gUsart[com].tx_channel, DMA_ITx_TC, ENABLE);
			
			ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
			ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

			/* Enable the USART Tx/Rx DMA requests */		
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
			USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

			/* Global DMA Enable */
			DMA_GlobalCmd(ENABLE);
			
			#endif
		}		
	}
	else if(com == COM2 && gUsart[com].usart != Usart_Printf)
	{
	#if 0	
		#if (UART2_DMA_ENABLE==1u)
		
		printf("%s COM%d Using DMA Mode\r\n", __FUNCTION__,(int)com);
		
		/* Disable RXNE and TC  */
		USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);
		USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);			

		/* ��ʼ����DMA���� */
		gUsart[com].dma_enable = 1;
		gUsart[com].tx_channel = DMA1_Channel0;
		gUsart[com].rx_channel = DMA1_Channel3;

		/* Enable The DMA controller clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

		/* Disable the USART Tx/Rx DMA channel */
		DMA_Cmd(gUsart[com].tx_channel, DISABLE);
		DMA_Cmd(gUsart[com].rx_channel, DISABLE);

		/* Deintialize DMA channels */
		DMA_DeInit(gUsart[com].tx_channel);
		DMA_DeInit(gUsart[com].rx_channel);	
		
		/* Intialize DMA channels */
		DMA_Init(gUsart[com].tx_channel, (uint32_t)gUsart[com].tx_data, (uint16_t)gUsart[com].usart->DR, gUsart[com].tx_size, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);
		DMA_Init(gUsart[com].rx_channel, (uint32_t)gUsart[com].rx_data, (uint16_t)gUsart[com].usart->DR, gUsart[com].rx_size, DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_VeryHigh, DMA_MemoryDataSize_Byte);

		/* DMA_ITConfig */
		DMA_ClearFlag(DMA1_FLAG_TC0);			
		DMA_ITConfig(gUsart[com].tx_channel, DMA_ITx_TC, ENABLE);

		/* DMA_ITConfig */
		ITC_SetSoftwarePriority(DMA1_CHANNEL0_1_IRQn, ITC_PriorityLevel_2);
		ITC_SetSoftwarePriority(DMA1_CHANNEL2_3_IRQn, ITC_PriorityLevel_2); 

		/* Enable the USART Tx/Rx DMA requests */		
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, ENABLE);
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, ENABLE);

		/* Global DMA Enable */
		DMA_GlobalCmd(ENABLE);			
		#endif	
	#endif
	}
	
	return 0;
}

/*******************************************************************************
 * ����: vkUsart_Deinit
 * ����: USART��ʼ������
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_Deinit(vkCOM com)
{
	/* USART check */
	if(com>=COMMAX)
	{
		return -1;
	}	
	
	if(com == COM1)
	{
		/* Disable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);

		#if 0
		/* Disable USART1 Tx- Rx remapping to PA2- PA3 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, DISABLE);
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_2, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_3, DISABLE);
		#endif
		
		#if 1
		/* �Ƿ�ʹ���ڲ��������� */
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, DISABLE);
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

		/* 48���� �Ƿ�ʹ���ڲ��������� */
		#if 0
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_6, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOE, GPIO_Pin_7, DISABLE);	
		#endif 

		/* 64���� �Ƿ�ʹ���ڲ��������� */
		#if 1
		GPIO_ExternalPullUpConfig(GPIOG, GPIO_Pin_1, DISABLE);
		GPIO_ExternalPullUpConfig(GPIOG, GPIO_Pin_0, DISABLE);	
		#endif 
	}
	else
	{
		return -1;
	}

  	/* Disable the USART Receive interrupt: this interrupt is generated 
  	   when the USART receive data register is not empty */
 	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);

	/* Disable the USART Transmit complete interrupt: this interrupt is 
	   generated when the USART transmit Shift Register is empty */
	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_TC);
  	USART_ITConfig(gUsart[com].usart, USART_IT_TC, DISABLE);
	
	/* Disable the USART IDLE line interrupt */
  	USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, DISABLE);	

	/* Disable USART */
	USART_Cmd(gUsart[com].usart, DISABLE);	

	/* RESET USART */
  	USART_DeInit(gUsart[com].usart);

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
	
		/* Disable the USART Tx/Rx DMA requests */		
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_TX, DISABLE);
		USART_DMACmd(gUsart[com].usart, USART_DMAReq_RX, DISABLE);

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
	int send_timeout = USART_SEND_RECV_TIMEOUT;

	if(Usart_Printf == NULL)
	{
		return -1;
	}

	/* Write a character to the USART */  	
  	USART_SendData8(Usart_Printf, (uint8_t)c);

	/* Loop until the end of transmission or timeout */ 
  	while (USART_GetFlagStatus(Usart_Printf, USART_FLAG_TC) == RESET && --send_timeout)
	{nop(); /* ��ֹ�������Ż��� */}

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
	int recv_timeout = USART_SEND_RECV_TIMEOUT;
	
	if(Usart_Printf == NULL)
	{
		return -1;
	}

	/* Loop until the Read data register flag is SET or timeout */  
  	while (USART_GetFlagStatus(Usart_Printf, USART_FLAG_RXNE) == RESET && --recv_timeout)
	{nop(); /* ��ֹ�������Ż��� */}

	/* timeout return -1 */
	if(recv_timeout == 0)
	{
		return -1;
	}

	c = (int)USART_ReceiveData8(Usart_Printf);  

	return c;
}

/*******************************************************************************
 * ����: vkUsart_INT_Recv_Byte_Over_Handler
 * ����: USART�жϷ�ʽ����һ���ֽ����ݴ�����
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_INT_Recv_Byte_Over_Handler(vkCOM com)
{
	int ret = 0;

	/* һ֡���ݽ���δ��ɣ�����ȡ���� */
	if(gUsart[com].rx_idle == 0 && gUsart[com].rx_count < gUsart[com].rx_size)
	{
		gUsart[com].rx_data[gUsart[com].rx_index++] = USART_ReceiveData8(gUsart[com].usart);
		gUsart[com].rx_count++;
	}
	else
	{
		ret = -1;
	}

	return ret;
}

/*******************************************************************************
 * ����: vkUsart_Recv_Line_Handler
 * ����: USART����һ֡���ݴ�����
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_Recv_Line_Handler(vkCOM com)
{	
	int ret = -1;
	int size = 0;
	int full = 0;
	uint8_t data[USARTX_UFRAME_SIZE] = {0};

	if(gUsart[com].dma_enable == 1)
	{
		/* �������� */
		size = vkUsart_DMA_Recv(com, data+1, USARTX_UFRAME_SIZE-1);
		data[0] = size;
	}
	else
	{
		/* �������� */
		size = vkUsart_INT_Recv(com, data+1, USARTX_UFRAME_SIZE-1);
		data[0] = size;
	}

	/* ���뻺�� */
	if(size > 0)
	{
		ret = vkUsart_Buffer_Insert(&gUsart[com].rx_buffer, data, size+1);
	}
	
	full = vkUsart_Buffer_Full(&gUsart[com].rx_buffer);

	if(ret == 0)
	{
		ret = size;
	}

	if(full == 1)
	{
		/* ���뻺�����ݺ󣬻�����������˵�������ٽ������ݣ���Ҫֹͣ */
		/* ֹͣ���ݽ��� */
		if(gUsart[com].dma_enable == 1)
		{
			/* �ر�DMAͨ�� */
			DMA_Cmd(gUsart[com].rx_channel, DISABLE); 

			/* ��IDLE�ж� */	
			USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, DISABLE);	
		}
		else
		{
			/* ���ֽڽ����ж� */
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);

			/* ��IDLE�ж� */		
			USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, DISABLE);			
		}
	}
	
	return 0;
}

/*******************************************************************************
 * ����: vkUsart_INT_Send_Byte_Over_Handler
 * ����: USART�жϷ�ʽ����һ���ֽ����ݴ�����
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_INT_Send_Byte_Over_Handler(vkCOM com)
{
	/* ���ֽڷ������� */
	if(gUsart[com].tx_index < gUsart[com].tx_count)
	{
		/* Write a character to the USART */	
		USART_SendData8(gUsart[com].usart, gUsart[com].tx_data[gUsart[com].tx_index++]);

		gUsart[com].tx_doing = 1;
	}
	/* ���ݷ������ */
	else
	{	
		/* һ֡���ݷ�����ɣ�ֹͣ�ж��ֽڷ������� */
		gUsart[com].tx_doing = 0;

		/* һ֡���ݷ�����ɣ�ȡ��һ֡���� */
		vkUsart_INT_Send_Line_Over_Handler(com);
	}	

	return 0;
}

/*******************************************************************************
 * ����: vkUsart_INT_Send
 * ����: USART�жϷ�ʽ����һ֡������
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_INT_Send(vkCOM com, uint8_t *buf, int size)
{
	int ret = -1;	

	/* �������ݵ����ͻ��� */
	ret = (size>gUsart[com].tx_size)?gUsart[com].tx_size:size;
	memcpy(gUsart[com].tx_data, buf, ret);

	/* ��ʼ�������� */
	gUsart[com].tx_count = ret;
	gUsart[com].tx_index = 0;

	/* ���û�����ڷ����ֽ����� */
	if(gUsart[com].tx_doing == 0)
	{		
		vkUsart_INT_Send_Byte_Over_Handler(com);
	}
	
	return ret;
}

/*******************************************************************************
 * ����: vkUsart_INT_Recv
 * ����: USART�жϷ�ʽ����һ֡������
 * �β�: ����com
 * ����: �ɹ�0��ʧ��-1
 * ˵��: �� 
 ******************************************************************************/
int vkUsart_INT_Recv(vkCOM com, uint8_t *buf, int size)
{
	int ret = -1;

	gUsart[com].rx_idle  = 1;
	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, DISABLE);

	/* �����Ѿ��յ���һ֡���� */
	ret = (size>gUsart[com].rx_count)?gUsart[com].rx_count:size;
	memcpy(buf, gUsart[com].rx_data, ret);

	/* ���¼����յ������� */
	gUsart[com].rx_index = 0; 
	gUsart[com].rx_count = 0;
	gUsart[com].rx_idle  = 0;
	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, ENABLE);

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
	int ret = -1;

	DMA_Cmd(gUsart[com].tx_channel, DISABLE); 

	/* Copy data to Tx Channel */
	ret = (size<gUsart[com].tx_size)?size:gUsart[com].tx_size;
	memcpy(gUsart[com].tx_data, buf, ret);

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
	int ret = -1;

	DMA_Cmd(gUsart[com].rx_channel, DISABLE); 

	/* ��ȡ�������ݴ�С���������� */
	int len = gUsart[com].rx_size-DMA_GetCurrDataCounter(gUsart[com].rx_channel);
	ret = (len<size)?len:size;
	memcpy(buf, gUsart[com].rx_data, ret);

	/* ׼����һ�ν������� */
	DMA_SetCurrDataCounter(gUsart[com].rx_channel, gUsart[com].rx_size); 
	DMA_Cmd(gUsart[com].rx_channel, ENABLE);

	return ret;	
}

/*
 * DAM��������жϴ�����
 */
int vkUsart_DMA_Send_Line_Over_Handler(vkCOM com)
{
	int ret = -1;
	uint8_t data[USARTX_UFRAME_SIZE] = {0};

	/* ɾ��һ֡�������� */
	ret = vkUsart_Buffer_Delete(&gUsart[com].tx_buffer, data, USARTX_UFRAME_SIZE);

	if(ret == 0)
	{
		/* �ɹ��ӷ��ͻ���ɾ��һ֡���ݣ��������� */
		ret = data[0];
		vkUsart_DMA_Send(com, data+1, ret);

		gUsart[com].tx_frame = 1;
	}
	else
	{
		gUsart[com].tx_frame = 0;
	}

	return ret;
}

int vkUsart_INT_Send_Line_Over_Handler(vkCOM com)
{
	int ret = -1;
	uint8_t data[USARTX_UFRAME_SIZE] = {0};

	/* ɾ��һ֡�������� */
	ret = vkUsart_Buffer_Delete(&gUsart[com].tx_buffer, data, USARTX_UFRAME_SIZE);

	if(ret == 0)
	{
		/* �ɹ��ӷ��ͻ���ɾ��һ֡���ݣ��������� */
		ret = data[0];
		vkUsart_INT_Send(com, data+1, ret);

		gUsart[com].tx_frame = 1;
	}
	else
	{
		gUsart[com].tx_frame = 0;
	}

	return ret;	
}

/*******************************************************************************
 * ����: vkUsartSend
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: �ɹ������ַ�����С
 * ˵��: �� 
 ******************************************************************************/
int vkUsartSend(vkCOM com, uint8_t *buf, int size)
{
	/* ������� */
	if(com != COM1 && com != COM2 && com != COM3)
	{
		return -1;
	}

	if(buf == NULL || size <= 0)
	{	
		return -1;
	}

	int ret = -1;
	uint8_t data[USARTX_UFRAME_SIZE] = {0};

	memcpy(data+1, buf, (size<USARTX_UFRAME_SIZE-1)?size:USARTX_UFRAME_SIZE-1);
	data[0] = size;

	/* �������ݵ����ͻ��� */
	ret = vkUsart_Buffer_Insert(&gUsart[com].tx_buffer, data, size+1);
	
	if(ret == 0)
	{
		/* �������ݵ����ͻ���ɹ� */
		ret = size;
	}

	if(gUsart[com].tx_frame == 0)
	{
		
		/* �������ݵ����ͻ���󣬼���Ѿ�ֹͣ�˷������ݣ��ֶ�����һ֡���� */
		if(gUsart[com].dma_enable == 1)
		{
			vkUsart_DMA_Send_Line_Over_Handler(com);
		}
		else
		{
			vkUsart_INT_Send_Line_Over_Handler(com);
		}
	}

	return ret;
}

/*******************************************************************************
 * ����: vkUsartRecv
 * ����: �����ַ���
 * �β�: �ַ���ָ��buf,�ַ�����Сsize
 * ����: �ɹ������ַ�����С/�ɹ�����0
 * ˵��: �� 
 ******************************************************************************/
int vkUsartRecv(vkCOM com, uint8_t *buf, int size)
{
	/* ������� */
	if(com != COM1 && com != COM2 && com != COM3)
	{
		return -1;
	}

	if(buf == NULL || size <= 0)
	{	
		return -1;
	}

	int ret = -1;
	int full = 0;
	uint8_t data[USARTX_UFRAME_SIZE] = {0};

	/* ���ն����Ƿ��� */
	full = vkUsart_Buffer_Full(&gUsart[com].rx_buffer);

	/* ɾ���������ݣ��ڼ䲻�������IDLE�жϲ������� */
	ret = vkUsart_Buffer_Delete(&gUsart[com].rx_buffer, data, USARTX_UFRAME_SIZE);
			
	if(ret == 0)
	{
		/* ɾ���������ݳɹ����������� */
		size = data[0];
		memcpy(buf, data+1, size);

		ret = size;
	}

	if(full == 1)	
	{
		/* ɾ����������ǰ��������������˵�����������Ѿ�ֹͣ����Ҫ���¿��� */
		/* �������ݽ��� */
		if(gUsart[com].dma_enable == 1)
		{
			/* ׼����һ�ν������� */
			DMA_Cmd(gUsart[com].rx_channel, DISABLE); 
			DMA_SetCurrDataCounter(gUsart[com].rx_channel, gUsart[com].rx_size); 
    		DMA_Cmd(gUsart[com].rx_channel, ENABLE);

			/* ��IDLE�ж� */	
			USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, ENABLE);	
		}
		else
		{
			/* ׼����һ�ν������� */
			gUsart[com].rx_index = 0;	
			gUsart[com].rx_count = 0;
			USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, ENABLE);

			/* ��IDLE�ж� */		
			USART_ITConfig(gUsart[com].usart, USART_IT_IDLE, ENABLE);			
		}
	}

	return ret;
}

/* ���ݻ���������ݿ� */
int vkUsart_Buffer_Insert(vkUBuffer *ubuf, uint8_t *data, uint16_t len)
{
	if(ubuf->locked == 1)
	{	
		/* ����ռ����ڲ���������ʧ�� */
		return -1;
	}

	/* ��������ռ� */
	ubuf->locked = 1;

	/* ����ռ����� */
	if(ubuf->frame_stored == ubuf->frame_nums)
	{
		/* ��������ռ� */
		ubuf->locked = 0;
		
		return -1;
	}
	
	/* ������ */
	memset(ubuf->buffer + ubuf->index_insert*ubuf->frame_size, 0, ubuf->frame_size);
	
	/* �������� */
	memcpy(ubuf->buffer + ubuf->index_insert*ubuf->frame_size, data, (len<ubuf->frame_size)?len:ubuf->frame_size);

	/* ����������1 */
	ubuf->index_insert = (ubuf->index_insert+1)%(ubuf->frame_nums);

	/* �Ѵ������1 */
	ubuf->frame_stored = ubuf->frame_stored+1;

	/* ��������ռ� */
	ubuf->locked = 0;
	
	return 0;
}

/* ���ݻ���ȡ�����ݿ� */
int vkUsart_Buffer_Delete(vkUBuffer *ubuf, uint8_t *data, uint16_t len)
{
	if(ubuf->locked == 1)
	{	
		/* ����ռ����ڲ���������ʧ�� */
		return -1;
	}

	/* ��������ռ� */
	ubuf->locked = 1;

	/* ����ռ������� */
	if(ubuf->frame_stored == 0)
	{
		/* ��������ռ� */
		ubuf->locked = 0;
		
		return -1;
	}

	/* ȡ������ */
	memcpy(data, ubuf->buffer + ubuf->index_delete*ubuf->frame_size, (len<ubuf->frame_size)?len:ubuf->frame_size);

	/* ɾ��������1 */
	ubuf->index_delete = (ubuf->index_delete+1)%(ubuf->frame_nums);

	/* �Ѵ������1 */
	ubuf->frame_stored = ubuf->frame_stored-1;

	/* ��������ռ� */
	ubuf->locked = 0;
	
	return 0;
}

/* �Ƿ�� */
int vkUsart_Buffer_Null(vkUBuffer *ubuf)
{
	return (ubuf->frame_stored==0)?1:0;
}

/* �Ƿ��� */
int vkUsart_Buffer_Full(vkUBuffer *ubuf)
{
	return (ubuf->frame_stored==ubuf->frame_nums)?1:0;
}

