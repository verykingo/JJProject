
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

/* �Զ����������� */

/* Callback function prototype */
typedef void ( * vkAT_FUNC ) ( void* at_data );

typedef struct at
{
	uint16_t			at_id;			//ATʶ���
	char * 				at_cmd;			//AT����
	vkAT_FUNC			at_fun;			//AT����

} vkAT;

void AT_Reset(void *data);
void AT_Test(void *data);


#endif

