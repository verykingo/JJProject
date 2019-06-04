/*
 * 应用层协议，一般需要有应答
 */
 
#include <string.h>
#include "protocol.h"

void protocol_reply(void* ptr)

TIMER cmd_timer[4];

PROTOCOL protocol[]= {
	/*id, 	cmd, 		rep_flag,			rep_ways			rep_cout		rep_nums		rep_time	rep_func 			timer_cb */
	{0, 	APP_CMD_1, 	REP_FLAG_ZERO, 	REP_WYAS_NOW, 	0, 			3, 			1000, 	protocol_reply, &cmd_timer[0]},
	{1, 	APP_CMD_2, 	REP_FLAG_ZERO, 	REP_WYAS_NOW, 	0, 			3, 			1000, 	protocol_reply, &cmd_timer[1]},
	{2, 	APP_CMD_3, 	REP_FLAG_ZERO, 	REP_WYAS_NOW, 	0, 			3, 			1000, 	protocol_reply,	&cmd_timer[2]},
	{3, 	APP_CMD_4, 	REP_FLAG_ZERO, 	REP_WYAS_NOW, 	0, 			3, 			1000, 	protocol_reply, &cmd_timer[3]}
};

void protocol_reply(void* ptr)
{
	uint16_t* parm = (uint16_t*)ptr;
	uint16_t id = parm[0];
	uint16_t rep_flag = parm[2];
	uint16_t rep_ways = parm[3];
	uint16_t rep_cout = parm[4];
	uint16_t rep_cout = parm[5];
	
	return;
};


