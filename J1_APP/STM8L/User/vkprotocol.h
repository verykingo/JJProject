/*
 * Ӧ�ò�Э�飬һ����Ҫ��Ӧ��
 */

#ifndef __VKPROTOCOL_H_
#define __VKPROTOCOL_H_

/* ƽ̨����������Ͷ��� */
#include "stm8l15x.h"
#include "vksofttimer.h"

/* Ӧ�ò�Э������ */
typedef enum
{
	APP_CMD_1	= ((uint16_t)0xC001),	//Ӧ�ò�Э������1
	APP_CMD_2	= ((uint16_t)0xC002),	//Ӧ�ò�Э������2
	APP_CMD_3	= ((uint16_t)0xC003),	//Ӧ�ò�Э������3
	APP_CMD_4	= ((uint16_t)0xC004),	//Ӧ�ò�Э������4
}APP_CMD_TypeDef;

/* Э��Ӧ���� */
typedef enum
{
	REP_FLAG_ZERO	= ((uint16_t)0x0000),	//Э��Ӧ���޻ظ�
	REP_FLAG_ACK	= ((uint16_t)0x0001),	//Э��Ӧ���ȷ��
	REP_FLAG_SIZE	= ((uint16_t)0x0002),	//Э��Ӧ�����ݳ��ȴ���
	REP_FLAG_CMD	= ((uint16_t)0x0003),	//Э��Ӧ����ʶ������
	REP_FLAG_CRC	= ((uint16_t)0x0004),	//Э��Ӧ������У�����
	REP_FLAG_PRMA	= ((uint16_t)0x0005),	//Э��Ӧ���������
}REP_FLAG_TypeDef;

/* Э��Ӧ���� */
typedef enum
{
	REP_WYAS_OVER	= ((uint16_t)0x0000),		//Э��ͨ�Ž���
	REP_WYAS_NOW	= ((uint16_t)0x0001),		//Э��ͨ�������ش�
	REP_WYAS_INC    = ((uint16_t)0x0002),		//Э��ͨ�����Ե����ش�����ʱrep_cout����ش�
	REP_WYAS_EXP    = ((uint16_t)0x0003),		//Э��ͨ��ָ�������ش�����ʱ2^rep_cout����ش�
	REP_WYAS_DELAY1	= ((uint16_t)0x0004),		//Э��ͨ������ʱ1���ش�
}REP_WAYS_TypeDef;


typedef struct protocol
{
	uint16_t			id;				//����ʶ���
	uint16_t 			cmd;			//Э���������
	uint16_t			rep_flag;		//Ӧ����
	uint16_t			rep_ways;		//Ӧ�����
	uint16_t			rep_cout;		//�ش���������ֵ��0.ÿ�ش�1�Σ�����1
	uint16_t			rep_nums;		//�ش���������������ش��������ش�������������ش��������Զ������ش�
	uint16_t			rep_time;		//Ӧ��ʱ�䣬��ʱrep_time�󣬼������ظ��������λms
	uint16_t			rpt_flag;		//�����ط�ʹ�ܱ��
	vkTIMER*			rep_timer;		//Ӧ��ʱ�ص�
	vkTIMER*			rpt_timer;		//�����ط���ʱ�ص�

} PROTOCOL;


void protocol_command(void* ptr);

#endif
