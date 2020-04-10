#ifndef _MAK_PID_H
#define _MAK_PID_H

#include "makos_type.h"

typedef struct 
{
	float kp;
	float ki;
	float kd;
	float errILim_up;//积分上限
	float errILim_down;//积分上限
	float errLim;
	float errNow;
	float errOld;
	float errP;
	float errI;
	float errD;
	float ctrOut;
} PID_AbsoluteType_holder;

typedef struct 
{
 /*PID算法接口变量，用于给用户获取或修改PID算法的特性*/
 float kp;     //比例系数
 float ki;     //积分系数
 float kd;     //微分系数
 
 float errNow; //当前的误差
 float dCtrOut;//控制增量输出
 float ctrOut;//控制输出
 
 float ILim;//积分限幅
 float IncLim;//增量限幅

 /*PID算法内部变量，其值不能修改*/
 float errOld1;
 float errOld2;
 
}PID_IncrementType;

void	pid_init_absolute(PID_AbsoluteType_holder* PID,float kp, float ki, float kd, float errlimit);
float PID_Update(PID_AbsoluteType_holder* PID,int16_t Target,int16_t Current);

void	pid_init_increment(PID_IncrementType* PID,float kp, float ki, float kd, float ILim,float IncLim);
float PID_IncrementMode(PID_IncrementType* PID,int16_t Target,int16_t Current);

int16_t  KalmanFilter(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction);

#endif
