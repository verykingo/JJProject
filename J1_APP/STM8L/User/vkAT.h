
/******************************************************************************
 * 文件  ：vkAT.h
 * 描述  ：AT命令
 * 平台  ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKAT_H
#define __VKAT_H

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "vkusart.h"
#include "vksofttimer.h"
#include "vktimetick.h"

#define AT_COMMANDLINE_SIZE_MAX		16	//最大值255
#define TERMINAL_BUFFER_SIZE		16	//最大值255
#define COMMUNICATION_BUFFER_SIZE	64	//最大值255


/* 自定义数据类型 */

/* Callback function prototype */
typedef void ( * vkAT_FUNC ) ( void* at_data, int len);

/*
 * AT命令结构体
 */
typedef struct at
{
	uint16_t			at_id;			//AT识别号
	char * 				at_cmd;			//AT命令
	vkAT_FUNC			at_fun;			//AT函数

} vkAT;

/*
 * 窗口终端数据结构体
 */
typedef struct terminal
{
	uint8_t tern_enable;
	vkCOM 	term_com;							//终端串口
	uint8_t term_count;							//终端数据计数
	uint8_t term_cursor;						//终端数据游标
	uint8_t term_buffer[TERMINAL_BUFFER_SIZE];	//终端数据缓存
	vkTIMER term_timer;							//终端定时器
} vkTERMINAL;

typedef struct communication
{
	uint8_t comm_enable;
	vkCOM 	comm_com;								//终端串口
	uint8_t comm_buffer[COMMUNICATION_BUFFER_SIZE];	//终端数据缓存
	vkTIMER comm_timer;								//终端定时器
} vkCOMMUNICATION;

int vkAT_TerminalStart(vkCOM com);
int vkAT_TerminalStop();
int vkAT_TerminalPrint(uint8_t *buf, int size);

int vkAT_CommunicationStart(vkCOM com);
int vkAT_CommunicationStop();

int vkAT_Parser(const char * data, int data_len);
void AT_Reset(void *data, int len);
void AT_Test(void *data, int len);

#endif

