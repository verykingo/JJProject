
#ifndef __ANALOG_I2C_H__
#define __ANALOG_I2C_H__
#include "stdio.h"

/*************** 硬件平台无关宏定义 ***************/
/* 时钟频率 */
#define I2C_FREQUENCY  	100000	
#define I2C_DELAYTIME  	((1000000/I2C_FREQUENCY)/3)

/* 硬件平台相关使能 */
#define USING_REALTEK_RTL8710BX 0u


/*************** 硬件平台相关宏定义 ***************/
#if USING_REALTEK_RTL8710BX==1u
#include "gpio_api.h"  		//gpio
#include "FreeRTOS.h"

/* SCL&SDA引脚类型变量 */
gpio_t 		rtl_gpio_i2c_scl;
gpio_t 		rtl_gpio_i2c_sda;

/* SCL&SDA引脚 */
#define I2C_SCL_PIN	PA_22	//PA_18
#define I2C_SDA_PIN	PA_19	//PA_23

/* SCL&SDA初始化 */
#define I2C_SCL_INIT()	gpio_init(&rtl_gpio_i2c_scl, I2C_SCL_PIN);gpio_mode(&rtl_gpio_i2c_scl, OpenDrain);gpio_dir(&rtl_gpio_i2c_scl, PIN_INPUT)
#define I2C_SDA_INIT()	gpio_init(&rtl_gpio_i2c_sda, I2C_SDA_PIN);gpio_mode(&rtl_gpio_i2c_sda, OpenDrain);gpio_dir(&rtl_gpio_i2c_sda, PIN_INPUT)

/* SCL&SDA设值，拉高x=1，拉低x=0 */
#define I2C_SCL_SET(x)	gpio_write(&rtl_gpio_i2c_scl, x)
#define I2C_SDA_SET(x)	gpio_write(&rtl_gpio_i2c_sda, x)

/* SCL&SDA取值，返回1拉高，返回0拉低 */
#define I2C_SCL_GET()	gpio_read(&rtl_gpio_i2c_scl)
#define I2C_SDA_GET()	gpio_read(&rtl_gpio_i2c_sda)

/* SCL&SDA方向，输出x=1，输入x=0 */
#define I2C_SCL_DIR(x)	gpio_dir(&rtl_gpio_i2c_scl, x)
#define I2C_SDA_DIR(x)	gpio_dir(&rtl_gpio_i2c_sda, x)

/* I2C微秒/毫秒延时 */
#define I2C_DELAYUS(x)	DelayUs(x)
#define I2C_DELAYMS(x)	DelayMs(x)

/* I2C临界区保护 */
#define I2C_ENTER_CRITICAL() taskENTER_CRITICAL()
#define I2C_EXIT_CRITICAL()  taskEXIT_CRITICAL()

/* I2C打印 */
#define I2C_PRINTF(Fmt, Arg...) printf(Fmt, ##Arg)
#else
/* SCL&SDA初始化 */
#define I2C_SCL_INIT()	
#define I2C_SDA_INIT()	

/* SCL&SDA设值，拉高x=1，拉低x=0 */
#define I2C_SCL_SET(x)	
#define I2C_SDA_SET(x)	

/* SCL&SDA取值，返回1拉高，返回0拉低 */
#define I2C_SCL_GET()	
#define I2C_SDA_GET()	

/* SCL&SDA方向，输出x=1，输入x=0 */
#define I2C_SCL_DIR(x)	
#define I2C_SDA_DIR(x)	

/* I2C微秒/毫秒延时 */
#define I2C_DELAYUS(x)	
#define I2C_DELAYMS(x)	

/* I2C临界区保护 */
#define I2C_ENTER_CRITICAL() 
#define I2C_EXIT_CRITICAL()  

/* I2C打印 */
#define I2C_PRINTF(Fmt, Arg...) printf(Fmt, ##Arg)

#endif


/* 模拟I2C接口函数 */
int i2c_Write(uint8_t slave_addr, uint8_t *data, int length);
int i2c_Read(uint8_t slave_addr, uint8_t *data, int length);
int i2c_Find(uint8_t slave_addr);
int i2c_Scan(void);


#endif


