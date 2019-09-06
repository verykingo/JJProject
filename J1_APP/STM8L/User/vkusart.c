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

/* 包含自定义头文件 */
#include "vkusart.h"
#include "vkQueue.h"

/* 自定义新类型 */

/* 自定义宏 */
#define UART_SEND_RECV_TIMEOUT 1000		//发送接收超时
#define UART_SEND_RECV_BUFSIZE 64		//发送接收缓存大小

#define gCOM COM1						//系统默认printf打印串口

/* 全局变量定义 */
static vkUSART gUsart[3] = {0x00};

/*******************************************************************************
 * 名称: vkUsart_Init
 * 功能: USART初始化操作
 * 形参: 串口USARTx, 波特率BaudRate
 * 返回: 成功0，失败-1
 * 说明: 无 
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

		/* USART1 Tx- Rx (PC3- PC2) remapping to (PA2- PA3)/(PC5- PC6) */
		//SYSCFG_REMAPDeInit();
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOA, GPIO_Pin_2, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOA, GPIO_Pin_3, GPIO_Mode_In_PU_No_IT);
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

		return -1;
	}
	else if(com == COM3)
	{
		gUsart[com].usart = USART3;

		/* USART3 Tx- Rx (PG1- PG0) remapping to (PF0- PF1) */
		//SYSCFG_REMAPDeInit();
		//SYSCFG_REMAPPinConfig(REMAP_Pin_USART3TxRxPortF, ENABLE);

		/* Enable USART clock */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);

		/* Configure USART Tx pin output*/
		GPIO_Init(GPIOG, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Fast);

		/* Configure USART Rx pin input*/
		GPIO_Init(GPIOG, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);	
		
		return -1;
	}

	/* RESET USART */
  	USART_DeInit(gUsart[com].usart);

  	/* USART configuration */
  	USART_Init(gUsart[com].usart, BaudRate, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx|USART_Mode_Rx);	
	
  	/* Enable the USART Receive interrupt: this interrupt is generated 
  	   when the USART receive data register is not empty */
  	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_RXNE);
 	USART_ITConfig(gUsart[com].usart, USART_IT_RXNE, ENABLE);

	/* Enable the USART Transmit complete interrupt: this interrupt is 
	   generated when the USART transmit Shift Register is empty */
	USART_ClearITPendingBit(gUsart[com].usart, USART_IT_TC);
  	USART_ITConfig(gUsart[com].usart, USART_IT_TC, ENABLE);	

	/* Enable USART */
	USART_Cmd(gUsart[com].usart, ENABLE);

	/* Malloc buffer memmory */
	if(NULL == (gUsart[com].tx_buffer = (uint8_t *)malloc(UART_SEND_RECV_BUFSIZE)))
	{
		return -1;
	}
	if(NULL == (gUsart[com].rx_buffer = (uint8_t *)malloc(UART_SEND_RECV_BUFSIZE)))
	{
		return -1;
	}
	
	/* Create Tx/Rx queue buffer */
	if(vkQUEUE_OK != vkQueueCreate(&gUsart[com].tx_queue,gUsart[com].tx_buffer,sizeof(char),UART_SEND_RECV_BUFSIZE))
	{
		return -1;
	}
	if(vkQUEUE_OK != vkQueueCreate(&gUsart[com].rx_queue,gUsart[com].rx_buffer,sizeof(char),UART_SEND_RECV_BUFSIZE))
	{
		return -1;
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

		/* Disable USART1 Tx- Rx remapping to PA2- PA3 */
		SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, DISABLE);
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
 * 接收中断处理函数
 */
int vkUsart_Recv_Byte(vkCOM com)
{
	uint8_t c = USART_ReceiveData8(gUsart[com].usart); 

	if(vkQUEUE_OK != vkQueuePut(&gUsart[com].rx_queue, 0, &c))
	{
		return -1;
	}
	
	return 0;
}

/*
 * 发送中断处理函数
 */
int vkUsart_Send_Byte(vkCOM com)
{
	uint8_t c;
	
	if(vkQUEUE_OK != vkQueueGet(&gUsart[com].tx_queue, 0, &c))
	{
		return -1;
	}

	/* Write a character to the USART */
  	USART_SendData8(gUsart[com].usart, (uint8_t)c);	
	
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
	int ret = 0;

	/* 发送数据放入缓存队列 */
	for(int i=0; i<size; i++)
	{
		if(vkQUEUE_OK != vkQueuePut(&gUsart[com].tx_queue, 0, (uint8_t *)buf+i))
		{
			break;
		}

		ret++;
	}

	/* 缓存队列有数据且发送数据寄存器是空 */
	if((vkQueueEmpty(&gUsart[com].tx_queue)!=vkQUEUE_NULL) && (USART_GetFlagStatus(gUsart[com].usart, USART_FLAG_TXE) != RESET))
	{
		vkUsart_Send_Byte(com);
	}	
	
	return ret;
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
	int ret = 0;

	/* 从缓存队列中读出数据 */
	for(int i=0; i<size; i++)
	{
		if(vkQUEUE_OK != vkQueueGet(&gUsart[com].rx_queue, 0, (uint8_t *)buf+i))
		{
			break;
		}

		ret++;		
	}
	
	return ret;
}

