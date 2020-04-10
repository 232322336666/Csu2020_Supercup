#include "task_UI.h"
#include "UI_lib.h"
#include "keyboard.h"
#include "UI.h"
#include "oled.h"

#define 	task_delay_ms(s)	task_delay_ms(s)

void task_keyscan(void);

extern unsigned char pic_test[];
extern unsigned char pic_test2[][32];

void    task_UI()
{
	task_insert(task_keyscan, NULL, 3);	
	
	OLED_DrawBMP(33,0,33+58,7,pic_test,0);//开机动画显示
	 
	task_delay_ms(1500);             
													
    while(1)
    {		
		OS_menu();
		
        task_delay_ms(1000);
    }
}

void task_keyscan()
{	
	
	while(1)
	{
		key_scan();
		
		task_delay_ms(10);
	}
}
