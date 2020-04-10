#ifndef _MAKOS_ATOMIC_H
#define _MAKOS_ATOMIC_H

#include "makos_kernel_includes.h"
#include "makos_schedule.h"

typedef	U32	atomic;

static	__inline
atomic	atomic_set(atomic* p, U32 set)
{
	irq_close();
	*p	= set;
	irq_restore();
	return	*p;
}

static	__inline
atomic	atomic_get(atomic* p)
{
	return	*p;
}

static	__inline
atomic	atomic_inc(atomic* p)
{
	irq_close();
	*p++;
	irq_restore();
	return	*p;
}

static	__inline
atomic	atomic_dec(atomic* p)
{
	irq_close();
	*p--;
	irq_restore();
	return	*p;
}

static	__inline
atomic	atomic_add(atomic* p, U32 add)
{
	irq_close();
	*p	+= add;
	irq_restore();
	return	*p;
}

static	__inline
atomic	atomic_sub(atomic* p, U32 sub)
{
	irq_close();
	*p	-= dec;
	irq_restore();
	return	*p;
}




#endif
