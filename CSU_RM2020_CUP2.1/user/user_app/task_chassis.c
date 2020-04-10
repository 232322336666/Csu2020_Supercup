/**
  * @file 		task_chassic.c
  * @brief		Overall control of the chassis, site mode conversion.Based on the main control board and motor baseplate.The mode switch has not been resolved
  * @author		fu zhengnan
  * @date		    2019.2.21
  * @copyright    CSU RM. All rights reserved.
  */
#include "task_chassis.h"
#include "task_holder.h"
#include "task_remote.h"
#include "headfile.h"
#include "task_nimin.h"
#include "flash_data.h"
#include "judgement_info.h"
#include "motor.h"

int16_t chassis_V_T[4] = {0};//目标速度,依次对应HL,HR,BL,BR
int16_t chassis_V_C[4] = {0};//实际速度,依次对应HL,HR,BL,BR
int16_t current_C[4] = {0};//实际电流,依次对应HL,HR,BL,BR
int16_t current_T[4] = {0};//目标电流,依次对应HL,HR,BL,BR
int16_t CAN_data[4] = {0};//CAN发送数据,依次对应HL,HR,BL,BR

chassis_mode_e chassis_mode;
u8 holder_fixed_flag = 0;
u8 super_cup_flag=0;
u16 super_cup_residue_V;
float cup_percent;

s16 chassis_speed_to_holder = 0;
s16 chassis_speed_to_holder_l=0;

/**
  * @brief  波形发送函数.
  * @param  电机ID.
  * @note   山外上位机.
  * @retval 无.
  */
void send_speed_wave(u32 i)
{   
		//**************???????**************	
		#define CMD_WARE     3
	uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};   
	uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};
	usart2_send_string(cmdf, sizeof(cmdf));    
 	
	usart2_send_string((uint8_t *)&(chassis_V_T[i]), 2);    
	usart2_send_string((uint8_t *)&(chassis_V_C[i]), 2);  
	usart2_send_string((uint8_t *)&(current_T[i]), 2);
	usart2_send_string((uint8_t *)&(current_C[i]), 2);
	usart2_send_string((uint8_t *)&(CAN_data[i]), 2);
		
    usart2_send_string(cmdr, sizeof(cmdr));    
//******************************************
}

void task_chassis_wave(void* param)
{
	while(1)
	{  
		task_delay_ms(10);	
	}
}

void task_chassis_tranf(void* param)
{
	while(1)
	{
		//chassis_move_current();
		calculate_chassis_speed_to_holder();
		task_delay_ms(5);
	}
}

s32 W_chassis = 0;
void	task_chassis_power(void* param)
{
	u16 power_limit;//功率限制   W
	u16 limit_last;
	u32 i;
	
	u16 current[4];
	u16 chassis[4];
	if(FD.car_id==CAR_ID_SOLDIER_MAIN)
	{
		power_limit = 500;//功率限制   W
	}
	else if(FD.car_id==CAR_ID_HERO_MAIN)
	{
		power_limit = 500;//功率限制   W
	}
	else if(FD.car_id==CAR_ID_GUARD)
	{
		power_limit = 200;//功率限制   W
	}
	
    while(1)
	{
		cup_percent=(super_cup_residue_V-1400)/6;
		if(FD.car_id==CAR_ID_SOLDIER_MAIN||FD.car_id==CAR_ID_HERO_MAIN)
		{
			for(i = 0;i < 4; i++)
			{
				if(current_C[i]<0){current[i]=-current[i];}else {current[i]=current[i];}
				if(chassis_V_C[i]<0){chassis[i]=-chassis_V_C[i];}else {chassis[i]=chassis_V_C[i];}
			}			
			W_chassis=(current[0]*chassis[0]+current[1]*chassis[1]+current[2]*chassis[2]+current[3]*chassis[3])/40000;
			if(W_chassis>power_limit)
			{
				if(super_cup_flag==0)limit_last = 2000;
				if(super_cup_flag==1)limit_last = 4000;
			}
			if(W_chassis<power_limit)limit_last = 4000;
			for(i = 0;i < 4; i++)
			{
				Pid_speed[i].limit = limit_last;
			}
			task_delay_ms(10);
		}
		if(FD.car_id==CAR_ID_GUARD)
		{
			for(i = 0;i < 2; i++)
			{
				if(current_C[i]<0){current[i]=-current[i];}else {current[i]=current[i];}
				if(chassis_V_C[i]<0){chassis[i]=-chassis_V_C[i];}else {chassis[i]=chassis_V_C[i];}
			}	
			W_chassis=(current[0]*chassis[0]+current[1]*chassis[1])/10000;
			if(W_chassis>power_limit)limit_last = 500;
			if(W_chassis<power_limit)limit_last = 10000;
			for(i = 0;i < 4; i++)
			{
				Pid_speed[i].limit = limit_last;
			}
			task_delay_ms(5);
		}
	}
}	

void chassis_follow_deal(chassis_ctrl_p ctrl);
void chassis_nohead_deal(chassis_ctrl_p ctrl);
void chassis_scope_deal(chassis_ctrl_p ctrl);
void chassis_catwalk_deal(chassis_ctrl_p ctrl);
void chassis_send_can(void);

