#ifndef __USART_DMA_H
#define	__USART_DMA_H

#include "stm32f4xx.h"
#include "protocol.h"
#include <stdio.h>


//USART
#define DEBUG_USART                       USART3
#define DEBUG_USART_CLK                   RCC_APB1Periph_USART3
#define DEBUG_USART_RX_GPIO_PORT          GPIOC
#define DEBUG_USART_RX_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define DEBUG_USART_RX_PIN                GPIO_Pin_11
#define DEBUG_USART_RX_AF                 GPIO_AF_USART3
#define DEBUG_USART_RX_SOURCE             GPIO_PinSource11

#define DEBUG_USART_TX_GPIO_PORT          GPIOC
#define DEBUG_USART_TX_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define DEBUG_USART_TX_PIN                GPIO_Pin_10
#define DEBUG_USART_TX_AF                 GPIO_AF_USART3
#define DEBUG_USART_TX_SOURCE             GPIO_PinSource10

#define DEBUG_USART_BAUDRATE              115200
#define DEBUG_USART_IRQ							USART3_IRQn
#define DEBUG_USART_IRQHandler					USART3_IRQHandler

//DMA_TX
#define DEBUG_USART_TX_DR_BASE               (USART3_BASE+0x04)		
#define SENDBUFF_SIZE                     28				//发送的数据量
#define DEBUG_USART_TX_DMA_CLK               RCC_AHB1Periph_DMA1	
#define DEBUG_USART_TX_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_TX_DMA_STREAM            DMA1_Stream3

//DMA_RX
#define DEBUG_USART_RX_DR_BASE               (USART3_BASE+0x04)		
#define READBUFF_SIZE                     1024				//发送的数据量
#define DEBUG_USART_RX_DMA_CLK               RCC_AHB1Periph_DMA1	
#define DEBUG_USART_RX_DMA_CHANNEL           DMA_Channel_4
#define DEBUG_USART_RX_DMA_STREAM            DMA1_Stream1
#define USART_RX_DMA_CHANNEL_IRQ		 		 	   DMA1_Stream1_IRQn
#define DMA_IT_TCIFX												 DMA_IT_TCIF1//通道4
#define DMA_IRQHandler   										 DMA1_Stream1_IRQHandler

extern uint8_t SendBuff[SENDBUFF_SIZE];
extern uint8_t ReadBuff[READBUFF_SIZE];
extern uint8_t ReadBuff2[READBUFF_SIZE];
extern uint8_t trans_ok;
extern uint8_t DMA_memory_id;
extern uint8_t uart_idle_times;
extern u32 uart_idle_G;
extern uint8_t uart_DMA_full_G;

void Debug_USART_Config(void);
void USART_DMA_TX_Config(void);
void USART_DMA_RX_Config(void);
void USART_DoubleDMA_RX_Config(void);

#endif /* __USART1_H */
