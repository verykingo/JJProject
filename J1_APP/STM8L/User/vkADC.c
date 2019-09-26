/******************************************************************************
 * 文件  ：vkADC.c
 * 描述    ：ADC
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */

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
 * 名称: vkADC_Read
 * 功能: 读取ADC值
 * 形参: ADC通道号channel
 * 返回: ADC值
 * 说明: 无 
 ******************************************************************************/
inline uint16_t vkADC_Read(ADC_Channel_TypeDef channel)
{
	/* Enable ADC1 Channel used for IDD measurement */
	ADC_ChannelCmd(ADC1, channel, ENABLE);
	
	/* Start ADC1 Conversion using Software trigger*/
	ADC_SoftwareStartConv(ADC1);	//clear by hardware

	/* Wait until ADC Channel 1 end of conversion */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	
	/* Read ADC Convertion Result */
	uint16_t value = ADC_GetConversionValue(ADC1);

	ADC_ChannelCmd(ADC1, channel, DISABLE);

	return value;	
}

