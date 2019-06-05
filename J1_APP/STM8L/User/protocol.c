/*
 * Ӧ�ò�Э�飬һ����Ҫ��Ӧ��
 */
 
#include <string.h>
#include "protocol.h"
#include "timetick.h"

void protocol_reply(void* ptr);

TIMER rep_timer[4];
TIMER cmd_timer[4];

PROTOCOL protocol[]= {
	/*id, 	cmd, 		rep_flag,			rep_ways		rep_cout	rep_nums	rep_time	rep_func 			rep_timer_cb 		cmd_timer_cb */
	{0, 	APP_CMD_1, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		protocol_reply, 	&rep_timer[0],		&cmd_timer[0]},
	{1, 	APP_CMD_2, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		protocol_reply, 	&rep_timer[1],		&cmd_timer[1]},
	{2, 	APP_CMD_3, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		protocol_reply,		&rep_timer[2],		&cmd_timer[2]},
	{3, 	APP_CMD_4, 	REP_FLAG_ZERO, 		REP_WYAS_NOW, 	0, 			3, 			1000, 		protocol_reply, 	&rep_timer[3],		&cmd_timer[3]}
};

void protocol_reply(void* ptr)
{
	PROTOCOL* Pptr = (PROTOCOL*)ptr;
	
	uint16_t id 		= Pptr->id;
	uint16_t cmd       	= Pptr->cmd;
	uint16_t rep_flag 	= Pptr->rep_flag;
	uint16_t rep_ways 	= Pptr->rep_ways;
	uint16_t rep_cout 	= Pptr->rep_cout;
	uint16_t rep_nums 	= Pptr->rep_nums;
	uint16_t rep_time 	= Pptr->rep_time;
	TIMER_CB_FUNC rep_func = Pptr->rep_func; 
	TIMER*	rep_timer_cb    = Pptr->rep_timer_cb;
	
	switch(rep_flag)
	{
		case REP_FLAG_ZERO:	//Ӧ��ʱ��δ�ڹ涨ʱ�����յ��Է������ݺܿ��ܶ�ʧ
		{
			//�����ش�����
			#if 0
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
					}
					break;
				}
				case REP_WYAS_INC:	//ͨ�ż�������ʱrep_cout*1���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(rep_cout*1)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1)*rep_cout;
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_EXP:	//ͨ�ż�������ʱ2^rep_cout*1���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(2^rep_cout*1)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1)*(2^rep_cout);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY1:	//ͨ�ż�������ʱ1���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(1)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY2:	//ͨ�ż�������ʱ2���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(2)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(2);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY3:	//ͨ�ż�������ʱ3���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(3)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(3);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY5:	//ͨ�ż�������ʱ5���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(5)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(5);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY7:	//ͨ�ż�������ʱ7���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(7)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(7);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY10:	//ͨ�ż�������ʱ10���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(10)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(10);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY15:	//ͨ�ż�������ʱ15���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(15)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(15);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY30:	//ͨ�ż�������ʱ30���ش�
				{
					if(rep_cout != 0)
					{
						//ִ���ش�
						//send(cmd);
					}
					
					/* �ش���������������ش����� */
					if(++rep_cout <= rep_nums)
					{
						//��ʱ(30)��ִ���ش�
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(30);
						TimerInsert(timer_cb);
					}					
					break;
				}

				default:
					break;
			}
			break;

			#endif
		}
		case REP_FLAG_ACK:	//Ӧ��ȷ��
		{
			//һ��ͨ�Ž���
			break;
		}
		case REP_FLAG_SIZE:	//Ӧ�����ݴ�С����
		{
			//�����ش�����
			break;
		}
		case REP_FLAG_CMD:	//Ӧ����ʶ������
		{
			//һ��ͨ�Ž���
			break;
		}
		case REP_FLAG_CRC:	//Ӧ������У�����
		{
			//�����ش�����
			break;
		}
		case REP_FLAG_PRMA:	//Ӧ���������
		{
			//�����ش�����
			break;
		}			
		default:
			break;
	}
	
	return;
};

void protocol_repeat(void* ptr)
{
	
}

