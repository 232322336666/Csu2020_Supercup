/*
	FILE NAME:	task_chassis.c
	DESCRIPTION:	contain the bassis work in chassis task, 
								1.get the message from remote or upper monit,and then drive the 
								motor by doing some pid and calman processing.
								2.modify some param of pid and return the speed of the head left motor if the upper monit
								gives some command.
								3.read the corect direction of the motor and their order from the flash.	
	EDIT RECORDS:
	---------
	AUTHOR:         
	EDIT TIME:	2017/11/21-14:57
	MODIFICATION: the bassis contents
	---------
	AUTHOR:		
	EDIT TIME:	
	MODIFICATION:	
	---------
*/
#ifndef _TASK_CHASSIS_H
#define _TASK_CHASSIS_H

#include "makos_includes.h"
#include "motor.h"

typedef enum
{
	SOLDIER_STOPPED,
	SOLDIER_FOLLOW,
	SOLDIER_NOHEAD,
	SOLDIER_SCOPE,
	SOLDIER_CATWALK
}chassis_mode_e;

typedef struct
{
	s16 front;
	s16 right;
	s16 clockwise;
}chassis_ctrl,*chassis_ctrl_p;

extern u8 holder_fixed_flag,super_cup_flag;
extern u16 super_cup_residue_V;
extern float cup_percent;

void	task_chassis(void* param);
void	task_chassis_control(void* param);

extern  s16 chassis_speed_to_holder;
extern 	s16 chassis_speed_to_holder_l;						
//********************************************************

#endif


