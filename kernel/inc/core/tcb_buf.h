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
int add_task(taskbuff_t *tasks, tcb_t *new_task);
int remove_task(taskbuff_t *tasks, tid_t target_tid);
int run_task(taskbuff_t *tasks, tid_t target_tid);
int suspend_task(taskbuff_t *tasks, tid_t target_tid);

#endif /* __TCB_BUF_H__ */
