#ifndef _FLASH_DATA_H
#define _FLASH_DATA_H

#include "makos_includes.h"

#define	CAR_ID_SOLDIER_MAIN		((U32)(0x12340001))
#define	CAR_ID_SOLDIER_HOLDER	((U32)(0x12340002))
#define	CAR_ID_HERO_MAIN		((U32)(0x12340003))
#define	CAR_ID_HERO_HOLDER		((U32)(0x12340004))
#define	CAR_ID_ENGINEER		((U32)(0x12340005))
#define	CAR_ID_GUARD		((U32)(0x12340006))

#define	PCB_SQUARE		((U32)(0x43210001))
#define	PCB_RECTANGLE		((U32)(0x43210002))


typedef	struct	__FD
{
	U32	version;
	U32	date;
	U32	car_id;
	U32	pcb_type;
	U32 s_id;
}_FD;

extern	_FD	FD;


void	FD_set(	U32 date,
		U32 car,
		U32 pcb,
		U32 s_id);

void	FD_check(void);

void	FD_print(void);





#endif
