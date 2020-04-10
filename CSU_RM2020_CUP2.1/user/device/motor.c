/**
  * @file 		motor.c
  * @brief		Site overall control and speed settlement and AD acquisition conversion.
  *             底盘的底层驱动文件，希望以后可以固定下来，保留底层驱动功能即可
  * @author		L.Z.M(重写的)
  * @date		  2019.9.27
  * @copyright    CSU RM. All rights reserved.
  */
#include "motor.h"
#include "can.h"
#include "user_can.h"
#include "stdlib.h"
#include "headfile.h"
#include "task_holder.h"
#include "task_chassis.h"
/*****************************************************************************/
//**********电机ID***********
U16 Driver_ID[4] = {0x201,0x202,0x203,0x204};
u8 speed_data[8];
//******************************************


/**
  * @brief  底盘速度计算函数.
  * @param  三方向矢量.
  * @note   无.
  * @retval 无.
  */
#define   	GAIN_I		0.1
#define		GAIN_J		0.1
#define		GAIN_K		0.2
void chassis_move_speed(S32	vx,S32	vy,S32	vr)
{			
	chassis_V_T[0]	= (int)(vx/GAIN_I - vy/GAIN_J + vr/GAIN_K);
	chassis_V_T[1]	= (int)(vx/GAIN_I + vy/GAIN_J + vr/GAIN_K);
	chassis_V_T[2]	= (int)(-vx/GAIN_I - vy/GAIN_J + vr/GAIN_K);
	chassis_V_T[3]	= (int)(-vx/GAIN_I + vy/GAIN_J + vr/GAIN_K);	  	
}

/**
  * @brief  CAN发送函数.
  * @param  无.
  * @note   无.
  * @retval 无.
  */
void chassis_send_can(void)
{
	CanTxMsg	canmsg;
	u32 i;
	
	for(i = 0; i < 4;i++)
	{
		speed_data[2*i] = (char)((CAN_data[i])>>8);
		speed_data[(2*i) + 1] = (char)(CAN_data[i]);
	}
	can_set(&canmsg, Control_ID,speed_data);
	CAN_Transmit(CAN2,&canmsg);	
}

void chassis_stop()
{
	chassis_move_speed(0 ,0 ,0 );
}

s16 front_c,clockwise_c,right_c;
void calculate_chassis_speed_to_holder()
{
	float holder_angle = Dir_angle;

	right_c=( chassis_V_C[0]+chassis_V_C[1]-chassis_V_C[2]-chassis_V_C[3])/4;
	front_c=(-chassis_V_C[0]+chassis_V_C[1]-chassis_V_C[2]+chassis_V_C[3])/4;
	clockwise_c=( chassis_V_C[0]+chassis_V_C[1]+chassis_V_C[2]+chassis_V_C[3])/4;
	right_c = SOLDIER_RPC_ZERO(right_c,50);
	front_c = SOLDIER_RPC_ZERO(front_c,80);
	clockwise_c = SOLDIER_RPC_ZERO(clockwise_c,50);
	holder_angle = -holder_angle/180*3.14f;	
	chassis_speed_to_holder=(front_c)*sin(holder_angle)+right_c*cos(holder_angle)+clockwise_c;
	
//	put_angle(chassis_speed_to_holder);
//	chassis_speed_to_holder_l=get_angle();
}
