#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "tcb.h"
#include "tcb_buf.h"

/* must match the .equ of the same name in switch.s, asserted in scheduler.c */
#define TCB_SP_OFFSET   24

extern volatile tcb_t* current_task;
extern volatile tcb_t* next_task;

extern taskbuff_t kernel_tasks;

tcb_t* _scheduler(taskbuff_t* tasks, tcb_t* current);
void sched_tick(void);
void sched_yield(void);

#endif /* __SCHEDULER_H__ */
