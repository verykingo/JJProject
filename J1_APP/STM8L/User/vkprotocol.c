/*
 * 应用层协议，一般需要有应答
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
		case REP_FLAG_ACK:	//应答确认通讯无误
		case REP_FLAG_CMD:	//应答不能识别命令
		{
			//一次通信结束
			//do nothing
			break;
		}
		case REP_FLAG_ZERO:	//应答超时，未在应答超时时间内收到对方，数据很可能丢失
		case REP_FLAG_SIZE:	//应答数据大小出错
		case REP_FLAG_CRC:	//应答数据校验错误
		case REP_FLAG_PRMA:	//应答参数错误
		{
			//调用重传处理
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
				protocol_command(Pptr);
			}
			break;
		}
		case REP_WYAS_INC:	//通信继续，延时rep_cout*1秒重传
		{
			if(rpt_flag == 0)
			{				
				/* 重传计数不大于最大重传次数 */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//定时(rep_cout*1)秒执行重传
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
				//执行重传
				protocol_command(Pptr);	
			}
			
			break;
		}
		case REP_WYAS_EXP:	//通信继续，延时2^rep_cout*1秒重传
		{
			if(rpt_flag == 0)
			{				
				/* 重传计数不大于最大重传次数 */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//定时(rep_cout*1)秒执行重传
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
				//执行重传
				protocol_command(Pptr);	
			}

			break;
		}
		case REP_WYAS_DELAY1:	//通信继续，延时1秒重传
		{
			if(rpt_flag == 0)
			{				
				/* 重传计数不大于最大重传次数 */
				if(++rep_cout <= rep_nums)
				{
					Pptr->rpt_flag = 1;
					
					//定时(rep_cout*1)秒执行重传
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
				//执行重传
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
		case APP_CMD_1:	//命令1
		{
			/* 发送数据 */
			//send_data(APP_CMD_1);			
			break;
		}
		case APP_CMD_2:	//命令2
		{
			/* 发送数据 */
			//send_data(APP_CMD_2);
			break;
		}
		case APP_CMD_3:	//命令3
		{
			/* 发送数据 */
			//send_data(APP_CMD_3);			
			break;
		}
		case APP_CMD_4:	//命令4
		{
			/* 发送数据 */
			//send_data(APP_CMD_4);
			break;
		}
		default:
			break;
	}

	/* 启动应答定时器 */
	rep_timer->timer_name	= (void*)rep_timer;
	rep_timer->cb_func		= protocol_reply;
	rep_timer->cb_data		= Pptr;
	rep_timer->cb_ticks 	= vkMS_TO_TICKS(rep_time);
	vkTimerInsert(rep_timer);
	
}
