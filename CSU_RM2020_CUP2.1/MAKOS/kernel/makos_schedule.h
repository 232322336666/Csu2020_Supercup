#ifndef _MAKOS_SCHEDULE_H
#define _MAKOS_SCHEDULE_H

#include "makos_kernel_includes.h"
#include "makos_task.h"
#include "makos_system.h"



void	task_delay_ms(U32 tick);

void	task_period_set(U32 tick);
void	task_period_delay(void);

void	task_yield(void);

void	task_suspend(p_TCB ptcb);
void	task_resume(p_TCB ptcb);

void	task_set_prio(p_TCB ptcb, task_prio_t prio);

#endif
