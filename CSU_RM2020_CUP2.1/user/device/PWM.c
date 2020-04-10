#include "flash_data.h"
#include "PWM.h"

void PWM_init()
{
    TIM_TimeBaseInitTypeDef tim;
    GPIO_InitTypeDef gpio;
    TIM_OCInitTypeDef oc;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);                 //2019主控
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &gpio);

    TIM_DeInit(TIM1);

    tim.TIM_Prescaler = 168 - 1;                           //注意   频率调成50HZ左右就可以
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_ClockDivision = TIM_CKD_DIV2;                  //注意
    tim.TIM_Period = 20000;//20ms
    TIM_TimeBaseInit(TIM1, &tim);

    oc.TIM_OCMode = TIM_OCMode_PWM2;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OutputNState = TIM_OutputState_Disable;
    oc.TIM_Pulse = 0;
    oc.TIM_OCPolarity = TIM_OCPolarity_Low;
    oc.TIM_OCNPolarity = TIM_OCPolarity_High;
    oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
    oc.TIM_OCNIdleState = TIM_OCIdleState_Set;

    TIM_OC1Init(TIM1, &oc);
    TIM_OC2Init(TIM1, &oc);
    TIM_OC3Init(TIM1, &oc);
    TIM_OC4Init(TIM1, &oc);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);

    TIM1->CCR1 = 1000;
    TIM1->CCR2 = 1000;
    TIM1->CCR3 = 1000;
    TIM1->CCR4 = 1000;
    task_delay_ms(2000);

}

void TIM1_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
//	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;			
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse=420000;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);  

	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);


	
//	TIM_BDTRInitStructure.TIM_OSSRState=TIM_OSSRState_Enable;
//	TIM_BDTRInitStructure.TIM_OSSIState=TIM_OSSIState_Enable;
//	TIM_BDTRInitStructure.TIM_LOCKLevel=TIM_LOCKLevel_OFF;
//	TIM_BDTRInitStructure.TIM_DeadTime=10;
//	TIM_BDTRInitStructure.TIM_Break=TIM_Break_Disable;
//	TIM_BDTRInitStructure.TIM_BreakPolarity=TIM_BreakPolarity_Low;
//	TIM_BDTRInitStructure.TIM_AutomaticOutput=TIM_AutomaticOutput_Disable;
//	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStructure);


	TIM_Cmd(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}

void TIM8_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;			
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse=420000;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM8,&TIM_OCInitStructure);  

	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);

	TIM_Cmd(TIM8,ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
  
}

void TIM10_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM8);

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;			
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period=arr;
	TIM_TimeBaseStructure.TIM_Prescaler=psc;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse=420000;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM10,&TIM_OCInitStructure);  

	TIM_OC1PreloadConfig(TIM10,TIM_OCPreload_Enable);

	TIM_Cmd(TIM10,ENABLE);
//	TIM_CtrlPWMOutputs(TIM8,ENABLE);
  
}

void	enable_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SC_Discharge_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
}