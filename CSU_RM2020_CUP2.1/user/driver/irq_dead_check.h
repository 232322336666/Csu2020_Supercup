#ifndef _IRQ_DEAD_CHECK_H
#define _IRQ_DEAD_CHECK_H

#include "makos_includes.h"


typedef	struct	_irq_check_t
{
	U32	enters;
	U32	clock_prev;
	char*	name;
}irq_check_t, *p_irq_check;


#define	IRQ_CHECK_DEFINE(name)\
	irq_check_t	name##_irq_check = {0, 0, #name};

#define	IRQ_CHECK(name)\
	do{\
		irq_dead_check(&name##_irq_check, system_clock);\
	}while(0)



void	irq_dead_check(p_irq_check pic, U32 clock);




#endif
