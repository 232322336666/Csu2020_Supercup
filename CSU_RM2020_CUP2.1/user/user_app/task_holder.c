/**
  ****************************(C)                 ****************************
  * @file       task_holder.c/h
  * @brief      云台控制任务应用层，完成pitch，yaw两个自由度的控制
  *				硬件平台为STM32F405RGT6
  * @note       针对不同机器人种类，采用不同的控制策略
					步兵：两个6020	 英雄：两个6623   无人机：两个3510  哨兵：3510+6020
				1. 使用陀螺仪做速度反馈时，切记配置MPU6050的MPU_LOW_Pass为0不然滤波延迟会导致控制比较难做
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     2019-1-26       L.Z.M           1.基本完成步兵云台控制（两个6020，暂时两个环都使用电调信息为反馈）
  *                                             2.基本完成3510俯仰控制（单独3510，位置环用电调角度，速度环用陀螺仪反馈）
  *	 V1.0.1		2019-3-3		L.Z.M			增加哨兵控制（6020偏航+3510俯仰）（陀螺仪的软件IIC暂时使用自己的）			
  *  
  *  V2.0.0		2019-9-11		L.Z.M			1.初步精简程序，此程序从此只用作步兵
  *												2.云台的yaw控制采用增量叠加
  *  V2.0.1		2019-9-12		L.Z.M			增加陀螺仪位置选择（云台或底盘）
  * @copyright    CSU RM. All rights reserved.
  ==============================================================================
  */
#include "task_holder.h"
#include "task_chassis.h"
#include "task_remote.h"
#include "task_nimin.h"
#include "holder_pid.h"
#include "stm32f4xx_gpio.h"
#include "usart.h"
#include "user_usart.h"
#include "UI.h"
#include "led.h"
#include "can.h"
#include "user_can.h"
#include "math.h"
#include "flash_data.h"
#include "stm32_flash.h"
#include "pos_ctrl.h"
#include "task_vision.h"

//*********特殊点宏定义**********///
//#define	S_holder_center	599		//枪管指向车头时yaw电机的编码器绝对位置holder_yaw_angle
//#define	SOLDIER1_PITCH_HIGH				31400		//PITCH电机限位参数
//#define SOLDIER1_PITCH_LOW				28600
//#define	S_holder_center	322		//枪管指向车头时yaw电机的编码器绝对位置holder_yaw_angle

#define	SOLDIER1_PITCH_HIGH				25000		//PITCH电机限位参数
#define SOLDIER1_PITCH_LOW				22000
#define SOLDIER1_PITCH_MID				24188
#define	SOLDIER2_PITCH_HIGH				25800		//PITCH电机限位参数
#define SOLDIER2_PITCH_LOW				22700
#define SOLDIER2_PITCH_MID				25050
	
//******************************//
CanTxMsg	Holder_Motor_set_data;

u8 Vision_Flag = 0,MyScope_Flag = 0;
u32		S_holder_center;//729  322
extern S16 camera_num;

float	holder_x_target,holder_x_target_last,holder_x_target2,holder_y_target,holder_scope_target,Motor_center,test_temp_y;	
float	holder_y_target,holder_y_target2,holder_y_target_last;
S16 	holder_x_init,holder_y_init;
	//以下是IMU反馈量
S16		holder_x_mpu_fix,holder_y_mpu_fix,holder_x_mpu,holder_y_mpu;
	//以下是电机电调反馈量
float	holder_yaw_angle,holder_pitch_angle,pitch_ex_v,yaw_ex_v;
S16 pitch_temperature,yaw_temperature;
int16_t pitch_torque,yaw_torque,pitch_speed,yaw_speed;
int32_t yaw_output,pitch_ouput;
U8 Soilder_ID;
int32_t yaw_output_temp,pitch_ouput_temp;
int32_t yaw_output_last,pitch_ouput_last;

U32	holder_print		= 0;
U32	holder_print_mark	= 0;


u8 Send_Flag = 1;//是否发送波形
u8 camera_biggest_num;

void	holder_control(U32 param)
{
	if(param <= 2)
		holder_print	= param;
}

