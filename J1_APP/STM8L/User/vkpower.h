
/******************************************************************************
 * 文件  ：vkpower.h
 * 描述    ：STM8L 电源管理
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKPOWER_H
#define __VKPOWER_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */

/* 自定义数据类型 */

#define TLY_SWITCH_IO_PORT	GPIOC
#define TLY_SWITCH_PIN		GPIO_Pin_6
#define BLE_SWITCH_IO_PORT	GPIOC
#define BLE_SWITCH_PIN		GPIO_Pin_5
#define DJA_SWITCH_IO_PORT	GPIOD
#define DJA_SWITCH_PIN		GPIO_Pin_7
#define DJB_SWITCH_IO_PORT	GPIOF
#define DJB_SWITCH_PIN		GPIO_Pin_0

#define BTN_SWITCH_IO_PORT	GPIOD
#define BTN_SWITCH_PIN		GPIO_Pin_2

int vkPower_Init(void);
int vkPowerBLESet(uint8_t mode);
int vkPowerTLYSet(uint8_t mode);
int vkPowerPoleASet(uint8_t mode);
int vkPowerPoleBSet(uint8_t mode);
int vkPowerInitError(uint8_t error, uint16_t duration);
void vkPower_Interrupt_Callback();

#endif
