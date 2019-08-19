
#ifndef __ANALOG_I2C_H__
#define __ANALOG_I2C_H__
#include "stdio.h"

/*************** Ӳ��ƽ̨�޹غ궨�� ***************/
/* ʱ��Ƶ�� */
#define I2C_FREQUENCY  	100000	
#define I2C_DELAYTIME  	((1000000/I2C_FREQUENCY)/3)

/* Ӳ��ƽ̨���ʹ�� */
#define USING_REALTEK_RTL8710BX 0u


/*************** Ӳ��ƽ̨��غ궨�� ***************/
#if USING_REALTEK_RTL8710BX==1u
#include "gpio_api.h"  		//gpio
#include "FreeRTOS.h"

/* SCL&SDA�������ͱ��� */
gpio_t 		rtl_gpio_i2c_scl;
gpio_t 		rtl_gpio_i2c_sda;

/* SCL&SDA���� */
#define I2C_SCL_PIN	PA_22	//PA_18
#define I2C_SDA_PIN	PA_19	//PA_23

/* SCL&SDA��ʼ�� */
#define I2C_SCL_INIT()	gpio_init(&rtl_gpio_i2c_scl, I2C_SCL_PIN);gpio_mode(&rtl_gpio_i2c_scl, OpenDrain);gpio_dir(&rtl_gpio_i2c_scl, PIN_INPUT)
#define I2C_SDA_INIT()	gpio_init(&rtl_gpio_i2c_sda, I2C_SDA_PIN);gpio_mode(&rtl_gpio_i2c_sda, OpenDrain);gpio_dir(&rtl_gpio_i2c_sda, PIN_INPUT)

/* SCL&SDA��ֵ������x=1������x=0 */
#define I2C_SCL_SET(x)	gpio_write(&rtl_gpio_i2c_scl, x)
#define I2C_SDA_SET(x)	gpio_write(&rtl_gpio_i2c_sda, x)

/* SCL&SDAȡֵ������1���ߣ�����0���� */
#define I2C_SCL_GET()	gpio_read(&rtl_gpio_i2c_scl)
#define I2C_SDA_GET()	gpio_read(&rtl_gpio_i2c_sda)

/* SCL&SDA�������x=1������x=0 */
#define I2C_SCL_DIR(x)	gpio_dir(&rtl_gpio_i2c_scl, x)
#define I2C_SDA_DIR(x)	gpio_dir(&rtl_gpio_i2c_sda, x)

/* I2C΢��/������ʱ */
#define I2C_DELAYUS(x)	DelayUs(x)
#define I2C_DELAYMS(x)	DelayMs(x)

/* I2C�ٽ������� */
#define I2C_ENTER_CRITICAL() taskENTER_CRITICAL()
#define I2C_EXIT_CRITICAL()  taskEXIT_CRITICAL()

/* I2C��ӡ */
#define I2C_PRINTF(Fmt, Arg...) printf(Fmt, ##Arg)
#else
/* SCL&SDA��ʼ�� */
#define I2C_SCL_INIT()	
#define I2C_SDA_INIT()	

/* SCL&SDA��ֵ������x=1������x=0 */
#define I2C_SCL_SET(x)	
#define I2C_SDA_SET(x)	

/* SCL&SDAȡֵ������1���ߣ�����0���� */
#define I2C_SCL_GET()	
#define I2C_SDA_GET()	

/* SCL&SDA�������x=1������x=0 */
#define I2C_SCL_DIR(x)	
#define I2C_SDA_DIR(x)	

/* I2C΢��/������ʱ */
#define I2C_DELAYUS(x)	
#define I2C_DELAYMS(x)	

/* I2C�ٽ������� */
#define I2C_ENTER_CRITICAL() 
#define I2C_EXIT_CRITICAL()  

/* I2C��ӡ */
#define I2C_PRINTF(Fmt, Arg...) printf(Fmt, ##Arg)

#endif


/* ģ��I2C�ӿں��� */
int i2c_Write(uint8_t slave_addr, uint8_t *data, int length);
int i2c_Read(uint8_t slave_addr, uint8_t *data, int length);
int i2c_Find(uint8_t slave_addr);
int i2c_Scan(void);


#endif


