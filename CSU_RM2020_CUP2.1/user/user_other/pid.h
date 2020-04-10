#ifndef _PID_H
#define _PID_H

#include "makos_includes.h"

#define KF_Q 10
#define KF_R 10

#define SUM_LIMIT   400
#define VUM_LIMIT   3000

#define max(a, b)			(a>b? a:b)
#define min(a, b)			(a<b? a:b)
#define range(x, a, b)		(min(max(x, a), b))//a-最小值   b-最大值

typedef struct 
{
	float limit;
	float kp;
	float ki;
	float kd;
	float e0;
	float e1;
	float e2;
	float out;
} PID_IncreType;//增量式PID

typedef struct 
{
	float limit;	//????
	float Kp;		
	float Ki;
	float Kd;
	float eSum;
	float e0;		
	float e1;		
}PID_AbsoluteType;//位置式PID

extern PID_AbsoluteType Pid_speed[4];//速度环
extern PID_IncreType Pid_current[4];//电流环
extern PID_AbsoluteType Pid_caterpillar[2];;

float PID_Update_Incre(PID_IncreType* PID,float tar,float cur);
float PID_Update_Absolute2(PID_AbsoluteType* PID,float tar,float cur);
float PID_Update_Absolute3(PID_AbsoluteType* PID,float tar,float cur);
float PID_Update_Absolute4(PID_AbsoluteType* PID,float tar,float cur);
void PID_Struct_Init(PID_IncreType* PID,float kp,float ki,float kd,float out_max);
void PID_speed_Init(float kp,float ki,float kd,float out_max);
void PID_current_Init(float kp,float ki,float kd,float out_max);
void PID_caterpillar_Init(float kp,float ki,float kd,float out_max);
void PID_Struct_Init_abs(PID_AbsoluteType* PID,float kp,float ki,float kd,float out_max);
void PID_Struct_Init_abs_nimi(PID_AbsoluteType* PID,float kp,float ki,float kd,float out_max);

#endif
