/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重现c库printf函数到usart端口,使用DMA模式发送数据
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "uart_dma.h"


uint8_t SendBuff[SENDBUFF_SIZE];
uint8_t	ReadBuff[READBUFF_SIZE];
uint8_t	ReadBuff2[READBUFF_SIZE];



 /**
  * @brief  USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
			//2018
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
//		
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
//		
//		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_10 | GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
//		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);		

      //2019
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
		
		GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_8 | GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
			
			
  /* USART mode config */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(DEBUG_USART, &USART_InitStructure); 
  USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE); //????2??????
	USART_Cmd(DEBUG_USART, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel	= DEBUG_USART_IRQ;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}



/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
 DMA_InitTypeDef DMA_InitStructure_tx;

void USART_DMA_TX_Config(void)
{
 

  /*开启DMA时钟*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_TX_DMA_CLK, ENABLE);
  
  /* 复位初始化DMA数据流 */
  DMA_DeInit(DEBUG_USART_TX_DMA_STREAM);

  /* 确保DMA数据流复位完成 */
  while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx对应dma2，通道4，数据流7*/	
  DMA_InitStructure_tx.DMA_Channel = DEBUG_USART_TX_DMA_CHANNEL;  
  /*设置DMA源：串口数据寄存器地址*/
  DMA_InitStructure_tx.DMA_PeripheralBaseAddr = DEBUG_USART_TX_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure_tx.DMA_Memory0BaseAddr = (u32)SendBuff;
  /*方向：从内存到外设*/		
  DMA_InitStructure_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;	
  /*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure_tx.DMA_BufferSize = SENDBUFF_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：不断循环*/
  DMA_InitStructure_tx.DMA_Mode = DMA_Mode_Normal;	 //一次传输
  /*优先级：中*/	
  DMA_InitStructure_tx.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 16个节拍*/
  DMA_InitStructure_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*配置DMA2的数据流7*/		   
  DMA_Init(DEBUG_USART_TX_DMA_STREAM, &DMA_InitStructure_tx);
  
  /*使能DMA*/
  DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE);
  
  /* 等待DMA数据流有效*/
  while(DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != ENABLE)
  {
  }   
}

/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART_DMA_RX_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	
  /*开启DMA时钟*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_DMA_CLK, ENABLE);
  
  /* 复位初始化DMA数据流 */
  DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);

  /* 确保DMA数据流复位完成 */
  while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx对应dma2，通道4，数据流7*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_RX_DMA_CHANNEL;  
  /*设置DMA源：串口数据寄存器地址*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_RX_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReadBuff;
  /*方向：从内存到外设*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = READBUFF_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：不断循环*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*优先级：中*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 16个节拍*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*配置DMA2的数据流7*/		   
  DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);
  
  /*使能DMA*/
  DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);
  
  /* 等待DMA数据流有效*/
  while(DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != ENABLE)
  {
  }   
}

/**
  * @brief  USART1 TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
void USART_DoubleDMA_RX_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  /*开启DMA时钟*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_DMA_CLK, ENABLE);
  
  /* 复位初始化DMA数据流 */
  DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);

  /* 确保DMA数据流复位完成 */
  while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx对应dma2，通道4，数据流7*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_RX_DMA_CHANNEL;  
  /*设置DMA源：串口数据寄存器地址*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_RX_DR_BASE;	 
  /*内存地址(要传输的变量的指针)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReadBuff;
  /*方向：从内存到外设*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = READBUFF_SIZE;
  /*外设地址不增*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*内存地址自增*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*外设数据单位*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*内存数据单位 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMA模式：不断循环*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*优先级：中*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*禁用FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*存储器突发传输 16个节拍*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*外设突发传输 1个节拍*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	/*DMA double buffer mode*///??????
	DMA_DoubleBufferModeConfig(DEBUG_USART_RX_DMA_STREAM,(uint32_t)&ReadBuff2,DMA_Memory_0);//DMA_Memory_0?????
	DMA_DoubleBufferModeCmd(DEBUG_USART_RX_DMA_STREAM,ENABLE);
  /*配置DMA2的数据流7*/		   
  DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);
  
  /*使能DMA*/
  DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);
  
  /* 等待DMA数据流有效*/
  while(DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != ENABLE)
  {
  
	}
	
	NVIC_InitStructure.NVIC_IRQChannel = USART_RX_DMA_CHANNEL_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DEBUG_USART_RX_DMA_STREAM,DMA_IT_TC,ENABLE);
	
}

//IRQ_Handler
//DMA??????
uint8_t trans_ok = 0;
uint8_t DMA_memory_id = 0;
uint8_t uart_DMA_full_G = 0;


//void DMA_IRQHandler(void)
//{
//	if(DMA_GetITStatus(DEBUG_USART_RX_DMA_STREAM,DMA_IT_TCIFX) != RESET)
//	{
//		DMA_ClearITPendingBit(DEBUG_USART_RX_DMA_STREAM,DMA_IT_TCIFX);
//		DMA_memory_id = DMA_GetCurrentMemoryTarget(DEBUG_USART_RX_DMA_STREAM);
//		trans_ok ++;
//		uart_DMA_full_G = 1;
//	}
//}



uint8_t uart_idle_times = 0;
u8 Free_Read_Rst = 0;//?DR??????
u32 uart_idle_G = 0;

//裁判系统通信时开DEBUG_USART_IRQHandler();  云台板调试时开USART3_IRQHandler(),不可同时开

//void DEBUG_USART_IRQHandler()
//{
//	if(USART_GetITStatus(DEBUG_USART,USART_IT_IDLE) == SET)
//	{
//		Free_Read_Rst = DEBUG_USART->DR; //?USART_IT_IDLE??
//		uart_idle_times ++;
//		uart_idle_G = 1;
//	}
//}

/*********************************************END OF FILE**********************/
