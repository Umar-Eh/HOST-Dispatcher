/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Define your FIFO queue functions here, these will most likely be the
// push and pop functions that you declared in your header file

// Pushes a node onto a queue. The queue is accessed from the tail
// instead of the head so pushing is just a simple operation of 
// insertion
void push(node_t *tail, process *proc){
    node_t *new_node = (node_t*) malloc(sizeof(node_t));
    new_node->proc = proc;
    new_node->next_node = tail->next_node;
    tail->next_node = new_node;
}


// Pops a process from the queue. Since the queue is accessed through
// the tail instead of the head we need to iterate to the front of the 
// queue to pop the first element that was pushed (FIFO)
process *pop(node_t *tail){
    process *return_process;
    node_t *current_node, *previous_node;
    if (tail-> next_node == NULL){
        return NULL;
    }
    else {
        current_node = tail->next_node;
        previous_node = tail;
        while(current_node->next_node != NULL){
            previous_node = current_node;
            current_node = current_node->next_node;
        }
    }
    previous_node->next_node = NULL;
    return_process =  current_node->proc;
    return return_process;
}
