#include "makos_pendsv.h"
#include "makos_type.h"

U32	svc_exc_return;

__asm void PendSV_Handler(void)
{
	IMPORT	ptcb_curr//IMPORT 导入外部变量
	IMPORT	ptcb_next
	
	CPSID	I
	MRS	R0, PSP //将PSP寄存器内容读出到R0
	CBZ	R0, OS_CPU_PendSVHandler_nosave
	
	//special for f4
	TST	LR,	#0x10
	IT	EQ
	VSTMDBEQ	R0!,	{S16-S31}
	//
	
	SUBS	R0, R0, #0x20
	STM     R0, {R4-R11}

	LDR     R1, =ptcb_curr
	LDR     R1, [R1]
	STR     R0, [R1]
	
OS_CPU_PendSVHandler_nosave
	
	LDR	R0, =ptcb_curr	
	LDR	R1, =ptcb_next
	//R1 -> R0
	LDR	R2, [R1] 	//加载寄存器，读[R1]处的值到R2
	STR	R2, [R0]	//把R2的值送到[R0]处

	LDR	R0, [R2]
	LDM	R0, {R4-R11}
	ADDS	R0, R0, #0x20
	
	//special for f4
	TST	LR,	#0x10
	IT	EQ
	VLDMIAEQ	R0!,	{S16-S31}
	//
	
	MSR	PSP, R0
	ORR	LR, LR, #0x04	//按位或，把LR的值或上0X04后放入LR中
	CPSIE	I
	BX	LR
	
}




