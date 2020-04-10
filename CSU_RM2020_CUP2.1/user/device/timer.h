#ifndef _TIMER_H
#define _TIMER_H
#include "headfile.h"

void Delay_Timer_Init(void);
void delay_us(uint32_t us_cnt);

typedef struct can_test
{
	u32 can1_tx;
	u32 can2_tx;
	u32 dbus;
	u32 rx_motor[6];
	
}can_test;

typedef struct can_rate
{
	can_test inc;//在中断里面自增
	can_test send;//发给ui
	
}can_rate_t,*p_can_rate_t;
extern can_rate_t canrate;

void TIM12_Int_Init(u16 arr,u16 psc);

#endif
