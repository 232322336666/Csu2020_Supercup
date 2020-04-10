#include "task_mems.h"
#include "headfile.h"
int16_t off_gyro_X;
void task_mems(void* param)
{
	u16 count_avg=0xffff;
	s32 avg_gyro_temp=0;
	int16_t avg_gyro;
	int16_t off_gyro;
//	float temp_off;
	
	off_gyro_X=0;
	SPI2_Init();//spi2初始化
//	task_delay_ms(10000);
	
//	mems_read_and_write(0xB901); //量程设为最小
	mems_read_and_write(0xB904); //量程设为最大
	mems_read_and_write(0xb601);//采样率
//	mems_read_and_write(0xBE02);//恢复出厂校准
//	mems_offset(-20);//清零offset
//	mems_read_and_write(0xBE01);//自动零值
//	task_delay_ms(1000);
	mems_read_and_write(0xb804);//数字滤波
	
//	mems_read_and_write(0xBE08);//写入flash
//	task_delay_ms(52);

	mems_read_and_write(0xBE80);//软件复位
	task_delay_ms(52);

	while(1)
	{
		if(count_avg!=0xfffe&&count_avg!=0xffff)//正在收集数据
		{
			mems_read_gyro_and_yaw();
			count_avg++;
			avg_gyro_temp+=mems_gyro;
			if(count_avg==38400)
			{
				avg_gyro_temp/=38400;
				count_avg=0xfffe;
			}
			task_delay_ms(4);
		}
		else if(count_avg==0xfffe)//将数据写入flash
		{
			avg_gyro=-(int16_t)(avg_gyro_temp);
			avg_gyro&=0xfff;
			mems_offset(avg_gyro);
			mems_read_and_write(0xBE08);
			task_delay_ms(52);
			count_avg=0xffff;
			task_delay_ms(4);
		}
		else if(count_avg==0xffff)//打印校正后的数据
		{
//			mems_offset(off_gyro_X);
			mems_read_gyro_and_yaw();
			
			mems_read_and_write(0x1400);
			off_gyro=mems_read_and_write(0x1400);
//			temp_off=off_gyro;
			if(off_gyro);//就是为了少报个warnings请淡定
			
//			print_wave(2,4,&mems_gyro_f,&mems_yaw_f);
//			print_wave(1,2,&avg_gyro);
//			print_wave(1,2,&mems_gyro);
			task_delay_ms(64);
		}
	}
}

