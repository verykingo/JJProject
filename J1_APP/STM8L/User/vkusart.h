
/******************************************************************************
 * �ļ�  ��vkuart.h
 * ����    ��STM8L����
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKUSART_H
#define __VKUSART_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "vkqueue.h"

/* �Զ����������� */
#define UART_SEND_RECV_BUFSIZE 64		//���ͽ��ջ����С,���ֵ255

typedef struct USART
{	
	USART_TypeDef* 	usart;			//����
	uint8_t			tx_buffer[UART_SEND_RECV_BUFSIZE];		//���ͻ���
	uint8_t			rx_buffer[UART_SEND_RECV_BUFSIZE];		//���ջ���
	uint8_t			tx_index;								//��������
	uint8_t			rx_index;								//��������
	uint8_t			tx_count;								//���ͼ���
	uint8_t			rx_count;								//���ռ���
	uint8_t			tx_doing;		//���ڷ���
	uint8_t			rx_idle;		//�յ�һ֡����

	uint8_t			dma_enable;			//dmaʹ�ܱ��
	DMA_Channel_TypeDef* tx_channel;	//dma����ͨ��
	DMA_Channel_TypeDef* rx_channel;	//dma����ͨ��
} vkUSART;

typedef enum COM
{
	COM1 = 0,	//����1
	COM2 = 1,	//����2
	COM3 = 2	//����3
} vkCOM;

#define UART1_DMA_ENABLE	0u
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

int vkUsart_Init(vkCOM com, uint32_t BaudRate);
int vkUsart_Recv(vkCOM com, uint8_t *buf, int size);
int vkUsart_Send(vkCOM com, uint8_t *buf, int size);

int vkUsart_Recv_Byte(vkCOM com);
int vkUsart_Send_Byte(vkCOM com);
int vkUsart_Recv_Line(vkCOM com);

int putchar(int c);
int getchar(void);

#endif


