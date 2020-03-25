
/******************************************************************************
 * �ļ�  ��vkuart.h
 * ����    ��STM8L����
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKUSART_H
#define __VKUSART_H

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */

#define USART1_BUFFER_SIZE 		512			//���ݻ���ռ��С
#define USART1_BUFFER_TX_SIZE	512
#define USART1_BUFFER_RX_SIZE	512

#define USART2_BUFFER_SIZE 		1			//���ݻ���ռ��С
#define USART3_BUFFER_SIZE 		32			//���ݻ���ռ��С

/* ����һ֡������󳤶ȣ�����һ�ν������ݵ���󳤶Ⱥ��ܹ���������֡����Ŀ */
#define USARTX_UFRAME_SIZE 		64

/* �Զ����������� */

/* �������ݻ���ṹ�� */
typedef struct UBUFFER
{
	uint8_t *buffer;			//������ַ
	uint8_t locked;				//��������
	uint8_t frame_size;			//����֡��С
	uint8_t frame_nums;			//����֡��Ŀ
	uint8_t frame_stored;		//���ÿ���
	uint8_t index_insert;		//�����λ��
	uint8_t index_delete;		//ɾ����λ��
} vkUBuffer;

typedef struct USART
{	
	USART_TypeDef* 	usart;			//�����豸
	uint8_t*		tx_data;		//���ͻ���
	uint8_t*		rx_data;		//���ջ���
	uint8_t			tx_size;		//�����С
	uint8_t			rx_size;		//�����С
	uint8_t			tx_index;		//�����������жϷ���ʹ��
	uint8_t			rx_index;		//�����������жϽ���ʹ��
	uint8_t			tx_count;		//���ͼ���
	uint8_t			rx_count;		//���ռ���
	uint8_t			tx_doing;		//���ڷ���
	uint8_t			rx_idle;		//�յ�һ֡����
	uint8_t			dma_enable;			//dmaʹ�ܱ��
	DMA_Channel_TypeDef* tx_channel;	//dma����ͨ��
	DMA_Channel_TypeDef* rx_channel;	//dma����ͨ��

	uint8_t			tx_frame;			//���ڷ���һ֡����
	vkUBuffer		tx_buffer;
	vkUBuffer		rx_buffer;	
} vkUSART;

typedef enum com
{
	COM1 = 0,	//����1
	COM2 = 1,	//����2
	COM3 = 2,	//����3
	COMMAX = 3
} vkCOM;

/* �����Ƿ�ʹ��DMA���ݴ��䷽ʽ
 * UART1��UART3ͬʱֻ��һ��ʹ��DMA��ʽ
 * ��������Ǳ�׼print��ӡ�������ý�ʧЧ
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

/* �ⲿ���� */
int vkUsart_Init(vkCOM com, uint32_t baudrate, uint8_t* tx_fbuf, uint8_t tx_fsize, uint8_t* rx_fbuf, uint8_t rx_fsize);
int vkUsart_Deinit(vkCOM com);

int putchar(int c);
int getchar(void);

int vkUsartRecv(vkCOM com, uint8_t *buf, int size);
int vkUsartSend(vkCOM com, uint8_t *buf, int size);

/* �ڲ����� */
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


