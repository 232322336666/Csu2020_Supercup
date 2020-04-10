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
*/
#include "led.h"


void	led_init(GPIO_TypeDef* gpiox, U16 pin, U32 gpiorcc, U32 light)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(gpiorcc, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= pin;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(gpiox, &GPIO_InitStructure);
	
	led_off(gpiox, pin, light);
}

void	led_on(GPIO_TypeDef* gpiox, U16 pin, U32 light)
{
	if(light)
		GPIO_SetBits(gpiox, pin);
	else
		GPIO_ResetBits(gpiox, pin);
}

void	led_off(GPIO_TypeDef* gpiox, U16 pin, U32 light)
{
	if(light)
		GPIO_ResetBits(gpiox, pin);
	else
		GPIO_SetBits(gpiox, pin);
}

void	led_toggle(GPIO_TypeDef* gpiox, U16 pin)
{
	GPIO_ToggleBits(gpiox, pin);
}








void	mled_init(p_mled pled)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(pled->rcc, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= pled->pin;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(pled->port, &GPIO_InitStructure);
	
	mled_off(pled);
}

void	mled_on(p_mled pled)
{
	if(pled->light)
		GPIO_SetBits(pled->port, pled->pin);
	else
		GPIO_ResetBits(pled->port, pled->pin);
}

void	mled_off(p_mled pled)
{
	if(pled->light)
		GPIO_ResetBits(pled->port, pled->pin);
	else
		GPIO_SetBits(pled->port, pled->pin);
}

void	mled_toggle(p_mled pled)
{
	GPIO_ToggleBits(pled->port, pled->pin);
}












