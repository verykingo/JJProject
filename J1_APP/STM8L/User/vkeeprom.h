
/******************************************************************************
 * 文件  ：vkeeprom.h
 * 描述    ：EEPROM
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#ifndef __VKEEPROM_H
#define __VKEEPROM_H

#include "stm8l15x.h"
#include "mpu6500.h"
#include "BltProtocol.h"

#define VKEEPROM_ADDRESS_MPU6500_BIAS 		FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0
#define VKEEPROM_ADDRESS_BLTPROTOCOL_CONF	FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0+sizeof(struct MPU6500_BIAS_GYRO_ACCEL) 

int vkEepromWrite(uint32_t addr, uint8_t *data, uint16_t len);
int vkEepromRead(uint32_t addr, uint8_t *data, uint16_t len);
int vkEepromClean(void);

#endif
