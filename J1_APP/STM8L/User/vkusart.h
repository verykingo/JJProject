
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

#define USART1_BUFFER_SIZE 		512			//数据缓存空间大小
#define USART1_BUFFER_TX_SIZE	512
#define USART1_BUFFER_RX_SIZE	512

#define USART2_BUFFER_SIZE 		1			//数据缓存空间大小
#define USART3_BUFFER_SIZE 		32			//数据缓存空间大小

/* 串口一帧数据最大长度，决定一次接收数据的最大长度和能够缓存数据帧的数目 */
#define USARTX_UFRAME_SIZE 		64

/* 自定义数据类型 */

/* 串口数据缓存结构体 */
typedef struct UBUFFER
{
	uint8_t *buffer;			//缓存首址
	uint8_t locked;				//缓存锁定
	uint8_t frame_size;			//缓存帧大小
	uint8_t frame_nums;			//缓存帧数目
	uint8_t frame_stored;		//已用块数
	uint8_t index_insert;		//插入块位置
	uint8_t index_delete;		//删除块位置
} vkUBuffer;

typedef struct USART
{	
	USART_TypeDef* 	usart;			//串口设备
	uint8_t*		tx_data;		//发送缓存
	uint8_t*		rx_data;		//接收缓存
	uint8_t			tx_size;		//缓存大小
	uint8_t			rx_size;		//缓存大小
	uint8_t			tx_index;		//发送索引，中断发送使用
	uint8_t			rx_index;		//接收索引，中断接收使用
	uint8_t			tx_count;		//发送计数
	uint8_t			rx_count;		//接收计数
	uint8_t			tx_doing;		//正在发送
	uint8_t			rx_idle;		//收到一帧数据
	uint8_t			dma_enable;			//dma使能标记
	DMA_Channel_TypeDef* tx_channel;	//dma发送通道
	DMA_Channel_TypeDef* rx_channel;	//dma接收通道

	uint8_t			tx_frame;			//正在发送一帧数据
	vkUBuffer		tx_buffer;
	vkUBuffer		rx_buffer;	
} vkUSART;

typedef enum com
{
	COM1 = 0,	//串口1
	COM2 = 1,	//串口2
	COM3 = 2,	//串口3
	COMMAX = 3
} vkCOM;

/* 串口是否使能DMA数据传输方式
 * UART1和UART3同时只能一个使能DMA方式
 * 如果串口是标准print打印串口设置将失效
 */
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

/* 外部函数 */
int vkUsart_Init(vkCOM com, uint32_t baudrate, uint8_t* tx_fbuf, uint8_t tx_fsize, uint8_t* rx_fbuf, uint8_t rx_fsize);
int vkUsart_Deinit(vkCOM com);

int putchar(int c);
int getchar(void);

int vkUsartRecv(vkCOM com, uint8_t *buf, int size);
int vkUsartSend(vkCOM com, uint8_t *buf, int size);

/* 内部函数 */
int vkUsart_DMA_Send_Line_Over_Handler(vkCOM com);
int vkUsart_INT_Send_Line_Over_Handler(vkCOM com);

int vkUsart_INT_Recv_Byte_Over_Handler(vkCOM com);
int vkUsart_INT_Send_Byte_Over_Handler(vkCOM com);

int vkUsart_Recv_Line_Handler(vkCOM com);

int vkUsart_DMA_Recv(vkCOM com, uint8_t *buf, int size);
int vkUsart_DMA_Send(vkCOM com, uint8_t *buf, int size);

int vkUsart_INT_Recv(vkCOM com, uint8_t *buf, int size);
int vkUsart_INT_Send(vkCOM com, uint8_t *buf, int size);

int vkUsart_Buffer_Insert(vkUBuffer *ubuf, uint8_t *data, uint16_t len);
int vkUsart_Buffer_Delete(vkUBuffer *ubuf, uint8_t *data, uint16_t len);
int vkUsart_Buffer_Full(vkUBuffer *ubuf);
int vkUsart_Buffer_Null(vkUBuffer *ubuf);

#endif


