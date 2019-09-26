/******************************************************************************
 * 文件  ：vkpluse.c
 * 描述    ：电极脉冲
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include "stdlib.h"
#include <math.h>
#include <string.h>

/* 包含自定义头文件 */
#include "vkpluse.h"
#include "vktimetick.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */
static vkPLUSE gPluse0 = {0};
static vkPLUSE gPluse1 = {0};
static uint8_t gMutex = 0;
static uint8_t gDelay_times = 0;	//gPluse1到gPluse0滞后时间
static vkTIMER gDelay_Timer;		//gPluse1到gPluse0滞后定时器

void vkPluse_Timer_Callback(void * pdata);
void gDelay_Timer_Callback(void * pdata);

int vkPluse_Init(void)
{
	/* 脉冲0 */
	gPluse0.p_mode = 0;
	gPluse0.p_expect = 0;
	gPluse0.p_ration = 0;
	gPluse0.p_current = 0;
	
	gPluse0.p_duration = 0;
	gPluse0.p_interval = 0xFF;
	gPluse0.p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse0.p_poqflags = 0;
	gPluse0.p_poqtimer.timer_name 	= &gPluse0.p_poqtimer;
	gPluse0.p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse0.p_poqtimer.cb_data  	= &gPluse0;
	gPluse0.p_poqtimer.cb_ticks 	= 0;

	gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
	gPluse0.p_bodcount = 0;
	memset(gPluse0.p_bodvalue,0,PLUSE_COUNT_HIGH-1);

	gPluse0.p_s0_gpio = PLUSE0_S0_GPIO;
	gPluse0.p_s0_pin  = PLUSE0_S0_PIN;	
	gPluse0.p_s1_gpio = PLUSE0_S1_GPIO;
	gPluse0.p_s1_pin  = PLUSE0_S1_PIN;
	gPluse0.p_s2_gpio = PLUSE0_S2_GPIO;
	gPluse0.p_s2_pin  = PLUSE0_S2_PIN;
	gPluse0.p_s3_gpio = PLUSE0_S3_GPIO;
	gPluse0.p_s3_pin  = PLUSE0_S3_PIN;
	GPIO_Init(gPluse0.p_s0_gpio,gPluse0.p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s1_gpio,gPluse0.p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s2_gpio,gPluse0.p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse0.p_s3_gpio,gPluse0.p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	gPluse0.p_adcchnl  = ADC_Channel_16;
	gPluse0.p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse0.p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse0.p_adc_gpio,gPluse0.p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse0.pwm = PWM1;
	vkPWM_Init(gPluse0.pwm);	

	/* 脉冲1 */
	gPluse1.p_mode = 0;
	gPluse1.p_expect = 0;
	gPluse1.p_ration = 0;
	gPluse1.p_current = 0;
	
	gPluse1.p_duration = 0;
	gPluse1.p_interval = 0xFF;
	gPluse1.p_poqlevel = PLUSE_LEVEL_LOW;
	gPluse1.p_poqflags = 0;
	gPluse1.p_poqtimer.timer_name 	= &gPluse1.p_poqtimer;
	gPluse1.p_poqtimer.cb_func  	= vkPluse_Timer_Callback;
	gPluse1.p_poqtimer.cb_data  	= &gPluse1;
	gPluse1.p_poqtimer.cb_ticks 	= 0;

	gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
	gPluse1.p_bodcount = 0;
	memset(gPluse1.p_bodvalue,0,PLUSE_COUNT_HIGH-1);

	gPluse1.p_s0_gpio = PLUSE0_S0_GPIO;
	gPluse1.p_s0_pin  = PLUSE0_S0_PIN;	
	gPluse1.p_s1_gpio = PLUSE0_S1_GPIO;
	gPluse1.p_s1_pin  = PLUSE0_S1_PIN;
	gPluse1.p_s2_gpio = PLUSE0_S2_GPIO;
	gPluse1.p_s2_pin  = PLUSE0_S2_PIN;
	gPluse1.p_s3_gpio = PLUSE0_S3_GPIO;
	gPluse1.p_s3_pin  = PLUSE0_S3_PIN;
	GPIO_Init(gPluse1.p_s0_gpio,gPluse1.p_s0_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s1_gpio,gPluse1.p_s1_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s2_gpio,gPluse1.p_s2_pin,GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(gPluse1.p_s3_gpio,gPluse1.p_s3_pin,GPIO_Mode_Out_PP_Low_Slow);

	gPluse1.p_adcchnl  = ADC_Channel_16;
	gPluse1.p_adc_gpio = PLUSE0_ADC_GPIO;
	gPluse1.p_adc_pin  = PLUSE0_ADC_PIN;
	GPIO_Init(gPluse1.p_adc_gpio,gPluse1.p_adc_pin,GPIO_Mode_In_FL_No_IT);

	gPluse1.pwm = PWM2;
	vkPWM_Init(gPluse1.pwm);

	gDelay_Timer.timer_name = &gDelay_Timer;
	gDelay_Timer.cb_func  = gDelay_Timer_Callback;
	gDelay_Timer.cb_data  = NULL;
	gDelay_Timer.cb_ticks = 0;

	return 0;
}

/*
 * 设置脉冲模式
 */
int vkPluseSetMode(uint8_t pluse, uint8_t mode)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_mode = mode;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_mode = mode;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * 设置电流强度
 */
int vkPluseSetExpt(uint8_t pluse, uint8_t expert)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_expect = expert;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_expect = expert;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * 设置脉冲持续时间和间隔时间
 */
int vkPluseSetTime(uint8_t pluse, uint8_t duration, uint8_t interval)
{
	if(pluse == PLUSE0)
	{
		gPluse0.p_duration = duration;
		gPluse0.p_interval = interval;
	}
	else if(pluse == PLUSE1)
	{
		gPluse1.p_duration = duration;
		gPluse1.p_interval = interval;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * 设置脉冲滞后时间
 */
int vkPluseSetDelay(uint8_t delay)
{
	gDelay_times = delay;

	return 0;
}

/*
 * 启动脉冲输出
 */
int vkPluseStart(uint8_t pluse)
{	
	if(pluse == PLUSE0)
	{
		gPluse0.p_poqtimer.cb_ticks = vkSS_TO_TICKS(gPluse0.p_duration)/2;
		vkTimerInsert(&gPluse0.p_poqtimer);
		gPluse0.p_poqlevel = PLUSE_LEVEL_HIGH;
	}
	else if(pluse == PLUSE1)
	{		
		gPluse1.p_poqtimer.cb_ticks = vkSS_TO_TICKS(gPluse1.p_duration)/2;
		vkTimerInsert(&gPluse1.p_poqtimer);
		gPluse1.p_poqlevel = PLUSE_LEVEL_HIGH;
	}
	else
	{
		return -1;
	}
	
	return 0;
}

/*
 * 停止脉冲输出
 */
int vkPluseStop(uint8_t pluse)
{	
	if(pluse == PLUSE0)
	{
		vkTimerCancel(&gPluse0.p_poqtimer);
		gPluse0.p_ration  = 0;
		gPluse0.p_current = 0;

		gPluse0.p_poqlevel = PLUSE_LEVEL_LOW;
		gPluse0.p_poqflags = 0;

		gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
		gPluse0.p_bodcount = 0;

		vkPWMStop(gPluse0.pwm);
			
		GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//断开
		GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);	//断开
		GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//断开		
	}
	else if(pluse == PLUSE1)
	{
		vkTimerCancel(&gPluse1.p_poqtimer);
		gPluse1.p_ration  = 0;
		gPluse1.p_current = 0;

		gPluse1.p_poqlevel = PLUSE_LEVEL_LOW;
		gPluse1.p_poqflags = 0;

		gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
		gPluse1.p_bodcount = 0;

		vkPWMStop(gPluse1.pwm);
			
		GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//断开
		GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//断开
		GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//断开
	}
	else
	{
		return -1;
	}

	return 0;
}


void vkPluse_Timer_Callback(void * pdata)
{
	vkPLUSE *pluse = (vkPLUSE *)pdata;
	if(pluse == NULL)
	{
		return;
	}
	/* 周期模式 */
	if(pluse->p_mode == PLUSE_MODE_CONTINUE)
	{
		if(pluse->p_poqlevel == PLUSE_LEVEL_LOW)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_duration)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_interval)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}
	}
	/* 单个模式 */
	else if(pluse->p_mode == PLUSE_MODE_ONLYONCE)
	{
		if(pluse->p_poqlevel == PLUSE_LEVEL_LOW)
		{
			pluse->p_poqtimer.cb_ticks 	= vkSS_TO_TICKS(pluse->p_duration)/2;
			vkTimerInsert(&pluse->p_poqtimer);
			
			pluse->p_poqlevel = PLUSE_LEVEL_HIGH;
		}
		else if(pluse->p_poqlevel == PLUSE_LEVEL_HIGH)
		{			
			pluse->p_poqlevel = PLUSE_LEVEL_LOW;			
		}		
	}
}

