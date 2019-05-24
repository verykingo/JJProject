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
/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "spi_flash.h"


/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: SPI_FLASH_Init
 * 功能: SPI初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void SPI_FLASH_Init(void)
{
  	/* 初始化SPI */
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,\
		SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, \
		SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
	
	SPI_Cmd(ENABLE);	/* 使能SPI */
	
	/* 配置CS管脚 */
	GPIO_Init(SPI_CS , SPI_Pin_CS, GPIO_MODE_OUT_PP_HIGH_FAST);
	SPI_FLASH_CS_HIGH();		/* 不使能外部SPI设备 */
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_GetFlagStatus( SPI_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_SendData(byte);

  /* Wait to receive a byte */
  while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_ReceiveData();	 
 }

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(WREN);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(RDSR);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);

  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}
void W25X16_SPI_FLASH_WriteOneByte(u8* pBuffer,uint32_t nSector ,u8 nBytes)
{
  u8 i=0;
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(WRITE);

  /****************************************************************/
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((nSector & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((nSector & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(nSector & 0xFF);
 /*****************************************************************/
  for(i=0;i<nBytes;i++)
    SPI_FLASH_SendByte(pBuffer[i]);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "W25X16_PAGE_SIZE" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(WRITE);

  /****************************************************************/
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
 /*****************************************************************/

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % W25X16_PAGE_SIZE;
  count = W25X16_PAGE_SIZE - Addr;
  NumOfPage =  NumByteToWrite / W25X16_PAGE_SIZE;
  NumOfSingle = NumByteToWrite % W25X16_PAGE_SIZE;

  if (Addr == 0) /* WriteAddr is W25X16_PAGE_SIZE aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < W25X16_PAGE_SIZE */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > W25X16_PAGE_SIZE */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, W25X16_PAGE_SIZE);
        WriteAddr +=  W25X16_PAGE_SIZE;
        pBuffer += W25X16_PAGE_SIZE;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not W25X16_PAGE_SIZE aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < W25X16_PAGE_SIZE */
    {
      if (NumOfSingle > count)
      { /* (NumByteToWrite + WriteAddr) > W25X16_PAGE_SIZE */
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > W25X16_PAGE_SIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / W25X16_PAGE_SIZE;
      NumOfSingle = NumByteToWrite % W25X16_PAGE_SIZE;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, W25X16_PAGE_SIZE);
        WriteAddr +=  W25X16_PAGE_SIZE;
        pBuffer += W25X16_PAGE_SIZE;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(READ);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
u16 SPI_FLASH_ReadID(void)
{
  u16 Device_ID = 0;
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(0x90);
  
  SPI_FLASH_SendByte(0X00);
  SPI_FLASH_SendByte(0X00);
  SPI_FLASH_SendByte(0X00);
  /* Read a byte from the FLASH */
  Device_ID = (SPI_FLASH_SendByte(Dummy_Byte)<<8);
  Device_ID|=SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Device_ID;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(READ);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI1 peripheral */
  SPI_SendData(HalfWord);

  /* Wait to receive a Half Word */
  while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_ReceiveData();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  SectorAddr*=W25X16_SECTOR_SIZE;
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(SE);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BlockErase
* Description    : Erases the specified FLASH block.
* Input          : SectorAddr: address of the block to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BlockErase(u32 BlockAddr)
{
  BlockAddr*=W25X16_BLOCK_SIZE;
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Block Erase instruction */
  SPI_FLASH_SendByte(BE);
  /* Send BlockAddr high nibble address byte */
  SPI_FLASH_SendByte((BlockAddr & 0xFF0000) >> 16);
  /* Send BlockAddr medium nibble address byte */
  SPI_FLASH_SendByte((BlockAddr & 0xFF00) >> 8);
  /* Send BlockAddr low nibble address byte */
  SPI_FLASH_SendByte(BlockAddr & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}
/*******************************************************************************
* Function Name  : SPI_FLASH_ChipErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_ChipErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Chip Erase instruction  */
  SPI_FLASH_SendByte(CE);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}



/*************** (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/