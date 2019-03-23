/*
 * Host Dispatcher Shell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <GROUP MEMBERS>
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "utility.h"

#define BUFFER_LEN 100

// Define your utility functions here, you will likely need to add more...

int alloc_mem(resources *res, int size, int priority){
    int max_memory = 0;
    int memory_count = size;
    int memory_index = 0;
    if (priority == 0){
        max_memory = 1024;
        memory_index = 960;
    }
    else {
        max_memory = 960;
    }
    for (; memory_index < max_memory && memory_count > 0; memory_index++){
        if (res->memory[memory_index] == 0){
            res->memory[memory_index] = 1;
            memory_count--;
        }
    }
    return memory_index - size;
}

void free_mem(resources *res, int index, int size){
    int memory_index = index;
    while (memory_index < index + size){
        res->memory[memory_index] = 0;
        memory_index++;
    }  
}


// Loads the processes listed in the input file into the dispatch
// list array
int load_dispatch(char *dispatch_file, int process_array[][9]){
    char input_buffer[BUFFER_LEN], *split_input;
    FILE *file = fopen(dispatch_file, "r");
    int process_index = 0, resource_index = 0;

    // Checking if input file exits
    if (file == NULL){
        printf("Error, file does not exit.\n");
    }
    else {
        // start processing the file line by line. First the
        // split_input gets each line from the strtok function,
        // then the string is further split according to commas
        // and resulting strings converted to ints and copied to the
        // process values as defined in queue.h
        while(fgets(input_buffer, BUFFER_LEN, file) != NULL){
            split_input = strtok(input_buffer, "\n");
            split_input = strtok(split_input, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            split_input = strtok(NULL, ",");
            process_array[process_index][resource_index] = atoi(split_input);
            resource_index++;
            process_array[process_index][resource_index] = -1;
            resource_index = 0;
            process_index++;
        }
    }
    fclose(file);
    return process_index;
}

// Add each process structure instance to the job dispatch list queue
// The job queue is filled according to the arrival time of each process
// The priority and real time queues are filled according to resource availability
void load_jobs(int time, int num_processes, int dispatch_list[][9], node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority, resources *available_res){
    int queued_jobs = 0;
    
    // if dispatcher time = process arrival time, push process into the job queue
    for (int i = 0; i < num_processes; i++){
        if (time == dispatch_list[i][0]){
            process *new_process = (process*)malloc(sizeof(process));
            new_process->arrival_time = dispatch_list[i][0];
            new_process->priority = dispatch_list[i][1];
            new_process->processor_time = dispatch_list[i][2];
            new_process->mbytes = dispatch_list[i][3];
            new_process->printers = dispatch_list[i][4];
            new_process->scanners = dispatch_list[i][5];
            new_process->modems = dispatch_list[i][6];
            new_process->cds = dispatch_list[i][7];
            new_process->memory_index = dispatch_list[i][8];
            push(job_queue, new_process);
            free(new_process);
            new_process = NULL;
        }
    }
    // if resources are available, push the process in job queue into the real time or priority 
    // user queues. if not, push it back into the job queue
    if (job_queue->next_node != NULL){
        node_t *curr = job_queue->next_node;
        queued_jobs++;
        while(curr->next_node != NULL){
            curr = curr->next_node;
            queued_jobs++;
        }
        curr = NULL;
        for (int i = 0; i < queued_jobs; i++){
            process *proc = pop(job_queue);
            if(resource_available(proc, available_res)){
                available_res->cds -= proc->cds;
                available_res->scanners -= proc->scanners;
                available_res->printers -= proc->printers;
                available_res->modems -= proc->modems;
                if (proc->priority == 0){
                    available_res->realtime_mem -= proc->mbytes;
                }
                else {
                    available_res->memoryleft -= proc->mbytes;
                }
                switch (proc->priority){
                    case 0:
                        push(realtime_queue, proc);
                        break;
                    case 1:
                        push(first_priority, proc);
                        break;
                    case 2:
                        push(second_priority, proc);
                        break;
                    case 3:
                        push(third_priority, proc);
                        break;
                    default:
                        break;
                }
            }
            else {
                push(job_queue, proc);
            }
            proc = NULL;
        }   
    }
}

// Returns true if process resources are less than equal to available resources
// false otherwise
bool resource_available(process *proc, resources *available_res){
    int cds = available_res->cds - proc->cds;
    int modems = available_res->modems - proc->modems;
    int printers = available_res->printers - proc->printers;
    int scanners = available_res->scanners - proc->scanners;
    int memory = 0;
    if (proc->priority == 0){
        memory =  available_res->realtime_mem - proc->mbytes;
    } 
    else {
        memory = available_res->memoryleft - proc->mbytes;
    }
    if (cds >=0 && modems >=0 && printers >=0 && scanners >=0 && memory >= 0){
        return true;
    }
    else {
        return false;
    }
}


// Returns true if all queues are empty, false otherwise
bool terminate_dispatcher(node_t *job_queue, node_t *realtime_queue, node_t *first_priority, node_t *second_priority, node_t *third_priority){
    if (job_queue->next_node == NULL && realtime_queue->next_node == NULL && first_priority->next_node == NULL && second_priority->next_node == NULL && third_priority->next_node == NULL){
        return true;
    }
    else {
        return false;
    }
}
