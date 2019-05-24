/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��adc.c
 * ����    ��adc2�����ļ�   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-11-18

  ���STM8������Ӳ������
    |-------------------------------|
    |  ����/���� - PF0(Adc2->CH10)  |
    |-------------------------------|

*******************************************************************************/

/* ����ϵͳͷ�ļ� */


/* �����Զ���ͷ�ļ� */
#include "adc.h"
#include "uart1.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */
static uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel);
static void Delay(u16 nCount);

/*******************************************************************************
 * ����: ADC_Init
 * ����: adc2��ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: ADC2_Init�������ββ���ʹ�û������ʼ����ͨ��ADC 
 ******************************************************************************/
void ADC_Init(void)
{
    /**< ����ת��ģʽ */
    /**< ʹ��ͨ�� */
    /**< ADCʱ�ӣ�fADC2 = fcpu/18 */
    /**< ���������˴�TIM TRGO ����ת������ʵ����û���õ���*/
    /**  ��ʹ�� ADC2_ExtTriggerState**/
    /**< ת�������Ҷ��� */
    /**< ��ʹ��ͨ��10��˹���ش����� */
    /**  ��ʹ��ͨ��10��˹���ش�����״̬ */
    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS , ADC2_CHANNEL_10, ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL10,DISABLE);
    ADC2_Cmd(ENABLE);
  
}

/**************************************************************************
 * ��������Send_ADC_Value
 * ����  ��ADCת�������ʾ����
 * ����  ��AD_Value--ADCת�����ֵ
 *
 * ���  ����
 * ����  ���� 
 * ����  ���ڲ����� 
 *************************************************************************/
void Send_ADC_Value(u16 AD_Value)
{
    UART1_SendByte(AD_Value/1000+0x30);
    UART1_SendByte(AD_Value%1000/100+0x30);
    UART1_SendByte(AD_Value%1000%100/10+0x30);
    UART1_SendByte(AD_Value%1000%100%10+0x30);
}
/**************************************************************************
 * ��������MultipleChannelGetADValue
 * ����  ����ӡADC��ͨ����ȡADֵ
 * ����  ����
 *
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ���� 
 *************************************************************************/
void MultipleChannelGetADValue(void)
{
 
    printf("\r\n��ǰAD Channel is ADC2_CHANNEL_9,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_9,ADC2_SCHMITTTRIG_CHANNEL9));
    Delay(0xffff);
    printf("\r\n��ǰAD Channel is ADC2_CHANNEL_10,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_10,ADC2_SCHMITTTRIG_CHANNEL10));
    Delay(0xffff);
    
}
/*******************************************************************************
 * ����: OneChannelGetADValue
 * ����: ADC2��ͨ��ѡ���ȡADֵ
 * �β�: ADC2_Channel,ADC2_SchmittTriggerChannel
 * ����: ��ͨ����ADֵ
 * ˵��: ��Ҫ��ȡ��ͨ��ADֵ�ɵ��øú���������ֻ�ǳ�ʼ��ADC_Init���� 
 ******************************************************************************/
static uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,\
                                     ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel)
{
    uint16_t ADConversion_Value;
    /**< ����ת��ģʽ */
    /**< ʹ��ͨ�� */
    /**< ADCʱ�ӣ�fADC2 = fcpu/18 */
    /**< ���������˴�TIM TRGO ����ת������ʵ����û���õ���*/
    /**  ��ʹ�� ADC2_ExtTriggerState**/
    /**< ת�������Ҷ��� */
    /**< ��ʹ��ͨ��10��˹���ش����� */
    /**  ��ʹ��ͨ��10��˹���ش�����״̬ */
    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS , ADC2_Channel, ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SchmittTriggerChannel,DISABLE);
    ADC2_Cmd(ENABLE);
    ADC2_StartConversion();
    ADConversion_Value = ADC2_GetConversionValue();
    return ADConversion_Value;
}


static void Delay(u16 nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}