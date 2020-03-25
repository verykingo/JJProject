/******************************************************************************
 * �ļ�  ��vkADC.c
 * ����    ��ADC
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include <stdio.h>

/* �����Զ���ͷ�ļ� */
#include "vkADC.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

/*******************************************************************************
 * ����: vkADC_Init
 * ����: ADC��ʼ��
 * �β�: ��
 * ����: �ɹ�����0
 * ˵��: ����ģʽ��12λ��һ�β���ʱ��96Cycles=10us�������ڲ��ο���ѹ
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

	/* Configure Sample Time��16Cycles=5us, 96Cycles=10us,192Cycles=16us(40usӰ�첨��),384Cycles=28us */	
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
 * ����: vkADC_Deinit
 * ����: ADC��ʼ������
 * �β�: 
 * ����: ����0
 * ˵��: �� 
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
 * ����: vkADCStart
 * ����: ADC����
 * �β�: ��
 * ����: �ɹ�����0
 * ˵��: ����ģʽ��12λ��һ�β���ʱ��96Cycles=10us�������ڲ��ο���ѹ
 ******************************************************************************/
int vkADCStart(void)
{
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	return 0;
}

/*******************************************************************************
 * ����: vkADCStop
 * ����: ADC�ر�
 * �β�: ��
 * ����: �ɹ�����0
 * ˵��: ����ģʽ��12λ��һ�β���ʱ��96Cycles=10us�������ڲ��ο���ѹ
 ******************************************************************************/
int vkADCStop(void)
{
	/* Disable ADC1 */
	ADC_Cmd(ADC1, DISABLE);
	
	return 0;
}

/*******************************************************************************
 * ����: vkADCRead
 * ����: ��ȡADCֵ
 * �β�: ADCͨ����channel
 * ����: ADCֵ
 * ˵��: �� 
 ******************************************************************************/
inline uint16_t vkADCRead(ADC_Channel_TypeDef channel)
{
	uint16_t value;
	
#if 1/* ���ٺ������û��ѵ�ʱ�� */
	/* 1us = Enable ADC1 Channel used for IDD measurement */
	/* ADC_ChannelCmd(ADC1, ADC_Channel, ENABLE) */
	ADC1->SQR[(uint8_t)((uint16_t)channel >> 8)] |= (uint8_t)(channel);

	/* 240ns = Start ADC1 Conversion using Software trigger*/
	/* ADC_SoftwareStartConv(ADC1); */
	ADC1->CR1 |= ADC_CR1_START;

	/* 7us = Wait until ADC Channel end of conversion */
	/* while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) */
	while (!(ADC1->SR & ADC_FLAG_EOC))
	{nop(); /* ��ֹ�������Ż��� */}

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
	{nop(); /* ��ֹ�������Ż��� */}
	
	/* Read ADC Convertion Result */
	value = ADC_GetConversionValue(ADC1);

	ADC_ChannelCmd(ADC1, channel, DISABLE);
#endif

	return value;	
}

