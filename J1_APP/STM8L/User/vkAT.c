/******************************************************************************
 * 文件  ：vkAT.c
 * 描述    ：STM8L节拍定时器
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vkAT.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

static const vkAT AT[]= {
	/* id, 	AT Commond, 			AT Function */
	{0, 	"AT+TEST", 				AT_Test},
	{1, 	"AT+RESET", 			AT_Reset}
};

/*******************************************************************************
 * 名称: AT命令
 * 功能: AT命令
 * 形参: AT命令
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void AT_Test(void *data)
{
	return;
}

void AT_Reset(void *data)
{
	return;
}

static int AT_parser(const char * data, int data_len)
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

	// 解析每一个字段行 parse each field line
	int start = 0;
	for (int i = start; i < data_len; i++) 
	{
		if (data[i] == '\n' && i - 1 > start && data[i - 1] == '\r') 
		{
			parse_field_line(data, start, i - 2);
			start = i + 1;
		}
	}
}

/** 解析字段行函数
 *	@brief	解析字段行
 *	@param	
 *	@return 0成功，-1失败
 *	@note	无
 **/	
static int parse_field_line(const char * data, int start, int end) 
{
	// 1.定位冒号 find the colon
	if (data[start] == '+' || data[start+1] == '+') 
	{
		printf("the first character of line should not be colon\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	int colon = get_colon_index(data, start + 2, end);
	if (colon == -1) 
	{
		printf("there is no colon in line\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	if (colon == end) 
	{
		// value is empty
		return -1;
	}

	// 1.定位空格号
	if (data[colon+1] == ' ') 
	{
		printf("the first character of AT+ should not be space\r\n");
		printf("%s\r\n", data);
		return -1;
	}

	int space = get_space_index(data, colon+1, end)
	if (space == -1) 
	{
		printf("there is no space in line\r\n");
		printf("%s\r\n", data);
		space = end+1;
	}
	

	// 2.提取字段和字段长度 get field, field_len
	int i = start;
	int j = space - 1;
	if (trim_spaces(data, &i, &j) == -1) 
	{
		return -1;
	}
	const char * field = &data[i];
	int field_len = j - i + 1;


	// 3. 提取值和值长度 get value, value_len
	if(space < end)
	{
		i = space + 1;
		j = end;
		if (trim_spaces(data, &i, &j) == -1) 
		{
			return -1;
		};
		const char * value = &data[i];
		size_t value_len = j - i + 1;
	}


	// 4.设置每个字段值数据包结构体中 set each field's value to packet
	//查询目标
	if (field_len == strlen("st") && strncasecmp(field, "st", field_len) == 0) 
	{
		memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
		return 0;
	}
	//通知目标
	if (field_len == strlen("nt") && strncasecmp(field, "nt", field_len) == 0) 
	{
		memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
		return 0;
	}
	//唯一服务号
	if (field_len == strlen("usn") && strncasecmp(field, "usn", field_len) == 0) 
	{
		memcpy(packet->usn, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
	}
	//设备位置
	if (field_len == strlen("location") && strncasecmp(field, "location", field_len) == 0) 
	{
		memcpy(packet->location, value, value_len < LSSDP_LOCATION_LEN ? value_len : LSSDP_LOCATION_LEN - 1);
		return 0;
	}
	//SM识别号
	if (field_len == strlen("sm_id") && strncasecmp(field, "sm_id", field_len) == 0) 
	{
		memcpy(packet->sm_id, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
	}
	//设备类型
	if (field_len == strlen("dev_type") && strncasecmp(field, "dev_type", field_len) == 0) 
	{
		memcpy(packet->device_type, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
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
static int get_colon_index(const char * string, int start, int end) 
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
static int trim_spaces(const char * string, size_t * start, size_t * end) 
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



