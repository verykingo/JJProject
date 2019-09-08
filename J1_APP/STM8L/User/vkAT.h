
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

/* 自定义数据类型 */

/* Callback function prototype */
typedef void ( * vkAT_FUNC ) ( void* at_data );

typedef struct at
{
	uint16_t			at_id;			//AT识别号
	char * 				at_cmd;			//AT命令
	vkAT_FUNC			at_fun;			//AT函数

} vkAT;

void AT_Reset(void *data);
void AT_Test(void *data);


#endif

