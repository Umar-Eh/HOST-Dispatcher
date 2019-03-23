/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdbool.h>
#include "queue.h"

// The amount of available memory
#define MEMORY 1024


// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following examples:

// Function to allocate a contiguous chunk of memory in your resources structure
// memory array, always make sure you leave the last 64 values (64 MB) free, should
// return the index where the memory was allocated at
extern int alloc_mem(resources *res, int size, int priority);

// Function to free the allocated contiguous chunk of memory in your resources
// structure memory array, should take the resource struct, start index, and 
// size (amount of memory allocated) as arguments
extern void free_mem(resources *res, int index, int size);

// Function to parse the file and initialize each process structure and add
// it to your job dispatch list queue (linked list)
extern int load_dispatch(char *dispatch_file, int process_array[][9]);

// Add each process structure instance to the job dispatch list queue
// The job queue is filled according to the arrival time of each process
// The dispatch list is empty after the job queue is filled up.
extern void load_jobs(int time, int num_processes, int dispatch_list[][9], node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority, resources *available_res);


// Returns true if process resources are less than equal to available resources
// false otherwise
extern bool resource_available(process *proc, resources *available_res);

// Returns true if all queues are empty, false otherwise
extern bool terminate_dispatcher(node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority);

// Run dispatcher
extern void run_jobs(node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority);

#endif /* UTILITY_H_ */