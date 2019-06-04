/*
 * Ӧ�ò�Э�飬һ����Ҫ��Ӧ��
 */

#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/* ƽ̨����������Ͷ��� */
#include "stm8l15x.h"
#include "softtimer.h"

typedef enum
{
	APP_CMD_1	= ((uint16_t)0xC001),
	APP_CMD_2	= ((uint16_t)0xC002),
	APP_CMD_3	= ((uint16_t)0xC003),
	APP_CMD_4	= ((uint16_t)0xC004),
}APP_CMD_TypeDef;

typedef struct protocol
{
	uint16_t			id;				//����ʶ���
	APP_CMD_TypeDef 	cmd;			//Э���������
	uint8_t				ack_flag;		//Ӧ����
	uint16_t			ack_time;		//Ӧ��ʱ��
	TIMER				timer_cb;		//����Ӧ��ص�
	
} PROTOCOL;

#endif

