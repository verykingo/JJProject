/******************************************************************************
 * 文件  vki2c_analog.h
 * 描述    ：模拟I2C
 * 平台    ：ALL
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKI2C_ANALOG_H__
#define __VKI2C_ANALOG_H__
#include "stdio.h"

/*************** 硬件平台无关宏定义 ***************/
/* 时钟频率 */
#define I2C_FREQUENCY  	100000	
#define I2C_DELAYTIME  	((1000000/I2C_FREQUENCY)/3)

/* 硬件平台相关使能 */
#define USING_REALTEK_RTL8710BX 0u
#define USING_STM8L15X 			1u

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

#elif USING_STM8L15X == 1u
#include "stm8l15x.h"

/* SCL&SDA引脚 */
#define I2C_SCL_GPIO GPIOE
#define I2C_SDA_GPIO GPIOE

#define I2C_SCL_PIN	GPIO_Pin_2	//PE_2
#define I2C_SDA_PIN	GPIO_Pin_3	//PE_3

/* SCL&SDA初始化 */
#define I2C_SCL_INIT()	GPIO_Init(I2C_SCL_GPIO, I2C_SCL_PIN, GPIO_Mode_Out_OD_HiZ_Fast)
#define I2C_SDA_INIT()	GPIO_Init(I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_Mode_Out_OD_HiZ_Fast)

/* SCL&SDA设值，拉高x=1，拉低x=0 */
#define I2C_SCL_SET(x)	(x==1)?GPIO_SetBits(I2C_SCL_GPIO, I2C_SCL_PIN):GPIO_ResetBits(I2C_SCL_GPIO, I2C_SCL_PIN)
#define I2C_SDA_SET(x)	(x==1)?GPIO_SetBits(I2C_SDA_GPIO, I2C_SDA_PIN):GPIO_ResetBits(I2C_SDA_GPIO, I2C_SDA_PIN)

/* SCL&SDA取值，返回1拉高，返回0拉低 */
#define I2C_SCL_GET()	(RESET==GPIO_ReadInputDataBit(I2C_SCL_GPIO, I2C_SCL_PIN))?0:1
#define I2C_SDA_GET()	(RESET==GPIO_ReadInputDataBit(I2C_SDA_GPIO, I2C_SDA_PIN))?0:1

/* SCL&SDA方向，输出x=1，输入x=0 */
#define I2C_SCL_DIR(x)	(x==1)?( I2C_SCL_GPIO->DDR |= I2C_SCL_PIN):(I2C_SCL_GPIO->DDR &= (uint8_t)(~(I2C_SCL_PIN)))
#define I2C_SDA_DIR(x)	(x==1)?( I2C_SDA_GPIO->DDR |= I2C_SDA_PIN):(I2C_SDA_GPIO->DDR &= (uint8_t)(~(I2C_SDA_PIN)))

/* I2C微秒/毫秒延时 */
#define I2C_DELAYUS(x)	nop();nop();nop();nop()
#define I2C_DELAYMS(x)	vkTimerDelayMS(x)

/* I2C临界区保护 */
#define I2C_ENTER_CRITICAL()  __istate_t _istate = __get_interrupt_state(); __disable_interrupt()
#define I2C_EXIT_CRITICAL()   __set_interrupt_state(_istate)

/* I2C打印 */
#define I2C_PRINTF(Fmt, Arg...) printf(Fmt, ##Arg)

#endif


/* 模拟I2C接口函数 */
int vkI2C_Init(void);
int vkI2C_Write(uint8_t slave_addr, uint8_t *data, int length);
int vkI2C_Read(uint8_t slave_addr, uint8_t *data, int length);
int vkI2C_Find(uint8_t slave_addr);
int vkI2C_Scan(void);


#endif


