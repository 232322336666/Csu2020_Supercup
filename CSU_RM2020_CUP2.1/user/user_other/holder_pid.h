#ifndef __PID_H__
#define __PID_H__
#include "stm32f4xx.h"
#include "task_holder.h"

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
	float tempkp[3];
	float err[3];
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
	float ERRMAX;
	float OUTMAX;
	float AccErr;
	
} PID_AbsoluteType_section;

typedef struct 
{
 /*PID算法接口变量，用于给用户获取或修改PID算法的特性*/
 float kp;     //比例系数
 float ki;     //积分系数
 float kd;     //微分系数
 
 float errNow; //当前的误差
 float dCtrOut;//控制增量输出
 float ctrOut;//控制输出
 
 float ILim;
 float IncLim;
 /*PID算法内部变量，其值不能修改*/
 float errOld1;
 float errOld2;
 
}PID_IncrementType;

struct HolderKalmanType
{
	float ResrcData;
	float ProcessNiose_Q;
	float MeasureNoise_R;
	float InitialPrediction;
	float x_last;
	float p_last;
	void (*holder_kalman_init)(float ProcessNiose_Q,float MeasureNoise_R,struct HolderKalmanType* Kalman);
	int16_t (*holder_KalmanFilter_cal)(const float ResrcData,struct HolderKalmanType* Kalman);
};

#define INFO_MAX 100
struct GetOldDataType
{
	int data_index;
	int info_max;
	int delay_num;
	float data_buff[INFO_MAX];
	
	void (*data_info_init)(int delay_num);
	void (*put_data)(struct GetOldDataType* data_info,float data);
	float (*get_data)(struct GetOldDataType* data_info);
};

extern
PID_IncrementType	pid_holder_x_speed;
extern
PID_IncrementType	pid_holder_y_speed;
extern
PID_AbsoluteType_holder	pid_holder_y_position;


void	holder_pid_init_absolute(PID_AbsoluteType_holder* PID,\
		float kp, float ki, float kd, float errlimit);

void	holder_pid_init_absolute_section(PID_AbsoluteType_section* PID,\
		float kp1,float kp2,float kp3, float ki, float kd, float errlimit,\
			float err1,float err2,float err3,float AccErr,float ERRMAX,float OUTMAX);

void	holder_pid_init_increment(PID_IncrementType* PID,\
		float kp, float ki, float kd, float ILim,float IncLim);

void PID_Init(PID_AbsoluteType_holder* PID);
float  PID_IncrementMode_Yaw(int16_t Target,int16_t Current,PID_IncrementType* PID);
float PID_Update_Yaw(int16_t Target,int16_t Current,PID_AbsoluteType_holder* PID);
float PID_Update_Yaw_index(int16_t Target,int16_t Current,PID_AbsoluteType_section* PID);

void holder_kalman_init (float ProcessNiose_Q,float MeasureNoise_R,struct HolderKalmanType* Kalman);
int16_t  holder_KalmanFilter_cal(const float ResrcData,struct HolderKalmanType* Kalman);

void old_data_init(struct GetOldDataType* data_info,int delay_num);
void put_data(struct GetOldDataType* data_info,float data);
float get_data(struct GetOldDataType* data_info);

#endif
