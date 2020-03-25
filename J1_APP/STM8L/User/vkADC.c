/******************************************************************************
 * 文件  ：vkADC.c
 * 描述    ：ADC
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include <stdio.h>

/* 包含自定义头文件 */
#include "vkADC.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: vkADC_Init
 * 功能: ADC初始化
 * 形参: 无
 * 返回: 成功返回0
 * 说明: 单步模式，12位，一次采样时间96Cycles=10us，启用内部参考电压
 ******************************************************************************/
int vkADC_Init(void)
{
	printf("%s Set ADC1 96Cycles\r\n", __FUNCTION__);
	
	/* Enable ADC CLK */
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

	/* Deinit ADC1 */
	ADC_DeInit(ADC1);
		
	/* ADC_ConversionMode_Continuous or ADC_ConversionMode_Single */
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_1);

	/* Configure Sample Time，16Cycles=5us, 96Cycles=10us,192Cycles=16us(40us影响波形),384Cycles=28us */	
	ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_96Cycles);
	ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_96Cycles);

	/* Disable DMA when Convert Mode is Single */
	ADC_DMACmd(ADC1, DISABLE);

	/* Enable Verf */
	ADC_VrefintCmd(ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	return 0;
}

/*******************************************************************************
 * 名称: vkADC_Deinit
 * 功能: ADC初始化操作
 * 形参: 
 * 返回: 返回0
 * 说明: 无 
 ******************************************************************************/
int vkADC_Deinit(void)
{
	/* Disable ADC CLK */
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, DISABLE);

	/* Deinit ADC1 */
	ADC_DeInit(ADC1);
	
	/* Disable Verf */
	ADC_VrefintCmd(DISABLE);

	/* Disable ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	return 0;
}

/*******************************************************************************
 * 名称: vkADCStart
 * 功能: ADC启动
 * 形参: 无
 * 返回: 成功返回0
 * 说明: 单步模式，12位，一次采样时间96Cycles=10us，启用内部参考电压
 ******************************************************************************/
int vkADCStart(void)
{
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	return 0;
}

/*******************************************************************************
 * 名称: vkADCStop
 * 功能: ADC关闭
 * 形参: 无
 * 返回: 成功返回0
 * 说明: 单步模式，12位，一次采样时间96Cycles=10us，启用内部参考电压
 ******************************************************************************/
int vkADCStop(void)
{
	/* Disable ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	return 0;
}

/*******************************************************************************
 * 名称: vkADCRead
 * 功能: 读取ADC值
 * 形参: ADC通道号channel
 * 返回: ADC值
 * 说明: 无 
 ******************************************************************************/
inline uint16_t vkADCRead(ADC_Channel_TypeDef channel)
{
	uint16_t value;
	
#if 1/* 减少函数调用花费的时间 */
	/* 1us = Enable ADC1 Channel used for IDD measurement */
	/* ADC_ChannelCmd(ADC1, ADC_Channel, ENABLE) */
	ADC1->SQR[(uint8_t)((uint16_t)channel >> 8)] |= (uint8_t)(channel);

	/* 240ns = Start ADC1 Conversion using Software trigger*/
	/* ADC_SoftwareStartConv(ADC1); */
	ADC1->CR1 |= ADC_CR1_START;

	/* 7us = Wait until ADC Channel end of conversion */
	/* while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) */
	while (!(ADC1->SR & ADC_FLAG_EOC))
	{nop(); /* 防止被编译优化掉 */}

	/* 400ns = Read ADC Convertion Result */
	/* ADC_GetConversionValue(ADC1) */
	value = (uint16_t)(ADC1->DRH);
	value = (uint16_t)((value << 8) | (uint16_t)(ADC1->DRL));

	/* 1.2us = Disable ADC1 Channel used for IDD measurement */
	/* ADC_ChannelCmd(ADC1, ADC_Channel, DISABLE) */
	ADC1->SQR[(uint8_t)((uint16_t)channel >> 8)] &= (uint8_t)(~(uint8_t)(channel));
#else
	/* Enable ADC1 Channel used for IDD measurement */
	ADC_ChannelCmd(ADC1, channel, ENABLE);
	
	/* Start ADC1 Conversion using Software trigger*/
	ADC_SoftwareStartConv(ADC1);	//clear by hardware

	/* Wait until ADC Channel 1 end of conversion */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
	{nop(); /* 防止被编译优化掉 */}
	
	/* Read ADC Convertion Result */
	value = ADC_GetConversionValue(ADC1);

	ADC_ChannelCmd(ADC1, channel, DISABLE);
#endif

	return value;	
}

