/**
  * @file 		pos_ctrl.h
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
#ifndef __POS_CTRL_H
#define __POS_CTRL_H

#define abs_f(x)               ((x)>0?(x):-(x))

#include "math.h"
#include "stdio.h"
#include "stdint.h"
	
typedef struct
{
    float   x_limit;//界限调整距离
    float   v_max;//最大速度
    float   a;
    float   vk;//速度归一化比例系数
    float   xk;//位置归一化比例系数
}pctrl_t;

void    pos_init(pctrl_t* pos);
float   pctrl_out(pctrl_t * pctrl,float tar_pos,float cur_pos);

#endif
