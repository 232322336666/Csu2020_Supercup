#ifndef _MAKOS_SEMAPHORE_H
#define _MAKOS_SEMAPHORE_H

#include "makos_kernel_includes.h"
#include "makos_schedule.h"

typedef	struct _sem_t
{
	U32	count;
	dnode_t	dhead_wait;
}sem_t, *p_sem;

void	sem_init(p_sem psem, U32 cnt);
void	sem_ask(p_sem psem);
void	sem_release(p_sem psem);



#endif
