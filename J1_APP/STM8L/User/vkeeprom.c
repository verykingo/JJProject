
/******************************************************************************
 * �ļ�  ��vkeeprom.h
 * ����    ��EEPROM������������С2KB
 * ƽ̨    ��STM8L
 * ʱ��  ��2019-04-01

*******************************************************************************/

#include <stdio.h>
#include "stm8l15x.h"
#include "vkeeprom.h"

/*
 * eepromд����
 */
int vkEepromWrite(uint32_t addr, uint8_t *data, uint16_t len)
{
	/* ������� */
	if((addr == NULL) || (data == NULL) || (len == 0))
	{
		printf("%s Param Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* ��ַ�ռ��顡*/
	if((addr < FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS) || (addr+len > FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS))
	{	
		printf("%s Addr Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* ���� */
	FLASH_Unlock(FLASH_MemType_Data);

	/* �ȴ�������� */
    while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
	{nop(); /* ��ֹ�������Ż��� */}

	/* �������� */
	for(int i=0; i<len; i++)
	{
		FLASH_EraseByte(addr+i);
	}

	/* д������ */
	for(int i=0; i<len; i++)
	{
		FLASH_ProgramByte(addr+i, data[i]);
	}
		
	/* ���� */	
	FLASH_Lock(FLASH_MemType_Data);
	
	return len;
}


/*
 * eeprom������
 */
int vkEepromRead(uint32_t addr, uint8_t *data, uint16_t len)
{
	/* ������� */
	if((addr == NULL) || (data == NULL) || (len == 0))
	{
		printf("%s Param Error!!!\r\n", __FUNCTION__);
		return -1;
	}

	/* ��ַ�ռ��顡*/
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
 * eeprom�������2KB
 */
int vkEepromClean(void)
{
	/* ���� */
	FLASH_Unlock(FLASH_MemType_Data);

	/* �ȴ�������� */
    while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
	{nop(); /* ��ֹ�������Ż��� */}

	/* �������� */
	for(uint32_t i=FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS; i<FLASH_DATA_EEPROM_END_PHYSICAL_ADDRESS; i++)
	{
		FLASH_EraseByte(i);
	}	

	/* ���� */	
	FLASH_Lock(FLASH_MemType_Data);

	return 0;
}

