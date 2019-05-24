/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：led.h
 * 描述    ：LED外设库函数   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-10-20

  风驰STM8开发板硬件连接
    |--------------------|
    |  LED1-PD0          |
    |  LED2-PD1          |
    |  LED3-PD2          |
    |  LED4-PD3          |
    |--------------------|

*******************************************************************************/

#ifndef __LED_H
#define __LED_H

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"

/* 自定义数据类型 */

/* 自定义常量宏和表达式宏 */
/* LED 灯所接的GPIO端口定义 */
#define LED_PORTC	GPIOC	/* 定义LED外设所接GPIO端口 */
#define LED_PORTD	GPIOD	

/* LED灯所接的GPIO引脚定义 */
#define LED_1     	GPIO_PIN_1
#define LED_2       GPIO_PIN_0
#define LED_3       GPIO_PIN_3


#define ON  0		/* 定义LED灯亮 -- 低电平*/
#define OFF 1		/* 定义LED灯灭 -- 高电平*/

/* 声明给外部使用的变量 */

/* 声明给外部使用的函数 */

/*******************************************************************************
 * 名称: LED_Init
 * 功能: LED外设GPIO引脚初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_Init(void);

/*******************************************************************************
 * 名称: SetLedOFF
 * 功能: 关闭所有LED灯
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void SetLedOFF(void);

/*******************************************************************************
 * 名称: LED_Operation
 * 功能: LED 灯亮灭设置操作
 * 形参: ledx -> 要操作的led灯
 * 		 state -> 亮还是灭
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_Operation(GPIO_Pin_TypeDef ledx, u8 state);

/*******************************************************************************
 * 名称: LED_ShowOneToOne
 * 功能: 宏定义的方式实现流水灯
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_ShowOneToOne(void);

/*******************************************************************************
 * 名称: Delay
 * 功能: 简单的延时函数
 * 形参: nCount -> 延时时间数
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
static void Delay(u32 nCount);

#endif


/*************** (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/