#define	ALPHA	0.2
#define CMD_WAVE 3	//山外上位机对应的波形指令
/**
  * @brief  波形发送函数任务.
  * @param  none.
  * @note   对应山外上位机.
  * @retval 无.
  */
extern s16 yawt;
extern float yaw,Dir_angle_adj,spin_center;
extern u8 Target_Flag,holder_fixed_flag;
float angle_spin;
extern float x_tar_base,x_tar_base_last,y_tar_base,a_moment_x;

void 	task_holder_sendwave(void* param)
{
	uint8_t cmdf[2] = {CMD_WAVE, ~CMD_WAVE};   
	uint8_t cmdr[2] = {~CMD_WAVE, CMD_WAVE};
	S32 DataInfo[8];
	while(1)
	{
		//控制
//		DataInfo[0]	= (S32)(real_mpu_yaw);  
//		DataInfo[1] = (S32)can_yaw;
//		DataInfo[2] = (S32)can_gyro_f;
//		DataInfo[3]	= (S32)(holder_y_target);
//		DataInfo[4]	= (S32)holder_x_target;
//		DataInfo[5]	= (S32)(holder_yaw_angle);
//		DataInfo[6] = (S32)yaw_ex_v;
//		DataInfo[7]	= (S32)(yaw_speed);
		
		//视觉 - x
		DataInfo[0]	= (S32)(chassis_V_T[1]);  //像素偏移
		DataInfo[1] = (S32)chassis_V_C[1];			//原始模型值
		DataInfo[2] = (S32)current_T[1];		//视觉预测速度
		DataInfo[3]	= (S32)current_C[1];	//矫正模型值
		DataInfo[4]	= (S32)CAN_data[1];	//实际位置
		DataInfo[5]	= (S32)Dir_angle;
		DataInfo[6] = (S32)CAN_data[0];
		DataInfo[7]	= (S32)CAN_data[1];    //丢帧信号
		
//		//视觉 - y
//		DataInfo[0]	= (S16)(Camera_Y_offset);  //像素偏移
//		DataInfo[1] = (S16)y_tar_base;			//原始模型值
//		DataInfo[2] = (S16)vision_y_speed;		//视觉预测速度
//		DataInfo[3]	= (S16)(holder_y_target);	//矫正模型值
//		DataInfo[4]	= (S16)holder_pitch_angle;	//实际位置
//		DataInfo[5]	= (S16)(holder_pitch_angle);
//		DataInfo[6] = (S16)holder_y_target;
//		DataInfo[7]	= (S16)(Target_Flag);    //丢帧信号		
//						
		usart2_send_string(cmdf, sizeof(cmdf));    
		
		usart2_send_string((uint8_t *)&DataInfo[0],4);
		usart2_send_string((uint8_t *)&DataInfo[1],4);
		usart2_send_string((uint8_t *)&DataInfo[2],4);
		usart2_send_string((uint8_t *)&DataInfo[3],4);
		usart2_send_string((uint8_t *)&DataInfo[4],4);
		usart2_send_string((uint8_t *)&DataInfo[5],4);
		usart2_send_string((uint8_t *)&DataInfo[6],4);
		usart2_send_string((uint8_t *)&DataInfo[7],4);
		
		usart2_send_string(cmdr, sizeof(cmdr)); 
		
		task_delay_ms(10);
	}
}
/**
  * @brief  云台部分数据的初始化
  * @param  yaw，pitch方向的比例系数
  * @note   .
  * @retval none.
  */
void	holder_data_init(float* Remote_x,float* Remote_y)
{
	Holder_Motor_set_data_init(&Holder_Motor_set_data);

	Dir_angle_adj = 24;
	holder_y_init = SOLDIER1_PITCH_MID + 800;
	S_holder_center = 766;
	
	holder_x_init = S_holder_center;
	holder_y_target  = holder_y_init;
	holder_x_target  = holder_x_init;
	*Remote_y = -1.85;
	*Remote_x = -0.18;
}

/**
  * @brief  目标值保护，防止发生意外
  * @param  
  * @note   .
  * @retval none.
  */
