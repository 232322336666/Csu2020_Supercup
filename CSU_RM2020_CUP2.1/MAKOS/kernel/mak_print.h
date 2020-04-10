/*
	FILE NAME:	mak_print.h
	DESCRIPTION:	a printf using macro to suit varargus arguments
			only can handle simple formats
			
			
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/04/15
	MODIFICATION:	create and tested the file on CROS
			change the macro in 2018/04/14
			now can accept all type argument, but need to type in the type of the argument.
*/


#ifndef _MAK_PRINT_H
#define _MAK_PRINT_H

#include "makos_kernel_includes.h"

#define	FinU(data)		(*((U32*)(&(data))))

#define	MAK_PRINT0(fmt)\
	do{\
		irq_close();\
		mak_print(fmt, NULL);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT1(fmt, t1, p1)\
	do{\
		U32		buff[1] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT2(fmt, t1, p1, t2, p2)\
	do{\
		U32		buff[2] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT3(fmt, t1, p1, t2, p2, t3, p3)\
	do{\
		U32		buff[3] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)


#define	MAK_PRINT4(fmt, t1, p1, t2, p2, t3, p3, t4, p4)\
	do{\
		U32		buff[4] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		p		= ((U8*)p)+4;\
		*((t4*)p)	= p4;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT5(fmt, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5)\
	do{\
		U32		buff[5 = {0}];\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		p		= ((U8*)p)+4;\
		*((t4*)p)	= p4;\
		p		= ((U8*)p)+4;\
		*((t5*)p)	= p5;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT6(fmt, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6)\
	do{\
		U32		buff[6] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		p		= ((U8*)p)+4;\
		*((t4*)p)	= p4;\
		p		= ((U8*)p)+4;\
		*((t5*)p)	= p5;\
		p		= ((U8*)p)+4;\
		*((t6*)p)	= p6;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT7(fmt, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7)\
	do{\
		U32		buff[7] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		p		= ((U8*)p)+4;\
		*((t4*)p)	= p4;\
		p		= ((U8*)p)+4;\
		*((t5*)p)	= p5;\
		p		= ((U8*)p)+4;\
		*((t6*)p)	= p6;\
		p		= ((U8*)p)+4;\
		*((t7*)p)	= p7;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)

#define	MAK_PRINT8(fmt, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6, t7, p7, t8, p8)\
	do{\
		U32		buff[8] = {0};\
		void*		p;\
		p		= &buff;\
		*((t1*)p)	= p1;\
		p		= ((U8*)p)+4;\
		*((t2*)p)	= p2;\
		p		= ((U8*)p)+4;\
		*((t3*)p)	= p3;\
		p		= ((U8*)p)+4;\
		*((t4*)p)	= p4;\
		p		= ((U8*)p)+4;\
		*((t5*)p)	= p5;\
		p		= ((U8*)p)+4;\
		*((t6*)p)	= p6;\
		p		= ((U8*)p)+4;\
		*((t7*)p)	= p7;\
		p		= ((U8*)p)+4;\
		*((t8*)p)	= p8;\
		irq_close();\
		mak_print(fmt, buff);\
		irq_restore();\
	}while(0)


U32	mak_print(U8* fmt, U32* data);

void	mak_print3(U8* fmt, void* p1, void* p2, void* p3);







#endif
