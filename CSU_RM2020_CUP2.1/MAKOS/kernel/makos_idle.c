#include "makos_idle.h"
#include "makos_print.h"
#include "makos_tick_handle.h"
#include "makos_resolver.h"

#include "mak_print.h"


U32	cpu_print = 0;
U32	cpu_print_mark = 0;

void	set_cpu_print(void * param)
{
	if(TO_U32(param) <= 2)
		cpu_print = TO_U32(param);
}

void	task_idle(void* param)
{
	U32	clock_temp = 0, clock_gap = 0;
	U32	tick = 0;
	U32	clock_check = 0;
	float	output_percent = 0;
	
	resolve_add("cpu", set_cpu_print);
	
	while(1)
	{
		clock_gap	= system_clock - clock_temp;
		if(clock_gap)
		{
			tick		= tick + SysTick->VAL;
			//tick		= SysTick->VAL;
			clock_temp	= clock_temp + clock_gap;
			clock_check	= clock_check + clock_gap;
			
			if(clock_check > 1000)
			{
				output_percent	= MT2F(100) - MT2F(tick)/MT2F(CLOCK_FREQ/100);
				
				tick		= 0;
				clock_check	= 0;
			}
		}
		if(cpu_print && (system_clock%1000 < 50) && cpu_print_mark)
		{
			//PRINT("cpu usage tick: %d\r\n", tick);
			MAK_PRINT1("cpu usage: %f%%\r\n", float, output_percent);//输出浮点为0，有bug,待修复
			printf("cpu usage:%f\r\n",output_percent);
			
			if(cpu_print == 2)
				cpu_print	= 0;
			cpu_print_mark	= 0;
		}
		if(system_clock%1000>100)
			cpu_print_mark	= 1;
//		task_delay_ms(0);
	}
}









