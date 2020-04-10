#ifndef _USART_H
#define _USART_H

#include "makos_includes.h"
#include "task_Ano.h"

extern	USART_TypeDef*		pserial;
extern u8 TxBuffer[256];
extern u8 count;
void	serial_init(U32 num, U32 baudrate);
void	serial_handler(void);

void	usart1_init(void);
void    uasrt6_init(u32 baud);

void usart3_send_char(char ch);
void usart3_send_string(u8 *buff, u32 len);
void usart6_send_char(char ch);
void usart6_send_string(u8 *buff, u32 len);
void usart2_send_char(char ch);
void usart2_send_string(u8 *buff, u32 len);

void	serial_handler(void);

extern	voidf
	uif1, uif2, uif3, uif4, uif5;

void Usart_Send ( unsigned char *DataToSend , u8 data_num );
void print_wave(u8 number, u8 length, ...);
	
#endif
