#include "task_vision.h"

float holder_old_yaw = 0,holder_old_pitch = 0;

s16 Camera_X_offset = 0,Camera_Y_offset = 0 ,Camera_X_KF,Camera_Y_KF,Camera_X_speed,Camera_Y_speed,Camera_Z_offset = 0;

u8 vs_get = 0,vs_connect = 0;

visual_send vs_send; 

float camera_deal_kx,camera_deal_ky;//像素点转换成电机码盘
float camera_deal_kz;		//采用y=kx+b的形式对子弹下坠等进行补偿
float camera_deal_kb = 0;
float camera_deal_kb_pitch = -20;
float camera_chasis_k;	//根据底盘垂直于枪管的速度进行补偿
float vision_speed_kx,vision_speed_ky;
float vision_x_speed,vision_y_speed;	//一阶微分获得的速度
S16 vision_xspeed_limit = 50,vision_yspeed_limit = 30;

struct HolderKalmanType vision_yaw_kalman,vision_yaw_kalman_speed,vision_yaw_kalman_tar;

float temp_x_target;
void pack_vs_info_send_init(visual_send robo2vs);

void	task_vision_get_old_data(void* param)
{
	struct GetOldDataType yaw_data_info;
	struct GetOldDataType pitch_data_info;
	int vision_delay_ms = 18;
	
	old_data_init(&yaw_data_info,vision_delay_ms);
	old_data_init(&pitch_data_info,vision_delay_ms);
	
//	pack_vs_info_send_init(vs_send);
	
	while(1)
	{
		put_data(&yaw_data_info,holder_yaw_angle);
		put_data(&pitch_data_info,holder_pitch_angle);
		
		holder_old_yaw = get_data(&yaw_data_info);
		holder_old_pitch = get_data(&pitch_data_info);
		
		task_delay_ms(1);
	}
}


void	task_vision_get_target(void* param)
{
	
}

void 	vision_data_init(void)
{
	camera_deal_kx = 0.062;camera_deal_ky = 2.2;	//将像素点对应到电机码盘
	camera_deal_kz = 0.0;camera_chasis_k = 0.01;
	vision_xspeed_limit = 50;vision_yspeed_limit = 10;
	vision_speed_kx = 50;vision_speed_ky = 5;
}

float kf_q1 = 0.15,kf_r1 = 1;
float kf_q2 = 0.1,kf_r2 = 5;
float v_base = 0;
u8 flag_pass1 = 0;

float get_x_tart_base(u8 flag_pass)
{
	float x_tar_base;
	static u8 data_count = 0;
	static float init_data = 0;
	
	temp_x_target = holder_old_yaw - (float)(Camera_X_offset * camera_deal_kx + camera_deal_kb);	
	
	if(data_count == 0)
	{
		init_data = temp_x_target;
		data_count++;
	}
	if(init_data)
		;
	holder_kalman_init(kf_q1,kf_r1,&vision_yaw_kalman);
	x_tar_base = holder_KalmanFilter_cal(temp_x_target,&vision_yaw_kalman);
	
	if(flag_pass)
		return x_tar_base;
	else
		return temp_x_target;
}

/*
	包含了对丢帧的处理，直接使用
*/
float get_speed(float x_now,float x_last)
{
	float	speed = 0;
	static float last_speed;
		 
	if(Target_Flag == 255 && Target_Flag_last == 255)
		speed = (x_now - x_last)*vision_speed_kx;
	else
		speed = last_speed * 0.5f;//衰减
	
	last_speed = speed;
	
//	v_base = speed;
	holder_kalman_init(kf_q2,kf_r2,&vision_yaw_kalman_speed);
	speed = holder_KalmanFilter_cal(speed,&vision_yaw_kalman_speed);
	
	return speed;
}

void pack_vs_info_send(visual_send robo2vs)    //robot -> PC
{
	u8 data_buff[5] = {0};
	
	data_buff[0] = 0x30;    //CMD
	data_buff[1] = 0xCF;    //~CMD
	data_buff[2] = (robo2vs.armor_type<<2)|(robo2vs.aim_or_rune<<3)|(robo2vs.enemy_color<<4)|(robo2vs.debug_flag<<5)|(robo2vs.set_flag<<6)|(robo2vs.init_flag<<7);
	//data_buff[2] = (robo2vs.armor_type<<2)|(robo2vs.aim_or_rune<<3)|(robo2vs.enemy_color<<4)|(robo2vs.debug_flag<<5)|(0<<6)|(0<<7);
	data_buff[3] = 0xCF;    //~CMD
	data_buff[4] = 0x30;    //CMD
	
  usart6_send_string(data_buff,5);
}

void robo2vs_send(u8 init,u8 set,u8 debug,u8 color,u8 aim_or_rune,u8 armor)
{
	vs_send.init_flag = init;
	vs_send.set_flag = set;
	vs_send.debug_flag = debug;
	vs_send.enemy_color = color;
	vs_send.aim_or_rune = aim_or_rune;
	vs_send.armor_type = armor;
	
	pack_vs_info_send(vs_send);
}

