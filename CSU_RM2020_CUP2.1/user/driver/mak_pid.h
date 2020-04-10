#ifndef _MAK_PID_H
#define _MAK_PID_H

#include "makos_type.h"

typedef struct 
{
	float kp;
	float ki;
	float kd;
	float errILim_up;//��������
	float errILim_down;//��������
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
 /*PID�㷨�ӿڱ��������ڸ��û���ȡ���޸�PID�㷨������*/
 float kp;     //����ϵ��
 float ki;     //����ϵ��
 float kd;     //΢��ϵ��
 
 float errNow; //��ǰ�����
 float dCtrOut;//�����������
 float ctrOut;//�������
 
 float ILim;//�����޷�
 float IncLim;//�����޷�

 /*PID�㷨�ڲ���������ֵ�����޸�*/
 float errOld1;
 float errOld2;
 
}PID_IncrementType;

void	pid_init_absolute(PID_AbsoluteType_holder* PID,float kp, float ki, float kd, float errlimit);
float PID_Update(PID_AbsoluteType_holder* PID,int16_t Target,int16_t Current);

void	pid_init_increment(PID_IncrementType* PID,float kp, float ki, float kd, float ILim,float IncLim);
float PID_IncrementMode(PID_IncrementType* PID,int16_t Target,int16_t Current);

int16_t  KalmanFilter(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction);

#endif
