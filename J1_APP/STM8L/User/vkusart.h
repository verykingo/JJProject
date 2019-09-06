
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
typedef struct USART
{	
	USART_TypeDef* 	usart;			//����
	uint8_t*		tx_buffer;		//���ͻ���
	uint8_t*		rx_buffer;		//���ջ���
	vkQUEUE 		tx_queue;		//���Ͷ���
	vkQUEUE 		rx_queue;		//���ն���
} vkUSART;

typedef enum COM
{
	COM1 = 0,	//����1
	COM2 = 1,	//����2
	COM3 = 2	//����3
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


