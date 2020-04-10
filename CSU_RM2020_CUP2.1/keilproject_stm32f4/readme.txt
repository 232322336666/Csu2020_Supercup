10/30  resolver添加蓝牙app的接收功能
	1.可接收bar数据条，获得0-255的调试数据
	2.可接受app按键触发。

	PS：通过在 makos_config.h 中 RESOLVER_TYPE 的设置来开启 resolver的app功能。

eg:

	void app_key_test(void * p_param)
	{
		printf("app_key1 has been down\r\n");
	}

	void app_bar_test(void * p_param)
	{
		U32 data;
		
		data = TO_U32(p_param);//数据条数据
		
		printf("the bar is %d\r\n",data);
	}

	void task()
	{
		resolve_add(APP_KEY1, app_key_test);//接收按键
		resolve_add(APP_BAR1, app_bar_test);//接收数据条数据
	
	}
	
	

	
	
	
	
	
	
	