#ifndef _MAKOS_MEMORY_H
#define _MAKOS_MEMORY_H

#include "makos_type.h"
#ifndef NULL
#define NULL 0
#endif

//定义2个内存池
#define SRAMIN	 0		//内部内存池
#define SRAMEX   1		//外部内存池
#define SRAMCCM  2		//CCM内存池(此部分SRAM仅仅CPU可以访问!!!)

#define SRAMBANK 	3	//定义支持的SRAM块数.	


//mem1内存参数设定.mem1完全处于内部SRAM里面.
#define MEM1_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM1_MAX_SIZE			70*1024  						//最大管理内存 100K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小,内存块数量

//mem2内存参数设定.mem2的内存池处于外部SRAM里面
#define MEM2_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM2_MAX_SIZE			960 *1024  						//最大管理内存960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//内存表大小,内存块数量

//mem3内存参数设定.mem3处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define MEM3_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM3_MAX_SIZE			30 *1024  						//最大管理内存60K
#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	//内存表大小

//内存管理控制器   函数指针
struct malloc_cortol_struct
{
	void (*init)(uint8_t);					//初始化
	uint8_t (*perused)(uint8_t);		  	    	//内存使用率
	uint8_t 	*membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
	U16 *memmap[SRAMBANK]; 				//内存管理状态表
	uint8_t  memrdy[SRAMBANK]; 				//内存管理是否就绪,0 - 未就绪,1 - 就绪
};
extern struct malloc_cortol_struct malloc_cortol;	 //在mallco.c里面定义

void my_mem_set(void *s,uint8_t c,U32 num)  ;	//设置内存
void my_mem_cpy(void *des,void *src,U32 len)  ;//复制内存     
void my_mem_init(uint8_t memx);				//内存管理初始化函数(外/内部调用)
U32 my_mem_malloc(uint8_t memx,U32 size);	//内存分配(内部调用)
uint8_t my_mem_free(uint8_t memx,U32 offset);		//内存释放(内部调用)
uint8_t my_mem_perused(uint8_t memx);				//获得内存使用率(外/内部调用) 
////////////////////////////////////////////////////////////////////////////////
//用户调用函数
void myfree(uint8_t memx,void *paddr) ;  			//内存释放(外部调用)
void *mymalloc(uint8_t memx,U32 size);			//内存分配(外部调用)
void *myrealloc(uint8_t memx,void *paddr,U32 size) ;//重新分配内存(外部调用)


#endif
