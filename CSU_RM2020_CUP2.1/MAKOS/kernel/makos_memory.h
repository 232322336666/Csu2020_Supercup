#ifndef _MAKOS_MEMORY_H
#define _MAKOS_MEMORY_H

#include "makos_type.h"
#ifndef NULL
#define NULL 0
#endif

//����2���ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��
#define SRAMEX   1		//�ⲿ�ڴ��
#define SRAMCCM  2		//CCM�ڴ��(�˲���SRAM����CPU���Է���!!!)

#define SRAMBANK 	3	//����֧�ֵ�SRAM����.	


//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			70*1024  						//�������ڴ� 100K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С,�ڴ������

//mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define MEM2_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM2_MAX_SIZE			960 *1024  						//�������ڴ�960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//�ڴ���С,�ڴ������

//mem3�ڴ�����趨.mem3����CCM,���ڹ���CCM(�ر�ע��,�ⲿ��SRAM,��CPU���Է���!!)
#define MEM3_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM3_MAX_SIZE			30 *1024  						//�������ڴ�60K
#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	//�ڴ���С

//�ڴ���������   ����ָ��
struct malloc_cortol_struct
{
	void (*init)(uint8_t);					//��ʼ��
	uint8_t (*perused)(uint8_t);		  	    	//�ڴ�ʹ����
	uint8_t 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
	U16 *memmap[SRAMBANK]; 				//�ڴ����״̬��
	uint8_t  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����,0 - δ����,1 - ����
};
extern struct malloc_cortol_struct malloc_cortol;	 //��mallco.c���涨��

void my_mem_set(void *s,uint8_t c,U32 num)  ;	//�����ڴ�
void my_mem_cpy(void *des,void *src,U32 len)  ;//�����ڴ�     
void my_mem_init(uint8_t memx);				//�ڴ�����ʼ������(��/�ڲ�����)
U32 my_mem_malloc(uint8_t memx,U32 size);	//�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint8_t memx,U32 offset);		//�ڴ��ͷ�(�ڲ�����)
uint8_t my_mem_perused(uint8_t memx);				//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(uint8_t memx,void *paddr) ;  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(uint8_t memx,U32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(uint8_t memx,void *paddr,U32 size) ;//���·����ڴ�(�ⲿ����)


#endif
