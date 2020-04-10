#ifndef _DBUS_H
#define _DBUS_H

#include "stm32f4xx.h"

#include "MAKOS_type.h"
#include "MAKOS_tick_handle.h"


extern
U8	remote_buff[18];

extern
U32	dbus_error_ms;


void	dbus_init(U32 num, voidf handle);

void	dbus_mode_dma(void);
void	dbus_mode_usart(void);

U32	dbus_buff_checked(void);

extern
voidf	dmaif11, dmaif22;






#endif
