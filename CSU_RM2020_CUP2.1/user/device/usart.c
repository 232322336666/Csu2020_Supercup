#include "usart.h"
#include "user_usart.h"
#include "can.h"
#include "user_can.h"
#include "task_holder.h"
#include "makos_print.h"
#include "makos_resolver.h"
#include "task_nimin.h"
#include "data_fifo.h"
#include "communicate_nimin.h"
#include "nimin_info.h"
#include "stdarg.h"



USART_TypeDef*		pserial;

static
void	emptyf(void)
{
	;
}

voidf	uif1 = emptyf;
voidf	uif2 = emptyf;
voidf	uif3 = emptyf;
voidf	uif4 = emptyf;
voidf	uif5 = emptyf;
voidf	uif6 = emptyf;
U32	serial_init_flag = 0;

//串口5初始化（视觉接收用）
void uart5_init_vs(U32 baudrate)
{
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate		= baudrate;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_WordLength		= USART_WordLength_8b;
	USART_Init(UART5, &USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(UART5, ENABLE);
}


void    uasrt6_init(u32 baud)
{
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
		
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
		
	NVIC_InitStructure.NVIC_IRQChannel	= USART6_IRQn;
				
	USART_InitStructure.USART_BaudRate		= baud;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_WordLength		= USART_WordLength_8b;
	USART_Init(USART6, &USART_InitStructure);
	
	USART_Cmd(USART6, ENABLE);
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void	serial_init(U32	num, U32 baudrate)
{
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	if(serial_init_flag)
		printf("serial_init() can't be excuted twice\r\n");
	else
		serial_init_flag = 1;
	
	if(num == 0)
		while(1);
	if(num == 1)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
			
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);//?????
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_9 | GPIO_Pin_10;   //????
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		pserial	= USART1;
		
		NVIC_InitStructure.NVIC_IRQChannel	= USART1_IRQn;
		
		uif1	= serial_handler;
	}
	if(num == 2)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);//?????
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_5 | GPIO_Pin_6;   //????
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		pserial	= USART2;
		
		NVIC_InitStructure.NVIC_IRQChannel	= USART2_IRQn;
		
		uif2	= serial_handler;
	}
	if(num == 3)
	{
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		pserial	= USART3;
		
		NVIC_InitStructure.NVIC_IRQChannel	= USART3_IRQn;
		
		uif3	= serial_handler;

	}
	if(num == 4)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		pserial	= UART4;
		
		NVIC_InitStructure.NVIC_IRQChannel	= UART4_IRQn;
		
		uif4	= serial_handler;
	}
	if(num == 5)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		pserial	= UART5;
		
		NVIC_InitStructure.NVIC_IRQChannel	= UART5_IRQn;
		
		uif5	= serial_handler;
	}
	if(num == 6)
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		pserial	= USART6;
		
		NVIC_InitStructure.NVIC_IRQChannel	= USART6_IRQn;
		
		uif6	= serial_handler;

	}
	USART_InitStructure.USART_BaudRate		= baudrate;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_WordLength		= USART_WordLength_8b;
	USART_Init(pserial, &USART_InitStructure);
	
	USART_Cmd(pserial, ENABLE);
	
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u8 TxBuffer[256];
u8 count = 0;

void Usart_Send ( unsigned char *DataToSend , u8 data_num )
{
    u8 i;
    for ( i = 0; i < data_num; i++ )
    {
        TxBuffer[count++] = * ( DataToSend + i );
    }

    if ( ! ( pserial->CR1 & USART_CR1_TXEIE ) )
    {
        USART_ITConfig ( pserial, USART_IT_TXE, ENABLE ); //打开发送中断
    }
}

void serial_handler(void)
{
//	USART_ClearITPendingBit(pserial, USART_IT_RXNE);
//	 fifo_s_put(&rx_fifo,pserial->DR);              //老匿名
//	if(pipe_resolver.length < 32)                 //山外
//		pipe_write(&pipe_resolver, pserial->DR);
//	u8 com_data;
//	if ( USART2->SR & USART_SR_ORE ) //ORE中断（溢出中断）
//    {
//        com_data = USART2->DR;
//    }

//    //接收中断
//    if ( USART_GetITStatus ( USART2, USART_IT_RXNE ) )
//    {
//        USART_ClearITPendingBit ( USART2, USART_IT_RXNE ); //清除中断标志

//        com_data = USART2->DR;
//		//	printf("data = %x\n",com_data);
//        ANO_DT_Data_Receive_Prepare ( com_data ); //接收处理
//    }
//    //发送（进入移位）中断
//    if ( USART_GetITStatus ( USART2, USART_IT_TXE ) )
//    {

//        USART2->DR = TxBuffer[TxCounter++]; //写DR清除中断标志
//        if ( TxCounter == count )
//        {
//            USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
//        }

//        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
//    }	
}

int	fputc(int ch, FILE *f)//自定义printf重定向
{
	if(system_param.use_print && print_flag){
		pipe_write(&ptcb_curr->pipe_print, (U8)ch);
	}else if(record_flag){
		if(record_write < 512)
			record_buff[record_write++] = (U8)ch;
	}else{
		while(((pserial->SR)&0X40)==0);
		pserial->DR	= (u8)ch;
	}
	return ch;
}


#pragma import(__use_no_semihosting)


struct __FILE 
{ 
	int handle; 

};

typedef	struct __FILE	FILE;

FILE __stdout;       

_sys_exit(int x) 
{ 
	x = x; 
} 
