/******************************************************************************
 * �ļ�  ��vkspi.c
 * ����    ��STM8L SPI
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

/* ����ϵͳͷ�ļ� */
#include "stm8l15x.h"
#include <stdio.h>

/* �����Զ���ͷ�ļ� */
#include "vkspi.h"

/* �Զ��������� */

/* �Զ���� */
#define SPI_WAIT_TIMEOUT 	200		//���ͽ��ճ�ʱ
#define SPI_DUMMY_BYTE		0xAA	

/* SPI1�ӿ����Ŷ��� */
#define SPI1_DEN_IO_PIN			GPIO_Pin_3
#define SPI1_CS_IO_PIN			GPIO_Pin_4
#define SPI1_SCK_IO_PIN			GPIO_Pin_5
#define SPI1_MOSI_IO_PIN		GPIO_Pin_6
#define SPI1_MISO_IO_PIN		GPIO_Pin_7
#define SPI1_IO_PORT			GPIOB

/* ȫ�ֱ������� */

/* Ԥ������������ */
uint8_t vkSPISendByte(vkSPI spi, uint8_t byte);
uint8_t vkSPIRecvByte(vkSPI spi);
int vkSPISetCS(vkSPI spi, uint8_t state);

/*******************************************************************************
 * ����: vkSPI_Init
 * ����: SPI�ӿڳ�ʼ��
 * �β�: 
 * ����: �ɹ�0��ʧ��-1
 * ˵��: SPIʱ��Ƶ��16MHz/16 = 1MHz�����ݴ�������(1/8)MB/s, 1���ֽ�8us
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
* ����: vkSPI_Deinit
* ����: SPI�ӿڳ�ʼ��
* �β�: 
* ����: �ɹ�0��ʧ��-1
* ˵��: 
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
* ����: vkSPISetCS
* ����: SPI����Ƭѡ�ź�
* �β�: 
* ����: �ɹ�0
* ˵��: 
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
* ����: vkSPISendByte
* ����: SPI�ӿڷ���һ���ֽ�����
* �β�: ��������byte
* ����: 
* ˵��: 
******************************************************************************/
inline uint8_t vkSPISendByte(vkSPI spi, uint8_t byte)
{
	uint16_t send_timeout = SPI_WAIT_TIMEOUT;
	uint16_t recv_timeout = SPI_WAIT_TIMEOUT;

	/* Loop while DR register in not emplty */
	while ((SPI_GetFlagStatus(spi, SPI_FLAG_TXE) == RESET) && (--send_timeout))
	{nop(); /* ��ֹ�������Ż��� */}

	/* Send time out, Return 0x00 */
	if(send_timeout == 0)
	{
		return 0x00;
	}

	/* Send byte through the SPI peripheral */
	SPI_SendData(spi, byte);

	/* Wait to receive a byte */
	while ((SPI_GetFlagStatus(spi, SPI_FLAG_RXNE) == RESET) && (--recv_timeout))
	{nop(); /* ��ֹ�������Ż��� */}

	/* Recv time out, Return 0x00 */
	if(recv_timeout == 0)
	{
		return 0x00;
	}

	/* Return the byte read from the SPI bus */
	return SPI_ReceiveData(spi);
}

/*******************************************************************************
* ����: vkSPIRecvByte
* ����: SPI�ӿڽ���һ���ֽ�����
* �β�: 
* ����: 
* ˵��: 
******************************************************************************/
inline uint8_t vkSPIRecvByte(vkSPI spi)
{
 	return (vkSPISendByte(spi, (uint8_t)SPI_DUMMY_BYTE));
}

/*******************************************************************************
* ����: vkSPISend
* ����: SPI��������
* �β�: 
* ����: �ɹ�����0��ʧ�ܷ���-1
* ˵��: 
******************************************************************************/
int vkSPISend(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
	 if(data == NULL || length == 0)
	 {
		return -1;
	 }

	 CRITICAL_STORE;
	 
	 /* SPI transmit start �͵�ƽ��ʼ�������� */
	vkSPISetCS(spi, 0);

	/* �����ٽ��� */
	CRITICAL_START();

	/* ���ͼĴ�����ַ */
	vkSPISendByte(spi, reg_addr);

	/* �������� */
	for(uint8_t i = 0; i < length; i++)
	{
		vkSPISendByte(spi, data[i]);
	}

	/* �˳��ٽ��� */
	CRITICAL_END();

	/* SPI transmit stop �ߵ�ƽֹͣ�������� */
	vkSPISetCS(spi, 1);
		
	return 0;
}

/*******************************************************************************
* ����: vkSPIRecv
* ����: SPI��������
* �β�: 
* ����: �ɹ�����0��ʧ�ܷ���-1
* ˵��: 
******************************************************************************/
int vkSPIRecv(vkSPI spi, uint8_t reg_addr, uint8_t length, uint8_t *data)
{
	 if(data == NULL || length == 0)
	 {
		return -1;
	 }

	CRITICAL_STORE;
	 
	/* SPI transmit start �͵�ƽ��ʼ�������� */
	vkSPISetCS(spi, 0);

	/* �����ٽ��� */
	CRITICAL_START();

	/* ���ͼĴ�����ַ, 0x80 ��ʾ���� */
	vkSPISendByte(spi, reg_addr|0x80);

	/* receive the rest of the bytes from the MPU */
	for(uint8_t i = 0; i < length; i++)
	{
		data[i] = vkSPISendByte(spi, SPI_DUMMY_BYTE); // Read the value
	}

	/* �˳��ٽ��� */
	CRITICAL_END();

	/* SPI transmit stop �ߵ�ƽֹͣ�������� */
	vkSPISetCS(spi, 1);	

	return 0;
}

