
#include "makos_tick.h"

tick_callback_t	tick_callback;

void	tick_init(int ms)
{
	SysTick->LOAD	= CLOCK_FREQ/1000*ms - 1;//72000000/1000=72000(cm3)
	SysTick->VAL	= 0;
	NVIC_SetPriority(SysTick_IRQn, 0xff);
	SysTick->CTRL	=	/*SysTick_CTRL_ENABLE	|*/	\
				/*SysTick_CTRL_TICKINT	|*/	\
				SysTick_CTRL_CLKSOURCE_Msk|	\
				0				;
}

void	tick_start(void)
{
	SysTick->CTRL	|=	SysTick_CTRL_ENABLE_Msk;
	SysTick->CTRL	|=	SysTick_CTRL_TICKINT_Msk;
}

void	SysTick_Handler(void)
{
	tick_callback();
}


