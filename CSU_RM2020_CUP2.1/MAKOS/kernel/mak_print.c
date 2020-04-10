/*
	FILE NAME:	mak_print.c
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


#include "mak_print.h"
#include "makos_pipe.h"
#include "makos_system.h"


#define F(data)		((U32)(*((U32*)(&data))))
#define FS(data)	(((U32)(F(data)&((U32)0x80000000)))>>31)
#define FE(data)	((((U32)(F(data)&((U32)0x7f800000)))>>23)-127)
#define FF(data)	((U32)(F(data)&((U32)0x007fffff)))

#define	bit(data,n)	((F(data)>>(n))&1)

#define	pow2(n)		((U32)(1<<n))




void	mak_print_u8(U8 data)
{
	//printf("%c", data);
	pipe_write(&ptcb_curr->pipe_print, data);
}

void	mak_print_string(U8* data)
{
	while(*data){
		mak_print_u8(*data++);
	}
}


void	mak_print_s32(S32 data)
{
	U32	i=0;
	U32	temp = data & 0x7fffffff;
	U32	sign = data & 0x80000000;
	U8	buff[10];

	if(data == 0){
		mak_print_u8('0');
		return;
	}
	if(sign)
		mak_print_u8('-');
	while(temp){
		buff[i++] = temp%10 + '0';
		temp = temp/10;
	}
	while(i){
		mak_print_u8(buff[--i]);
	}
}


void	mak_print_float(float data)
{
	U32	round=0, small=0;
	
	if(FE(data)>>31 == 0)//larger than 1?
	{
		U32	temp = FE(data);
		U32	div = 1000000;
		round	+= pow2(temp);
		while(1)
		{
			if(temp == 0)
				break;
			temp--;
			round	+= bit(data, 23-FE(data)+temp)*pow2(temp);
		}
		while(1)
		{
			if(temp+FE(data) >= 16)
				break;
			temp++;
			div	= div>>1;
			small	+= bit(data, 23-FE(data)-temp)*div;
		}
	}
	else
	{
		U32	div = 1000000;
		U32	temp = 0;
		div	= div>>(-FE(data));
		small	+= div;
		while(1)
		{
			div	= div>>1;
			small	+= bit(data, 22-temp)*div;
			if(temp == 16)
				break;
			temp++;
		}
	}
	mak_print_s32(round);
	mak_print_u8('.');
	mak_print_s32(small);
}


//return 0 when mistake ocurrs
U32	mak_print(U8* fmt, U32* data)
{
	U8*	pfmt = fmt;
	U32*	pdata = data;

	do{
		switch(*pfmt++){
		case '%':
			switch(*pfmt++){
			case 'c':
				mak_print_u8(*pdata++);
				break;
			case 's':
				mak_print_string((U8*)(*pdata));
				pdata++;
				break;
			case 'd':
				mak_print_s32(*pdata++);
				break;
			case 'f':
				mak_print_float(*((float*)pdata));
				pdata++;
				break;
			case '%':
				mak_print_u8('%');
				break;
			default:
				return 0;
			}
			break;
		case '\\':
			switch(*pfmt++){
			case 'n':
				mak_print_u8('\n');
				break;
			case 'r':
				mak_print_u8('\r');
				break;
			case '0':
				mak_print_u8('\0');
				break;
			case '?':
				mak_print_u8('\?');
				break;
			case 't':
				mak_print_u8('\t');
				break;
			default:
				return 0;
			}
			break;
		default:
			mak_print_u8(*(pfmt-1));
		}
	}while(*pfmt);
	return	1;
}



void	mak_print3(U8* fmt, void* p1, void* p2, void* p3)
{
	U32	buff[3];
	buff[0]	= (U32)p1;
	buff[1]	= (U32)p2;
	buff[2]	= (U32)p3;
	mak_print(fmt, buff);
}







