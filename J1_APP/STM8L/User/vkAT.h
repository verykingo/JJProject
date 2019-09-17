
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

/* �Զ����������� */

/* Callback function prototype */
typedef void ( * vkAT_FUNC ) ( void* at_data, int len );

typedef struct at
{
	uint16_t			at_id;			//ATʶ���
	char * 				at_cmd;			//AT����
	vkAT_FUNC			at_fun;			//AT����

} vkAT;


int vkATParser(const char * data, int data_len);
int vkATEcho(vkCOM com);

void AT_Reset(void *data, int len);
void AT_Test(void *data, int len);

#define AT_COMMANDLINE_SIZE_MAX	16

#endif

