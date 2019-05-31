/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：iwdg.c
 * 描述    ：独立看门狗实验函数   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-11-22
*******************************************************************************/

#ifndef __IWDG_H
#define __IWDG_H
/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"

/* 自定义新类型 */

/* 自定义宏 */
#define FEEDWDOGTIME	1000	/* 设置定时器的喂狗时间 */

/* 全局变量定义 */
extern u32 ReloadDogTime;

/*******************************************************************************
 * 名称: IWDG_Conf
 * 功能: 独立看门狗初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void IWDG_Conf(void);

/*******************************************************************************
 * 名称: Test_IWDGReset
 * 功能: 判断独立看门狗是否复位
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Test_IWDGReset(void);

#endif