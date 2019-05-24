/*************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ********************
 * 文件名  ：spi_flash.c
 * 描述    ：SPI总线配置读写函数库  
 * 实验平台：iCreate STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 *修改时间 ：2012-11-20
  iCreate STM8开发板硬件连接
    |--------------------|
    |  SPI_CS---PE6      |
    |  SPI_MISO-PC7      |
    |  SPI_MOSI-PC6      |
    |  SPI_CLK--PC5      |
    |--------------------|
*******************************************************************************/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H


/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "stm8s.h"

/* 自定义新类型 */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* 自定义宏 */
#define SPI_CS                   GPIOE
#define SPI_Pin_CS               GPIO_PIN_6
#define W25X16_PAGE_SIZE         256
#define W25X16_SECTOR_SIZE	 4096
#define W25X16_SECTOR_COUNT	 512
#define W25X16_BLOCK_SIZE	 65536
#define W25X16_PAGES_PER_SECTOR	W25X16_SECTOR_SIZE/W25X16_PAGE_SIZE
#define WRITE      0x02  /* Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0x20  /*Sector Erase instruction*/
#define BE         0xD8  /* Block Erase instruction */
#define CE         0xC7  /* Bulk Erase instruction */

#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xFF

#define  FLASH_WriteAddress     18*W25X16_BLOCK_SIZE
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress

/* 使能SPI外部设备，拉低CS引脚电平 */
#define SPI_FLASH_CS_LOW()       GPIO_WriteLow(SPI_CS, SPI_Pin_CS)
/* 禁止SPI外部设备，拉高CS引脚电平 */
#define SPI_FLASH_CS_HIGH()      GPIO_WriteHigh(SPI_CS, SPI_Pin_CS)


/* 全局变量定义 */


/*******************************************************************************
 * 名称: SPI_FLASH_Init
 * 功能: SPI初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void SPI_FLASH_Init(void);

u8 SPI_FLASH_SendByte(u8 byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u16 SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
u8 SPI_FLASH_ReadByte(void);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BlockErase(u32 BlockAddr);
void SPI_FLASH_ChipErase(void);
void W25X16_SPI_FLASH_WriteOneByte(u8* pBuffer,uint32_t nSector ,u8 nBytes);

#endif

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/