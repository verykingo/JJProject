/******************************************************************************
 * �ļ�  ��vkAT.c
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* �����Զ���ͷ�ļ� */
#include "vkAT.h"

/* �Զ��������� */

/* �Զ���� */
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

/* ȫ�ֱ������� */
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
 * ����: AT�������
 * ����: AT����
 * �β�: data�ַ���ָ�룬data_len�ַ�������
 * ����: ��
 * ˵��: �� 
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

	// ����ÿһ��AT������ parse each AT command line
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

/** ����AT�����
 *	@brief	�����ֶ���
 *	@param	
 *	@return 0�ɹ���-1ʧ��
 *	@note	��
 **/	
static int parse_atcmd_line(const char * data, int start, int end) 
{
	// 1.��λ����
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

	// 2.��λ�ո��
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

	// 3.��ȡ�ֶκ��ֶγ��� get field, field_len
	int i = start;
	int j = space - 1;
	if (trim_spaces(data, &i, &j) == -1) 
	{
		return -1;
	}
	const char * field = &data[i];
	int field_len = j - i + 1;


	// 4. ��ȡֵ��ֵ���� get value, value_len
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
		// 5.����ÿ���ֶ�ֵ���ݰ��ṹ���� set each field's value to packet
		if (field_len == strlen(AT[k].at_cmd) && strncasecmp(field, AT[k].at_cmd, field_len) == 0) 
		{
			AT[k].at_fun((void*)value, value_len);

			break;
		}		
	}
	
	// the field is not in the struct packet
	return 0;
}

/** ��ȡ+��λ�ú���
 *	@brief	��ȡ+��λ��
 *	@param	
 *	@return -1ʧ�ܣ����򷵻�λ��
 *	@note	��
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

/** ��ȡ�ո��λ�ú���
 *	@brief	��ȡ�ո��λ��
 *	@param	
 *	@return -1ʧ�ܣ����򷵻�λ��
 *	@note	��
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

/** �����ո��ַ�����
 *	@brief	�����ո��ַ�
 *	@param	
 *	@return -1ʧ�ܣ�0�ɹ�
 *	@note	��
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
 * ����: AT�����
 * ����: AT�����
 * �β�: ����com
 * ����: ��
 * ˵��: �� 
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


