#include "user_can.h"
#include "can.h"
#include "user_can.h"
#include "task_holder.h"
#include "motor.h"
#include "flash_data.h"
#include "timer.h"

void	CAN1_TX_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1, CAN_IT_TME) != RESET)
	{
		CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
		canrate.inc.can1_tx++;
	}
}

void	CAN2_TX_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN2,CAN_IT_TME) != RESET)
	{		
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
		canrate.inc.can2_tx++;
	}
}

#define	CIRCLE_FASTEST	1000

void	circle_count(p_mcircle pmc, U16 angle, S16 speed)
{
	if(	(angle < CIRCLE_FASTEST) && (pmc->angle > 8192-CIRCLE_FASTEST) && (speed > 0) )
	{
		pmc->circle++;
	}
	else if((angle > 8192-CIRCLE_FASTEST) && (pmc->angle < CIRCLE_FASTEST) && (speed < 0) )
	{
		pmc->circle--;
	}
	pmc->angle = angle;
}

mcircle_t	motor_circle[2][8] = {0};

void	circle_count_simple(p_mcircle pmc, U16 angle)
{
	if(	(angle < CIRCLE_FASTEST) && (pmc->angle > 8192-CIRCLE_FASTEST) )
	{
		pmc->circle++;
	}
	else if((angle > 8192-CIRCLE_FASTEST) && (pmc->angle < CIRCLE_FASTEST) )
	{
		pmc->circle--;
	}
	pmc->angle = angle;
}

mcircle_t	motor_circle_simple[2][8] = {0};

volatile int16_t V_C_HL,V_C_HR,V_C_BL,V_C_BR;//实际速度，C,current
volatile int16_t angle_HL,angle_HR,angle_BL,angle_BR;//机械角度

volatile int16_t supply_read_speed;
volatile U16 supply_read_angle;

S16	shoot_speed = 0;

volatile int16_t rise_speed[2];
volatile int16_t guard_move_speed[2];
volatile int16_t strech_speed;
extern int16_t pitch_torque;
extern int16_t yaw_torque;
extern int16_t pitch_speed;
extern int16_t yaw_speed;

volatile S16	liftf_read;
volatile S16	liftb_read;
s16 can_data_targt_power=1200;

void CAN1_RX0_IRQHandler(void)
{
	CanRxMsg	RxMessage;
        
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
		CAN_ClearFlag(CAN1, CAN_FLAG_FMP0);
		
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
		//htest_id	= data.StdId;
		//htest_angle	= (U32)(data.Data[0]<<8) | (data.Data[1]);
		
		if((RxMessage.StdId >= 0x201) && (RxMessage.StdId <= 0x208))
			circle_count(&(motor_circle[0][RxMessage.StdId-0x201]), RxMessage.Data[0]<<8|RxMessage.Data[1],\
						RxMessage.Data[2]<<8|RxMessage.Data[3]);
		if(RxMessage.StdId >= 0x201)
		 {	
			 
		 }
		 if(RxMessage.StdId >= 0x202)
		 {
			 
		 }
		
		if(RxMessage.StdId == 0x203)
		{	
			strech_speed = RxMessage.Data[2]<<8|RxMessage.Data[3];
		}
		if(RxMessage.StdId == 0x205)
		{
			supply_read_angle = RxMessage.Data[0]<<8|RxMessage.Data[1];
			supply_read_speed = RxMessage.Data[2]<<8|RxMessage.Data[3];
			
			//shoot_handle(supply_read_angle, supply_read_speed);
			shoot_speed = supply_read_speed;
		}
		if(RxMessage.StdId == 0x201)
			rise_speed[0] = (RxMessage.Data[2]<<8)|RxMessage.Data[3];
		if(RxMessage.StdId == 0x202)
			rise_speed[1] = (RxMessage.Data[2]<<8)|RxMessage.Data[3];
		if(RxMessage.StdId == 0x207)
			guard_move_speed[0] = (RxMessage.Data[2]<<8)|RxMessage.Data[3];
		if(RxMessage.StdId == 0x208)
			guard_move_speed[1] = (RxMessage.Data[2]<<8)|RxMessage.Data[3];
	
		if(RxMessage.StdId == 0x210)
		{
			can_data_targt_power = (int16_t) RxMessage.Data[0]<<8|RxMessage.Data[1];
			
		}
	}
}


S16	drag_angle;


extern S16 tar_speed;      //发弹信息
extern S16 tar_speed_temp;      //发弹信息
extern u8 shoot_single_G;
extern u8 shoot_multi_G;
extern u8 SHOOT_SINGLE_FLAG;

extern u8 ReceivedFlag;         //can能够接受到时置一
extern u8 Can_ConnectionFlag;       //can线连接无误置一
extern u8 MultiRecv_Flag;
extern u8 up2down_print;
extern u8 down2up_print;

//extern u8 SHOOT_MULTI_ENABLE;

float Real_Center = 0,Dir_angle = 0,Dir_angle_adj = 22;
int Dir_circle = 0,count_flag = 1;

