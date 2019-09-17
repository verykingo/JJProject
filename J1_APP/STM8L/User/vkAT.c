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

/* 自定义新类型 */

/* 自定义宏 */
#define isspace(c)	((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define isascii(c)	(((c) & ~0x7F) == 0)
#define isupper(c)	((c) >= 'A' && (c) <= 'Z')
#define islower(c)	((c) >= 'a' && (c) <= 'z')
#define isalpha(c)	(isupper(c) || islower(c))
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define isxdigit(c)	(isdigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#define isprint(c)	((c) >= ' ' && (c) <= '~')
#define toupper(c)	((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
#define tolower(c)	((c) + 0x20 * (((c) >= 'A') && ((c) <= 'Z')))
#define toascii(c)	((unsigned)(c) & 0x7F)
#define isalnum(c)	(isalpha(c) || isdigit(c))
#define isodigit(c)	((c) >= '0' && (c) <= '7')

/* 全局变量定义 */
static const vkAT AT[]= {
  /* id, 	AT Commond, 			AT Function */
	{0, 	"AT+TEST", 				AT_Test},
	{1, 	"AT+RESET", 			AT_Reset}
};

static vkCOM ATCOM = (vkCOM)-1;

static int parse_atcmd_line(const char * data, int start, int end);
static int get_plus_index(const char * string, int start, int end);
static int get_space_index(const char * string, int start, int end);
static int trim_spaces(const char * string, int * start, int * end);

/*******************************************************************************
 * 名称: AT命令解析
 * 功能: AT命令
 * 形参: data字符串指针，data_len字符串长度
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkATParser(const char * data, int data_len)
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

/*******************************************************************************
 * 名称: AT命令环境
 * 功能: AT命令环境
 * 形参: 串口com
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkATEcho(vkCOM com)
{		
	uint8_t data[AT_COMMANDLINE_SIZE_MAX];
	static uint8_t command[AT_COMMANDLINE_SIZE_MAX];
	static uint8_t index = 0;
		
	ATCOM = com;
	
	int ret = vkUsart_Recv(ATCOM, data, AT_COMMANDLINE_SIZE_MAX);	

	if(ret > 0)
	{
		if(ret==1 && data[0] == '\r')
		{
			vkATParser((const char *)command, index);
			index = 0;
			memset(command, 0, sizeof(command));
			vkUsart_Send(ATCOM ,"\r\n#", 3);			
		}
		else
		{
			vkUsart_Send(ATCOM ,data, ret);
			if(index+ret <= AT_COMMANDLINE_SIZE_MAX)
			{
				memcpy(command+index, data, ret);
				index += ret;
			}
		}		
	}
	
	return 0;
}


void AT_Test(void *data, int len)
{
	vkUsart_Send(ATCOM ,"OK\r\n", 4);
	
	return;
}

void AT_Reset(void *data, int len)
{
	vkUsart_Send(ATCOM ,"OK\r\n", 4);
	
	return;
}


