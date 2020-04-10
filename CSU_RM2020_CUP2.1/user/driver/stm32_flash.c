#include "stm32_flash.h"
#include "stm32F4xx.h"

uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }
  return sector;
}
 
void Flash_EraseSector(uint16_t SectorNum)
{
	FLASH_Unlock(); 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	if (FLASH_EraseSector(SectorNum, VoltageRange_3) != FLASH_COMPLETE) while (1);
	FLASH_Lock(); 
}

/****************************************************************************
* ?    ?: ?????length?32???
* ????:address:??
length: ????
data_32:????????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Write32BitDatas(uint32_t address, uint16_t length, int32_t* data_32)
{
	uint16_t StartSector, EndSector,i;
	FLASH_Unlock(); //??FLASH????FLASH?????
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	StartSector = GetSector(address);//??FLASH?Sector??
	EndSector =GetSector(address+4*length);
	for (i = StartSector; i < EndSector; i += 8) //??FLASH????8,?????FLASH Sector????
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramWord(address, data_32[i]) == FLASH_COMPLETE)  //?DATA_32????????
		{
			address = address + 4;
		}
		else
		{
			while (1);
		}
	}
	FLASH_Lock();//?FLASH???FLASH???????
}

/****************************************************************************
* ?    ?: ?????length?32???
* ????:address:??
length: ????
data_32  ???????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Read32BitDatas(uint32_t address, uint16_t length, int32_t* data_32)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_32[i]=*(__IO int32_t*)address;
		address=address + 4;
	}
}

/****************************************************************************
* ?    ?: ?????length?16???
* ????:address:??
length: ????
data_16:????????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Write16BitDatas(uint32_t address, uint16_t length, int16_t* data_16)
{
	uint16_t StartSector, EndSector,i;
	FLASH_Unlock(); //??FLASH????FLASH?????
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	StartSector = GetSector(address);//??FLASH?Sector??
	EndSector = GetSector(address+2*length);
	for (i = StartSector; i < EndSector; i += 8)  //??FLASH????8,?????FLASH Sector????
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramHalfWord(address, data_16[i]) == FLASH_COMPLETE) 
		{ 
			address = address + 2;
		}
		else
		{
			while (1);
		}
	}
	FLASH_Lock();  //?FLASH???FLASH???????
}

/****************************************************************************
* ?    ?: ?????length?16???
* ????:address:??
length: ????
data_16  ???????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Read16BitDatas(uint32_t address, uint16_t length, int16_t* data_16)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_16[i]=*(__IO int16_t*)address;
		address=address + 2;
	}
}

/****************************************************************************
* ?    ?: ?????length?8???
* ????:address:??
length: ????
data_8:????????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Write8BitDatas(uint32_t address, uint16_t length, int8_t* data_8)
{
	uint16_t StartSector, EndSector,i;
	FLASH_Unlock(); //??FLASH????FLASH?????
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	StartSector = GetSector(address);   //??FLASH?Sector??
	EndSector = GetSector(address+length);
	for (i = StartSector; i < EndSector; i += 8)   //??FLASH????8,?????FLASH Sector????
	{
		if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE) while (1);
	}
	for(i=0; i<length; i++)
	{
		if (FLASH_ProgramByte(address, data_8[i]) == FLASH_COMPLETE) 	
		{
			address++;
		}
		else
		{ 
			while (1);	
		}
	}
	FLASH_Lock();  //?FLASH???FLASH???????
}

/****************************************************************************
* ?    ?: ?????length?8???
* ????:address:??
length: ????
data_8  ???????
* ????:?
* ?    ?:?
* ????:?
****************************************************************************/
void Flash_Read8BitDatas(uint32_t address, uint16_t length, int8_t* data_8)
{
	uint8_t i;
	for(i=0; i<length; i++)
	{
		data_8[i]=*(__IO int8_t*)address;
		address++;
	}
}
