10/30  resolver�������app�Ľ��չ���
	1.�ɽ���bar�����������0-255�ĵ�������
	2.�ɽ���app����������

	PS��ͨ���� makos_config.h �� RESOLVER_TYPE ������������ resolver��app���ܡ�

eg:

	void app_key_test(void * p_param)
	{
		printf("app_key1 has been down\r\n");
	}

	void app_bar_test(void * p_param)
	{
		U32 data;
		
		data = TO_U32(p_param);//����������
		
		printf("the bar is %d\r\n",data);
	}

	void task()
	{
		resolve_add(APP_KEY1, app_key_test);//���հ���
		resolve_add(APP_BAR1, app_bar_test);//��������������
	
	}
	
	

	
	
	
	
	
	
	