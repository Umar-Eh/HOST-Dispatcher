/*
 * Host Dispatcher Shell Project 
 */

#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdbool.h>
#include <sys/types.h>

// Processes structure containing all of the process details parsed from the 
// input file, should also include the memory address (an index) which indicates
// where in the resources memory array its memory was allocated
typedef struct {
    int process_num; 
    int arrival_time;
    int priority;
    int processor_time;
    int mbytes;
    int printers;
    int scanners;
    int modems;
    int cds;
    int memory_index;
    bool already_running;
    bool suspended;
    pid_t process_id;
} process_t;


// The linked list structure for process queue
typedef struct node{
    process_t proc;
    struct node *next_node;
} node_t;

typedef struct {
    char name[50];
    struct node *head; 
} queue_t;

// Add a new process to the queue
extern void push(queue_t *q, process_t proc);

// Pop a process from the queue and return its address
extern process_t *pop(queue_t *q);


#endif /* QUEUE_H_ */