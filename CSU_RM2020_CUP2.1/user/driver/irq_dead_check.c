#include "irq_dead_check.h"


//#define	IRQ_CHECK_ENABLE

#define	IRQ_FREQ_LIMIT	((U32)10)


void	irq_dead_check(p_irq_check pic, U32 clock)
{
#ifdef	IRQ_CHECK_ENABLE
	
	U32	gap = clock - pic->clock_prev;
	
	if(gap == 0){
		pic->enters	+= 1;
		if(pic->enters > IRQ_FREQ_LIMIT){
			PRINT_ERROR("%s irq frequency out of limits", pic->name);
		}
	}else{
		pic->clock_prev	= clock;
		pic->enters	= 1;
	}
	
#endif
}





