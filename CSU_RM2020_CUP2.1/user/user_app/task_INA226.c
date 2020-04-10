#include "task_INA226.h"
#include "i2c.h"
#include "headfile.h"
#include "PWM.h"
s16 Charge_Curr=0;
s16 Charge_vbus =0;
s16 Charge_power=0;

s16 CC_LAST =0;

s16 actual_CC=0;
s16 actual_vbus=0;
s16 actual_power =0;
void	task_INA226(void* param)
{
	INA226_1_Init();
	SC_Discharge_init();
	task_delay_ms(2000);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	float n =0.7;
	while(1)
	{	
		Charge_Curr = INA226_ReadData(INA226_1_ADDR, Current_Reg_ADDR);
		Charge_vbus =  INA226_ReadData(INA226_1_ADDR, BusVoltage_Reg_ADDR);
		Charge_power =  INA226_ReadData(INA226_1_ADDR, Power_Reg_ADDR);
		
		actual_CC = Charge_Curr*0.455;
		actual_vbus = Charge_vbus *0.125;//0.01v
		actual_power = Charge_power*0.125;//.1w
		
		actual_CC = CC_LAST*(1-n) + actual_CC*n;
		CC_LAST = actual_CC;
		task_delay_ms(1);
		
	}
}
