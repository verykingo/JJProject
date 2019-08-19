
#include "analog_i2c.h"

/* I2C��ʼ�� */  
void i2c_Init(void)
{
   	/* ��ʼ�� */
   	I2C_SCL_INIT();
	I2C_SDA_INIT();

	/* ��ʱ10ms */
	I2C_DELAYMS(10);	
}

 /* I2C��ʼ�ź� */	
 void i2c_Start(void)  
 {	
	/* �����������ƽ���� */
	I2C_SCL_DIR(1);
	I2C_SDA_DIR(1);
	I2C_SCL_SET(1);
	I2C_SDA_SET(1);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* ��ʼ�źţ�SCL����ʱ��SDA�ɸ�����   */
	I2C_SDA_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);	
 }

/* I2C��ֹ�ź� */  
void i2c_Stop(void)  
{ 
	/* ���������SCL���ߣ�SDA���� */
	I2C_SCL_DIR(1);
	I2C_SDA_DIR(1);
	I2C_SCL_SET(1);
	I2C_SDA_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* ��ֹ�źţ�SCL����ʱ��SDA�ɵ����� */	
	I2C_SDA_SET(0);

	/* ��ʱ */
  	I2C_DELAYUS(10*I2C_DELAYTIME);

	/* �ͷ�SDA��SCL������ */
	I2C_SCL_DIR(0);
	I2C_SDA_DIR(0);
} 

/* I2C��ȡACK */  
uint8_t i2c_Read_Ack(void)
{
	/* SCL���� */
	I2C_SCL_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* SDA���� */
	I2C_SDA_DIR(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);
	
	/* SCL���� */
	I2C_SCL_SET(1);

	/* ����SDA */
	uint8_t ret = I2C_SDA_GET();

	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL���� */
	I2C_SCL_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(2*I2C_DELAYTIME);

	/* SDA��� */
	I2C_SDA_DIR(1);

	return ret;
}

/*   
 * I2C����ACK�ź�
*/  
void i2c_Send_Ack(void)
{
	/* SDA��� */
	I2C_SDA_DIR(1);

	/* SCL��SDA���� */
	I2C_SCL_SET(0);
	I2C_SDA_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL���� */
	I2C_SCL_SET(1);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL���� */
	I2C_SCL_SET(0);	
}

/*   
 * I2C����NACK�ź�
*/  
void i2c_Send_NAck(void)
{
	/* SDA��� */
	I2C_SDA_DIR(1);

	/* SCL���ͣ�SDA���� */
	I2C_SCL_SET(0);
	I2C_SDA_SET(1);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL���� */
	I2C_SCL_SET(1);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL���� */
	I2C_SCL_SET(0);	
}

/* I2C�ֽ�д */  
void i2c_Write_Byte(uint8_t data)
{
	/* SDA��� */
	I2C_SDA_DIR(1);

	for(int i=0; i<8; i++)
	{
		/* SCL���� */
		I2C_SCL_SET(0);

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);

		/* �Ӹ��ֽڷ������� */
		if(data & 0x80)
		{
			I2C_SDA_SET(1);			// SDA=1
		}
		else
		{
			I2C_SDA_SET(0);			// SDA=0
		}

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);	
		
		/* SCL���� */
		I2C_SCL_SET(1);

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);					//��ʱ

		data = data<<1;			
	}	
}

/* I2C�ֽڶ� */  
uint8_t i2c_Read_Byte(void)
{
	uint8_t data = 0;
	
	/* SDA���� */
	I2C_SDA_DIR(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME);

	for(int i=0; i<8; i++)
	{
		/* SCL���� */
		I2C_SCL_SET(0);

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);	
			
		/* �Ӹ��ֽڽ������� */
		data |= (I2C_SDA_GET() << (7-i));  //�Ӹ�λ����λ����׼�����ݽ��ж�ȡ

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);

		/* SCL���� */
		I2C_SCL_SET(1);

		/* ��ʱ */
		I2C_DELAYUS(I2C_DELAYTIME);
	}

	/* SCL���� */
	I2C_SCL_SET(0);

	/* ��ʱ */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* SDA��� */
	I2C_SDA_DIR(1);
        
   	return data;
}