void gDelay_Timer_Callback(void * pdata)
{
	return;
}

/*
 * 设置PWM占空比
 */
inline uint8_t vkPluseRation(vkPWM pwm, uint8_t ration)
{
	vkPWMStart(pwm, PLUSE_PWM_PERIOD, ration);

	return 0;
}

/*
 * 自调节脉冲强度
 */
inline uint8_t vkPluseAdjust(vkPWM pwm, uint8_t expect, uint8_t ration, uint8_t current)
{	
	uint8_t ret_ration = 0;
		
	/* 占空比不为0，说明电路有电压，电流为0，电路断开，说明电极脱落，此时不再调占空比升压*/
	if(ration!=0 && current ==0)
	{
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, 0);
	}
	else if(ration==0 && current <=2) 
	{
		ration = 2;
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, ration);
	}
	else
	{		
		/* 误差范围大于容忍范围 */
		if(fabs(current-expect)>PLUSE_PWM_TOLERANCE)
		{
			uint8_t step =(current==0)?1:(int)(fabs(current-expect)*(1/2)*0.7 + 1);
			
			if(current > expect)	//当前电流大于设定电流，占空比减小
			{	
				ret_ration = (ration-step <=0)?0:(ration-step);
			}
			else if(current < expect)	//当前电流小于设定电流，占空比增大，最大值90
			{	
				ret_ration = (ration + step >=90)?90:(ration + step);
			}
		}
		
		//pwm设定 //默认为100hz  10ms
		vkPWMStart(pwm, PLUSE_PWM_PERIOD, ret_ration);
	}
	
	return ret_ration;
}


