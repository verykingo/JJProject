
/******************************************************************************
 * 文件  ：vkleds.h
 * 描述    ：led指示灯
 * 平台    ：STM8L
 * 时间  ：2019-09-04

*******************************************************************************/

#ifndef __VKLEDS_H
#define __VKLEDS_H

/* 包含系统头文件 */

/* 包含自定义头文件 */

/* 自定义数据类型 */
typedef struct LED_GPIO
{
	GPIO_TypeDef* GPIOx; 
	uint16_t GPIO_Pin;
} LED_GPIO;

enum vkLED
{
	LED0 = 0,
	LED1,
	LEDMAX
};

/* 自定义变量 */
const static struct LED_GPIO led_gpio[] = 	{{GPIOE, GPIO_Pin_0},
											 {GPIOE, GPIO_Pin_1}	
											};

int vkLeds_Init();
int vkLedsSet(enum vkLED led, int state);

#endif
