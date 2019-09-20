
/******************************************************************************
 * 文件  ：vkuart.h
 * 描述    ：STM8L串口
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKUSART_H
#define __VKUSART_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */

typedef struct USART
{	
	USART_TypeDef* 	usart;			//串口设备
	uint8_t*		tx_buffer;		//发送缓存
	uint8_t*		rx_buffer;		//接收缓存
	uint8_t			tx_bufsize;		//缓存大小
	uint8_t			rx_bufsize;		//缓存大小
	uint8_t			tx_index;		//发送索引
	uint8_t			rx_index;		//接收索引
	uint8_t			tx_count;		//发送计数
	uint8_t			rx_count;		//接收计数
	uint8_t			tx_doing;		//正在发送
	uint8_t			rx_idle;		//收到一帧数据

	uint8_t			dma_enable;			//dma使能标记
	DMA_Channel_TypeDef* tx_channel;	//dma发送通道
	DMA_Channel_TypeDef* rx_channel;	//dma接收通道
} vkUSART;

typedef enum com
{
	COM1 = 0,	//串口1
	COM2 = 1,	//串口2
	COM3 = 2	//串口3
} vkCOM;

#define UART1_DMA_ENABLE	1u
#define UART2_DMA_ENABLE	0u
#define UART3_DMA_ENABLE	0u

#ifdef _RAISONANCE_  
#define PUTCHAR_PROTOTYPE int putchar (char c)  
#define GETCHAR_PROTOTYPE int getchar (void)  
#elif defined (_COSMIC_)  
#define PUTCHAR_PROTOTYPE char putchar (char c)  
#define GETCHAR_PROTOTYPE char getchar (void)  
#else /* _IAR_ */  
#define PUTCHAR_PROTOTYPE int putchar (int c)  
#define GETCHAR_PROTOTYPE int getchar (void)  
#endif /* _RAISONANCE_ */  

int vkUsart_Init(vkCOM com, uint32_t baudrate, uint8_t* tx_buffer, uint8_t tx_bufsize, uint8_t* rx_buffer, uint8_t rx_bufsize);
int vkUsart_Recv(vkCOM com, uint8_t *buf, int size);
int vkUsart_Send(vkCOM com, uint8_t *buf, int size);

int vkUsart_Send_Over(vkCOM com);
int vkUsart_Recv_Byte(vkCOM com);
int vkUsart_Send_Byte(vkCOM com);
int vkUsart_Recv_Line(vkCOM com);

int putchar(int c);
int getchar(void);

#endif


