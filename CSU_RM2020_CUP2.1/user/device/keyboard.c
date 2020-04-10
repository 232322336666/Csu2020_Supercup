#include "keyboard.h"

uint8_t KeyBox[Key_MAX] = {0, 1, 2, 3};
Key_msg_t  key_msg[FifoSize];            
volatile uint8_t      key_msg_front = 0, key_msg_rear = 0;   
volatile uint8_t      key_msg_flag = 0;          

void key_init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	 //PD3,PD6推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);	  //初始化GPIOD3,6

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14;	 //PD3,PD6推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	 //PD3,PD6推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	  //初始化GPIOD3,6		

}

uint8_t key_get(Key_e key)
{
  uint32_t key_state;
  switch(key)
  {
    case Key_U:
    {
      key_state = K_UP; 
    };break;
    case Key_D:
    {
      key_state = K_DOWN;  
    };break;
    case Key_Y:
    {
      key_state = K_RIGHT;  
    };break;
    case Key_N:
    {
      key_state = K_LEFT;  
    };break;   	
  }

  return key_state;
}

uint8_t key_check(Key_e key)
{
    if(key_get(key) == 0)
    {
        CommonDelay(10);      
        if( key_get(key) == 0)
        {
            return 0;
        }
    }
    return 1;
}

void SendToFifo(Key_msg_t keymsg)
{
    uint8_t temp = 0;
    if(key_msg_flag == 1)
    {
        return ;
    }
    
    key_msg[key_msg_rear].key = keymsg.key;
    key_msg[key_msg_rear].status = keymsg.status;

    key_msg_rear++;

    if(key_msg_rear >= FifoSize)
    {
        key_msg_rear = 0;                     
    }

    temp = key_msg_rear;
    if(temp == key_msg_front)                   
    {
        key_msg_flag = 1;
    }
    else
    {
        key_msg_flag = 2;
    }
}

uint8_t Get_KeyFifo(Key_msg_t *keymsg)
{
    uint8_t temp;

    if(key_msg_flag == 0)              
    {
        return 0;
    }

    keymsg->key = key_msg[key_msg_front].key;       
    keymsg->status = key_msg[key_msg_front].status; 

    key_msg_front++;                                

    if(key_msg_front >= FifoSize)         
    {
        key_msg_front = 0;                       
    }

    temp = key_msg_rear;
    if(key_msg_front == temp)                       
    {
        key_msg_flag = 0;
    }
    else
    {
        key_msg_flag = 2;
    }

    return 1;
}

void key_scan(void)
{
  Key_e num;
  uint8_t status = 0;
  Key_msg_t keymsg;
  static uint8_t keytime[Key_MAX];
	
  for(num = (Key_e)0; num < Key_MAX; num++)  
  {
    status = key_get(num);
    if(status == 0)      
    {
      keytime[num]++;
      if(keytime[num] < 2)     
      {
        continue;      
      }
      else if(keytime[num] == 2)   
      {
         keymsg.key = num;
         keymsg.status = 0;
         SendToFifo(keymsg);
      }
      else if(keytime[num] < 40)
      {
        continue;
      }
      else if(keytime[num] >= 40)
      {
         keymsg.key = num;
         keymsg.status = 1;        
         SendToFifo(keymsg);
         keytime[num] = 0;
		  
      }
    }
    else
    {
      if(keytime[num] >= 2)   
      {
        keymsg.key = num;
        keymsg.status = 2;        
        SendToFifo(keymsg);
        keytime[num] = 0;
      }
    }
  }
}
