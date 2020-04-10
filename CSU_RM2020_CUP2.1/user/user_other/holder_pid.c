#include "holder_pid.h"
#include "flash_data.h"
#include "makos_type.h"

/**				
  * @file 		  holder_pid.c/h
  * @brief		  ��̨�õ�PID����
  * @author		
  * @date		  2019-1-31
  * @copyright    CSU RM. All rights reserved.
  */
  
/**
  * @brief  ����ʽPID��ʼ��.
  * @param  PID�ṹ���ַ��P,I,D,�����޷�.
  * @note   .
  * @retval none.
  */
void	holder_pid_init_absolute(PID_AbsoluteType_holder* PID,float kp, float ki, float kd, float errlimit)
{
	PID->kp		= kp;
	PID->ki		= ki;
	PID->kd		= kd;
	PID->errLim 	= errlimit;
	PID->errNow= 0;
	PID->errP= 0;
	PID->errI= 0;
	PID->errD= 0;
	PID->errOld= 0;
	PID->ctrOut= 0;
}
/**
  * @brief  ����ʽPID��ʼ��.
  * @param  PID�ṹ���ַ��P1,P2,P3,I,D,�����޷������ֶε�0��1���ɽ��ܵ���������ERRMAX��������ֱֵ�������ֵOUTMAX��.
  * @note   .
  * @retval none.
  */
void	holder_pid_init_absolute_section(PID_AbsoluteType_section* PID,\
		float kp1,float kp2,float kp3, float ki, float kd, float errlimit,\
		float err0,float err1,float err2,float AccErr,float ERRMAX,float OUTMAX)
{
	PID->tempkp[0]		= kp1;
	PID->tempkp[1]		= kp2;
	PID->tempkp[2]		= kp3;
	PID->err[0]   =  err0;
	PID->err[1]   =  err1;		
	PID->err[1]   =  err2;	
	PID->ki		= ki;
	PID->kd		= kd;
	PID->errLim 	= errlimit;
	PID->errNow= 0;
	PID->errP= 0;
	PID->errI= 0;
	PID->errD= 0;
	PID->errOld= 0;
	PID->ctrOut= 0;
	PID->AccErr = AccErr;
	PID->ERRMAX = ERRMAX;
	PID->OUTMAX = OUTMAX;
}

/**
  * @brief  ����ʽPID��ʼ��.
  * @param  PID�ṹ���ַ��P,I,D,�����޷��������޷�.
  * @note   .
  * @retval none.
  */
void	holder_pid_init_increment(PID_IncrementType* PID,\
		float kp, float ki, float kd, float ILim,float IncLim)
{
	PID->kp		= kp;
	PID->ki		= ki;
	PID->kd		= kd;
	PID->ILim	= ILim;
	PID->IncLim = IncLim;
}
/**
  * @brief  ��ͨ����ʽPID.
  * @param  Ŀ��ֵ��ʵ��ֵ��PID�ṹ���ַ.
  * @note   .
  * @retval ��Ҫ�����ֵ.
  */
