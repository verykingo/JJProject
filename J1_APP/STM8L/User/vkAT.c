/******************************************************************************
 * 文件  ：vkAT.c
 * 描述    ：STM8L节拍定时器
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* 包含自定义头文件 */
#include "vkAT.h"
#include "vkusart.h"

/* 自定义新类型 */

/* 自定义宏 */
#define isspace(c)	((c) == ' ' || ((c) >= '\t' && (c) <= '\r')) //空格
#define isascii(c)	(((c) & ~0x7F) == 0) //ASCII字符
#define isupper(c)	((c) >= 'A' && (c) <= 'Z') //大写
#define islower(c)	((c) >= 'a' && (c) <= 'z') //小写
#define isalpha(c)	(isupper(c) || islower(c)) //字母
#define isdigit(c)	((c) >= '0' && (c) <= '9') //数字
#define isxdigit(c)	(isdigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#define isprint(c)	((c) >= ' ' && (c) <= '~') //打印
#define toupper(c)	((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z'))) //变大写
#define tolower(c)	((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z'))) //变小写
#define toascii(c)	((unsigned)(c) & 0x7F) //变ASSIC码
#define isalnum(c)	(isalpha(c) || isdigit(c)) //字母数字
#define isodigit(c)	((c) >= '0' && (c) <= '7') //八进制
#define isdispch(c) ((c) >= 0x20 && (c) <= 0x7E) //可显示字符

/* 全局变量定义 */
static const vkAT AT[]= {
  /* id, 	AT Commond, 			AT Function */
	{0, 	"AT+TEST", 				AT_Test},
	{1, 	"AT+RESET", 			AT_Reset}
};

static vkTERMINAL ATTerminal = {0};
vkCOMMUNICATION   ATCommunication = {0};
	
static int parse_atcmd_line(const char * data, int start, int end);
static int get_plus_index(const char * string, int start, int end);
static int get_space_index(const char * string, int start, int end);
static int trim_spaces(const char * string, int * start, int * end);

/*******************************************************************************
 * 名称: AT命令解析函数
 * 功能: AT命令
 * 形参: data字符串指针，data_len字符串长度
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkAT_Parser(const char *data, int data_len)
{
	if (data == NULL) 
	{
		printf("data should not be NULL\r\n");
		return -1;
	}

	if (data_len != strlen(data)) 
	{
		printf("data_len (%d) is not match to the data length (%zu)\r\n", data_len, strlen(data));
		return -1;
	}

	// 解析每一个AT命令行 parse each AT command line
	int start = 0;
	for (int i = start; i < data_len; i++) 
	{
		if (data[i] == '\n' && i - 1 > start && data[i - 1] == '\r') 
		{
			parse_atcmd_line(data, start, i - 2);
			start = i + 1;
		}
	}

	return 0;
}

/** 解析AT命令函数
 *	@brief	解析字段行
 *	@param	
 *	@return 0成功，-1失败
 *	@note	无
 **/	
