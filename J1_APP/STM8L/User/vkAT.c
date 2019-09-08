/******************************************************************************
 * �ļ�  ��vkAT.c
 * ����    ��STM8L���Ķ�ʱ��
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"

/* �����Զ���ͷ�ļ� */
#include "vkAT.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

static const vkAT AT[]= {
	/* id, 	AT Commond, 			AT Function */
	{0, 	"AT+TEST", 				AT_Test},
	{1, 	"AT+RESET", 			AT_Reset}
};

/*******************************************************************************
 * ����: AT����
 * ����: AT����
 * �β�: AT����
 * ����: ��
 * ˵��: �� 
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

	// ����ÿһ���ֶ��� parse each field line
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

/** �����ֶ��к���
 *	@brief	�����ֶ���
 *	@param	
 *	@return 0�ɹ���-1ʧ��
 *	@note	��
 **/	
static int parse_field_line(const char * data, int start, int end) 
{
	// 1.��λð�� find the colon
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

	// 1.��λ�ո��
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
	

	// 2.��ȡ�ֶκ��ֶγ��� get field, field_len
	int i = start;
	int j = space - 1;
	if (trim_spaces(data, &i, &j) == -1) 
	{
		return -1;
	}
	const char * field = &data[i];
	int field_len = j - i + 1;


	// 3. ��ȡֵ��ֵ���� get value, value_len
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


	// 4.����ÿ���ֶ�ֵ���ݰ��ṹ���� set each field's value to packet
	//��ѯĿ��
	if (field_len == strlen("st") && strncasecmp(field, "st", field_len) == 0) 
	{
		memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
		return 0;
	}
	//֪ͨĿ��
	if (field_len == strlen("nt") && strncasecmp(field, "nt", field_len) == 0) 
	{
		memcpy(packet->st, value, value_len < LSSDP_FIELD_LEN ? value_len : LSSDP_FIELD_LEN - 1);
		return 0;
	}
	//Ψһ�����
	if (field_len == strlen("usn") && strncasecmp(field, "usn", field_len) == 0) 
	{
		memcpy(packet->usn, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
	}
	//�豸λ��
	if (field_len == strlen("location") && strncasecmp(field, "location", field_len) == 0) 
	{
		memcpy(packet->location, value, value_len < LSSDP_LOCATION_LEN ? value_len : LSSDP_LOCATION_LEN - 1);
		return 0;
	}
	//SMʶ���
	if (field_len == strlen("sm_id") && strncasecmp(field, "sm_id", field_len) == 0) 
	{
		memcpy(packet->sm_id, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
	}
	//�豸����
	if (field_len == strlen("dev_type") && strncasecmp(field, "dev_type", field_len) == 0) 
	{
		memcpy(packet->device_type, value, value_len < LSSDP_MINI_LEN ? value_len : LSSDP_MINI_LEN - 1);
		return 0;
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



