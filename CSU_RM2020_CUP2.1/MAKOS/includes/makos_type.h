#ifndef _MAKOS_TYPE_H
#define _MAKOS_TYPE_H

#include <stdint.h>

typedef enum{FALSE = 0,TRUE = 1} bool;

typedef		 int8_t		S8;
typedef		 int16_t	S16;
typedef		 int32_t	S32;

typedef		 uint8_t	U8;
typedef		 uint16_t	U16;
typedef		 uint32_t	U32;

typedef	volatile int8_t		VS8;
typedef	volatile int16_t	VS16;
typedef	volatile int32_t	VS32;

typedef	volatile uint8_t	VU8;
typedef	volatile uint16_t	VU16;
typedef	volatile uint32_t	VU32;


#ifndef	NULL
#define		 NULL		((void*)0)
#endif

typedef enum
{
	enable,
	disable,
} usage_t;

#ifndef	size_t
typedef		unsigned int	size_t;
#endif

#ifdef		OFFSETOF
#undef		OFFSETOF
#endif
#define		OFFSETOF(TYPE, MEMBER)		((size_t)&((TYPE*)0)->MEMBER)

#ifdef		CONTAINER_OF
#undef		CONTAINER_OF
#endif
#define		CONTAINER_OF(ptr, type, member)	((type*)((U8*)(ptr) - OFFSETOF(type, member)))

#define	CAT(a,b)	CAT0(a,b)
#define CAT0(a,b)	a ## b

typedef	void*	(*void41f)(void*, void*, void*, void*);
typedef	void*	(*void31f)(void*, void*, void*);
typedef	void*	(*void21f)(void*, void*);
typedef	void*	(*void11f)(void*);
typedef	void*	(*void01f)(void);
typedef	void	(*void40f)(void*, void*, void*, void*);
typedef	void	(*void30f)(void*, void*, void*);
typedef	void	(*void20f)(void*, void*);
typedef	void	(*void10f)(void*);
typedef	void	(*voidf)(void);

#define		SI				\
		__attribute__((always_inline))	\
		static	__inline

typedef	union	_mt8
{
	char	c;
	U8	u8;
	S8	s8;
}mt8, *pmt8;

typedef	union	_mt16
{
	char	c[2];
	U8	u8;
	S8	s8;
	U16	u16;
	S16	s16;
}mt16, *pmt16;

typedef	union	_mt32
{
	char	c[4];
	U8	u8;
	S8	s8;
	U16	u16;
	S16	s16;
	U32	u32;
	S32	s32;
	float	f;
}mt32, pmt32;

#define	MT8(data, type)		((*((pmt8)(&(data)))).type)
#define	MT16(data, type)	((*((pmt16)(&(data)))).type)
#define	MT32(data, type)	((*((pmt32)(&(data)))).type)
#define	MT64(data, type)	((*((pmt64)(&(data)))).type)

#define	MT8inC(data)		MT8(data, c)
#define	MT8inU(data)		MT8(data, u8)
#define	MT8inS(data)		MT8(data, s8)

#define	MT16inC(data)		MT16(data, c)
#define	MT16inU(data)		MT16(data, u16)
#define	MT16inS(data)		MT16(data, s16)

#define	MT32inC(data)		MT32(data, c)
#define	MT32inU(data)		MT32(data, u32)
#define	MT32inS(data)		MT32(data, s32)
#define	MT32inF(data)		MT32(data, f)

#define	MT2C(data)		((char)(data))
#define	MT2U8(data)		((U8)(data))
#define	MT2U16(data)		((U16)(data))
#define	MT2U32(data)		((U32)(data))
#define	MT2S8(data)		((S8)(data))
#define	MT2S16(data)		((S16)(data))
#define	MT2S32(data)		((S32)(data))
#define	MT2F(data)		((float)(data))

#define	MTU8toU16(data)		MT2U16(MT2U8(data))
#define	MTU8toU32(data)		MT2U32(MT2U8(data))
#define	MTU16toU32(data)	MT2U32(MT2U16(data))

#define	MTS8toS16(data)		MT2S16(MT2S8(data))
#define	MTS8toS32(data)		MT2S32(MT2S8(data))
#define	MTS16toS32(data)	MT2S32(MT2S16(data))

#define	MTS8toF(data)		MT2F(MT2S32(MT2S8(data)))
#define	MTS16toF(data)		MT2F(MT2S32(MT2S16(data)))
#define	MTS32toF(data)		MT2F(MT2S32(data))

#define	MTFtoS32(data)		MT2S32(MT2F(data))


#define	LIMIT(data, max, min)\
	do{\
		if((data) > (max)){\
			(data) = (max);\
		}else if((data) < (min)){\
			(data) = (min);\
		}\
	}while(0)

#define	LIMIT_SYM(data, max)\
	do{\
		if((data) > (max)){\
			(data) = (max);\
		}else if((data) < -(max)){\
			(data) = -(max);\
		}\
	}while(0)

#define	IN_RANGE(data, limit)\
	(((data)>(limit))? 0:(((data)<(-(limit)))? 0:1))

#endif
