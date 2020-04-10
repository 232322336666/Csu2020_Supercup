/*
	FILE NAME:	makos_F2S.c
	DESCRIPTION:	give a fun to turn float argument into strings
								this is just to handle the problem that using printf with float argument will cause a 
								mistake result.But the problem has been handle by making the task'stack align at 8 bytes.

	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/04/15
	MODIFICATION:	create and tested the file on CROS
			change the macro in 2018/04/14
			now can accept all type argument, but need to type in the type of the argument.
*/

#include "makos_F2S.h"

#define F(data)		((U32)(*((U32*)(&data))))
#define FS(data)	(((U32)(F(data)&((U32)0x80000000)))>>31)
#define FE(data)	((((U32)(F(data)&((U32)0x7f800000)))>>23)-127)
#define FF(data)	((U32)(F(data)&((U32)0x007fffff)))

#define	bit(data,n)	((F(data)>>(n))&1)

#define	pow2(n)		((U32)(1<<n))


U8*	F2S(float data, U8* buff)
{
	U32	round=0, small=0;

	if(FE(data)>>31 == 0)
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
	{
		size_t	w=0;
		size_t	i, j;
		U32	temp = round;
		for(i=0; temp; temp=temp/10,i++);
		temp	= round;
		if(i>0)
		{
			for(j=0; j<=i; j++)
			{
				buff[i-j-1] = '0' + temp%10;
				temp	= temp/10;
			}
			w+=i;
		}
		else
			buff[w++] = '0';
		buff[w++] = '.';

		temp	= small;
		for(i=0; temp; temp=temp/10,i++);
		if(i<6)
		{
			for(j=6-i; j; j--)
				buff[w++] = '0';
		}
		temp = small;
		for(j=0; j<=i; j++)
		{
			buff[w+i-j-1] = '0' + temp%10;
			temp	= temp/10;
		}
		w+=i;

		buff[w++] = 0;
	}
	return	buff;
}




