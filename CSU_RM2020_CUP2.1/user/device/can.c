/*
	FILE NAME:	can.c
	DESCRIPTION:	the task dispose remote messages by the control rules
			and send the result to the relatively tasks
	
	EDIT RECORDS:
	---------
	AUTHOR:                               
	EDIT TIME:	2017/11/18-13:47
	MODIFICATION:	built the frame of remote task, simply send the msg to chassis and holder.
	---------
	AUTHOR:		
	EDIT TIME:	
	MODIFICATION:	
	---------
*/
#include "can.h"
#include "user_can.h"
#include "task_holder.h"
#include "motor.h"
#include "flash_data.h"
#include "timer.h"

void CAN1_Configuration(uint16_t IdHigh,uint16_t IdLow)
{
	CAN1_GPIO_Configuration();
	CAN1_NVIC_Configuration();
	CAN1_mode_Configuration(IdHigh,IdLow);
}

void CAN1_GPIO_Configuration()
{
	GPIO_InitTypeDef gpio;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	
	gpio.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&gpio);
}

void CAN1_NVIC_Configuration()
{
	NVIC_InitTypeDef nvic;
	nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority =0;
	nvic.NVIC_IRQChannelSubPriority = 0; 
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

void CAN1_mode_Configuration(uint16_t IdHigh, uint16_t IdLow)
{
	CAN_InitTypeDef can;
	CAN_FilterInitTypeDef can_filter;
	
	CAN_DeInit(CAN1);
	CAN_StructInit(&can);
	
	can.CAN_TTCM=DISABLE;
	can.CAN_ABOM=DISABLE;
	can.CAN_AWUM=DISABLE;
	can.CAN_NART=DISABLE;
	can.CAN_RFLM=DISABLE;
	can.CAN_TXFP=ENABLE;
	can.CAN_Mode=CAN_Mode_Normal;
	can.CAN_SJW=CAN_SJW_1tq;
	can.CAN_BS1=CAN_BS1_9tq;
	can.CAN_BS2=CAN_BS2_4tq;
	can.CAN_Prescaler=3;//???42/(1+9+4)/3=1Mbps
	CAN_Init(CAN1,&can);
	
	can_filter.CAN_FilterNumber=0;
	can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale=CAN_FilterScale_32bit;

	can_filter.CAN_FilterIdHigh = IdHigh ;
	can_filter.CAN_FilterIdLow = IdLow ;
	can_filter.CAN_FilterMaskIdHigh=0x0000;
	can_filter.CAN_FilterMaskIdLow=0x0000;
	can_filter.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

void can2_Configuration(uint16_t IdHigh,uint16_t IdLow)
{
	can2_GPIO_Configuration();
	can2_NVIC_Configuration();
	can2_mode_Configuration(IdHigh,IdLow);
}

void can2_GPIO_Configuration()//CAN_TX  CAN_RX  gpio
{	
	GPIO_InitTypeDef gpio;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOB,&gpio);
}

void can2_NVIC_Configuration(void)
{
 	NVIC_InitTypeDef nvic;
	
	nvic.NVIC_IRQChannel =CAN2_RX0_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority =0;
	nvic.NVIC_IRQChannelSubPriority =0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = CAN2_TX_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
}

void can2_mode_Configuration(uint16_t IdHigh,uint16_t IdLow)
{
	CAN_InitTypeDef can;
	CAN_FilterInitTypeDef can_filter;
	
	CAN_DeInit(CAN2);
	CAN_StructInit(&can);
	
	can.CAN_TTCM=DISABLE;
	can.CAN_ABOM=DISABLE;
	can.CAN_AWUM=DISABLE;
	can.CAN_NART=DISABLE;
	can.CAN_RFLM=DISABLE;
	can.CAN_TXFP=ENABLE;
	can.CAN_Mode=CAN_Mode_Normal;
	can.CAN_SJW=CAN_SJW_1tq;
	can.CAN_BS1=CAN_BS1_9tq;
	can.CAN_BS2=CAN_BS2_4tq;
	can.CAN_Prescaler=3;
	CAN_Init(CAN2,&can);
	
	can_filter.CAN_FilterNumber=14;//???0
	can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;//??????????
	can_filter.CAN_FilterScale=CAN_FilterScale_32bit;//??????32?
	can_filter.CAN_FilterIdHigh = IdHigh ;//???ID???
	can_filter.CAN_FilterIdLow = IdLow ;//???ID???
	can_filter.CAN_FilterMaskIdHigh=0x0000;//????16????????
	can_filter.CAN_FilterMaskIdLow=0x0000;///????16????????
	can_filter.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0 ;//??????FIFO0;#elif			(MCU_TYPE == HOLDER)
//	can_filter.CAN_FilterIdHigh = 0x0000 ;//过滤的ID的高位
//	can_filter.CAN_FilterIdLow = 0x0000 ;//过滤的ID的低位
//	can_filter.CAN_FilterMaskIdHigh=0x0000;//过滤器高16位每一位必须匹配
//	can_filter.CAN_FilterMaskIdLow=0x0000;///过滤器低16位每一位必须匹配
//	can_filter.CAN_FilterFIFOAssignment=0 ;//过滤器关联到FIFO0;
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);
}

//
void	can_set(CanTxMsg* msg, U32 id, U8* data)
{
	size_t	i;
	
	msg->StdId	= id;
	msg->IDE	= CAN_ID_STD;
	msg->RTR	= CAN_RTR_DATA;
	msg->DLC	= 8;
	for(i=0; i<8; i++)
	{
		msg->Data[i]	= data[i];
	}
}

void	can_set_id(CanTxMsg* msg, U32 id)
{
	msg->StdId	= id;
	msg->IDE	= CAN_ID_STD;
	msg->RTR	= CAN_RTR_DATA;
	msg->DLC	= 8;
}
