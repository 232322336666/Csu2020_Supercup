/**
  * @file 		pos_ctrl.c
  * @brief		位置环控制律，替代PD
  * @author		biu123biu
  * @date		19.9.13
  * @copyright    All rights reserved.
  * @tip:
  *     原理简单，v^2 = 2*a*x;符合加速度恒定的物理规律，理论上是最好的位置控制率。
  *     调节参数2：
  *             pctrl_t-v_max :最大跟踪速度
  *             pctrl_t-x_limit :速度回落临界距离 
  *     使用时切记与速度环同步，两者的绝对周期越小越好，5ms以内(1ms甚至)，以提高抗扰性，减少超调，加快响应。
  *     归一化速度与位置数据以方便不同控制对象，以及规范化调参步骤，先调大范围对应比例，再调设定参数(基准值标准化设定以方便按百分比变化范围调整参数)
  */
#include "pos_ctrl.h"

//#define XV2A(x,v)   (sqrt(2*x*v))
#define XV2A(x,v)   (v*v*0.5/x)
#define k_cal(realv_v,use_v)    ((float)realv_v/use_v)

/*
@func:符号函数
*/
int16_t Sign(float Input)
{
    int16_t output=0;
    if(Input>1E-6f) output=1;
    else if(Input<-1E-6f) output=-1;
    else output=0;
    return output;
}

void    pos_init(pctrl_t* pos)
{
    pos->v_max = 100;
    pos->x_limit = 50;
    pos->a = XV2A(pos->x_limit,pos->v_max);
    pos->vk = k_cal(2000,100); 
    pos->xk = k_cal(400,50);
}

float   pctrl_out(pctrl_t * pctrl,float tar_pos,float cur_pos){
    float   v_out;
    float   pos_err = (tar_pos - cur_pos)/pctrl->xk;
	if(pos_err < 0.1f && pos_err >-0.1f)
		pos_err = 0;
    if (abs_f(pos_err)>pctrl->x_limit)
        v_out = Sign(pos_err)*pctrl->v_max;//根据需要在此处反一下极性
    else
        v_out = Sign(pos_err)*sqrt(2*pctrl->a*abs_f(pos_err));
    return  v_out*pctrl->vk;
}

