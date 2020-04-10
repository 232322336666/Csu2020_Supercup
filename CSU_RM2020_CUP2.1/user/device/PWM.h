#ifndef _PWM_H
#define _PWM_H

#define SC_GPIO           GPIOC
#define SC_GPIO_Pin       GPIO_Pin_0

#include "makos_includes.h"
void TIM1_Init(u16 arr, u16 psc);
void TIM8_Init(u16 arr, u16 psc);
void TIM10_Init(u16 arr, u16 psc);
void PWM_init(void);
void enable_init(void);
void SC_Discharge_init();
#endif

