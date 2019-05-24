/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：adc.c
 * 描述    ：adc2设置文件   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-11-18

  风驰STM8开发板硬件连接
    |-------------------------------|
    |  光敏/热敏 - PF0(Adc2->CH10)  |
    |-------------------------------|

*******************************************************************************/

/* 包含系统头文件 */


/* 包含自定义头文件 */
#include "adc.h"
#include "uart1.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */
static uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel);
static void Delay(u16 nCount);

/*******************************************************************************
 * 名称: ADC_Init
 * 功能: adc2初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: ADC2_Init函数的形参不能使用或运算初始化多通道ADC 
 ******************************************************************************/
void ADC_Init(void)
{
    /**< 连续转换模式 */
    /**< 使能通道 */
    /**< ADC时钟：fADC2 = fcpu/18 */
    /**< 这里设置了从TIM TRGO 启动转换，但实际是没有用到的*/
    /**  不使能 ADC2_ExtTriggerState**/
    /**< 转换数据右对齐 */
    /**< 不使能通道10的斯密特触发器 */
    /**  不使能通道10的斯密特触发器状态 */
    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS , ADC2_CHANNEL_10, ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL10,DISABLE);
    ADC2_Cmd(ENABLE);
  
}

/**************************************************************************
 * 函数名：Send_ADC_Value
 * 描述  ：ADC转换结果显示函数
 * 输入  ：AD_Value--ADC转换结果值
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：内部调用 
 *************************************************************************/
void Send_ADC_Value(u16 AD_Value)
{
    UART1_SendByte(AD_Value/1000+0x30);
    UART1_SendByte(AD_Value%1000/100+0x30);
    UART1_SendByte(AD_Value%1000%100/10+0x30);
    UART1_SendByte(AD_Value%1000%100%10+0x30);
}
/**************************************************************************
 * 函数名：MultipleChannelGetADValue
 * 描述  ：打印ADC多通道读取AD值
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void MultipleChannelGetADValue(void)
{
 
    printf("\r\n当前AD Channel is ADC2_CHANNEL_9,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_9,ADC2_SCHMITTTRIG_CHANNEL9));
    Delay(0xffff);
    printf("\r\n当前AD Channel is ADC2_CHANNEL_10,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_10,ADC2_SCHMITTTRIG_CHANNEL10));
    Delay(0xffff);
    
}
/*******************************************************************************
 * 名称: OneChannelGetADValue
 * 功能: ADC2单通道选择读取AD值
 * 形参: ADC2_Channel,ADC2_SchmittTriggerChannel
 * 返回: 该通道的AD值
 * 说明: 当要读取多通道AD值采调用该函数，否则只是初始化ADC_Init即可 
 ******************************************************************************/
static uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,\
                                     ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel)
{
    uint16_t ADConversion_Value;
    /**< 连续转换模式 */
    /**< 使能通道 */
    /**< ADC时钟：fADC2 = fcpu/18 */
    /**< 这里设置了从TIM TRGO 启动转换，但实际是没有用到的*/
    /**  不使能 ADC2_ExtTriggerState**/
    /**< 转换数据右对齐 */
    /**< 不使能通道10的斯密特触发器 */
    /**  不使能通道10的斯密特触发器状态 */
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