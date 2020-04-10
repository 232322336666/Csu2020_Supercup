#ifndef _TASK_VISION_H
#define _TASK_VISION_H

#include "makos_includes.h"
#include "task_remote.h"
#include "usart.h"
#include "user_usart.h"
#include "can.h"
#include "user_can.h"
#include "holder_pid.h"

typedef struct 
{
	u8 init_flag;          //init: 1
	u8 set_flag;      //set: 1
	u8 debug_flag;      //debug: 1
	u8 enemy_color;      //red: 1, blue: 0
	u8 aim_or_rune;      //rune: 1, aim: 0
	u8 armor_type;      //small:1 big:0
}visual_send,*p_visual_send;

extern visual_send vs_send; 
extern u8 vs_get,vs_connect;
extern u8 Target_Flag,Target_Flag_last;
extern s16 Camera_X_offset,Camera_Y_offset,Camera_X_KF,Camera_Y_KF,Camera_X_speed,Camera_Y_speed,Camera_Z_offset;
extern float vision_x_speed,vision_y_speed,holder_old_yaw,holder_old_pitch;
extern S16 vision_xspeed_limit,vision_yspeed_limit;

extern struct HolderKalmanType vision_yaw_kalman,vision_yaw_kalman_speed,vision_yaw_kalman_tar;

void	task_vision_get_old_data(void* param);
float get_x_tart_base(u8 flag_pass);
float get_speed(float x_now,float x_last);
void pack_vs_info_send_init(visual_send robo2vs);
void robo2vs_send(u8 init,u8 set,u8 debug,u8 color,u8 aim_or_rune,u8 armor);
void pack_vs_info_send(visual_send robo2vs);

#endif
