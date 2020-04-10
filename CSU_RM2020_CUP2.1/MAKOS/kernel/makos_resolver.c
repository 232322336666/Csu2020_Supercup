#include "makos_resolver.h"
#include "makos_memory.h"
#include "makos_print.h"
#include "makos_dnode_queue.h"

#include "makos_config.h"
#include "stm32f4xx.h"

#include "usart.h"
#include "user_usart.h"

//****app_帧功能定义
//按键
u8 app_key1 = 0XB1;
u8 app_key2 = 0XB2;
u8 app_key3 = 0XB3;
u8 app_key4 = 0XB4;
u8 app_key5 = 0XB5;
u8 app_key6 = 0XB6;
//数据条
u8 app_bar1 = 0xF1;
u8 app_bar2 = 0xF2;
u8 app_bar3 = 0xF3;

dnode_t	dhead_resolve;
pipe_t	pipe_resolver;


void	resolver_init(void)
{
	dndqueue_init(&dhead_resolve);
	pipe_init(&pipe_resolver, 32);
	
	USART_ITConfig(pserial, USART_IT_RXNE, ENABLE);
}

void	resolve_add(U8* name, resolve_f func)
{
	p_resolve	pr;
	
	pr		= (p_resolve)mymalloc(SRAMIN,sizeof(resolve_t));
	pr->name	= (U8*)name;
	pr->func	= func;
	dndqueue_add_last(&dhead_resolve, &pr->dnd);
	
}

U8	string_compare(const U8* s1, const U8* s2, U32 length)
{
	U8*	ps1;
	U8*	ps2;
	U32	i;
	
	ps1	= (U8*)s1;
	ps2	= (U8*)s2;
	i	= length;
	while(i--)
	{
		if(*ps1 == 0 || *ps2 == 0 || *ps1 != *ps2)
			return 0;
		ps1++;
		ps2++;
	}
	return 1;
}

