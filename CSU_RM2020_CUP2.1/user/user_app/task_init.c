/*
	FILE NAME:	task_init.c
	DESCRIPTION:

	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/3/10
	MODIFICATION:
	---------
	AUTHOR:
	EDIT TIME:
	MODIFICATION:
	---------
*/
#include "task_init.h"
#include "headfile.h"
#include "task_led.h"
#include "task_remote.h"
#include "task_UI.h"
#include "UI_lib.h"
#include "task_judge.h"
#include "task_chassis.h"
#include "task_holder.h"
#include "task_mems.h"
#include "timer.h"
#include "task_vision.h"
#include "task_Ano.h"
#include "task_INA226.h"
#include "task_pwm_pid.h"

void	task_init(void* param)
{
	Delay_Timer_Init();
	TIM12_Int_Init(10000,8400-1);//can÷–∂œÀŸ¬ ºÏ≤‚
	
    CAN1_Configuration((((u32)Control_ID << 3) & 0xffff0000) >> 16, (((u32)Control_ID << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xffff);
    can2_Configuration((((u32)Control_ID << 3) & 0xffff0000) >> 16, (((u32)Control_ID << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xffff);

//   remote_send_msg_init();

//    UI_init();
    task_delay_ms(200);

    task_insert(task_led_sys, NULL,2);
	task_insert(task_INA226, NULL,2);
	task_insert(task_pwm_pid, NULL,2);
	
    task_suspend(ptcb_curr); 

    while(1)
    {
        task_delay_ms(1000);
    }
}


