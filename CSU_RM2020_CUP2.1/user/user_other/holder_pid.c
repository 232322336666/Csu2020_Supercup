#include "holder_pid.h"
#include "flash_data.h"
#include "makos_type.h"

/**				
  * @file 		  holder_pid.c/h
  * @brief		  云台用的PID代码
  * @author		
  * @date		  2019-1-31
  * @copyright    CSU RM. All rights reserved.
  */
  
/**
  * @brief  增量式PID初始化.
  * @param  PID结构体地址，P,I,D,积分限幅.
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
  * @brief  增量式PID初始化.
  * @param  PID结构体地址，P1,P2,P3,I,D,积分限幅，误差分段点0，1，可接受的误差，最大误差ERRMAX（超过此值直接输出恒值OUTMAX）.
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
  * @brief  增量式PID初始化.
  * @param  PID结构体地址，P,I,D,积分限幅，增量限幅.
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
  * @brief  普通绝对式PID.
  * @param  目标值，实际值，PID结构体地址.
  * @note   .
  * @retval 需要输出的值.
  */
float PID_Update_Yaw(int16_t Target,int16_t Current,PID_AbsoluteType_holder* PID)
{
	PID->errNow = Target - Current;

	PID->errP = PID->errNow;  //读取现在的误差，用于kp控制
	
	PID->errI += PID->errNow; //误差积分，用于ki控制

	if(PID->errLim != 0)	   //微分上限和下限
	{
		if(     PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//误差微分，用于kd控制

	PID->errOld = PID->errNow;	//保存现在的误差
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//计算绝对式PID输出
	
	return PID->ctrOut;
}

extern PID_AbsoluteType_section	pid_holder_x_position_section;

/**
  * @brief  3段P绝对式PID
  * @param  目标值，反馈值，PID结构体地址
  * @note   误差超过一定范围即直接输出恒值，暂未使用积分分离.
  * @retval 返回计算出的输出值.
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

	PID->errP = PID->errNow;  //读取现在的误差，用于kp控制
	
	PID->errI += PID->errNow; //误差积分，用于ki控制
		
	if(PID->errLim != 0)	   //微分上限和下限
	{
		if(PID->errI >  PID->errLim)  
			PID->errI =  PID->errLim;
		else if(PID->errI <  -PID->errLim)  
			PID->errI = -PID->errLim;
	}
 
	PID->errD = PID->errNow - PID->errOld;//误差微分，用于kd控制

	PID->errOld = PID->errNow;	//保存现在的误差
 
	PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//计算绝对式PID输出
	
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
  * @brief  普通的增量式PID（可以进行增量限幅）.
  * @param  目标值，反馈值，PID结构体地址.
  * @note   返回的并非是增量，而直接是需要输出的值.
  * @retval 需要输出的量.
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

//	if(PID->errNow * PID->errOld1 < 0)		//反向瞬间不需要积分
//		dErrI = 0;

	dErrD = PID->errNow - 2 * PID->errOld1 + PID->errOld2;

 /*增量式PID计算*/
	PID->dCtrOut = PID->kp * dErrP + PID->ki * dErrI + PID->kd * dErrD;
	
	PID->errOld2 = PID->errOld1; //二阶误差微分
	PID->errOld1 = PID->errNow;  //一阶误差微分
	
	if (PID->dCtrOut < -PID->IncLim)
		PID->dCtrOut = -PID->IncLim;
	else if (PID->dCtrOut > PID->IncLim)
		PID->dCtrOut = PID->IncLim;
 
	PID->ctrOut += PID->dCtrOut;
	
	if(PID->ctrOut > 32767)			//或许这里也可以区分机器人类型
		PID->ctrOut = 32767;
	if(PID->ctrOut < -32767)
		PID->ctrOut = -32767;
	out = (float)(PID->ctrOut);
	
	PID->ctrOut = (float)out;
	return out;
}

int16_t  KalmanFilter_pitch(const float ResrcData, float ProcessNiose_Q, float MeasureNoise_R, float InitialPrediction)
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

void holder_kalman_init (float ProcessNiose_Q,float MeasureNoise_R,struct HolderKalmanType* Kalman)
{
	Kalman->MeasureNoise_R = MeasureNoise_R;
	Kalman->ProcessNiose_Q = ProcessNiose_Q;
}
int16_t  holder_KalmanFilter_cal(const float ResrcData,struct HolderKalmanType* Kalman)
{
	float x_mid = Kalman->x_last;

	x_mid = Kalman->x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
	float p_mid = Kalman->p_last + Kalman->ProcessNiose_Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
	float kg = p_mid / (p_mid + Kalman->MeasureNoise_R); //kg为kalman filter，R为噪声
	float x_now = x_mid + kg*(ResrcData - x_mid);//估计出的最优值

	float p_now = (1 - kg)*p_mid;//最优值对应的covariance

	Kalman->p_last = p_now; //更新covariance值
	Kalman->x_last = x_now; //更新系统状态值

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
