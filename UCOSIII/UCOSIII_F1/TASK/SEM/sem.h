#ifndef _sem_h
#define _sem_h
#include "includes.h"
#include "sys.h"
extern u8 share_resource[30];   //¹²Ïí×ÊÔ´Çø

extern OS_SEM	MY_SEM;		//¶¨ÒåÒ»¸öĞÅºÅÁ¿£¬ÓÃÓÚ·ÃÎÊ¹²Ïí×ÊÔ´
extern OS_SEM	SYNC_SEM; //ÓÃÓÚÈÎÎñÍ¬²
extern OS_MUTEX	TEST_MUTEX;		//¶¨ÒåÒ»¸ö»¥³âĞÅºÅÁ¿ ½â¾öÓÅÏÈ¼¶·´×ª
void sem_task_create(void); 
void mutex_test_task_creat(void);
void sem_share(char str[]);
#endif

