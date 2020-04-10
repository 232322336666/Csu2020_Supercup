#include "task_init.h"
#include "headfile.h"

int	main(void)
{	
	serial_init(5, 115200);	//À¶ÑÀµ÷ÊÔ
//	uasrt6_init(115200);//×°¼×°åÊÓ¾õ
//	uart5_init_vs(115200);
	makos_init();
	
	task_create(task_init, NULL, 1);
	
	makos_run();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
	
	PRINT_ERROR("go back to main\r\n");
}
