#include "mak_pid.h"
/**
  * @brief  绝对式PID初始化.
  * @param  PID结构体地址，P,I,D,积分限幅.
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
  * @brief  绝对式PID.
  * @param  目标值，实际值，PID结构体地址.
  * @note   .
  * @retval 需要输出的值.
  */
float PID_Update(PID_AbsoluteType_holder* PID,int16_t Target,int16_t Current)
{
	PID->errNow = -Target + Current;

	PID->errP = PID->errNow;  //读取现在的误差，用于kp控制
	
	PID->errI += PID->errNow; //误差积分，用于ki控制

	if(PID->errLim != 0)	   //微分上限和下限
	{
		if( PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//误差微分，用于kd控制

	PID->errOld = PID->errNow;	//保存现在的误差
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//计算绝对式PID输出
	
	return PID->ctrOut;
}

/***********************************************************************************************************************************************/

/**
  * @brief  增量式PID初始化.
  * @param  PID结构体地址，P,I,D,积分限幅，增量限幅.
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
  * @brief  增量式PID（可以进行增量限幅）.
  * @param  目标值，反馈值，PID结构体地址.
  * @note   返回的并非是增量，而直接是需要输出的值.
  * @retval 需要输出的量.
  */
float PID_IncrementMode(PID_IncrementType* PID,int16_t Target,int16_t Current)
{
	float dErrP, dErrI, dErrD;
	float out;
	
	PID->errNow = Target - Current;

	dErrP = PID->errNow - PID->errOld1;

	dErrI = PID->errNow;
	
	if(dErrI >= PID->ILim)//积分限幅
		dErrI = PID->ILim;
	if(dErrI <= -PID->ILim)
		dErrI = -PID->ILim;

	dErrD = PID->errNow - 2 * PID->errOld1 + PID->errOld2;

	/*增量式PID计算*/
	PID->dCtrOut = PID->kp * dErrP + PID->ki * dErrI + PID->kd * dErrD;
	
	PID->errOld2 = PID->errOld1; //二阶误差微分
	PID->errOld1 = PID->errNow;  //一阶误差微分
	
	if (PID->dCtrOut < -PID->IncLim)//增量限幅
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
  * @brief  卡尔曼滤波.
  * @param  .
  * @note   .
  * @retval .
  */
int16_t  KalmanFilter(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction)
{
	float R = MeasureNoise_R;//测量噪声
	float Q = ProcessNiose_Q;//过程噪声

	static        float x_last;

	float x_mid = x_last;
	float x_now;

	static        float p_last;

	float p_mid;
	float p_now;
	float kg;

	x_mid = x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
	p_mid = p_last + Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
	kg = p_mid / (p_mid + R); //kg为kalman filter，R为噪声
	x_now = x_mid + kg*(ResrcData - x_mid);//估计出的最优值

	p_now = (1 - kg)*p_mid;//最优值对应的covariance

	p_last = p_now; //更新covariance值
	x_last = x_now; //更新系统状态值

	return x_now;
}

