#ifndef _TASK_HOLDER_H
#define _TASK_HOLDER_H

#include "makos_includes.h"


//#define USE_NEW_POS
#define USE_PID_POS

#define SOLDIER_LIMIT(IN,MIN,MAX)	(IN <= MIN ? MIN : (IN >= MAX ? MAX : IN))
#define SOLDIER_RPC_ZERO(IN,RANGE)	((IN < RANGE && IN > -RANGE) ? 0 : IN)
/**
  * @brief  电机类型结构体
  */
typedef enum
{
	GM3510,
	GM6020,
	RM6623,
	RM3510,
	RM6025,
}MotorType;

typedef enum
{
	Normal,
	Camera,
	Scope,
}Holder_Status_e;
typedef struct
{
	float position_target;
	float real_position;
	float expect_speed;
	float real_speed;
	float ouput;
}Holder_Control_e;
	
	//以下为输入值
extern float	holder_x_target,holder_y_target;	
extern S16 	holder_x_init,holder_y_init;
	//以下是IMU反馈量
extern S16	holder_x_mpu_fix,holder_y_mpu_fix;
extern S16 	holder_x_mpu,holder_y_mpu;
extern float	mag_x,mag_y,mag_z;
extern float angle;
	//以下是电机电调反馈量
extern float	holder_yaw_angle,holder_pitch_angle;
extern int16_t pitch_torque,yaw_torque;
extern int16_t pitch_speed,yaw_speed;
	//以下为输出量
extern int32_t yaw_output,pitch_ouput;
extern float pitch_ex_v,yaw_ex_v;
extern u8 Vision_Flag;

extern float Dir_angle,Real_Center;
extern int Dir_circle;
void	task_holder(void* param);
void 	task_holder_sendwave(void* param);

void	Holder_Motor_set_data_init(CanTxMsg* data);
U32		Holder_Motor_set_yaw_pitch(S16 yaw, S16 pitch);
U32		Holder_Motor_set_yaw(S16 yaw);
U32		Holder_Motor_set_pitch(S16 pitch);
S32 MyLimit(S32 in,S32 max);

#endif
