/*
 * Ӧ�ò�Э�飬һ����Ҫ��Ӧ��
 */
 
#include <string.h>
#include "vkprotocol.h"
#include "vktimetick.h"

void protocol_reply(void* ptr);
void protocol_repeat(void* ptr);
void protocol_command(void* ptr);


vkTIMER Rep_Timer[4];
vkTIMER Rtp_Timer[4];

static const vkPROTOCOL protocol[]= {
	/*id, 	cmd, 		rep_flag,			rep_ways		rep_cout	rep_nums	rep_time	rpt_flag	rep_timer 		rpt_timer */
	{0, 	APP_CMD_1, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		0,			&Rep_Timer[0],	&Rtp_Timer[0]},
	{1, 	APP_CMD_2, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		0,			&Rep_Timer[1],	&Rtp_Timer[1]},
	{2, 	APP_CMD_3, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		0,			&Rep_Timer[2],	&Rtp_Timer[2]},
	{3, 	APP_CMD_4, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		0,			&Rep_Timer[3],	&Rtp_Timer[3]}
};

void protocol_reply(void* ptr)
{
	vkPROTOCOL* Pptr = (vkPROTOCOL*)ptr;
	uint16_t rep_flag = Pptr->rep_flag;

	switch(rep_flag)
	{
		case REP_FLAG_ACK:	//Ӧ��ȷ��ͨѶ����
		case REP_FLAG_CMD:	//Ӧ����ʶ������
		{
			//һ��ͨ�Ž���
			//do nothing
			break;
		}
		case REP_FLAG_ZERO:	//Ӧ��ʱ��δ��Ӧ��ʱʱ�����յ��Է������ݺܿ��ܶ�ʧ
		case REP_FLAG_SIZE:	//Ӧ�����ݴ�С����
		case REP_FLAG_CRC:	//Ӧ������У�����
		case REP_FLAG_PRMA:	//Ӧ���������
		{
			//�����ش�����
			protocol_repeat(Pptr);
			break;
		}			
		default:
			break;
	}
	
	return;
};

void protocol_repeat(void* ptr)
{
	vkPROTOCOL* Pptr = (vkPROTOCOL*)ptr;
	
	uint16_t rep_ways 	= Pptr->rep_ways;
	uint16_t rep_cout 	= Pptr->rep_cout;
	uint16_t rep_nums 	= Pptr->rep_nums;	
	uint16_t rpt_flag	= Pptr->rpt_flag;
	vkTIMER*	rpt_timer   = Pptr->rpt_timer;

	switch(rep_ways)
	{
		case REP_WYAS_OVER:	//ͨ�Ž����������ش�
		{
			//Do nothing
			break;
		}
		case REP_WYAS_NOW:	//ͨ�ż����������ش�
		{
			/* �ش���������������ش����� */
			if(++rep_cout <= rep_nums)
			{
				//����ִ���ش�
				protocol_command(Pptr);
			}
			break;
		}
		case REP_WYAS_INC:	//ͨ�ż�������ʱrep_cout*1���ش�
		{
			if(rpt_flag == 0)
			{				
				/* �ش���������������ش����� */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//��ʱ(rep_cout*1)��ִ���ش�
					rpt_timer->timer_name 	= (void *)rpt_timer;
					rpt_timer->cb_func	 	= protocol_repeat;
					rpt_timer->cb_data		= Pptr;
					rpt_timer->cb_ticks		= vkSS_TO_TICKS(1)*rep_cout;
					vkTimerInsert(rpt_timer);
				}
			}
			else if(rpt_flag == 1)
			{
				Pptr->rpt_flag = 0;
				//ִ���ش�
				protocol_command(Pptr);	
			}
			
			break;
		}
		case REP_WYAS_EXP:	//ͨ�ż�������ʱ2^rep_cout*1���ش�
		{
			if(rpt_flag == 0)
			{				
				/* �ش���������������ش����� */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//��ʱ(rep_cout*1)��ִ���ش�
					rpt_timer->timer_name 	= (void *)rpt_timer;
					rpt_timer->cb_func	 	= protocol_repeat;
					rpt_timer->cb_data		= Pptr;
					rpt_timer->cb_ticks		= vkSS_TO_TICKS(1)*(2^rep_cout);
					vkTimerInsert(rpt_timer);
				}
			}
			else if(rpt_flag == 1)
			{
				Pptr->rpt_flag = 0;
				//ִ���ش�
				protocol_command(Pptr);	
			}

			break;
		}
		case REP_WYAS_DELAY1:	//ͨ�ż�������ʱ1���ش�
		{
			if(rpt_flag == 0)
			{				
				/* �ش���������������ش����� */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//��ʱ(rep_cout*1)��ִ���ش�
					rpt_timer->timer_name 	= (void *)rpt_timer;
					rpt_timer->cb_func	 	= protocol_repeat;
					rpt_timer->cb_data		= Pptr;
					rpt_timer->cb_ticks		= vkSS_TO_TICKS(1);
					vkTimerInsert(rpt_timer);
				}
			}
			else if(rpt_flag == 1)
			{
				Pptr->rpt_flag = 0;
				//ִ���ش�
				protocol_command(Pptr);	
			}

			break;
		}
		default:
			break;
	}
			
}

void protocol_command(void* ptr)
{
	vkPROTOCOL* Pptr = (vkPROTOCOL*)ptr;
	
	uint16_t cmd 		= Pptr->cmd;
	uint16_t rep_time 	= Pptr->rep_time;
	vkTIMER*	rep_timer  	= Pptr->rep_timer;
	
	switch(cmd)
	{
		case APP_CMD_1:	//����1
		{
			/* �������� */
			//send_data(APP_CMD_1);			
			break;
		}
		case APP_CMD_2:	//����2
		{
			/* �������� */
			//send_data(APP_CMD_2);
			break;
		}
		case APP_CMD_3:	//����3
		{
			/* �������� */
			//send_data(APP_CMD_3);			
			break;
		}
		case APP_CMD_4:	//����4
		{
			/* �������� */
			//send_data(APP_CMD_4);
			break;
		}
		default:
			break;
	}

	/* ����Ӧ��ʱ�� */
	rep_timer->timer_name	= (void*)rep_timer;
	rep_timer->cb_func		= protocol_reply;
	rep_timer->cb_data		= Pptr;
	rep_timer->cb_ticks 	= vkMS_TO_TICKS(rep_time);
	vkTimerInsert(rep_timer);
	
}
