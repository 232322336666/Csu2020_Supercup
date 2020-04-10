#include "task_led.h"
#include "headfile.h"

U8	ss[] = "hahahaha";
U8*	sp = ss;
U32	uu = 12345;
float	ff = -123.456;
void	mak_print_string(U8* data);

extern
volatile S16	liftf_read;
extern
volatile S16	liftb_read;

extern
size_t	iic_loop[10];
float test_test=5;
extern float test_temp_kp;
MLED_DEFINE(led1, B, 5, 0);
//MLED_DEFINE(led2, C, 7, 0);
//MLED_DEFINE(led3, C, 8, 0);
void	task_led_sys(void* param)
{
    mled_init(&led1);
//    mled_init(&led2);
//    mled_init(&led3);
//    task_insert(task_led_user, NULL, 4);

    while(1)
    {
        mled_toggle(&led1);
//        mled_toggle(&led2);
//        mled_toggle(&led3);

//        task_delay_ms(500);

//        mled_on(&led1);
//        mled_off(&led2);

//        task_delay_ms(500);

//        mled_on(&led1);
//        mled_on(&led2);

//        task_delay_ms(500);
//		
        task_delay_ms(500);
    }
}

U32	led_user_period	= 1200;

void	led_user_control(U32 ms)
{
    led_user_period = ms;
}

void app_key_test(void * p_param)
{
    printf("app_key1 has been down\r\n");
}

void app_key_test2(void * p_param)
{
    printf("app_key2 has been down\r\n");
}

void app_bar_test(void * p_param)
{
    U32 data;

    data = TO_U32(p_param);

    printf("the bar is %d\r\n", data);
}

//p_example_ctrl	data_example;
void	task_led_user(void* param)
{
//    s32 yaw_remote;
    p_mled	led_user = param;
    resolve_add((u8*)"led_user", (resolve_f)led_user_control);
    resolve_add(APP_KEY1, app_key_test);
    resolve_add(APP_KEY2, app_key_test2);
    resolve_add(APP_BAR1, app_bar_test);

    while(1)
    {
        if((led_user_period > 50) && (led_user_period < 10000))
        {
            mled_toggle(led_user);
            task_delay_ms(led_user_period);
        }
        else if(led_user_period == 0)
        {
            mled_off(led_user);
            task_delay_ms(1000);
        }
        else
        {
            PRINT("user led period should bewteen 50 and 10000(ms).\r\n");
            led_user_period	= 0;
        }

//        data_example	= msg_get_read_try(&example_msg);
//        if(data_example)
//        {
//            yaw_remote = data_example->yaw;
//            if(yaw_remote > 0)
//            {
//                mled_on(&led1);
//            }
//            else mled_off(&led1);
//            msg_read_finish_try(&example_msg);
//        }
		task_delay_ms(10);
    }
}