float PID_Update_Yaw(int16_t Target,int16_t Current,PID_AbsoluteType_holder* PID)
{
	PID->errNow = Target - Current;

	PID->errP = PID->errNow;  //��ȡ���ڵ�������kp����
	
	PID->errI += PID->errNow; //�����֣�����ki����

	if(PID->errLim != 0)	   //΢�����޺�����
	{
		if(     PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//���΢�֣�����kd����

	PID->errOld = PID->errNow;	//�������ڵ����
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//�������ʽPID���
	
	return PID->ctrOut;
}

extern PID_AbsoluteType_section	pid_holder_x_position_section;

/**
  * @brief  3��P����ʽPID
  * @param  Ŀ��ֵ������ֵ��PID�ṹ���ַ
  * @note   ����һ����Χ��ֱ�������ֵ����δʹ�û��ַ���.
  * @retval ���ؼ���������ֵ.
  */
float PID_Update_Yaw_index(int16_t Target,int16_t Current,PID_AbsoluteType_section* PID)
{

	PID->errNow = Target - Current;
	
	if(PID->errNow > -PID->err[0] && PID->errNow < PID->err[0])
		PID->kp = PID ->tempkp[2];
	else if(PID->errNow > -PID->err[1] && PID->errNow < PID->err[1])
		PID->kp = PID->tempkp[1];
	else
		PID->kp = PID->tempkp[0];

	PID->errP = PID->errNow;  //��ȡ���ڵ�������kp����
	
	PID->errI += PID->errNow; //�����֣�����ki����
		
	if(PID->errLim != 0)	   //΢�����޺�����
	{
		if(PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//���΢�֣�����kd����

	PID->errOld = PID->errNow;	//�������ڵ����
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//�������ʽPID���
	
	if(PID->errNow < -PID->ERRMAX)
		return -PID->OUTMAX;
	else if(PID->errNow > PID->ERRMAX)
		return PID->OUTMAX;
	else
	{
//		if(PID->ctrOut < -1800)
//			return -1800;
//		else if(PID->ctrOut > 1800)
//			return 1800;
//		else
			return PID->ctrOut;	
	}
}
/**
  * @brief  ��ͨ������ʽPID�����Խ��������޷���.
  * @param  Ŀ��ֵ������ֵ��PID�ṹ���ַ.
  * @note   ���صĲ�������������ֱ������Ҫ�����ֵ.
  * @retval ��Ҫ�������.
  */
float PID_IncrementMode_Yaw(int16_t Target,int16_t Current,PID_IncrementType* PID)
{
	float dErrP, dErrI, dErrD;
	float	out;
	
	PID->errNow = Target - Current;

	dErrP = PID->errNow - PID->errOld1;

	dErrI = PID->errNow;
	
	if(dErrI >= PID->ILim)
		dErrI = PID->ILim;
	if(dErrI <= -PID->ILim)
		dErrI = -PID->ILim;

//	if(PID->errNow * PID->errOld1 < 0)		//����˲�䲻��Ҫ����
//		dErrI = 0;

	dErrD = PID->errNow - 2 * PID->errOld1 + PID->errOld2;

 /*����ʽPID����*/
	PID->dCtrOut = PID->kp * dErrP + PID->ki * dErrI + PID->kd * dErrD;
	
	PID->errOld2 = PID->errOld1; //�������΢��
	PID->errOld1 = PID->errNow;  //һ�����΢��
	
	if (PID->dCtrOut < -PID->IncLim)
		PID->dCtrOut = -PID->IncLim;
	else if (PID->dCtrOut > PID->IncLim)
		PID->dCtrOut = PID->IncLim;
 
	PID->ctrOut += PID->dCtrOut;
	
	if(PID->ctrOut > 32767)			//��������Ҳ�������ֻ���������
		PID->ctrOut = 32767;
	if(PID->ctrOut < -32767)
		PID->ctrOut = -32767;
	out = (float)(PID->ctrOut);
	
	PID->ctrOut = (float)out;
	return out;
}

int16_t  KalmanFilter_pitch(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction)
{
	float R = MeasureNoise_R;//��������
	float Q = ProcessNiose_Q;//��������

	static        float x_last;

	float x_mid = x_last;
	float x_now;

	static        float p_last;

	float p_mid;
	float p_now;
	float kg;

	x_mid = x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
	p_mid = p_last + Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
	kg = p_mid / (p_mid + R); //kgΪkalman filter��RΪ����
	x_now = x_mid + kg*(ResrcData - x_mid);//���Ƴ�������ֵ

	p_now = (1 - kg)*p_mid;//����ֵ��Ӧ��covariance

	p_last = p_now; //����covarianceֵ
	x_last = x_now; //����ϵͳ״ֵ̬

	return x_now;
}

void holder_kalman_init (float ProcessNiose_Q,float MeasureNoise_R,struct HolderKalmanType* Kalman)
{
	Kalman->MeasureNoise_R = MeasureNoise_R;
	Kalman->ProcessNiose_Q = ProcessNiose_Q;
}
int16_t  holder_KalmanFilter_cal(const float ResrcData,struct HolderKalmanType* Kalman)
{
	float x_mid = Kalman->x_last;

	x_mid = Kalman->x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
	float p_mid = Kalman->p_last + Kalman->ProcessNiose_Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
	float kg = p_mid / (p_mid + Kalman->MeasureNoise_R); //kgΪkalman filter��RΪ����
	float x_now = x_mid + kg*(ResrcData - x_mid);//���Ƴ�������ֵ

	float p_now = (1 - kg)*p_mid;//����ֵ��Ӧ��covariance

	Kalman->p_last = p_now; //����covarianceֵ
	Kalman->x_last = x_now; //����ϵͳ״ֵ̬

	return x_now;
}

void old_data_init(struct GetOldDataType* data_info,int delay_num)
{
	data_info->delay_num = delay_num;
	data_info->info_max = INFO_MAX;
	data_info->data_index = 0;
}
void put_data(struct GetOldDataType* data_info,float data)
{
	if(data_info->data_index == data_info->info_max)
	{
		data_info->data_index = 0;
	}
	*(data_info->data_buff + data_info->data_index) = data;
	data_info->data_index ++;
}

float get_data(struct GetOldDataType* data_info)
{
	int read_index;
	if(data_info->data_index > data_info->delay_num)
		read_index = data_info->data_index - data_info->delay_num - 1;
	else
		read_index = data_info->data_index + data_info->info_max - 1 - data_info->delay_num;
	
	return data_info->data_buff[read_index];
}
