#ifndef _MAKOS_PENDSV_H
#define _MAKOS_PENDSV_H



#define	MEM32(addr)			*(volatile unsigned long *)(addr)
#define	MEM8(addr)			*(volatile unsigned char *)(addr)
#define	NVIC_INT_CTRL			0xE000Ed04
#define	NVIC_PENDSVSET			0x10000000
#define	NVIC_SYSPRI2			0xE000ED22
#define	NVIC_PENDSV_PRI			0x000000FF
#define	PENDSV_SET()			MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET



#endif
