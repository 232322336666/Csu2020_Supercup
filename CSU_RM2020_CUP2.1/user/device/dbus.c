/*
	FILE NAME:	dbus.c
	DESCRIPTION:	rewrote form receiver.c
	
	EDIT RECORDS:
	---------
	AUTHOR:		FAN YAOWEN
	EDIT TIME:	2018/3/30
	MODIFICATION:	rewrote the entire shit in receiver.
	---------
*/

#include "dbus.h"
#include "usart.h"
#include "user_usart.h"
#include "timer.h"

#define	DBUS_RESET_SPAN	((U32)3)//actually costs 5 ms to return to dma mode


U8	remote_buff[18];

USART_TypeDef*	pdbus;
GPIO_TypeDef*	gpiox;
U32		pin;
U8		irq_usart;
U8		irq_dma;
U32		channel;
DMA_Stream_TypeDef*
		stream;
U32		dma_it;

void	dbus_error_handle(void);
void	dbus_init(U32 num, voidf handle)
{
	if(num == 1)
	{
		pdbus		= USART1;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,	ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,	ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,	ENABLE);
		
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
		pin		= GPIO_Pin_10;
		gpiox		= GPIOA;
		
		irq_dma		= DMA2_Stream2_IRQn;
		irq_usart	= USART1_IRQn;
		DMA_DeInit(DMA2_Stream2);
		channel		= DMA_Channel_4;
		stream		= DMA2_Stream2;
		dma_it		= DMA_IT_TCIF2;
		
		uif1		= dbus_error_handle;
		
		dmaif22		= handle;
	}
	if(num == 3)
	{
		pdbus		= USART3;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,	ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,	ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,	ENABLE);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
		pin		= GPIO_Pin_11 ;
		gpiox		= GPIOC;
		
		irq_dma		= DMA1_Stream1_IRQn;
		irq_usart	= USART3_IRQn;
		DMA_DeInit(DMA1_Stream1);
		channel		= DMA_Channel_4;
		stream		= DMA1_Stream1;
		dma_it		= DMA_IT_TCIF1;
		
		uif3		= dbus_error_handle;
		
		dmaif11		= handle;
	}
	
	{
	GPIO_InitTypeDef	gpio;
	gpio.GPIO_Pin	= pin;
	gpio.GPIO_Mode	= GPIO_Mode_AF;
	gpio.GPIO_OType	= GPIO_OType_PP;
	gpio.GPIO_Speed	= GPIO_Speed_100MHz;
	gpio.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(gpiox, &gpio);
	}
	{	
	USART_InitTypeDef	usart;
	USART_DeInit(pdbus);
	usart.USART_BaudRate		= 100000;
	usart.USART_WordLength		= USART_WordLength_8b;
	usart.USART_StopBits		= USART_StopBits_1;
	usart.USART_Parity		= USART_Parity_Even;
	usart.USART_Mode		= USART_Mode_Rx;
	usart.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_Init(pdbus, &usart);
	}
	USART_Cmd(pdbus, ENABLE);
	{
	NVIC_InitTypeDef	nvic;
	nvic.NVIC_IRQChannel			= irq_dma;
	nvic.NVIC_IRQChannelPreemptionPriority	= 5;
	nvic.NVIC_IRQChannelSubPriority		= 5;
	nvic.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&nvic);
	
//	nvic.NVIC_IRQChannel			= irq_usart;
//	nvic.NVIC_IRQChannelPreemptionPriority	= 6;
//	nvic.NVIC_IRQChannelSubPriority		= 6;
//	nvic.NVIC_IRQChannelCmd			= ENABLE;
//	NVIC_Init(&nvic);
	}
	{
		DMA_InitTypeDef		dma;
		dma.DMA_Channel			= channel;
		dma.DMA_PeripheralBaseAddr	= (uint32_t)&(pdbus->DR);
		dma.DMA_Memory0BaseAddr		= (uint32_t)remote_buff;
		dma.DMA_DIR			= DMA_DIR_PeripheralToMemory;
		dma.DMA_BufferSize		= 18;
		dma.DMA_PeripheralInc		= DMA_PeripheralInc_Disable;
		dma.DMA_MemoryInc		= DMA_MemoryInc_Enable;
		dma.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
		dma.DMA_MemoryDataSize		= DMA_MemoryDataSize_Byte;
		dma.DMA_Mode			= DMA_Mode_Circular;
		dma.DMA_Priority		= DMA_Priority_VeryHigh;
		dma.DMA_FIFOMode		= DMA_FIFOMode_Disable;
		dma.DMA_FIFOThreshold		= DMA_FIFOThreshold_1QuarterFull;
		dma.DMA_MemoryBurst		= DMA_MemoryBurst_Single;
		dma.DMA_PeripheralBurst		= DMA_PeripheralBurst_Single;
		DMA_Init(stream,&dma);
	}
	
	DMA_ITConfig(stream, DMA_IT_TC | DMA_IT_TE | DMA_IT_FE,ENABLE);
	USART_DMACmd(pdbus, USART_DMAReq_Rx, ENABLE);
	
	dbus_mode_dma();
}

void	dbus_mode_dma(void)
{
	irq_close();
	USART_ITConfig(pdbus, USART_IT_RXNE, DISABLE);
	USART_ClearITPendingBit(pdbus, USART_IT_RXNE);
	DMA_Cmd(stream, ENABLE);
	irq_restore();
}

void	dbus_mode_usart(void)
{
	irq_close();
	DMA_Cmd(stream, DISABLE);
	DMA_ClearITPendingBit(stream, dma_it);
	//USART_ITConfig(pdbus, USART_IT_RXNE, ENABLE);
	irq_restore();
}

U32	dbus_buff_checked(void)
{
	U16	stick = (U16)((remote_buff[0] | (remote_buff[1]<<8)) & 0x07ff);
	
	if(	stick < 364 || stick > 1684	||
		remote_buff[12] > 1	)	//|| 		remote_buff[17] != 0)
	{
		dbus_error_ms	= system_clock;
		PRINT_RECORD("dbus error at %d\r\n", dbus_error_ms);
		dbus_mode_usart();
		return	0;
	}
	return	1;
}

U32	dbus_error_ms = 0;
void	dbus_error_handle(void)
{
	PRINT_RECORD("{");
	if(system_clock - dbus_error_ms > DBUS_RESET_SPAN)
	{
		dbus_mode_dma();
		PRINT_RECORD("dbus fix at %d\r\n", system_clock);
	}
}

static
void	emptyf(void)
{
	;
}

voidf	dmaif11 = emptyf;
voidf	dmaif22 = emptyf;

#include "irq_dead_check.h"

IRQ_CHECK_DEFINE(dma1);
//void	DMA1_Stream1_IRQHandler(void)
//{
//	IRQ_CHECK(dma1);
//	if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
//	{
//		DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
//		dmaif11();
//	}	
//	
//}


IRQ_CHECK_DEFINE(dma2);
void	DMA2_Stream2_IRQHandler(void)
{
	IRQ_CHECK(dma2);
	if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
	{
		DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
		dmaif22();
		canrate.inc.dbus++;
	}
}





