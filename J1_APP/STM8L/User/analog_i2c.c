
#include "analog_i2c.h"

/* I2C初始化 */  
void i2c_Init(void)
{
   	/* 初始化 */
   	I2C_SCL_INIT();
	I2C_SDA_INIT();

	/* 延时10ms */
	I2C_DELAYMS(10);	
}

 /* I2C起始信号 */	
 void i2c_Start(void)  
 {	
	/* 设置输出、电平拉高 */
	I2C_SCL_DIR(1);
	I2C_SDA_DIR(1);
	I2C_SCL_SET(1);
	I2C_SDA_SET(1);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* 起始信号，SCL拉高时，SDA由高拉低   */
	I2C_SDA_SET(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);	
 }

/* I2C终止信号 */  
void i2c_Stop(void)  
{ 
	/* 设置输出，SCL拉高，SDA拉低 */
	I2C_SCL_DIR(1);
	I2C_SDA_DIR(1);
	I2C_SCL_SET(1);
	I2C_SDA_SET(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* 终止信号，SCL拉高时，SDA由低拉高 */	
	I2C_SDA_SET(0);

	/* 延时 */
  	I2C_DELAYUS(10*I2C_DELAYTIME);

	/* 释放SDA和SCL，输入 */
	I2C_SCL_DIR(0);
	I2C_SDA_DIR(0);
} 

/* I2C读取ACK */  
uint8_t i2c_Read_Ack(void)
{
	/* SCL拉低 */
	I2C_SCL_SET(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* SDA输入 */
	I2C_SDA_DIR(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);
	
	/* SCL拉高 */
	I2C_SCL_SET(1);

	/* 读入SDA */
	uint8_t ret = I2C_SDA_GET();

	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL拉低 */
	I2C_SCL_SET(0);

	/* 延时 */
	I2C_DELAYUS(2*I2C_DELAYTIME);

	/* SDA输出 */
	I2C_SDA_DIR(1);

	return ret;
}

/*   
 * I2C发送ACK信号
*/  
void i2c_Send_Ack(void)
{
	/* SDA输出 */
	I2C_SDA_DIR(1);

	/* SCL和SDA拉低 */
	I2C_SCL_SET(0);
	I2C_SDA_SET(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL拉高 */
	I2C_SCL_SET(1);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL拉低 */
	I2C_SCL_SET(0);	
}

/*   
 * I2C发送NACK信号
*/  
void i2c_Send_NAck(void)
{
	/* SDA输出 */
	I2C_SDA_DIR(1);

	/* SCL拉低，SDA拉高 */
	I2C_SCL_SET(0);
	I2C_SDA_SET(1);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL拉高 */
	I2C_SCL_SET(1);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);

	/* SCL拉低 */
	I2C_SCL_SET(0);	
}

/* I2C字节写 */  
void i2c_Write_Byte(uint8_t data)
{
	/* SDA输出 */
	I2C_SDA_DIR(1);

	for(int i=0; i<8; i++)
	{
		/* SCL拉低 */
		I2C_SCL_SET(0);

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);

		/* 从高字节发送数据 */
		if(data & 0x80)
		{
			I2C_SDA_SET(1);			// SDA=1
		}
		else
		{
			I2C_SDA_SET(0);			// SDA=0
		}

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);	
		
		/* SCL拉高 */
		I2C_SCL_SET(1);

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);					//延时

		data = data<<1;			
	}	
}

/* I2C字节读 */  
uint8_t i2c_Read_Byte(void)
{
	uint8_t data = 0;
	
	/* SDA输入 */
	I2C_SDA_DIR(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME);

	for(int i=0; i<8; i++)
	{
		/* SCL拉低 */
		I2C_SCL_SET(0);

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);	
			
		/* 从高字节接收数据 */
		data |= (I2C_SDA_GET() << (7-i));  //从高位到低位依次准备数据进行读取

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);

		/* SCL拉高 */
		I2C_SCL_SET(1);

		/* 延时 */
		I2C_DELAYUS(I2C_DELAYTIME);
	}

	/* SCL拉低 */
	I2C_SCL_SET(0);

	/* 延时 */
	I2C_DELAYUS(I2C_DELAYTIME); 

	/* SDA输出 */
	I2C_SDA_DIR(1);
        
   	return data;
}

/* I2C写数据 */ 
int i2c_Write(uint8_t slave_addr, uint8_t *data, int length)
{
	int ret = 0;
	
	//发送地址和数据读写方向
	uint8_t WriteAddr = (uint8_t)(slave_addr<<1 | 0x00);

	I2C_ENTER_CRITICAL();

	/* I2C通信开始 */
	i2c_Start(); 

	/* 发送从设备地址 */
	i2c_Write_Byte(WriteAddr);
	if(i2c_Read_Ack()==0x01)
	{
		/* I2C通信结束 */
		i2c_Stop();  
		I2C_PRINTF("NACK ");
		ret = -1;
		goto EXIT;
	}

	/* 发送寄存地址和器数据 */
	for(int i=0;i<length;i++)
	{
		i2c_Write_Byte(data[i]);
		if(i2c_Read_Ack()==0x01)
		{
			/* I2C通信结束 */
			i2c_Stop();  
			I2C_PRINTF("NACK ");
			ret = -1;
			goto EXIT;
		}
		ret = i+1;
	}

	/* I2C通信结束 */
	i2c_Stop();

EXIT:
	I2C_EXIT_CRITICAL();

	return ret;
}

/* I2C读数据 */ 
int i2c_Read(uint8_t slave_addr, uint8_t *data, int length)
{
	int ret = 0;
	
	//发送地址和数据读写方向
	uint8_t ReadAddr = (uint8_t)(slave_addr<<1 | 0x01);

	I2C_ENTER_CRITICAL();
	
	/* I2C通信开始 */
	i2c_Start();  

	/* 发送从设备地址 */
	i2c_Write_Byte(ReadAddr);
	if(i2c_Read_Ack()==0x01)
	{
		/* I2C通信结束 */
		i2c_Stop();  
		I2C_PRINTF("NACK ");
		ret = -1;
		goto EXIT;
	}

	if(length == 1)
	{
		/* I2C读一次数据 */
		data[0] = i2c_Read_Byte();
		i2c_Send_NAck();

		ret = length;		
	}
	else
	{	
		/* I2C开始读数据 */
		for(int i=0;i<length-1;i++)
		{
			data[i] = i2c_Read_Byte();
			i2c_Send_Ack();		
		}

		/* I2C读最后一次数据 */
		data[length-1] = i2c_Read_Byte();
		i2c_Send_NAck();

		ret = length;
	}

	/* I2C通信结束 */
	i2c_Stop();
	
EXIT:
	I2C_EXIT_CRITICAL();
	
	return ret;
}

/* I2C扫描设备 */ 
int i2c_Scan(void)
{
	/* 初始化 */
	i2c_Init();

	/* 从0x00~0xFF 写人I2C从设备的地址，扫描从设备地址 */
	/* EMF32扩展板温度传感器地址0x41, 紫外传感器地址0x60 */
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

/* I2C查找设备 */ 
int i2c_Find(uint8_t slave_addr)
{
	/* 初始化 */
	i2c_Init();

	/* 从0x00~0xFF 写人I2C从设备的地址，扫描从设备地址 */
	/* EMF32扩展板温度传感器地址0x41, 紫外传感器地址0x60 */
	char buf = 0x55;
	I2C_PRINTF("I2C Find:");

	if(i2c_Write(slave_addr, &buf, 1) == 1)
	{
		I2C_PRINTF("0x%X\n", slave_addr);
		return 0;
	}
		
	return -1;	
}

