#include "mak_pid.h"
/**
  * @brief  ����ʽPID��ʼ��.
  * @param  PID�ṹ���ַ��P,I,D,�����޷�.
  * @note   .
  * @retval none.
  */
void pid_init_absolute(PID_AbsoluteType_holder* PID,float kp, float ki, float kd, float errlimit)
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
  * @brief  ����ʽPID.
  * @param  Ŀ��ֵ��ʵ��ֵ��PID�ṹ���ַ.
  * @note   .
  * @retval ��Ҫ�����ֵ.
  */
float PID_Update(PID_AbsoluteType_holder* PID,int16_t Target,int16_t Current)
{
	PID->errNow = -Target + Current;

	PID->errP = PID->errNow;  //��ȡ���ڵ�������kp����
	
	PID->errI += PID->errNow; //�����֣�����ki����

	if(PID->errLim != 0)	   //΢�����޺�����
	{
		if( PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//���΢�֣�����kd����

	PID->errOld = PID->errNow;	//�������ڵ����
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//�������ʽPID���
	
	return PID->ctrOut;
}

/***********************************************************************************************************************************************/

/**
  * @brief  ����ʽPID��ʼ��.
  * @param  PID�ṹ���ַ��P,I,D,�����޷��������޷�.
  * @note   .
  * @retval none.
  */
void	pid_init_increment(PID_IncrementType* PID,float kp, float ki, float kd, float ILim,float IncLim)
{
	PID->kp		= kp;
	PID->ki		= ki;
	PID->kd		= kd;
	PID->ILim	= ILim;
	PID->IncLim = IncLim;
}
/**
  * @brief  ����ʽPID�����Խ��������޷���.
  * @param  Ŀ��ֵ������ֵ��PID�ṹ���ַ.
  * @note   ���صĲ�������������ֱ������Ҫ�����ֵ.
  * @retval ��Ҫ�������.
  */
float PID_IncrementMode(PID_IncrementType* PID,int16_t Target,int16_t Current)
{
	float dErrP, dErrI, dErrD;
	float out;
	
	PID->errNow = Target - Current;

	dErrP = PID->errNow - PID->errOld1;

	dErrI = PID->errNow;
	
	if(dErrI >= PID->ILim)//�����޷�
		dErrI = PID->ILim;
	if(dErrI <= -PID->ILim)
		dErrI = -PID->ILim;

	dErrD = PID->errNow - 2 * PID->errOld1 + PID->errOld2;

	/*����ʽPID����*/
	PID->dCtrOut = PID->kp * dErrP + PID->ki * dErrI + PID->kd * dErrD;
	
	PID->errOld2 = PID->errOld1; //�������΢��
	PID->errOld1 = PID->errNow;  //һ�����΢��
	
	if (PID->dCtrOut < -PID->IncLim)//�����޷�
		PID->dCtrOut = -PID->IncLim;
	else if (PID->dCtrOut > PID->IncLim)
		PID->dCtrOut = PID->IncLim;
 
	PID->ctrOut += PID->dCtrOut;
	
	out = (float)(PID->ctrOut);
	
	PID->ctrOut = (float)out;
	return out;
}
/************************************************************/
/**
  * @brief  �������˲�.
  * @param  .
  * @note   .
  * @retval .
  */
int16_t  KalmanFilter(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction)
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

