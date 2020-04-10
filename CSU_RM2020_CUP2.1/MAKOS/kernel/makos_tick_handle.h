#ifndef _MAKOS_TICK_HANDLE_H
#define _MAKOS_TICK_HANDLE_H

#include "makos_kernel_includes.h"
#include "makos_system.h"
#include "makos_tick.h"

typedef	VU32	system_tick_t;
/*
typedef	struct	_system_clock_t
{
	system_tick_t	tick_high;
	system_tick_t	tick_low;
} system_clock_t;

extern	system_clock_t	system_clock;
*/
extern	system_tick_t	system_clock;

os_error	init_tick_callback(void);

void		tick_handle(void);

#endif
