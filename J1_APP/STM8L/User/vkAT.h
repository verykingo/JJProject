
/******************************************************************************
 * �ļ�  ��vkAT.h
 * ����  ��AT����
 * ƽ̨  ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#ifndef __VKAT_H
#define __VKAT_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "vkusart.h"
#include "vksofttimer.h"
#include "vktimetick.h"

#define AT_COMMANDLINE_SIZE_MAX		16	//���ֵ255
#define TERMINAL_BUFFER_SIZE		16	//���ֵ255
#define COMMUNICATION_BUFFER_SIZE	64	//���ֵ255


/* �Զ����������� */

/* Callback function prototype */
typedef void ( * vkAT_FUNC ) ( void* at_data, int len);

/*
 * AT����ṹ��
 */
typedef struct at
{
	uint16_t			at_id;			//ATʶ���
	char * 				at_cmd;			//AT����
	vkAT_FUNC			at_fun;			//AT����

} vkAT;

/*
 * �����ն����ݽṹ��
 */
typedef struct terminal
{
	uint8_t tern_enable;
	vkCOM 	term_com;							//�ն˴���
	uint8_t term_count;							//�ն����ݼ���
	uint8_t term_cursor;						//�ն������α�
	uint8_t term_buffer[TERMINAL_BUFFER_SIZE];	//�ն����ݻ���
	vkTIMER term_timer;							//�ն˶�ʱ��
} vkTERMINAL;

typedef struct communication
{
	uint8_t comm_enable;
	vkCOM 	comm_com;								//�ն˴���
	uint8_t comm_buffer[COMMUNICATION_BUFFER_SIZE];	//�ն����ݻ���
	vkTIMER comm_timer;								//�ն˶�ʱ��
} vkCOMMUNICATION;

int vkAT_TerminalStart(vkCOM com);
int vkAT_TerminalStop();
int vkAT_TerminalPrint(uint8_t *buf, int size);

int vkAT_CommunicationStart(vkCOM com);
int vkAT_CommunicationStop();

int vkAT_Parser(const char * data, int data_len);
void AT_Reset(void *data, int len);
void AT_Test(void *data, int len);

#endif

