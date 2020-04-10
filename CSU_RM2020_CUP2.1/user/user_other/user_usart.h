#ifndef _USER_USART_H
#define _USER_USART_H

#include "makos_includes.h"

void	usart1_init(void);
void    uasrt6_init(u32 baud);

void usart2_send_char(char ch);
void usart2_send_string(u8 *buff, u32 len);
void uart5_send_char(char ch);
void uart5_send_string(u8 *buff, u32 len);
void usart3_send_char(char ch);
void usart3_send_string(u8 *buff, u32 len);
void usart6_send_char(char ch);
void usart6_send_string(u8 *buff, u32 len);

void print_wave(u8 number, u8 length, ...);

#endif
