/*
	FILE NAME:	pid and calman.c
	DESCRIPTION:	the task dispose remote messages by the control rules
			and send the result to the relatively tasks
	
	EDIT RECORDS:
	---------
	AUTHOR:		the Seniors 
	EDIT TIME:	2017/11/18-13:47
	MODIFICATION:	built the frame of remote task, simply send the msg to chassis and holder.
	---------
	AUTHOR:		
	EDIT TIME:	
	MODIFICATION:	
	---------
*/
#include "pid.h"
//******************************************
PID_AbsoluteType Pid_speed[4];//�ٶȻ�
PID_IncreType Pid_current[4];//������
PID_AbsoluteType Pid_caterpillar[2];

//PID���£����������PID�ṹ�壬Ŀ��ֵ����ǰֵ��
//����ֵΪPID���
float PID_Update_Incre(PID_IncreType* PID,float tar,float cur)
{
	float ep,ei,ed;

	PID->e0 =tar - cur;  //��ȡ���ڵ�������kp����
	
	ep = PID->e0 - PID->e1;
	ei = PID->e0;
	ed = PID->e0 - 2*PID->e1 + PID->e2;
	
	PID->out += PID->kp*ep + PID->ki*ei + PID->kd*ed;
	
	PID->e2 = PID->e1;
	PID->e1 = PID->e0;	
	
	PID->out = range(PID->out, -PID->limit, PID->limit);//�����޷�
	
	return PID->out;
}

float PID_Update_Absolute2(PID_AbsoluteType* PID,float tar,float cur)
{
	float pe, ie, de;
	float out=0;
	
	PID->e0 = tar - cur;

	PID->eSum += PID->e0;
	
	de = PID->e0 - PID->e1;

	pe = PID->e0;
	ie = PID->eSum;

	PID->e1 = PID->e0;
	
	if((tar < 25) && (tar > -25))
	{
		PID->eSum = 0;
	}
	
	out = pe*(PID->Kp) + ie*(PID->Ki) + de*(PID->Kd);
	
	out = range(out, -PID->limit, PID->limit);
	return out;
}

float PID_Update_Absolute3(PID_AbsoluteType* PID,float tar,float cur)    
{
	float pe, ie, de;
	float out=0;
	
	PID->e0 = tar - cur;

	PID->eSum += PID->e0;
	
	PID->eSum = range(PID->eSum, -VUM_LIMIT, VUM_LIMIT);    //�����޷�
	
	de = PID->e0 - PID->e1;

	pe = PID->e0;
	ie = PID->eSum;

	PID->e1 = PID->e0;
	
  out = pe*(PID->Kp) + ie*(PID->Ki) + de*(PID->Kd);
	
	out = range(out, -PID->limit, PID->limit);
	return out;
}

float PID_Update_Absolute4(PID_AbsoluteType* PID,float tar,float cur)
{
	float pe, ie, de;
	float out=0;
	
	PID->e0 = tar - cur;

	PID->eSum += PID->e0;
	
  PID->eSum = range(PID->eSum, -SUM_LIMIT, SUM_LIMIT);
    
	de = PID->e0 - PID->e1;

	pe = PID->e0;
	ie = PID->eSum;

	PID->e1 = PID->e0;
	
    out = pe*(PID->Kp) + ie*(PID->Ki) + de*(PID->Kd);
	
    
    if(PID->e0 > 1500)                    
       out =  PID->limit;              
    else if(PID->e0 < -1500)           
       out =  -PID->limit;             
    
	out = range(out, -PID->limit, PID->limit);
	return out;
}

void PID_Struct_Init(PID_IncreType* PID,float kp,float ki,float kd,float out_max)
{
	PID->kp = kp;//����
	PID->ki = ki;//����
	PID->kd = kd;//΢��
	PID->e0 = 0;
	PID->e1 = 0;
	PID->e2 = 0;
	PID->limit = out_max;
	PID->out = 0;
}

void PID_Struct_Init_abs(PID_AbsoluteType* PID,float kp,float ki,float kd,float out_max)
{
	PID->Kp = kp;//����
	PID->Ki = ki;//����
	PID->Kd = kd;//΢��
	PID->e0 = 0;
	PID->e1 = 0;
	PID->eSum = 0;
	PID->limit = out_max;
}
void PID_Struct_Init_abs_nimi(PID_AbsoluteType* PID,float kp,float ki,float kd,float out_max)
{
	PID->Kp = kp;//����
	PID->Ki = ki;//����
	PID->Kd = kd;//΢��
	PID->limit = out_max;
}

void PID_speed_Init(float kp,float ki,float kd,float out_max)
{
	u32 i;
	for(i = 0;i < 4; i++)
	{
		PID_Struct_Init_abs(&Pid_speed[i],kp,ki,kd,out_max);
	}
}

void PID_caterpillar_Init(float kp,float ki,float kd,float out_max)
{
	u32 i;
	for(i = 0;i < 2; i++)
	{
		PID_Struct_Init_abs(&Pid_caterpillar[i],kp,ki,kd,out_max);
	}
}

void PID_current_Init(float kp,float ki,float kd,float out_max)
{
	u32 i;
	for(i = 0;i < 4; i++)
	{
		PID_Struct_Init(&Pid_current[i],kp,ki,kd,out_max);
	}	
}

/*******************************************************************************************************/