u8 sum_check(u8 * s1,u8 length, u8 sum)
{
	u8 sum_temp = 0;
	u8 i = 0;
	
	for(i = 2; i < length;i ++)
	{
		sum_temp += *(s1+i);
	}
	
	if(sum_temp == sum)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

U8	check_data(const U8* s1,u8 * data_type)
{
	U8*	ps1;
	ps1	= (U8*)s1;
	u8 length = 0;
	u8 target_sum;
	
	length = *(ps1+2);
	
	if(length == 5)
		target_sum = *(ps1 + 5);
	else
		target_sum = *(ps1 + 4);
	
	if(sum_check(ps1,length,target_sum))
	{
		if(*data_type == *(ps1 + 3))
			return 1;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}
U8	get_datatype(const U8* s1)
{
	U8*	ps1;
	ps1	= (U8*)s1;
	return *(ps1+2);
}

u16	app_getdata(U8* s1)
{	
	return *s1;
}

__inline
U32	pow_of_10(U8 p)
{
	U32	res;
	U8	i;
	
	res	= 1;
	for(i=p; i; i--)
		res	= res*10;
	
	return	res;
}

U32	string_get_U32(const U8* s, U32 length)
{
	U8*	ps;
	U32	res;
	U8	i;
	
	ps	= (U8*)s;
	res	= 0;
	for(i=length; i; i--)
	{
		res	= res + (*ps-0x30)*pow_of_10(i-1);
		ps++;
	}
	
	return res;
}

char*	cut_string(char* s, U32 length)
{
	U8*	ps;
	U8	i;
	char* temp_string;
	
	ps	= (U8*)s;
	
	for(i = 0; i < length; i--)
	{
		temp_string[i] =  *ps;
		ps++;		
	}
	
	return temp_string;
}	

// ?????????
// ??:str
// ??:???
float string_to_float(u8 *str)
{
     u8 i,j,k,negative=0;
    #define s_temp str
    float result=0,result_1=0;
    for(i=0;i<10;i++)
    {
         j=str[i];
        if(j==0||((j<'0'||j>'9')&&(j!='.')&&(j!='-')))break;             
    } 
    k=j=i;//?????
    for(i=0;i<j;i++)//????????,?????????i+1?
    {
         if(s_temp[i]=='.')break;         
    }
    
    for(j=0;j<i;j++)
    {
         if(s_temp[j]=='-')     {negative=1;continue;}        
         result=result*10+(s_temp[j]-'0');            
    }
    j++;//?1?j=i+1,???????
    i=j;//????????
    for(;j<k;j++)
    {
         if(s_temp[j]<'0'||s_temp[j]>'9')break;//????,??
        result_1=result_1*10+(s_temp[j]-'0');      
    }
    for(j=0;j<(k-i);j++)result_1*=0.1f;
    result+=result_1;
    
    if(negative)result=-result;
    return result;
}

void	task_resolver(void* param)
{
	U8		ch;
	U8		ch_last;
	p_dnode		curr;
	p_resolve	pr;
	U8		leftbrak;
//	u8 rightbrak;
	void * p_func_param;
//	u32 is_float = 0;
//	u8 string_temp[20];
//	float func_param_temp;
	u32 param_temp_u32;
	u8 data_type;
	
	resolver_init();
	
	curr	= &dhead_resolve;
	
	while(1)
	{
		retry:
		ch	= pipe_read(&pipe_resolver);
		#if(RESOLVER_TYPE == 0)
		if(ch == 0)
		{
			task_delay_ms(0);
			goto retry;
		}
		if(ch == 0x5A && ch_last == 0xA5)
		{
			leftbrak	= pipe_resolver.read-1 - pipe_resolver.start - 1;
		}
		
		ch_last = ch;
		
		if(ch == 0XAA)
		{
			
			curr	= dhead_resolve.next;
			while(curr != &dhead_resolve)//遍历所有resolve
			{
				pr	= CONTAINER_OF(curr, resolve_t, dnd);
				if(check_data(pipe_resolver.start,pr->name))
				{
					data_type = get_datatype(pipe_resolver.start);
					break;
				}
				curr	= curr->next;
			}
			if(curr != &dhead_resolve)//found matching name
			{
				if(data_type == 4)
				{
					p_func_param = NULL;
					pr->func(p_func_param);	
				}
				else
				{
					param_temp_u32 = app_getdata(pipe_resolver.start+leftbrak+4);
					p_func_param	= (void *)(&param_temp_u32);								
					pr->func(p_func_param);
				}
			}
			pipe_resolver.write	= pipe_resolver.start;
			pipe_resolver.read	= pipe_resolver.start;
			pipe_resolver.length	= 0;
			//PRINT("resolve pipe cleared\r\n");
		}
		#else		
		if(ch == 0)
		{
			task_delay_ms(0);
			goto retry;
		}
		if(ch == '(')
		{
			leftbrak	= pipe_resolver.read-1 - pipe_resolver.start;
		}
		
		if(ch == '.')
		{
			is_float = 1;
		}
		
		if(ch == ')')
		{
			rightbrak	= pipe_resolver.read-1 - pipe_resolver.start;
			
			curr	= dhead_resolve.next;
			while(curr != &dhead_resolve)//遍历所有resolve
			{
				pr	= CONTAINER_OF(curr, resolve_t, dnd);
				if(string_compare(pipe_resolver.start, pr->name, leftbrak))
					break;
				curr	= curr->next;
			}
			if(curr != &dhead_resolve)//found matching name
			{
				if(is_float == 1)
				{
					strncpy(string_temp,pipe_resolver.start+leftbrak+1,rightbrak-leftbrak-1);					
					func_param_temp = string_to_float(string_temp);
					p_func_param = (void *)(&func_param_temp);
				}
				else
				{
					param_temp_u32 = string_get_U32(pipe_resolver.start+leftbrak+1, rightbrak-leftbrak-1);
					p_func_param	= (void *)(&param_temp_u32);				
				}
				#ifdef	DEBUG_RESOLVER
				PRINT("***input received***\r\n");
				//PRINT("braks: %d/%d\r\n", leftbrak, rightbrak);
				if(is_float == 1)					
					PRINT("func: %s  param: %.2f\r\n", pr->name, *((float *)p_func_param));
				else
					PRINT("func: %s  param: %d\r\n", pr->name, *((U32 *)p_func_param));
				#endif
				pr->func(p_func_param);
				is_float = 0;				
			}
			else
			{
				PRINT("input cannot be recognized\r\n");
			}
			pipe_resolver.write	= pipe_resolver.start;
			pipe_resolver.read	= pipe_resolver.start;
			pipe_resolver.length	= 0;
			//PRINT("resolve pipe cleared\r\n");
		}		
		#endif
		
	}
}
