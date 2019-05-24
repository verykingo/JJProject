/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：uart1.c
 * 描述    ：UART1设置文件   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2012-12-8

*******************************************************************************/

/* 包含系统头文件 */


/* 包含自定义头文件 */
#include "stm8s.h"
#include "uart3.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: Uart_Init
 * 功能: UART1初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Uart3_Init(void)
{
    UART3_DeInit();
	
	/*
	 * 将UART1配置为：
	 * 波特率 = 115200
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 * 使能接收中断
	 */
	
    UART3_Init((u32)115200, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, \
    UART3_PARITY_NO , UART3_MODE_TXRX_ENABLE);
    UART3_ITConfig(UART3_IT_RXNE_OR,ENABLE  );
    UART3_Cmd(ENABLE);
  
}

/*******************************************************************************
 * 名称: UART3_SendByte
 * 功能: UART3发送一个字节
 * 形参: data -> 要发送的字节
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART3_SendByte(u8 data)
{
	UART3_SendData8((unsigned char)data);
	
	/* Loop until the end of transmission */
	while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
}

/*******************************************************************************
 * 名称: UART3_SendString
 * 功能: UART3发送字节串
 * 形参: data -> 要发送的字节
 * 		 len -> 字节长度
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART3_SendString(u8* Data,u16 len)
{
	u16 i=0;
	
	for(;i<len;i++)
		UART3_SendByte(Data[i]);
}

/*******************************************************************************
 * 名称: UART3_ReceiveByte
 * 功能: UART3接收字节串
 * 形参: 无
 * 返回: 接收到的字符串
 * 说明: 无 
 ******************************************************************************/
u8 UART3_ReceiveByte(void)
{
	u8 USART3_RX_BUF; 
	
	while (UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET);
	
	USART3_RX_BUF=UART3_ReceiveData8();
	return  USART3_RX_BUF;
}


/*************** (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/