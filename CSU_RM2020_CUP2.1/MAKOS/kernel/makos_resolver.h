#ifndef _MAKOS_RESOLVER_H
#define _MAKOS_RESOLVER_H

#include "makos_kernel_includes.h"

#include "makos_task.h"
#include "makos_system.h"
#include "makos_schedule.h"

#include "makos_pipe.h"

/*
	input format:	func_name(U32_param)
	for example:	cpu(0)
*/

//****app 帧功能定义****
extern u8 app_key1;
extern u8 app_key2;
extern u8 app_key3;
extern u8 app_key4;
extern u8 app_key5;
extern u8 app_key6;

extern u8 app_bar1;
extern u8 app_bar2;
extern u8 app_bar3;
//按键
#define	APP_KEY1	&app_key1
#define APP_KEY2	&app_key2
#define APP_KEY3	&app_key3
#define APP_KEY4	&app_key4
#define APP_KEY5	&app_key5
#define APP_KEY6	&app_key6
//滚动条
#define	APP_BAR1	&app_bar1
#define	APP_BAR2	&app_bar2
#define	APP_BAR3	&app_bar3
//**********************

#define TO_U32(x) *((U32 *)x)
#define TO_FLOAT(x) *((float *)x) 	

typedef	void	(*resolve_f)(void * p_param);

typedef	struct	_resolve_t
{
	dnode_t		dnd;
	U8*		name;
	resolve_f	func;
	u8	char_num;
}resolve_t, *p_resolve;


extern	dnode_t	dhead_resolve;
extern	pipe_t	pipe_resolver;

void	USART1_IRQ_init(void);
void	resolver_init(void);
void	resolve_add(U8* name, resolve_f func);

U32	string_get_U32(const U8* s, U32 length);

void	task_resolver(void* param);






#endif
