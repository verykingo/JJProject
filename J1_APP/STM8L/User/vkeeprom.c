
/******************************************************************************
 * 文件  ：vkeeprom.h
 * 描述    ：EEPROM操作函数，大小2KB
 * 平台    ：STM8L
 * 时间  ：2019-04-01

*******************************************************************************/

#include <stdio.h>
#include "stm8l15x.h"
#include "vkeeprom.h"

/*
 * eeprom写函数
 */
int vkEepromWrite(uint32_t addr, uint8_t *data, uint16_t len)
{
	/* 参数检查 */
	if((addr == NULL) || (data == NULL) || (len == 0))
	{
		printf("%s Param Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* 地址空间检查　*/
	if((addr < FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS) || (addr+len > FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS))
	{	
		printf("%s Addr Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* 解锁 */
	FLASH_Unlock(FLASH_MemType_Data);

	/* 等待解锁结果 */
    while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
	{nop(); /* 防止被编译优化掉 */}

	/* 擦除数据 */
	for(int i=0; i<len; i++)
	{
		FLASH_EraseByte(addr+i);
	}

	/* 写入数据 */
	for(int i=0; i<len; i++)
	{
		FLASH_ProgramByte(addr+i, data[i]);
	}
		
	/* 加锁 */	
	FLASH_Lock(FLASH_MemType_Data);
	
	return len;
}


/*
 * eeprom读函数
 */
int vkEepromRead(uint32_t addr, uint8_t *data, uint16_t len)
{
	/* 参数检查 */
	if((addr == NULL) || (data == NULL) || (len == 0))
	{
		printf("%s Param Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* 地址空间检查　*/
	if((addr < FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS) || (addr+len > FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS))
	{	
		printf("%s Addr Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	for(int i=0; i<len; i++)
	{
		data[i] = FLASH_ReadByte(addr+i);
	}

	return len;
}

/*
 * eeprom清除函数2KB
 */
int vkEepromClean(void)
{
	/* 解锁 */
	FLASH_Unlock(FLASH_MemType_Data);

	/* 等待解锁结果 */
    while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
	{nop(); /* 防止被编译优化掉 */}

	/* 擦除数据 */
	for(uint32_t i=FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS; i<FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS; i++)
	{
		FLASH_EraseByte(i);
	}	

	/* 加锁 */	
	FLASH_Lock(FLASH_MemType_Data);

	return 0;
}

