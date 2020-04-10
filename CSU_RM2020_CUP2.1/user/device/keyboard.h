#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "headfile.h"

//使用读取管脚状态库函数定义   GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)
#define K_UP      GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)    //key4
#define K_LEFT    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)		//key2
#define K_RIGHT    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_10)		//key3
#define K_DOWN   GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15)	//key1

#define FifoSize      20

#define  CommonDelay(s)      task_delay_ms(s);
	
typedef enum
{
    Key_U,  
    Key_D,  

    Key_Y,  
    Key_N,
	
    Key_MAX,
} Key_e;  


typedef struct
{
    Key_e key;       
    uint8_t status;    
} Key_msg_t;

extern Key_msg_t KeyStatus;

void key_init(void);
uint8_t key_check(Key_e key);
uint8_t key_get(Key_e key);               
void key_scan(void);
uint8_t Get_KeyFifo(Key_msg_t *keymsg);

#endif
