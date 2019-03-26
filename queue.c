/*
 * Host Dispatcher Shell Project
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Pushes a node onto a queue. 
void push(queue_t *q, process_t proc){
    node_t *current = q->head;
    while (current->next_node != NULL){
        current = current->next_node;
    }
    current->next_node = (node_t*)malloc(sizeof(node_t));
    current = current->next_node;
    current->next_node = NULL;
    current->proc = proc;
}

// Pops a process from the queue.
process_t *pop(queue_t *q){
    process_t *temp = NULL;
    if (q->head->next_node != NULL){
        temp = &(q->head->next_node->proc);
        q->head->next_node = q->head->next_node->next_node;
    }
    return temp;
}