/**
  ******************************************************************************
  * @file    bsp_usart_dma.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����c��printf������usart�˿�,ʹ��DMAģʽ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "uart_dma.h"


uint8_t SendBuff[SENDBUFF_SIZE];
uint8_t	ReadBuff[READBUFF_SIZE];
uint8_t	ReadBuff2[READBUFF_SIZE];



 /**
  * @brief  USART GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
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
  * @brief  USART1 TX DMA ���ã��ڴ浽����(USART1->DR)
  * @param  ��
  * @retval ��
  */
 DMA_InitTypeDef DMA_InitStructure_tx;

void USART_DMA_TX_Config(void)
{
 

  /*����DMAʱ��*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_TX_DMA_CLK, ENABLE);
  
  /* ��λ��ʼ��DMA������ */
  DMA_DeInit(DEBUG_USART_TX_DMA_STREAM);

  /* ȷ��DMA��������λ��� */
  while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx��Ӧdma2��ͨ��4��������7*/	
  DMA_InitStructure_tx.DMA_Channel = DEBUG_USART_TX_DMA_CHANNEL;  
  /*����DMAԴ���������ݼĴ�����ַ*/
  DMA_InitStructure_tx.DMA_PeripheralBaseAddr = DEBUG_USART_TX_DR_BASE;	 
  /*�ڴ��ַ(Ҫ����ı�����ָ��)*/
  DMA_InitStructure_tx.DMA_Memory0BaseAddr = (u32)SendBuff;
  /*���򣺴��ڴ浽����*/		
  DMA_InitStructure_tx.DMA_DIR = DMA_DIR_MemoryToPeripheral;	
  /*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure_tx.DMA_BufferSize = SENDBUFF_SIZE;
  /*�����ַ����*/	    
  DMA_InitStructure_tx.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*�ڴ��ַ����*/
  DMA_InitStructure_tx.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*�������ݵ�λ*/	
  DMA_InitStructure_tx.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_InitStructure_tx.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMAģʽ������ѭ��*/
  DMA_InitStructure_tx.DMA_Mode = DMA_Mode_Normal;	 //һ�δ���
  /*���ȼ�����*/	
  DMA_InitStructure_tx.DMA_Priority = DMA_Priority_Medium;      
  /*����FIFO*/
  DMA_InitStructure_tx.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure_tx.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*�洢��ͻ������ 16������*/
  DMA_InitStructure_tx.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*����ͻ������ 1������*/
  DMA_InitStructure_tx.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*����DMA2��������7*/		   
  DMA_Init(DEBUG_USART_TX_DMA_STREAM, &DMA_InitStructure_tx);
  
  /*ʹ��DMA*/
  DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE);
  
  /* �ȴ�DMA��������Ч*/
  while(DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != ENABLE)
  {
  }   
}

/**
  * @brief  USART1 TX DMA ���ã��ڴ浽����(USART1->DR)
  * @param  ��
  * @retval ��
  */
void USART_DMA_RX_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	
  /*����DMAʱ��*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_DMA_CLK, ENABLE);
  
  /* ��λ��ʼ��DMA������ */
  DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);

  /* ȷ��DMA��������λ��� */
  while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx��Ӧdma2��ͨ��4��������7*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_RX_DMA_CHANNEL;  
  /*����DMAԴ���������ݼĴ�����ַ*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_RX_DR_BASE;	 
  /*�ڴ��ַ(Ҫ����ı�����ָ��)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReadBuff;
  /*���򣺴��ڴ浽����*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = READBUFF_SIZE;
  /*�����ַ����*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*�ڴ��ַ����*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*�������ݵ�λ*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMAģʽ������ѭ��*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*���ȼ�����*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*����FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*�洢��ͻ������ 16������*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*����ͻ������ 1������*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
  /*����DMA2��������7*/		   
  DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);
  
  /*ʹ��DMA*/
  DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);
  
  /* �ȴ�DMA��������Ч*/
  while(DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != ENABLE)
  {
  }   
}

/**
  * @brief  USART1 TX DMA ���ã��ڴ浽����(USART1->DR)
  * @param  ��
  * @retval ��
  */
void USART_DoubleDMA_RX_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  /*����DMAʱ��*/
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_DMA_CLK, ENABLE);
  
  /* ��λ��ʼ��DMA������ */
  DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);

  /* ȷ��DMA��������λ��� */
  while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE)  {
  }

  /*usart1 tx��Ӧdma2��ͨ��4��������7*/	
  DMA_InitStructure.DMA_Channel = DEBUG_USART_RX_DMA_CHANNEL;  
  /*����DMAԴ���������ݼĴ�����ַ*/
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_RX_DR_BASE;	 
  /*�ڴ��ַ(Ҫ����ı�����ָ��)*/
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)ReadBuff;
  /*���򣺴��ڴ浽����*/		
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;	
  /*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
  DMA_InitStructure.DMA_BufferSize = READBUFF_SIZE;
  /*�����ַ����*/	    
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
  /*�ڴ��ַ����*/
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	
  /*�������ݵ�λ*/	
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /*�ڴ����ݵ�λ 8bit*/
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	
  /*DMAģʽ������ѭ��*/
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 
  /*���ȼ�����*/	
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;      
  /*����FIFO*/
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;    
  /*�洢��ͻ������ 16������*/
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;    
  /*����ͻ������ 1������*/
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	/*DMA double buffer mode*///??????
	DMA_DoubleBufferModeConfig(DEBUG_USART_RX_DMA_STREAM,(uint32_t)&ReadBuff2,DMA_Memory_0);//DMA_Memory_0?????
	DMA_DoubleBufferModeCmd(DEBUG_USART_RX_DMA_STREAM,ENABLE);
  /*����DMA2��������7*/		   
  DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);
  
  /*ʹ��DMA*/
  DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);
  
  /* �ȴ�DMA��������Ч*/
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

//����ϵͳͨ��ʱ��DEBUG_USART_IRQHandler();  ��̨�����ʱ��USART3_IRQHandler(),����ͬʱ��

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
