#ifndef _MOTOR_H
#define _MOTOR_H

#include "makos_type.h"
#include "pid.h"
#include "can.h"
#include "user_can.h"

extern U16 Driver_ID[4];
extern int16_t chassis_V_T[4];
extern int16_t chassis_V_C[4];
extern int16_t caterpillar_V_T[2];
extern int16_t CAN_data[4];//CAN发送电流
extern s16 current_T[4];
extern s16 current_C[4];

//***********************************************************************

void chassis_move_speed(S32	front,S32	right,S32	clockwise);//底盘电机移动
void chassis_stop(void);//底盘电机停止
void chassis_send_can(void);
void calculate_chassis_speed_to_holder(void);

	
#endif