//单轴陀螺仪
int16_t can_gyro,can_yaw;
float can_gyro_f,can_yaw_f,real_mpu_yaw;
mcircle_t can_mpu_yaw = {0};


s16 CANLASTOUT[4];
u8 can_101_flag=0;
void SHOOT_single(void *param);
void SHOOT_multi(void *param);
void SHOOT_multi_start(void);
void SHOOT_multi_stop(void);

extern s16 chassis_current[4];
void	CAN2_RX0_IRQHandler(void)
{
	size_t		i;
	CanRxMsg	RxMessage;
	float n = 0.3;

	if(CAN_GetITStatus(CAN2, CAN_IT_FMP0) != RESET)
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FMP0); 
		
		CAN_Receive(CAN2,CAN_FIFO0,&RxMessage);		
				
		if ((RxMessage.IDE==CAN_ID_STD)&&(RxMessage.DLC==8))	
		{	
			for(i = 0;i < 4;i++)		//这里是底盘的消息接收
			{
				if(RxMessage.StdId == Driver_ID[i])
				{
					chassis_V_C[i] = RxMessage.Data[2]<<8|RxMessage.Data[3];
					chassis_V_C[i] = SOLDIER_RPC_ZERO(chassis_V_C[i],10);		//*********死区处理**********
					
					current_C[i] = RxMessage.Data[4]<<8|RxMessage.Data[5];
					current_C[i] = SOLDIER_RPC_ZERO(current_C[i],10);
					
					current_C[i] = current_C[i]*n+(1-n)*CANLASTOUT[i];
					CANLASTOUT[i] = current_C[i];
					
					canrate.inc.rx_motor[i]++;
					break;		//进can中断一次只能接收到某个id的消息，检测到之后即可以跳出本次for循环了
				}
				
			}
		}
		if((RxMessage.StdId >= 0x201) && (RxMessage.StdId <= 0x208))
									circle_count(&(motor_circle[1][RxMessage.StdId-0x201]), \
			RxMessage.Data[0]<<8|RxMessage.Data[1],RxMessage.Data[2]<<8|RxMessage.Data[3]);
			
		if((RxMessage.StdId >= 0x201) && (RxMessage.StdId <= 0x208))
			circle_count_simple(&(motor_circle_simple[1][RxMessage.StdId-0x201]), ((U16)(RxMessage.Data[0]<<8))|((U16)RxMessage.Data[1]));
				
			//以下为云台电机反馈信号处理，根据不同机器人分别处理
		if(RxMessage.StdId == 0x205)
		{
			holder_yaw_angle = (int16_t)(RxMessage.Data[0]<<8) | (RxMessage.Data[1]);
			Dir_circle = motor_circle_simple[1][4].circle;
			Dir_angle = (float)((holder_yaw_angle - Real_Center) * 360 / 8192) + Dir_angle_adj;
			if(Dir_angle > 180)
				Dir_angle -= 360;
			if(Dir_angle < -180)
				Dir_angle += 360;
				
			holder_yaw_angle  = (float)(motor_circle_simple[1][4].angle) / 8192 + motor_circle_simple[1][4].circle;
			yaw_speed = (int16_t)(RxMessage.Data[2]<<8) | (RxMessage.Data[3]);
			yaw_speed *= 20;
			holder_yaw_angle *= 819;			//一圈对应819.2
			//holder_yaw_angle = holder_yaw_angle - ((int16_t)(holder_yaw_angle)) / 819 * 819;
			yaw_torque = (int16_t)((RxMessage.Data[4])<<8)|(int16_t)(RxMessage.Data[5]);
				
			canrate.inc.rx_motor[4]++;
		}

		if(RxMessage.StdId == 0x206)
		{
			holder_pitch_angle  = (int16_t)(RxMessage.Data[0]<<8) | (RxMessage.Data[1]);
			holder_pitch_angle *= 4;		//归一化到32767
			pitch_speed = (int16_t)(RxMessage.Data[2]<<8) | (RxMessage.Data[3]);
			pitch_speed *= 20;				//大概归一化到了6000，就先这样吧
			pitch_torque = (int16_t)((RxMessage.Data[4])<<8)|(int16_t)(RxMessage.Data[5]);
				
			canrate.inc.rx_motor[5]++;
		}
		
		
		if(RxMessage.StdId == 0x218)
		{
			can_gyro = RxMessage.Data[0]<<8|RxMessage.Data[1];
			can_yaw = RxMessage.Data[2]<<8|RxMessage.Data[3];
//			can_gyro_f=can_gyro*0.07326;
			can_gyro_f=can_gyro * 2000 / 327.68f;
//			can_yaw_f=can_yaw*0.03663;
//			can_yaw = can_yaw * 0.8192f;
			can_yaw = can_yaw / 8 + 4095;
			circle_count_simple(&can_mpu_yaw,can_yaw);
			real_mpu_yaw = 819 * (float)(can_mpu_yaw.circle + (float)(can_mpu_yaw.angle) / 8192);
									/* 乘一个819的目的是将陀螺仪的反馈与电机反馈归一到同一个量级 */
		}
	}
}

