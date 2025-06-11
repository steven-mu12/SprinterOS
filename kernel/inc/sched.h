/**
 ******************************************************************************
 * @file           : scheduler.h
 * @author         : Steven Mu
 * @summary        : SprinterOS Scheduler Header
 ******************************************************************************
 * MIT License

 * Copyright (c) 2025 Steven Mu

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 all
 * copies or substantial portions of the Software.
 ******************************************************************************
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <stdint.h>

typedef struct {
	uint16_t task_id;
	uint8_t priority;
	uint32_t estimated_time;
	uint32_t arrival_time;
	uint8_t state;
	void (*task_function)(void* args);
	void* task_arg;
} PCB_t;

/* definitions */
#define ALPHA 1
#define BETA 3
#define MAX_READY_QUEUE_SIZE 25

/* User Functions */
void scheduler_init(void);
void scheduler_add_task(PCB_t* task);
PCB_t* scheduler_get_next_task(void);
// void scheduler_periodic_update(void); (IMPLEMENT THIS)

#endif