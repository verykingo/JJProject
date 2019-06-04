/*
 * 应用层协议，一般需要有应答
 */

#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

/* 平台相关数据类型定义 */
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
	uint16_t			id;				//命令识别号
	APP_CMD_TypeDef 	cmd;			//协议命令代码
	uint8_t				ack_flag;		//应答标记
	uint16_t			ack_time;		//应答时间
	TIMER				timer_cb;		//命令应答回调
	
} PROTOCOL;

#endif

