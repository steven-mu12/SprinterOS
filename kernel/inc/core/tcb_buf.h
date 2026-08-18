#ifndef __TCB_BUF_H__
#define __TCB_BUF_H__

#include <stdint.h>

#include "sprinter_common.h"
#include "tcb.h"

typedef struct taskbuff_t {
	volatile tcb_t buffer[MAX_TASKS];
	volatile uint32_t tasks_in_buf;
} taskbuff_t; 

/**
 * @brief Task buffer user functionality
 */
int create_task(taskbuff_t* tasks, void (*callback)(void*), void* args);
int remove_task(taskbuff_t *tasks, tid_t target_tid);
int run_task(taskbuff_t *tasks, tid_t target_tid, volatile tcb_t** current_task);
int suspend_task(taskbuff_t *tasks, tid_t target_tid, volatile tcb_t** current_task);

#endif /* __TCB_BUF_H__ */