void	holder_target_protect(void)
{
	if(Soilder_ID == 1)
		holder_y_target = SOLDIER_LIMIT(holder_y_target,SOLDIER1_PITCH_LOW,SOLDIER1_PITCH_HIGH);
	else
		holder_y_target = SOLDIER_LIMIT(holder_y_target,SOLDIER2_PITCH_LOW,SOLDIER2_PITCH_HIGH);
			
	if(holder_pitch_angle < 100)	//电机还没有供电
		holder_y_target = holder_pitch_angle;		//防止没供电的时候PID计算

	if(holder_x_target - holder_yaw_angle > 419)
		while(holder_x_target - holder_yaw_angle > 419)
			holder_x_target -= 819;
	else if	(holder_x_target - holder_yaw_angle < -419)
		while(holder_x_target - holder_yaw_angle < -419)
			holder_x_target += 819;
}

void	task_holder_control(void* param);
void	task_holder(void* param)
{
	float	 Remote_PY,Remote_PX;			//修正遥控器的范围到云台实际可以转动的范围
	p_remote_data data = NULL;
	p_holder_ctrl holder_ctrl_data = mymalloc(SRAMIN,sizeof(holder_ctrl_t));
	
	holder_data_init(&Remote_PX,&Remote_PY);
	
	task_delay_ms(2000);
	task_insert(task_holder_control, NULL, 1);
	float last_x_tar = holder_x_init;
	
	int scope_count = 1;	//为1代表需要更新小陀螺的中位
	while(1)
	{
		data = msg_get_read_some(&remote_msg);
		if(data && !Vision_Flag)
		{
			if(data->SL == RP_S_DOWN)		//表示是电脑控制状态
			{
				s32 temp_mouse_x = data->MX * 5.5f;
				s32 temp_mouse_y = data->MY * 3.2f;

				holder_ctrl_data->mouse_x = SOLDIER_LIMIT(temp_mouse_x,-1000,1000);
				holder_ctrl_data->mouse_y = SOLDIER_LIMIT(temp_mouse_y,-1000,1000);
			}
			else
			{
				s32 temp = data->JR_LR - 1024;
				holder_ctrl_data->mouse_x = SOLDIER_RPC_ZERO(temp,5);
				
				temp = (data->JR_UD - 1024) * 1.5f;
				holder_ctrl_data->mouse_y = SOLDIER_RPC_ZERO(temp,5);
			}
			
			if(!holder_fixed_flag)//非小陀螺模式下的读取目标值
			{
				scope_count = 1;
				MyScope_Flag = 0;
				holder_y_target = holder_y_target - 0.13f * holder_ctrl_data->mouse_y;
				holder_x_target = holder_x_target - 0.03f * holder_ctrl_data->mouse_x;
				
				//进行一次简单的低通滤波
				holder_x_target = 0.5f * holder_x_target + 0.5f * last_x_tar;
				last_x_tar = holder_x_target;
			}
			else
			{
				MyScope_Flag = 1;
				if(scope_count)
				{
					holder_scope_target = real_mpu_yaw;//用于保存进入小陀螺模式的初始角度
					scope_count = 0;
				}
				holder_y_target = holder_y_target - 0.13f * holder_ctrl_data->mouse_y;
				holder_scope_target = holder_scope_target + 0.015f * holder_ctrl_data->mouse_x;
				
				//进行一次简单的低通滤波
				holder_scope_target = 0.5f * holder_scope_target + 0.5f * last_x_tar;
				last_x_tar = holder_scope_target;
			}
			msg_read_finish_some(&remote_msg);
		}
		task_delay_ms(5);
	}
}

PID_AbsoluteType_holder	pid_holder_x_speed_0;
PID_IncrementType	pid_holder_y_speed;
PID_AbsoluteType_section	pid_holder_y_position_section;
PID_AbsoluteType_section	pid_holder_x_position_section;
pctrl_t yaw_pos;

