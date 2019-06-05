/*
 * 应用层协议，一般需要有应答
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
		case REP_FLAG_ZERO:	//应答超时，未在规定时间内收到对方，数据很可能丢失
		{
			//调用重传处理
			#if 0
			switch(rep_ways)
			{
				case REP_WYAS_OVER:	//通信结束，放弃重传
				{
					//Do nothing
					break;
				}
				case REP_WYAS_NOW:	//通信继续，立即重传
				{
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//立即执行重传
					}
					break;
				}
				case REP_WYAS_INC:	//通信继续，延时rep_cout*1秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(rep_cout*1)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1)*rep_cout;
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_EXP:	//通信继续，延时2^rep_cout*1秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(2^rep_cout*1)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1)*(2^rep_cout);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY1:	//通信继续，延时1秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(1)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(1);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY2:	//通信继续，延时2秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(2)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(2);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY3:	//通信继续，延时3秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(3)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(3);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY5:	//通信继续，延时5秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(5)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(5);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY7:	//通信继续，延时7秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(7)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(7);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY10:	//通信继续，延时10秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(10)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(10);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY15:	//通信继续，延时15秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(15)秒执行重传
						timer_cb->timer_name 	= (void *)timer_cb;
						timer_cb->cb_func	 	= rep_func;
						timer_cb->cb_data		= &protocol[id];
						timer_cb->cb_ticks		= SS_TO_TICKS(15);
						TimerInsert(timer_cb);
					}					
					break;
				}
				case REP_WYAS_DELAY30:	//通信继续，延时30秒重传
				{
					if(rep_cout != 0)
					{
						//执行重传
						//send(cmd);
					}
					
					/* 重传计数不大于最大重传次数 */
					if(++rep_cout <= rep_nums)
					{
						//定时(30)秒执行重传
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
		case REP_FLAG_ACK:	//应答确认
		{
			//一次通信结束
			break;
		}
		case REP_FLAG_SIZE:	//应答数据大小出错
		{
			//调用重传处理
			break;
		}
		case REP_FLAG_CMD:	//应答不能识别命令
		{
			//一次通信结束
			break;
		}
		case REP_FLAG_CRC:	//应答数据校验错误
		{
			//调用重传处理
			break;
		}
		case REP_FLAG_PRMA:	//应答参数错误
		{
			//调用重传处理
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