/* I2Cд���� */ 
int i2c_Write(uint8_t slave_addr, uint8_t *data, int length)
{
	int ret = 0;
	
	//���͵�ַ�����ݶ�д����
	uint8_t WriteAddr = (uint8_t)(slave_addr<<1 | 0x00);

	I2C_ENTER_CRITICAL();

	/* I2Cͨ�ſ�ʼ */
	i2c_Start(); 

	/* ���ʹ��豸��ַ */
	i2c_Write_Byte(WriteAddr);
	if(i2c_Read_Ack()==0x01)
	{
		/* I2Cͨ�Ž��� */
		i2c_Stop();  
		I2C_PRINTF("NACK ");
		ret = -1;
		goto EXIT;
	}

	/* ���ͼĴ��ַ�������� */
	for(int i=0;i<length;i++)
	{
		i2c_Write_Byte(data[i]);
		if(i2c_Read_Ack()==0x01)
		{
			/* I2Cͨ�Ž��� */
			i2c_Stop();  
			I2C_PRINTF("NACK ");
			ret = -1;
			goto EXIT;
		}
		ret = i+1;
	}

	/* I2Cͨ�Ž��� */
	i2c_Stop();

EXIT:
	I2C_EXIT_CRITICAL();

	return ret;
}

/* I2C������ */ 
int i2c_Read(uint8_t slave_addr, uint8_t *data, int length)
{
	int ret = 0;
	
	//���͵�ַ�����ݶ�д����
	uint8_t ReadAddr = (uint8_t)(slave_addr<<1 | 0x01);

	I2C_ENTER_CRITICAL();
	
	/* I2Cͨ�ſ�ʼ */
	i2c_Start();  

	/* ���ʹ��豸��ַ */
	i2c_Write_Byte(ReadAddr);
	if(i2c_Read_Ack()==0x01)
	{
		/* I2Cͨ�Ž��� */
		i2c_Stop();  
		I2C_PRINTF("NACK ");
		ret = -1;
		goto EXIT;
	}

	if(length == 1)
	{
		/* I2C��һ������ */
		data[0] = i2c_Read_Byte();
		i2c_Send_NAck();

		ret = length;		
	}
	else
	{	
		/* I2C��ʼ������ */
		for(int i=0;i<length-1;i++)
		{
			data[i] = i2c_Read_Byte();
			i2c_Send_Ack();		
		}

		/* I2C�����һ������ */
		data[length-1] = i2c_Read_Byte();
		i2c_Send_NAck();

		ret = length;
	}

	/* I2Cͨ�Ž��� */
	i2c_Stop();
	
EXIT:
	I2C_EXIT_CRITICAL();
	
	return ret;
}

/* I2Cɨ���豸 */ 
int i2c_Scan(void)
{
	/* ��ʼ�� */
	i2c_Init();

	/* ��0x00~0xFF д��I2C���豸�ĵ�ַ��ɨ����豸��ַ */
	/* EMF32��չ���¶ȴ�������ַ0x41, ���⴫������ַ0x60 */
	char buf = 0x55;
	uint8_t addr = 0x00;
	I2C_PRINTF("I2C Scan:");
	for(int i=0; i<256; i++)
	{
		if(i2c_Write(addr, &buf, 1) == 1)
		{
			I2C_PRINTF("0x%X ", addr);			
		}
		
		addr=addr+1;

		DelayMs(10);	
	}
	I2C_PRINTF("\n");

	return 0;
}

/* I2C�����豸 */ 
int i2c_Find(uint8_t slave_addr)
{
	/* ��ʼ�� */
	i2c_Init();

	/* ��0x00~0xFF д��I2C���豸�ĵ�ַ��ɨ����豸��ַ */
	/* EMF32��չ���¶ȴ�������ַ0x41, ���⴫������ַ0x60 */
	char buf = 0x55;
	I2C_PRINTF("I2C Find:");

	if(i2c_Write(slave_addr, &buf, 1) == 1)
	{
		I2C_PRINTF("0x%X\n", slave_addr);
		return 0;
	}
		
	return -1;	
}

