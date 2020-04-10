#include "timer.h"

//tim11微妙延时
void Delay_Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStruct.TIM_Period = 100-1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = (84-1);
    TIM_TimeBaseInit(TIM11, &TIM_TimeBaseInitStruct);
    
    while((TIM11->SR & TIM_FLAG_Update)!=SET);
    TIM11->SR = (uint16_t)~TIM_FLAG_Update;
}
//0到100us延时
void delay_us(uint32_t us_cnt)
{
	LIMIT(us_cnt,100,1);
    TIM11->CNT = us_cnt-1;
    TIM11->CR1 |= TIM_CR1_CEN;    
    while((TIM11->SR & TIM_FLAG_Update)!=SET);
    TIM11->SR = (uint16_t)~TIM_FLAG_Update;
    TIM11->CR1 &= ~TIM_CR1_CEN;
}
//检测can中断的时钟
void TIM12_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM12,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM12,TIM_IT_Update,ENABLE); //允许定时器12更新中断
	TIM_Cmd(TIM12,ENABLE); //使能定时器12
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM8_BRK_TIM12_IRQn; //定时器12中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器12中断服务函数
can_rate_t canrate;
void TIM8_BRK_TIM12_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM12,TIM_IT_Update)==SET)
    {
		canrate.send=canrate.inc;
		memset(&canrate.inc,0,sizeof(canrate.inc));
    }                
    TIM_ClearITPendingBit(TIM12,TIM_IT_Update);
}