void	task_chassis_mode_deal(void* param)
{
	p_remote_data data = NULL;
	chassis_ctrl_p chassis_ctrl_data = mymalloc(SRAMIN,sizeof(chassis_ctrl));
	
	chassis_mode = SOLDIER_FOLLOW;
	
	while(1)
	{
		data=msg_get_read_some(&remote_msg);	//注意这里的读写消息函数必须是_some
		if(data != NULL)
		{
			chassis_ctrl_data->front = data->JL_UD -1024;
			chassis_ctrl_data->right = (s16)data->JL_LR - 1024;
			chassis_ctrl_data->clockwise = data->JR_LR - 1024;
			switch(data->SL)
			{
				case RP_S_UP:
					chassis_mode = SOLDIER_SCOPE;
					break;
				case RP_S_MID:
					chassis_mode = SOLDIER_FOLLOW;
					break;
				case RP_S_DOWN:
					chassis_mode = SOLDIER_STOPPED;
					break;
				default:
					chassis_mode = SOLDIER_STOPPED;
					break;
			}
			holder_fixed_flag = 0;
			switch(chassis_mode)
			{
				case SOLDIER_FOLLOW:
					chassis_follow_deal(chassis_ctrl_data);
					break;
				case SOLDIER_NOHEAD:
					chassis_nohead_deal(chassis_ctrl_data);
					break;
				case SOLDIER_SCOPE:
					holder_fixed_flag = 1;
					chassis_scope_deal(chassis_ctrl_data);
					break;
				case SOLDIER_CATWALK:
					holder_fixed_flag = 1;
					chassis_catwalk_deal(chassis_ctrl_data);
					break;	
				case SOLDIER_STOPPED:
					chassis_stop();
					break;
				default:
					break;
			}
			msg_read_finish_some(&remote_msg);
		}
		task_delay_ms(10);		
	}
}

void	task_chassis_control(void* param)
{
	PID_speed_Init(4,0.013 ,0,5000);
	PID_current_Init(0.8,0.4,0,20000);
	u8 i;
	static int tick_count = 0;
	
	task_insert(task_chassis_mode_deal,NULL,1);
	while(1)
	{
		tick_count ++;
		if(tick_count == 4)
		{
			tick_count = 0;
			for(i=0;i<4;i++)           
			{
				// SOLDIER_RPC_ZERO(chassis_V_T[i],30); 
				current_T[i] = (s16)PID_Update_Absolute2(&Pid_speed[i],(float)chassis_V_T[i],(float)chassis_V_C[i]);	
			}
		}
		for(i=0;i<4;i++)           
		{
			CAN_data[i] = (s16)PID_Update_Incre(&Pid_current[i],(float)current_T[i],(float)current_C[i]);
		}
		chassis_send_can();
		task_delay_ms(5);
	}
}

#define SCOPE_CLOCKWISE  		800
void chassis_follow_deal(chassis_ctrl_p data)
{
	s32 front,right,clockwise;
	float angle = -Dir_angle;

	front = -SOLDIER_LIMIT(data->front,-1024,1023);
	right = SOLDIER_LIMIT(data->right,-1024,1023);
	clockwise = SOLDIER_LIMIT(data->clockwise,-1024,1023);

	if(angle > 30)					
		clockwise = -910;	
	else if(angle < -30)				
		clockwise = 910;	
	else if((angle > -4) && (angle < 4))
		clockwise = 0;	
	else							
		clockwise = 0;
//		clockwise = ((angle > 0) ? -30 * angle - 10 : -30 * angle + 10);

	chassis_move_speed(front,right,clockwise);
}

void chassis_scope_deal(chassis_ctrl_p data)
{
	s32 front,right,clockwise,front_temp,right_temp;
	float angle_differ = -Dir_angle;

	front_temp = SOLDIER_LIMIT(data->front,-1024,1023);
	right_temp = SOLDIER_LIMIT(data->right,-1024,1023);

	front = -front_temp * cos(angle_differ * 3.14f / 180) + right_temp * sin(angle_differ * 3.14f / 180);
	right = front_temp * sin(angle_differ * 3.14f / 180) + right_temp * cos(angle_differ * 3.14f / 180);
	clockwise = SCOPE_CLOCKWISE;

	chassis_move_speed(front,right,clockwise);
}

void chassis_nohead_deal(chassis_ctrl_p data)
{
	s32 front,right,clockwise,front_temp,right_temp;
	float angle_differ = -Dir_angle;

	front_temp = SOLDIER_LIMIT(data->front,-1024,1023);
	right_temp = SOLDIER_LIMIT(data->right,-1024,1023);

	front = -front_temp * cos(angle_differ * 3.14f / 180) + right_temp * sin(angle_differ * 3.14f / 180);
	right = front_temp * sin(angle_differ * 3.14f / 180) + right_temp * cos(angle_differ * 3.14f / 180);
	clockwise = SOLDIER_LIMIT(data->clockwise,-1024,1023);

	chassis_move_speed(front,right,clockwise);
}

void chassis_catwalk_deal(chassis_ctrl_p ctrl)
{
//	s32 front,right,clockwise,front_temp,right_temp;
//	float angle_differ = -Dir_angle;

//	chassis_move_speed(front,right,clockwise);
}

void	task_chassis(void* param)
{
	;
}

