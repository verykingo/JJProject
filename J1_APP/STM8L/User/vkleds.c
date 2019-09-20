/******************************************************************************
 * 文件  ：vkleds.c
 * 描述    ：led指示灯
 * 平台    ：STM8L
 * 时间  ：2019-09-04

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vkleds.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: vkLeds_Init
 * 功能: Led灯初始化函数
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkLeds_Init()
{
	int size = sizeof(led_gpio)/sizeof(struct LED_GPIO);

	for(int i=0; i<size; i++)
	{
		GPIO_Init(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin, GPIO_Mode_Out_PP_High_Slow);
	}
	
	return 0;
}

/*******************************************************************************
 * 名称: vkLedsSet
 * 功能: 设置Led状态
 * 形参: state=0灯灭，state=1灯亮
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
int vkLedsSet(vkLED led, uint8_t state)
{
	if(led >=LEDMAX)
	{
		return -1;
	}
	
	int i = (int)led;
	
	(state == 0)?(GPIO_SetBits(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin)):(GPIO_ResetBits(led_gpio[i].GPIOx, led_gpio[i].GPIO_Pin));
	
	return 0;
}
