/*****************************************************************************
 * 文件名  ：vktimetick.h
 * 描述    ：RTOS和软定时器节拍时钟设置   
 * 修改时间 ：2019-04-01

*******************************************************************************/
#ifndef __VKTIMETICK_H
#define __VKTIMETICK_H

/* 包含系统头文件 */

/* 包含自定义头文件 */

/* 自定义数据类型 */

/* 每一个Tick等于10毫秒(最大定时时间2^32*10ms=42949000s)时间 */
#define US_PER_TICKS	((uint32_t)(10000ul))

/* 微秒转换 */
#define vkTICKS_TO_US(x) 	((uint32_t)(x*US_PER_TICKS))
#define vkUS_TO_TICKS(x) 	((uint32_t)(x/US_PER_TICKS))

/* 毫秒转换 */
#define vkTICKS_TO_MS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul))
#define vkMS_TO_TICKS(x) 	((uint32_t)(x*1000ul/US_PER_TICKS))

/* 秒转换 */
#define vkTICKS_TO_SS(x) 	((uint32_t)(x*US_PER_TICKS/1000ul*1000ul))
#define vkSS_TO_TICKS(x) 	((uint32_t)(x*1000ul*1000ul/US_PER_TICKS))

void vkTimeTick_Init(uint8_t TIM);

#endif

