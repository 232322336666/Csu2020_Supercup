#include "task_INA226.h"
#include "i2c.h"
#include "task_pwm_pid.h"
#include "headfile.h"
#include "PWM.h"
#include "pid.h"
#define TARGT_POWER 400
extern s16 actual_CC;
extern s16 actual_power; 
extern s16 actual_vbus; 
extern s16 can_data_targt_power;

s16 targt_power=TARGT_POWER;
s16 targt_cc=2600;
s16 pwm_out;
s32 cout=0;
PID_IncreType pid_cup;

void limit(s16* num,s16 min,s16 mix)
{
	if(*num > mix) *num = mix;
	if(*num < min) *num = min;
}

#define CMD_WARE     3

void send3_wave() 
{
	s16 temp1,temp2,temp3,temp4,temp5,temp6;
	
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};
	
	
	temp1= targt_power;
	temp2=actual_power;
	temp3=pwm_out;
	temp4=actual_CC;
	temp5=actual_vbus;
	temp6=targt_power;
	
    uart5_send_string(cmdf, sizeof(cmdf));
	
    uart5_send_string((uint8_t *)&(temp1), 2);
    uart5_send_string((uint8_t *)&(temp2), 2);
    uart5_send_string((uint8_t *)&(temp3), 2);
    uart5_send_string((uint8_t *)&(temp4), 2);
	uart5_send_string((uint8_t *)&(temp5), 2);
	uart5_send_string((uint8_t *)&(temp6), 2);
	
    uart5_send_string(cmdr, sizeof(cmdr));
}

void	task_send_wave(void* param)
{
	while(1)
	{
		send3_wave();
		task_delay_ms(1);
	}
}

void can_super_cup_send()
{
	u8 data[8];
	s16 data_vbus,data_cc,data_targt_power;
	
	data_vbus = actual_vbus;
	data_cc = actual_CC/10;
	data_targt_power=targt_power*100;
	
	CanTxMsg canmsg;

	data[0] = 0;
	data[1] = 0>>8;
	data[2] = actual_vbus;
	data[3] = actual_vbus>>8;	
	data[4] = data_cc;
	data[5] = data_cc>>8;
	data[6] = targt_power;
	data[7] = targt_power>>8;
	
//	data[0] = 0;
//	data[1] = 0>>8;
//	data[2] = 0x0960;
//	data[3] = 0x0960>>8;	
//	data[4] = 0x02;
//	data[5] = 0x02>>8;
//	data[6] = 0x03;
//	data[7] = 0x03>>8;
	
	can_set(&canmsg,0x211,data);
	CAN_Transmit(CAN1,&canmsg);
}


void	task_send_can(void* param)
{
	while(1)
	{
		can_super_cup_send();
		task_delay_ms(5);
	}
}

void	task_pwm_pid(void* param)
{
		task_delay_ms(2000);
	s16 temp=0;
	PID_Struct_Init(&pid_cup,0.0080 ,0.02, 0,1590);
//	PID_Struct_Init(&pid_cup,0.0005 ,0.003 ,0,1200);
	
	task_insert(task_send_wave, NULL,2);
	task_insert(task_send_can, NULL,2);
	enable_init(); //使能引脚初始化
	TIM1_Init(1680-1,1-1); //PWM init
//	TIM8_Init(1680-1,1-1); //PWM init
	TIM10_Init(1680-1,1-1);
	GPIO_SetBits(GPIOB,GPIO_Pin_9); //充电使能
	
	while(1)
	{
		targt_power=can_data_targt_power;		
		if(actual_vbus>1500)	
		{
			pid_cup.kp=0.02;
			pid_cup.ki=0.04;
			pwm_out = PID_Update_Incre(&pid_cup,targt_power,actual_power);
		}
		else 
		{
			pid_cup.kp=0.0080;
			pid_cup.ki=0.02;
			pwm_out = PID_Update_Incre(&pid_cup,targt_cc,actual_CC);
		}
//	    pwm_out = PID_Update_Incre(&pid_cup,targt_cc,actual_CC);
//		pwm_out = PID_Update_Incre(&pid_cup,targt_power,actual_power);
		limit(&pwm_out,10,1590);
		TIM_SetCompare1(TIM10,pwm_out);

		task_delay_ms(1);
	}
}

