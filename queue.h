/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdbool.h>

#define MEMORY 1024
// Resources structure containing integers for each resource constraint and an
// array of 1024 for the memory
typedef struct {
     int printers;
     int cds;
     int scanners;
     int modems;
     int memoryleft;
     int realtime_mem;
     int memory[MEMORY];
} resources;


// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
typedef struct {
     int arrival_time;
     int priority;
     int processor_time;
     int mbytes;
     int printers;
     int scanners;
     int modems;
     int cds;
     int memory_index;
     int process_id;
     bool running;
} process;


// Your linked list structure for your queue
typedef struct node{
    process *proc;
    struct node *next_node;
} node_t;

// Include your relevant FIFO queue functions declarations here they must start 
// with the extern keyword such as in the following examples:

// Add a new process to the queue, returns the address of the next node added
// to the linked list

extern void push(node_t *tail, process *proc);

extern process *pop(node_t *tail);


#endif /* QUEUE_H_ */