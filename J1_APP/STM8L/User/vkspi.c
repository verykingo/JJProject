/******************************************************************************
 * 文件  ：vkspi.c
 * 描述    ：STM8L SPI
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

/* 包含系统头文件 */
#include "stm8l15x.h"
#include <stdio.h>

/* 包含自定义头文件 */
#include "vkspi.h"

/* 自定义新类型 */

/* 自定义宏 */
#define SPI_WAIT_TIMEOUT 	200		//发送接收超时
#define SPI_DUMMY_BYTE		0xAA	

/* SPI1接口引脚定义 */
#define SPI1_DEN_IO_PIN			GPIO_Pin_3
#define SPI1_CS_IO_PIN			GPIO_Pin_4
#define SPI1_SCK_IO_PIN			GPIO_Pin_5
#define SPI1_MOSI_IO_PIN		GPIO_Pin_6
#define SPI1_MISO_IO_PIN		GPIO_Pin_7
#define SPI1_IO_PORT			GPIOB

/* 全局变量定义 */

/* 预函数定义声明 */
uint8_t vkSPISendByte(vkSPI spi, uint8_t byte);
uint8_t vkSPIRecvByte(vkSPI spi);
int vkSPISetCS(vkSPI spi, uint8_t state);

/*******************************************************************************
 * 名称: vkSPI_Init
 * 功能: SPI接口初始化
 * 形参: 
 * 返回: 成功0，失败-1
 * 说明: SPI时钟频率16MHz/16 = 1MHz，数据传输速率(1/8)MB/s, 1个字节8us
 ******************************************************************************/
int vkSPI_Init(vkSPI spi)
{
	if(spi == SPI1)
	{
		printf("%s SPI1\r\n", __FUNCTION__);
		
	  	/* Enable SPI1 clock */
	  	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);

	 	/* Set the SCK(PB5), MOSI(PB6) and MISO(PB7) at high level */
	 	GPIO_ExternalPullUpConfig(SPI1_IO_PORT, SPI1_SCK_IO_PIN | SPI1_MOSI_IO_PIN | SPI1_MISO_IO_PIN, ENABLE);

	  	/* Configure CS as Output push-pull */
	  	GPIO_Init(SPI1_IO_PORT, SPI1_CS_IO_PIN, GPIO_Mode_Out_PP_High_Fast);

		/* Deselect the SPI Dev: Chip Select high */
		GPIO_SetBits(SPI1_IO_PORT, SPI1_CS_IO_PIN);

		SPI_DeInit(SPI1);
		
		/* SPI configuration */
 		SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2, SPI_Mode_Master, SPI_CPOL_High, SPI_CPHA_2Edge, SPI_Direction_2Lines_FullDuplex,SPI_NSS_Soft, 0x07);

		/* Enable SPI1 */
		SPI_Cmd(SPI1, ENABLE);		   
	}
	else if(spi == SPI2)
	{		
		printf("%s SPI2\r\n", __FUNCTION__);
		return -1;
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		
		return -1;
	}
	
	return 0;
}

/*******************************************************************************
* 名称: vkSPI_Deinit
* 功能: SPI接口初始化
* 形参: 
* 返回: 成功0，失败-1
* 说明: 
******************************************************************************/
int vkSPI_Deinit(vkSPI spi)
{
	if(spi == SPI1)
	{		
		printf("%s SPI1\r\n", __FUNCTION__);
		
	  	/* Disable SPI1 clock */
	  	CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, DISABLE);		
	}
	else if(spi == SPI2)
	{
		printf("%s SPI2\r\n", __FUNCTION__);
		
	  	/* Disable SPI2 clock */
	  	CLK_PeripheralClockConfig(CLK_Peripheral_SPI2, DISABLE);	
	}
	else
	{
		printf("%s Failed!\r\n", __FUNCTION__);
		return -1;
	}

	/* Deinit SPI */
	SPI_DeInit(spi);

	/* Disable SPI */
	SPI_Cmd(spi, DISABLE);	

	return 0;
}

