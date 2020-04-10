/*
	FILE NAME:	task_init.h
	DESCRIPTION:	
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/3/10
	MODIFICATION:	
	---------
	AUTHOR:		
	EDIT TIME:	
	MODIFICATION:	
	---------
*/

#ifndef _TASK_INIT_H
#define _TASK_INIT_H

#include "makos_includes.h"
#include "task_led.h"
#include "flash_data.h"


void	task_init(void* param);
void	task_nimin(U32 param);

void	init_test(void* param);

void	set_id_soldier(void);
void	set_id_hero(void);
void	set_id_engineer(void);



#endif
