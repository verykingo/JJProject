
/******************************************************************************
 * 文件  ：vkADC.h
 * 描述  ：ADC
 * 平台  ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKADC_H
#define __VKADC_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */

int vkADC_Init(void);
int vkADC_Deinit(void);
uint16_t vkADC_Read(ADC_Channel_TypeDef channel);

#endif