/*******************************************************************************
* 名称: vkSPISetCS
* 功能: SPI设置片选信号
* 形参: 
* 返回: 成功0
* 说明: 
******************************************************************************/
inline int vkSPISetCS(vkSPI spi, uint8_t state)
{
	if(spi == SPI1)
	{
		(state == 0)?(GPIO_ResetBits(SPI1_IO_PORT, SPI1_CS_IO_PIN)):(GPIO_SetBits(SPI1_IO_PORT, SPI1_CS_IO_PIN));
	}
	else if(spi == SPI2)
	{
		return -1;
	}
	else
	{
		return -1;
	}

	return 0;
}

/*******************************************************************************
* 名称: vkSPISendByte
* 功能: SPI接口发送一个字节数据
* 形参: 发送数据byte
* 返回: 
* 说明: 
******************************************************************************/
inline uint8_t vkSPISendByte(vkSPI spi, uint8_t byte)
{
	uint16_t send_timeout = SPI_WAIT_TIMEOUT;
	uint16_t recv_timeout = SPI_WAIT_TIMEOUT;

	/* Loop while DR register in not emplty */
	while ((SPI_GetFlagStatus(spi, SPI_FLAG_TXE) == RESET) && (--send_timeout))
	{nop(); /* 防止被编译优化掉 */}

	/* Send time out, Return 0x00 */
	if(send_timeout == 0)
	{
		return 0x00;
	}

	/* Send byte through the SPI peripheral */
	SPI_SendData(spi, byte);

	/* Wait to receive a byte */
	while ((SPI_GetFlagStatus(spi, SPI_FLAG_RXNE) == RESET) && (--recv_timeout))
	{nop(); /* 防止被编译优化掉 */}

	/* Recv time out, Return 0x00 */
	if(recv_timeout == 0)
	{
		return 0x00;
	}

	/* Return the byte read from the SPI bus */
	return SPI_ReceiveData(spi);
}

/*******************************************************************************
* 名称: vkSPIRecvByte
* 功能: SPI接口接收一个字节数据
* 形参: 
* 返回: 
* 说明: 
******************************************************************************/
inline uint8_t vkSPIRecvByte(vkSPI spi)
{
 	return (vkSPISendByte(spi, (uint8_t)SPI_DUMMY_BYTE));
}

/*******************************************************************************
* 名称: vkSPISend
* 功能: SPI发送数据
* 形参: 
* 返回: 成功返回0，失败返回-1
* 说明: 
******************************************************************************/
int vkSPISend(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
	 if(data == NULL || length == 0)
	 {
		return -1;
	 }

	 CRITICAL_STORE;
	 
	 /* SPI transmit start 低电平开始传输数据 */
	vkSPISetCS(spi, 0);

	/* 进入临界区 */
	CRITICAL_START();

	/* 发送寄存器地址 */
	vkSPISendByte(spi, reg_addr);

	/* 发送数据 */
	for(uint8_t i = 0; i < length; i++)
	{
		vkSPISendByte(spi, data[i]);
	}

	/* 退出临界区 */
	CRITICAL_END();

	/* SPI transmit stop 高电平停止传输数据 */
	vkSPISetCS(spi, 1);
		
	return 0;
}

/*******************************************************************************
* 名称: vkSPIRecv
* 功能: SPI发送数据
* 形参: 
* 返回: 成功返回0，失败返回-1
* 说明: 
******************************************************************************/
int vkSPIRecv(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
	 if(data == NULL || length == 0)
	 {
		return -1;
	 }

	CRITICAL_STORE;
	 
	/* SPI transmit start 低电平开始传输数据 */
	vkSPISetCS(spi, 0);

	/* 进入临界区 */
	CRITICAL_START();

	/* 发送寄存器地址, 0x80 表示方向 */
	vkSPISendByte(spi, reg_addr|0x80);

	/* receive the rest of the bytes from the MPU */
	for(uint8_t i = 0; i < length; i++)
	{
		data[i] = vkSPISendByte(spi, SPI_DUMMY_BYTE); // Read the value
	}

	/* 退出临界区 */
	CRITICAL_END();

	/* SPI transmit stop 高电平停止传输数据 */
	vkSPISetCS(spi, 1);	

	return 0;
}

