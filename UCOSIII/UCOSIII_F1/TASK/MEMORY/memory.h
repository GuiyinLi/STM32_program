#ifndef _memory_h
#define _memory_h
#include "includes.h"
#include "sys.h"

//定义一个存储区
extern OS_MEM INTERNAL_MEM;	
//存储区中存储块数量
#define INTERNAL_MEM_NUM		5
//每个存储块大小
//由于一个指针变量占用4字节所以块的大小一定要为4的倍数
//而且必须大于一个指针变量(4字节)占用的空间,否则的话存储块创建不成功
#define INTERNAL_MEMBLOCK_SIZE	100 

//存储区的内存池，使用内部RAM
extern __align(4) CPU_INT08U Internal_RamMemp[INTERNAL_MEM_NUM][INTERNAL_MEMBLOCK_SIZE];

void memory_task_creat(void);

#endif



