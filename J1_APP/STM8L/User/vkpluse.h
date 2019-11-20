
/******************************************************************************
 * 文件  ：vkpluse.h
 * 描述    ：STM8L Pluse
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKPLUSE_H
#define __VKPLUSE_H

/* 包含系统头文件 */
#include "stm8l15x.h"

/* 包含自定义头文件 */
#include "vksofttimer.h"
#include "vkpwm.h"
#include "vkADC.h"

#define PLUSE_PWM_FREQUENCY		5600ul
#define PLUSE_PWM_PERIOD   		(uint32_t)((1000ul*1000ul)/PLUSE_PWM_FREQUENCY)
#define PLUSE_PWM_TOLERANCE 	2

#define PLUSE_TICK_TIME 		50		//一个Tick是50us
#define PLUSE_LEVEL_HIGH		1
#define PLUSE_LEVEL_LOW			0

#define PLUSE_TICKS_PER_MS		(1000/PLUSE_TICK_TIME)	//一秒钟TICK数

#define PLUSE_COUNT_WIDTH_MAX   (300/PLUSE_TICK_TIME)	//脉冲高电平最大计数次数（300us/50us=6）

#define PLUSE_MODE_ONLYONCE		0x01	//单个模式
#define PLUSE_MODE_CONTINUE		0x02	//循环模式	

#define PLUSE_VREFINT			1.200f	//芯片内部参考电压值	
#define PLUSE0_RESISTANCE		10.0f	//脉冲0强度参考电阻	
#define PLUSE1_RESISTANCE		10.0f	//脉冲1强度参考电阻


/* 电极A开关和ADC GPIO定义 */
#define PLUSE0_S0_GPIO			GPIOD
#define PLUSE0_S0_PIN			GPIO_Pin_7	
#define PLUSE0_S1_GPIO			GPIOD
#define PLUSE0_S1_PIN			GPIO_Pin_3	
#define PLUSE0_S2_GPIO			GPIOD	
#define PLUSE0_S2_PIN			GPIO_Pin_5
#define PLUSE0_S3_GPIO			GPIOE
#define PLUSE0_S3_PIN			GPIO_Pin_2
#define PLUSE0_ADC_GPIO			GPIOB
#define PLUSE0_ADC_PIN			GPIO_Pin_2

/* 电极B开关和ADC GPIO */
#define PLUSE1_S0_GPIO			GPIOF
#define PLUSE1_S0_PIN			GPIO_Pin_0	
#define PLUSE1_S1_GPIO			GPIOD
#define PLUSE1_S1_PIN			GPIO_Pin_1
#define PLUSE1_S2_GPIO			GPIOD
#define PLUSE1_S2_PIN			GPIO_Pin_6
#define PLUSE1_S3_GPIO			GPIOE
#define PLUSE1_S3_PIN			GPIO_Pin_3
#define PLUSE1_ADC_GPIO			GPIOE
#define PLUSE1_ADC_PIN			GPIO_Pin_5

#define PLUSE_EXPECT_MAX 80

/* 自定义数据类型 */
enum PLUSE
{
	PLUSE0 = 0,
	PLUSE1 = 1
};

typedef struct pluse
{
	uint8_t  p_mode;		//模式单次/循环
	uint8_t  p_expect;		//预设电流0~80mA
	uint8_t  p_current;		//当前电流0~80mA
	uint8_t  p_ration;		//当前占空比0~100%

	uint16_t p_rise;		//一次脉冲刺激上升时间(0~500ms依赖持续时间)
	uint16_t p_fall;		//一次脉冲刺激下降时间(0~500ms依赖持续时间)
	
	uint16_t p_duration;	//持续时长ms
	uint16_t p_interval;	//间隔时长ms		
	uint16_t p_poqcount;	//上层计数，1个计数1ms	
	uint8_t  p_1mscount;	//1毫秒计数
	uint8_t  p_poqlevel;	//上层状态
	uint8_t  p_poqflags;	//上层标记	
	vkTIMER  p_poqtimer;	//上层定时器
	
	uint16_t p_freqy;		//脉冲频率Hz(17/20/25/33/40)
	uint16_t p_width;		//100us~300us/50
	uint16_t p_tickfreqy;	//周期时间Tick数目
	uint16_t p_tickwidth;	//脉宽时间Tick数目		
	uint16_t p_bodcount;	//下层计数	
	uint16_t p_bodvalue[PLUSE_COUNT_WIDTH_MAX-1];	//ADC电流强度值
	uint8_t  p_bodlevel;	//下层状态

	GPIO_TypeDef* 		p_s0_gpio;	//供电开关S0
	GPIO_Pin_TypeDef	p_s0_pin;			
	GPIO_TypeDef* 		p_s1_gpio;	//输出开关S1
	GPIO_Pin_TypeDef	p_s1_pin;
	GPIO_TypeDef* 		p_s2_gpio;	//接地开关S2
	GPIO_Pin_TypeDef	p_s2_pin;
	GPIO_TypeDef* 		p_s3_gpio;	//检测开关S3
	GPIO_Pin_TypeDef	p_s3_pin;

	ADC_Channel_TypeDef p_adcchnl;	//ADC电流检测通道
	GPIO_TypeDef* 		p_adc_gpio;	//ADC电流检测GPIO
	GPIO_Pin_TypeDef	p_adc_pin;	//ADC电流检测引脚
	
	vkPWM pwm;
} vkPLUSE;

int vkPluse_Init(void);
int vkPluse_Deinit(void);

int vkPluseSetMode(uint8_t pluse, uint8_t mode);
int vkPluseSetExpt(uint8_t pluse, uint8_t expert);
int vkPluseSetTime(uint8_t pluse, uint16_t duration, uint16_t interval);
int vkPluseSetWidth(uint8_t pluse, uint16_t width);
int vkPluseSetFrequency(uint8_t pluse, uint16_t frequency);
int vkPluseSetRise(uint8_t pluse, uint16_t rise);
int vkPluseSetFall(uint8_t pluse, uint16_t fall);

int vkPluse1MSAdd(uint8_t pluse, uint8_t value);
int vkPluse1MSSet(uint8_t pluse, uint8_t value);

uint8_t vkPluseGetExpt(uint8_t pluse);
uint8_t vkPluseGetPoqL(uint8_t pluse);

int vkPluseStart(uint8_t pluse);
int vkPluseStop(uint8_t pluse);
int vkPluseGetIdle(void);

void vkPluseTick(void);
uint8_t vkPluseAdjust(uint8_t pluse, uint8_t expect, uint8_t ration, uint8_t current);
uint8_t vkPluseRation(uint8_t pluse, uint8_t ration);
uint8_t vkPluseTrimExpt(uint8_t expect ,uint16_t duration, uint16_t rise, uint16_t fall, uint16_t poqcount);

#endif
