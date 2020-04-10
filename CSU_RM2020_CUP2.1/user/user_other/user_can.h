#ifndef _USER_CAN_H
#define _USER_CAN_H

#include "makos_includes.h"
#include "timer.h"

extern u8	can_101_flag;
extern U16 Driver_ID_HL;
extern U16 Driver_ID_HR;
extern U16 Driver_ID_BL;
extern U16 Driver_ID_BR;

volatile extern int16_t V_C_HL,V_C_HR,V_C_BL,V_C_BR;
volatile extern int16_t angle_HL,angle_HR,angle_BL,angle_BR;//机械角度

extern
S16	shoot_speed;

extern
volatile int16_t strech_speed;
//单轴陀螺仪数据
extern int16_t can_gyro,can_yaw;
extern float can_gyro_f,can_yaw_f,real_mpu_yaw;

typedef	struct	_mcircle_t
{
	S32	circle;
	U32	angle;
}mcircle_t, *p_mcircle;

extern
mcircle_t	motor_circle[2][8];
extern mcircle_t can_mpu_yaw;

S32	can_get_circle(U32 can, U32 id);

#endif

