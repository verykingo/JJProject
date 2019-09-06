
/******************************************************************************
 * 文件  ：vkuart.h
 * 描述    ：STM8L串口
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKUSART_H
#define __VKUSART_H

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "vkqueue.h"

/* 自定义数据类型 */
typedef struct USART
{	
	USART_TypeDef* 	usart;			//串口
	uint8_t*		tx_buffer;		//发送缓存
	uint8_t*		rx_buffer;		//接收缓存
	vkQUEUE 		tx_queue;		//发送队列
	vkQUEUE 		rx_queue;		//接收队列
} vkUSART;

typedef enum COM
{
	COM1 = 0,	//串口1
	COM2 = 1,	//串口2
	COM3 = 2	//串口3
} vkCOM;

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

int vkUsart_Init(vkCOM com, uint32_t BaudRate);
int vkUsart_Recv(vkCOM com, uint8_t *buf, int size);
int vkUsart_Send(vkCOM com, uint8_t *buf, int size);
int vkUsart_Recv_Byte(vkCOM com);
int vkUsart_Send_Byte(vkCOM com);

int putchar(int c);
int getchar(void);

#endif


