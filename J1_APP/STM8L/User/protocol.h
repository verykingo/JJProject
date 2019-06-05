/*
 * 应用层协议，一般需要有应答
 */

#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/* 平台相关数据类型定义 */
#include "stm8l15x.h"
#include "softtimer.h"

/* 应用层协议命令 */
typedef enum
{
	APP_CMD_1	= ((uint16_t)0xC001),	//应用层协议命令1
	APP_CMD_2	= ((uint16_t)0xC002),	//应用层协议命令2
	APP_CMD_3	= ((uint16_t)0xC003),	//应用层协议命令3
	APP_CMD_4	= ((uint16_t)0xC004),	//应用层协议命令4
}APP_CMD_TypeDef;

/* 协议应答标记 */
typedef enum
{
	REP_FLAG_ZERO	= ((uint16_t)0x0000),	//协议应答无回复
	REP_FLAG_ACK	= ((uint16_t)0x0001),	//协议应答初确认
	REP_FLAG_SIZE	= ((uint16_t)0x0002),	//协议应答数据长度错误
	REP_FLAG_CMD	= ((uint16_t)0x0003),	//协议应答不能识别命令
	REP_FLAG_CRC	= ((uint16_t)0x0004),	//协议应答数据校验错误
	REP_FLAG_PRMA	= ((uint16_t)0x0005),	//协议应答参数错误
}REP_FLAG_TypeDef;

/* 协议应答处理 */
typedef enum
{
	REP_WYAS_OVER	= ((uint16_t)0x0000),		//协议通信结束
	REP_WYAS_NOW	= ((uint16_t)0x0001),		//协议通信立即重传
	REP_WYAS_INC    = ((uint16_t)0x0002),		//协议通信线性递增重传，延时rep_cout秒后重传
	REP_WYAS_EXP    = ((uint16_t)0x0003),		//协议通信指数递增重传，延时2^rep_cout秒后重传
	REP_WYAS_DELAY1	= ((uint16_t)0x0004),		//协议通信立延时1秒重传
	REP_WYAS_DELAY2	= ((uint16_t)0x0005),		//协议通信立延时2秒重传
	REP_WYAS_DELAY3	= ((uint16_t)0x0006),		//协议通信立延时3秒重传
	REP_WYAS_DELAY5	= ((uint16_t)0x0007),		//协议通信立延时5秒重传
	REP_WYAS_DELAY7	= ((uint16_t)0x0008),		//协议通信立延时7秒重传
	REP_WYAS_DELAY10	= ((uint16_t)0x0009),	//协议通信立延时10秒重传
	REP_WYAS_DELAY15	= ((uint16_t)0x000A),	//协议通信立延时15秒重传
	REP_WYAS_DELAY30	= ((uint16_t)0x000B),	//协议通信立延时30秒重传
	
}REP_WAYS_TypeDef;


typedef struct protocol
{
	uint16_t			id;				//命令识别号
	uint16_t 			cmd;			//协议命令代码
	uint16_t			rep_flag;		//应答标记
	uint16_t			rep_ways;		//应答机制
	uint16_t			rep_cout;		//重传计数，初值是0.每重传1次，增加1
	uint16_t			rep_nums;		//重传次数，设置最大重传次数，重传计数到达最大重传次数，自动放弃重传
	uint16_t			rep_time;		//应答时间，定时rep_time后，检测命令回复结果，单位ms
	TIMER_CB_FUNC   	rep_func;    	//应答函数，定时rep_time，对命令回复结果处理函数
	TIMER*				rep_timer_cb;	//应答定时回调
	TIMER*				cmd_timer_cb;	//命令重发定时回调

} PROTOCOL;

#endif

