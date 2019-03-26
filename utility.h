/*
 * Host Dispatcher Shell Project 
 *
 */
#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdbool.h>
#include "queue.h"

#define MEMORY 1024   // The amount of available memory
#define NUM_ATTR 8    // Number of process attributes
#define BASE_BLOCK 64 // Smallest block of memory
#define MEM_BLOCKS MEMORY/BASE_BLOCK // Number of blocks of memory

// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
     int printers;
     int cds;
     int scanners;
     int modems;
     int user_mem_left;
     int realtime_mem_left;
} resources_t;

// Function to parse the file and initialize each process structure and add
// it to the job dispatch list queue (linked list)
extern int load_dispatch(char *dispatch_file, int process_array[][NUM_ATTR]);

// Adds each process structure instance to the job dispatch list queue
// The job queue is filled according to the arrival time of each process
// The dispatch list is empty after the job queue is filled up.
extern void load_jobs(int time, int num_processes, int dispatch_list[][NUM_ATTR], queue_t *job_queue, queue_t *realtime_queue, queue_t *first_priority, queue_t *second_priority, queue_t *third_priority, resources_t *available_res);

// Returns true if process resources are less than equal to available resources
// false otherwise
extern bool resource_available(process_t *proc, resources_t *available_res);

// Allocates resources for a process
extern void alloc_resources(process_t *temp, resources_t *available_res);

// Function to allocate a contiguous chunk of memory.
// It returns the index where the memory was allocated at.
extern int alloc_mem(int size, int priority);

// De-allocates resources once a process terminates
extern void dealloc_res(resources_t *res, process_t *proc);

// Function to free the allocated contiguous chunk of memory.
// It takes the memory array, start index, and 
// size (amount of memory allocated) as arguments
extern void dealloc_mem(int *memory, int offset, int size);

#endif /* UTILITY_H_ */