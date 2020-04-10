/*
	FILE NAME:	led.c
	DESCRIPTION:	the file aims to make the operation of leds more convenient.
			ultimately call the gpio functions from stm32f4 std lib.
			make sure that required macros are defined correctly.
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2017/11/17-16:36
	MODIFICATION:	using stm32f4 std lib, considering the using of led operations is low-frequency.
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2017/04/05
	MODIFICATION:	add struct interface, in order to suit dynamic needs.
*/
#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"

#include "makos_type.h"

/*
	define macros like follow to use these macros.
	name##_port  --- gpiox
	name##_pin   --- pin
	name##_rcc   --- apbx/ahbx
	name##_light --- 1/0, the electric level when light is on
	for example:
	#define	led_sys_port	GPIOC
	#define	led_sys_pin	GPIO_Pin_13
	#define	led_sys_rcc	RCC_AHB1Periph_GPIOC
	#define	led_sys_light	((U32)1)
*/


#define	LED_INIT(name)		led_init(name##_port, name##_pin, name##_rcc, name##_light)
#define	LED_ON(name)		led_on(name##_port, name##_pin, name##_light)
#define	LED_OFF(name)		led_off(name##_port, name##_pin, name##_light)
#define	LED_TOGGLE(name)	led_toggle(name##_port, name##_pin)

void	led_init(GPIO_TypeDef* gpiox, U16 pin, U32 gpiorcc, U32 light);
void	led_on(GPIO_TypeDef* gpiox, U16 pin, U32 light);
void	led_off(GPIO_TypeDef* gpiox, U16 pin, U32 light);
void	led_toggle(GPIO_TypeDef* gpiox, U16 pin);


#define	MLED_DEFINE(name, port, pin, light)		\
	mled_t	name = {GPIO##port,			\
			RCC_AHB1Periph_GPIO##port,	\
			GPIO_Pin_##pin,			\
			light};

typedef	struct	_mled_t
{
	GPIO_TypeDef*	port;
	U32		rcc;
	U16		pin;
	U16		light;
}mled_t, *p_mled;

void	mled_init(p_mled pled);
void	mled_on(p_mled pled);
void	mled_off(p_mled pled);
void	mled_toggle(p_mled pled);





#endif