void vkPluseTick(void)
{
	//电极A间隔时间
	if(gPluse0.p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse0.p_poqflags == 0)
		{
			gPluse0.p_poqflags = 1;
			
			vkPWMStop(gPluse0.pwm);

			//第一路开关设置低电平，电极停止放电
			GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//断开
			//第二路开关设置低电平，保持3.3V
			GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);	//断开
	
			GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//断开
		}
	}

	//电极B间隔时间
	if(gPluse1.p_poqlevel == PLUSE_LEVEL_LOW)
	{
		if(gPluse1.p_poqflags == 0)
		{
			gPluse1.p_poqflags = 1;

			vkPWMStop(gPluse1.pwm);

			//第一路开关设置低电平，电极停止放电
			GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//断开
			//第二路开关设置低电平，保持3.3V
			GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//断开
	
			GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//断开
		}
	}

	//电极A持续时间
	if(gPluse0.p_poqlevel == PLUSE_LEVEL_HIGH)
	{	
		if(gPluse0.p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse0.p_bodcount++;
					
			if(gPluse0.p_bodcount==1)	//第一个计数，低电平，开始充电	
			{
				//第二路开关设置高电平，接地，迅速拉低电极电压OV
				GPIO_SetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);

				GPIO_ResetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//断开
				
				//第一路开关设置低电平，电极停止放电
				GPIO_ResetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin);	//断开	
			}			
			else if(gPluse0.p_bodcount == PLUSE_MUTEX_DELAY) //延时 PLUSE_MUTEX_DELAY 释放互斥量
			{
				//释放互斥量
				gMutex = 0;	

				//第二路开关设置低电平，
				GPIO_ResetBits(gPluse0.p_s2_gpio,gPluse0.p_s2_pin);			//断开				
			}			
			else if(gPluse0.p_bodcount == (PLUSE_MUTEX_DELAY+PLUSE_COUNT_HIGH+1)) //计算电流时间到
			{
				/* PE2置1 */
				//GPIOE->ODR |= GPIO_Pin_2;
				
				//计算电流，设置占空比
				//计算电流,此处代码优化
				
				uint16_t adc_V = vkADC_Read(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;
				for(uint8_t i=0; i<(PLUSE_COUNT_HIGH-1); i++)
				{
					adc_T += gPluse0.p_bodvalue[i];
				}
				adc_T = adc_T >>2;
				gPluse0.p_current = (uint16_t)((((1000*1.200f)/adc_V)*adc_T)/10.0f);
				//设置脉冲强度并启动PWM，开始充电，积聚电量				
				vkPluseRation(gPluse0.pwm, 50);
				//gPluse0.p_ration = vkPluseAdjust(gPluse0.pwm, gPluse0.p_expect, gPluse0.p_ration, gPluse0.p_current);	

				/* PE2置0 */
				//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);				
			}			
			else if(gPluse0.p_bodcount >= PLUSE_COUNT_LOW) //计数时间到
			{
				if(gMutex == 0)
				{
					//重置计数，改变电平状态
					gPluse0.p_bodcount = 0;
					gPluse0.p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse0.p_bodlevel == PLUSE_LEVEL_HIGH)	//200us
		{
			//占用互斥量
			gMutex = 1;							
			gPluse0.p_bodcount++;
			
			/* PE2置1 */
			//GPIOE->ODR |= GPIO_Pin_2;
			//第一个计数，高电平，开始放电
			if(gPluse0.p_bodcount == 1)
			{
				//关闭PWM，已经积聚了电量，开始放电							
				vkPWMStop(gPluse0.pwm);

				GPIO_SetBits(gPluse0.p_s3_gpio,gPluse0.p_s3_pin);	//接上		
				
				//第二路开关设置低电平，不接地
				//第二路开关设置低电平，保持3.3V
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//断开
				
				//第一路开关设置高电平，电极放电0.1ms
				GPIO_SetBits(gPluse0.p_s1_gpio,gPluse0.p_s1_pin); //接上
			}			
			else if(gPluse0.p_bodcount == PLUSE_COUNT_HIGH)	//计数时间到5*40us = 200us
			{
				/* 最后一次采集 */
				gPluse0.p_bodvalue[gPluse0.p_bodcount-2] = vkADC_Read(gPluse0.p_adcchnl);
				
				//重置计数，改变电平状态
				gPluse0.p_bodcount = 0;				
				gPluse0.p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse0.p_poqflags = 0;
			}
			//第一次不采ADC
			else
			{
				//采集ADC
				gPluse0.p_bodvalue[gPluse0.p_bodcount-2] = vkADC_Read(gPluse0.p_adcchnl);
			}

			/* PE2置0 */
			//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);
		}
	}

	//电极B持续时间
	if(gPluse1.p_poqlevel == PLUSE_LEVEL_HIGH)	
	{	
		if(gPluse1.p_bodlevel == PLUSE_LEVEL_LOW)	//30ms
		{	
			gPluse1.p_bodcount++;			
			
			if(gPluse1.p_bodcount == 1)	//第1个计数，低电平，开始充电			
			{
				//第二路开关设置高电平，接地，迅速拉低电极电压OV
				GPIO_SetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);		//接上

				GPIO_ResetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin);	//断开
				
				//第一路开关设置低电平，电极停止放电
				GPIO_ResetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin);	//断开	
			}			
			else if(gPluse1.p_bodcount == PLUSE_MUTEX_DELAY)	//延时 PLUSE_MUTEX_DELAY 释放互斥量
			{
				//释放互斥量
				gMutex = 0;	

				//第二路开关设置低电平，
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//断开				
			}			
			else if(gPluse1.p_bodcount == (PLUSE_MUTEX_DELAY+PLUSE_COUNT_HIGH+1)) //计算电流时间到
			{
				/* PE2置1 */
				//GPIOE->ODR |= GPIO_Pin_2;
				
				//计算电流，设置占空比
				//计算电流,此处代码优化
				
				uint16_t adc_V = vkADC_Read(ADC_Channel_Vrefint);
				uint16_t adc_T = 0;		
				for(uint8_t i=0; i<(PLUSE_COUNT_HIGH-1); i++)
				{
					adc_T += gPluse1.p_bodvalue[i];
				}
				adc_T = adc_T >>2;
				gPluse1.p_current = (uint16_t)((((1000*1.200f)/adc_V)*adc_T)/10.0f);
				//设置脉冲强度并启动PWM，开始充电，积聚电量				
				vkPluseRation(gPluse1.pwm, 50);
				//gPluse1.p_ration = vkPluseAdjust(gPluse1.pwm, gPluse1.p_expect, gPluse1.p_ration, gPluse0.p_current);	

				/* PE2置0 */
				//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2); 			
			}			
			else if(gPluse1.p_bodcount >= PLUSE_COUNT_LOW) //计数时间到
			{
				if(gMutex == 0)
				{
					//重置计数，改变电平状态
					gPluse1.p_bodcount = 0;
					gPluse1.p_bodlevel = PLUSE_LEVEL_HIGH;
				}
			}
		}
		else if(gPluse1.p_bodlevel == PLUSE_LEVEL_HIGH) //200us
		{
			//占用互斥量
			gMutex = 1;
			gPluse1.p_bodcount++;
			
			/* PE2置1 */
			//GPIOE->ODR |= GPIO_Pin_2;
			//第一个计数，高电平，开始放电
			if(gPluse1.p_bodcount == 1)
			{
				//关闭PWM，已经积聚了电量，开始放电							
				vkPWMStop(gPluse1.pwm);

				GPIO_SetBits(gPluse1.p_s3_gpio,gPluse1.p_s3_pin); 	//接上		
				
				//第二路开关设置低电平，不接地
				//第二路开关设置低电平，保持3.3V
				GPIO_ResetBits(gPluse1.p_s2_gpio,gPluse1.p_s2_pin);	//断开
				
				//第一路开关设置高电平，电极放电0.1ms
				GPIO_SetBits(gPluse1.p_s1_gpio,gPluse1.p_s1_pin); 	//接上
			}			
			else if(gPluse1.p_bodcount == PLUSE_COUNT_HIGH)		//计数时间到5*40us = 200us
			{
				/* 最后一次采集 */
				gPluse1.p_bodvalue[gPluse1.p_bodcount-2] = vkADC_Read(gPluse1.p_adcchnl);
								
				//重置计数，改变电平状态
				gPluse1.p_bodcount = 0; 			
				gPluse1.p_bodlevel = PLUSE_LEVEL_LOW;
				gPluse1.p_poqflags = 0;
			}
			else
			{
				//采集ADC
				gPluse1.p_bodvalue[gPluse1.p_bodcount-2] = vkADC_Read(gPluse1.p_adcchnl);
			}

			/* PE2置0 */
			//GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);
		}
	}
}