static int parse_atcmd_line(const char * data, int start, int end) 
{
	// 1.定位＋号
	if (data[start] == '+' || data[start+1] == '+')
	{
		printf("the first and second character of line should not be plus\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	int plus = get_plus_index(data, start + 2, end);
	if (plus == -1) 
	{
		printf("there is no plus in line\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	if (plus == end)
	{
		// atcmd is empty
		return -1;
	}

	// 2.定位空格号
	if (data[plus+1] == ' ')
	{
		printf("the first character of AT+ should not be space\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	int space = get_space_index(data, plus+1, end);
	if (space == -1) 
	{
		printf("there is no space in line\r\n");
		printf("%s\r\n", data);
		space = end+1;
	}	

	// 3.提取字段和字段长度 get field, field_len
	int i = start;
	int j = space - 1;
	if (trim_spaces(data, &i, &j) == -1) 
	{
		return -1;
	}
	const char * field = &data[i];
	int field_len = j - i + 1;


	// 4. 提取值和值长度 get value, value_len
	const char * value = NULL;
	int value_len = 0;
	if(space < end)
	{
		i = space + 1;
		j = end;
		if (trim_spaces(data, &i, &j) == -1) 
		{
			return -1;
		};
		value = &data[i];

		value_len = j - i + 1;
	}

	for(int k=0; k<(sizeof(AT)/sizeof(vkAT)); k++)
	{
		// 5.设置每个字段值数据包结构体中 set each field's value to packet
		if (field_len == strlen(AT[k].at_cmd) && strncasecmp(field, AT[k].at_cmd, field_len) == 0) 
		{
			AT[k].at_fun((void*)value, value_len);

			break;
		}		
	}
	
	// the field is not in the struct packet
	return 0;
}

/** 获取+号位置函数
 *	@brief	获取+号位置
 *	@param	
 *	@return -1失败，否则返回位置
 *	@note	无
 **/
static int get_plus_index(const char * string, int start, int end) 
{
	int i;
	for (i = start; i <= end; i++) 
	{
		if (string[i] == '+' && i-2>=0 && string[i-2]=='A' && string[i-1]=='T') 
		{
			return i;
		}
	}
	return -1;
}

/** 获取空格号位置函数
 *	@brief	获取空格号位置
 *	@param	
 *	@return -1失败，否则返回位置
 *	@note	无
 **/
static int get_space_index(const char * string, int start, int end) 
{
	int i;
	for (i = start; i <= end; i++) 
	{
		if (string[i] == ' ') 
		{
			return i;
		}
	}
	return -1;
}

/** 剪除空格字符函数
 *	@brief	剪除空格字符
 *	@param	
 *	@return -1失败，0成功
 *	@note	无
 **/
static int trim_spaces(const char * string, int * start, int * end) 
{
	int i = *start;
	int j = *end;

	while (i <= *end   && (!isprint(string[i]) || isspace(string[i]))) i++;
	while (j >= *start && (!isprint(string[j]) || isspace(string[j]))) j--;

	if (i > j) 
	{
		return -1;
	}

	*start = i;
	*end   = j;
	return 0;
}

void Terminal_Callback(void *pdata)
{
	uint8_t data[TERMINAL_BUFFER_SIZE];

	/* 获取串口终端数据 */
	int ret = vkUsartRecv(ATTerminal.term_com, data, TERMINAL_BUFFER_SIZE);	

	if(ret > 0)
	{
		/* 一个字符 */
		if(ret==1)
		{
			if(data[0] == '\r')	//Enter按键
			{
				/* 手动添加\r\n */
				ATTerminal.term_buffer[ATTerminal.term_cursor++]='\r';
				ATTerminal.term_buffer[ATTerminal.term_cursor++]='\n';
				ATTerminal.term_count += 2;

				/* 打印提示符 */
				vkUsartSend(ATTerminal.term_com ,"\r\n#", 3);
				
				/* 解析AT命令 */
				vkAT_Parser((const char *)ATTerminal.term_buffer, ATTerminal.term_cursor);

				ATTerminal.term_count = 0;
				ATTerminal.term_cursor = 0;
				memset(ATTerminal.term_buffer, 0, TERMINAL_BUFFER_SIZE);				
			}
			else if(data[0] == 0x7F)	//DEL按键
			{
				if(ATTerminal.term_count > 0)
				{
				 	vkUsartSend(ATTerminal.term_com ,data, ret);
					ATTerminal.term_buffer[--ATTerminal.term_cursor]='\0';
					ATTerminal.term_count -= 1;
				}
			}
			else if(isdispch(data[0]) == 1) //可显示字符
			{				
				if(ATTerminal.term_cursor+ret <= (TERMINAL_BUFFER_SIZE-2))
				{
					vkUsartSend(ATTerminal.term_com ,data, ret);					
					memcpy(ATTerminal.term_buffer+ATTerminal.term_cursor, data, ret);
					ATTerminal.term_count += ret;
					ATTerminal.term_cursor += ret;					
				}
			}
		}
		/* 多个字符 */
		else
		{	
			if(data[0] != 0x1B)	//ESC按键
			{
				if(ATTerminal.term_cursor+ret<=(TERMINAL_BUFFER_SIZE-2))
				{
					vkUsartSend(ATTerminal.term_com ,data, ret);
					memcpy(ATTerminal.term_buffer+ATTerminal.term_cursor, data, ret);
					ATTerminal.term_count += ret;
					ATTerminal.term_cursor += ret;
				}				
			}
		}		
	}	

	/* 再次启动软定时器 */
	vkTimerInsert(&ATTerminal.term_timer);
}

/*******************************************************************************
 * 名称: AT命令串口终端启动命令
 * 功能: AT命令环境
 * 形参: 串口com
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkAT_TerminalStart(vkCOM com)
{
	ATTerminal.tern_enable = 1;
	ATTerminal.term_com = com;
	ATTerminal.term_count = 0;
	ATTerminal.term_cursor = 0;	
	memset(ATTerminal.term_buffer, 0, TERMINAL_BUFFER_SIZE);
	
	/* 设置定时器回调，10ms执行一次回调函数 */
	ATTerminal.term_timer.timer_name = (void *)&Terminal_Callback;
	ATTerminal.term_timer.cb_func 	= Terminal_Callback;
	ATTerminal.term_timer.cb_data 	= NULL;
	ATTerminal.term_timer.cb_ticks	= vkMS_TO_TICKS(10);

	/* 显示提示符“#” */
	vkUsartSend(ATTerminal.term_com ,"\r\n#", 3);

	/* 启动软定时器 */
	if(vkTIMER_OK != vkTimerInsert(&ATTerminal.term_timer))
	{
		return -1;
	}
	
	return 0;
}

int vkAT_TerminalStop()
{
	ATTerminal.tern_enable = 0;	
	
	/* 停止软定时器 */
	vkTimerCancel(&ATTerminal.term_timer);

	return 0;
}

/*******************************************************************************
 * 名称: AT命令串口终端打印
 * 功能: 打印字符串
 * 形参: 串口com
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkAT_TerminalPrint(uint8_t *buf, int size)
{
	int ret;
	uint8_t data[TERMINAL_BUFFER_SIZE];
	uint8_t len = (size < (TERMINAL_BUFFER_SIZE-3))?size:(TERMINAL_BUFFER_SIZE-3);
	
	memcpy(data, buf, len);
	memcpy(data+len, "\r\n#", 3);
	len += 3;
	ret = vkUsartSend(ATTerminal.term_com , data, len);

	return ret;
}

void Communication_Callback(void *pdata)
{
	/* 清零 */
	memset(ATCommunication.comm_buffer, 0, COMMUNICATION_BUFFER_SIZE);
	
	/* 获取串口终端数据 */
	int ret = vkUsartRecv(ATCommunication.comm_com, ATCommunication.comm_buffer, COMMUNICATION_BUFFER_SIZE);

	if(ret > 0)
	{
		/* 解析AT命令 */
		vkAT_Parser((const char *)ATCommunication.comm_buffer, strlen((const char *)ATCommunication.comm_buffer));		
	}

	/* 再次启动软定时器 */
	vkTimerInsert(&ATCommunication.comm_timer);	
}

/*******************************************************************************
 * 名称: AT命令数据通信启动
 * 功能: 
 * 形参: 串口com
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkAT_CommunicationStart(vkCOM com)
{
	ATCommunication.comm_enable = 1;
	ATCommunication.comm_com = com;
	
	memset(ATTerminal.term_buffer, 0, TERMINAL_BUFFER_SIZE);

	/* 设置定时器回调，10ms执行一次回调函数 */
	ATCommunication.comm_timer.timer_name = (void *)&Communication_Callback;
	ATCommunication.comm_timer.cb_func 	= Communication_Callback;
	ATCommunication.comm_timer.cb_data 	= NULL;
	ATCommunication.comm_timer.cb_ticks	= vkMS_TO_TICKS(10);

	/* 启动软定时器 */
	if(vkTIMER_OK != vkTimerInsert(&ATCommunication.comm_timer))
	{
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * 名称: AT命令数据通信停止
 * 功能: 
 * 形参: 串口com
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkAT_CommunicationStop()
{
	ATCommunication.comm_enable = 0;

	/* 停止软定时器 */
	vkTimerCancel(&ATCommunication.comm_timer);

      return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//AT命令动作定义
/////////////////////////////////////////////////////////////////////////////////

void AT_Test(void *data, int len)
{
	if(data != NULL)
	{
	
	}
	
	if(ATTerminal.tern_enable == 1)
	{
		vkAT_TerminalPrint("OK\r\n", 4);
	}

	if(ATCommunication.comm_enable == 1)
	{
		vkUsartSend(ATCommunication.comm_com ,"OK\r\n", 4);
	}
	
	return;
}

void AT_Reset(void *data, int len)
{
	if(ATTerminal.tern_enable == 1)
	{
		vkAT_TerminalPrint("OK\r\n", 4);
	}
	if(ATCommunication.comm_enable == 1)
	{
		vkUsartSend(ATCommunication.comm_com ,"OK\r\n", 4);
	}	
	
	return;
}


