/******************************************************************************
 * �ļ�  ��vkADC.c
 * ����    ��ADC
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */

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
 * ����: vkADC_Read
 * ����: ��ȡADCֵ
 * �β�: ADCͨ����channel
 * ����: ADCֵ
 * ˵��: �� 
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

