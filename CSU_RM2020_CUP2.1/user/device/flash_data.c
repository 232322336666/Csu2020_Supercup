
#include "flash_data.h"

#include "stm32_flash.h"

U32	fd_version = 0x99990001;//must change this when _FD changed

_FD	FD;

void	FD_set(	U32 date,
		U32 car,
		U32 pcb,
		U32 s_id)
{
	_FD	buff;
	
	Flash_Read32BitDatas(ADDR_FLASH_SECTOR_9,  (sizeof(_FD)>>2)+1, (S32*)(&buff));
	
	FD.version	= fd_version;
	FD.date		= date;
	FD.car_id	= car;
	FD.pcb_type	= pcb;
	FD.s_id = s_id;
	
	if(	buff.version	!= FD.version	||
		buff.date	!= FD.date	||
		buff.car_id	!= FD.car_id	||
		buff.pcb_type	!= FD.pcb_type)//change this
	{
		Flash_EraseSector(FLASH_Sector_9);
		Flash_Write32BitDatas(ADDR_FLASH_SECTOR_9, (sizeof(_FD)>>2)+1, (S32*)(&FD));
	}
}

void	FD_check(void)
{
	Flash_Read32BitDatas(ADDR_FLASH_SECTOR_9, (sizeof(_FD)>>2)+1, (S32*)(&FD));
	if(FD.version != fd_version)
		while(1);
}

void	FD_print(void)//change this
{
	printf("flash data version: %d\r\n", FD.version & 0x0000ffff);
	
	printf("software download date: %d\r\n", FD.date);
	
	printf("car id: ");
	switch(FD.car_id)
	{
		case	CAR_ID_SOLDIER_MAIN:		printf("soldier");	break;
		case	CAR_ID_HERO_MAIN:		printf("hero");		break;
		case	CAR_ID_ENGINEER:	printf("engineer");	break;
		default:			printf("unknown");	break;
	}
	printf("\r\n");
	
	printf("pcb type: ");
	switch(FD.pcb_type)
	{
		case	PCB_RECTANGLE:		printf("rectangle");	break;
		case	PCB_SQUARE:		printf("square");	break;
		default:			printf("unknown");	break;
	}
	printf("\r\n");
}





