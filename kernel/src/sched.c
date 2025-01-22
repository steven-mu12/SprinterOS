
#include <stdio.h>
#include <stdlib.h>
#include "inc/sched.h"

/* Ready queue stuff in shared mem */
PCB_t *ready_queue[MAX_READY_QUEUE_SIZE];
int queue_size = 0;


/* HELPER FUNCTIONS - DO NOT USE DIRECTLY */
static uint32_t calculate_score(PCB_t *task) {
    return (ALPHA * task->estimated_time) + (BETA * task->priority);
}


/* USER FUNCTIONS */
void scheduler_init(void) {
    queue_size = 0;
    for (int i = 0; i < MAX_READY_QUEUE_SIZE; i++) {
        ready_queue[i] = NULL;
    }
}


void scheduler_add_task(PCB_t *task) {
    if (queue_size >= MAX_READY_QUEUE_SIZE) {
        printf("Ready queue is full!\n");
        return;
    }

    // insert the task into the queue
    ready_queue[queue_size++] = task;

    // sort the queue from low to high score
    for (int i = 0; i < queue_size - 1; i++) {
        for (int j = i+1; j < queue_size; j++) {
            if (calculate_score(ready_queue[i]) > calculate_score(ready_queue[j])) {
                PCB_t *temp = ready_queue[i];
                ready_queue[i] = ready_queue[j];
                ready_queue[j] = temp;
            }
        }
    }
}


PCB_t *scheduler_get_next_task(void) {
   
    // see if there's a task left
    if (queue_size == 0) {
        return NULL;
    }

    PCB_t *next_task = ready_queue[0];

    // shift the queue
    for (int i = 1; i < queue_size; i++) {
        ready_queue[i - 1] = ready_queue[i];
    }
    queue_size--;

    return next_task;
}