void	task_holder_control(void* param)
{
	if(Send_Flag)
		task_insert((task_f)task_holder_sendwave,NULL,3);
	task_delay_ms(20);
//	static S32 Last_yaw_v = 0;
	
	holder_pid_init_absolute_section(&pid_holder_x_position_section, 32,32, 32, 0.05, 10, 0,\
																				20, 50, 100, 3, 160, 2800 );
	holder_pid_init_absolute_section(&pid_holder_y_position_section, 1.4, 1.15, 0.8 , 0.089, 0.3, 300,\
																				50, 120, 300, 10,30000 , 600 );
	holder_pid_init_absolute(&pid_holder_x_speed_0, 36,  0.36,  0,  0);		//绝对式
	holder_pid_init_increment(&pid_holder_y_speed, 16, 0.2, 0.0, 1200, 8000);
	
	pos_init(&yaw_pos);
	while(1)
	{			
		holder_target_protect();

		static int count_ticks = 0;
		count_ticks ++;		
		if(count_ticks == 5)		//位置环控制周期10ms,速度环2ms
		{ 
			count_ticks = 0;
#if defined(USE_PID_POS)
			if(!MyScope_Flag)
				yaw_ex_v = PID_Update_Yaw_index(holder_x_target,holder_yaw_angle,&pid_holder_x_position_section);
			else
				yaw_ex_v = -PID_Update_Yaw_index(holder_scope_target,real_mpu_yaw,&pid_holder_x_position_section);
					
//			yaw_ex_v = (float)(yaw_ex_v * 0.5f + Last_yaw_v * 0.5f);
//			Last_yaw_v = yaw_ex_v;
#endif
			pitch_ex_v = PID_Update_Yaw_index(holder_y_target, (float)holder_pitch_angle, &pid_holder_y_position_section);
		}

#if defined(USE_NEW_POS)
		if(!MyScope_Flag)
			yaw_ex_v = pctrl_out(&yaw_pos,holder_x_target,holder_yaw_angle);
		else
			yaw_ex_v = -pctrl_out(&yaw_pos,holder_scope_target,real_mpu_yaw);
//		yaw_ex_v = (float)(yaw_ex_v * 0.5f + Last_yaw_v * 0.5f);
//		Last_yaw_v = yaw_ex_v;
#endif
		pitch_ouput = (S32)PID_IncrementMode_Yaw((float)pitch_ex_v,pitch_speed, &pid_holder_y_speed);
		yaw_output = PID_Update_Yaw(yaw_ex_v,yaw_speed,&pid_holder_x_speed_0);			
		
		//输出的低通滤波
		pitch_ouput = pitch_ouput*ALPHA + pitch_ouput_last * (1-ALPHA);		
		yaw_output = yaw_output*ALPHA + yaw_output_last * (1-ALPHA);
		pitch_ouput_last = pitch_ouput;
		yaw_output_last = yaw_output;
		
//		yaw_output = 0;
		pitch_ouput = 0;    
		Holder_Motor_set_yaw_pitch(yaw_output , pitch_ouput);

		task_delay_ms(2);
	}
}


/**
  * @brief  云台电机CAN结构体初始化.
  * @param  结构体的地址
  * @note   
  * @retval void
  */
void	Holder_Motor_set_data_init(CanTxMsg* data)
{
        data->StdId	= 0x1ff;
        data->IDE	= CAN_ID_STD;
        data->RTR	= CAN_RTR_DATA;
        data->DLC	= 8;
        data->Data[0]	= 0x00;
        data->Data[1]	= 0x00;
        data->Data[2]	= 0x00;
        data->Data[3]	= 0x00;
        data->Data[4]	= 0x00;
        data->Data[5]	= 0x00;
        data->Data[6]	= 0x00;
        data->Data[7]	= 0x00;
}

/**
  * @brief  向电调ID为205和206的电机发送信息
  * @param  ID205信息，ID206信息
  * @note   注意两个信息的顺序
  * @retval 成功则返回1
  */
U32	Holder_Motor_set_yaw_pitch(S16 yaw, S16 pitch)
{
	pitch = SOLDIER_LIMIT(pitch,-32767,32767);
	yaw = SOLDIER_LIMIT(yaw,-32767,32767);
	Holder_Motor_set_data.Data[0] = (U8)(yaw>>8);
	Holder_Motor_set_data.Data[1] = (U8)(yaw&0xff);
	Holder_Motor_set_data.Data[2] = (U8)(pitch>>8);
	Holder_Motor_set_data.Data[3] = (U8)(pitch&0xff);
	
	CAN_Transmit(CAN2, &Holder_Motor_set_data);
	return	1;
}
