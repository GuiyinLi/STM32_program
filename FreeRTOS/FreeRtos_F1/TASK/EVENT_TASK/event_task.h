#ifndef _event_task_h
#define _event_task_h
#include "start_task.h"
#include "event_groups.h"

extern EventGroupHandle_t EventGroupHandler;	

#define EVENTBIT_0	(1<<0)		
#define EVENTBIT_1	(1<<1)

#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1)

#endif
