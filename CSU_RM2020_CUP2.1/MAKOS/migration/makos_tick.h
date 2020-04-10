#ifndef _MAKOS_TICK_H
#define _MAKOS_TICK_H

#include "makos_kernel_includes.h"

typedef	void	(*tick_callback_t)(void);//函数指针类型

extern tick_callback_t	tick_callback;

void	tick_init(int ms);
void	tick_start(void);

#endif
