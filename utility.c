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

int memory_index[MEM_BLOCKS] = {0}; // Contains list of indices of used memory
int memory[MEMORY] = {0}; // Memory array

// Loads the processes listed in the input file into the dispatch list array
int load_dispatch(char *dispatch_file, int process_array[][NUM_ATTR]){
    char input_buffer[BUFFER_LEN];
    FILE *file = fopen(dispatch_file, "r");
    int process_index = 0;

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
            char *split_input = strtok(input_buffer, "\n");
            process_array[process_index][0] = atoi(strtok(split_input, ","));
            for (int index = 1; index < 8; index++){
                process_array[process_index][index] = atoi(strtok(NULL, ","));
            }
            process_index++;
        }
        fclose(file);
    }
    return process_index;
}

// Add each process structure instance to the job dispatch list queue
// The job queue is filled according to the arrival time of each process
// The priority and real time queues are filled according to resource availability
void load_jobs(int time, int num_processes, int dispatch_list[][NUM_ATTR], queue_t *job_queue, queue_t *realtime_queue, queue_t *first_priority, queue_t *second_priority, queue_t *third_priority, resources_t *available_res){
    int num_res = 0; 
    // if dispatcher time = process arrival time, push process into the job queue
    for (int i = 0; i < num_processes; i++){
        if (time == dispatch_list[i][0]){
            process_t new_process;
            new_process.arrival_time = dispatch_list[i][num_res++];
            new_process.priority = dispatch_list[i][num_res++];
            new_process.processor_time = dispatch_list[i][num_res++];
            new_process.mbytes = dispatch_list[i][num_res++];
            new_process.printers = dispatch_list[i][num_res++];
            new_process.scanners = dispatch_list[i][num_res++];
            new_process.modems = dispatch_list[i][num_res++];
            new_process.cds = dispatch_list[i][num_res++];
            new_process.memory_index = -1;
            new_process.already_running = false;
            new_process.process_id = -1;
            new_process.process_num = i + 1;
            new_process.suspended = false;
            num_res = 0;
            push(job_queue, new_process);
        }
    }
    // if resources are available, push the process in user queues. 
    if (job_queue->head->next_node != NULL){
        node_t *current = job_queue->head->next_node;
        while(current != NULL){
            if (resource_available(&(current->proc), available_res)){
                process_t *temp = pop(job_queue);
                alloc_resources(temp, available_res);
                switch (temp->priority){
                    case 0:
                        push(realtime_queue, *temp);
                        break;
                    case 1:
                        push(first_priority, *temp);
                        break;
                    case 2:
                        push(second_priority, *temp);
                        break;
                    case 3:
                        push(third_priority, *temp);
                        break;
                    default:
                        break;
                }
            }
            current = current->next_node;
        }
    }
}

// Returns true if process resources are less than equal to available resources
// false otherwise
bool resource_available(process_t *proc, resources_t *available_res){
    bool is_available = false;
    int memory = -1;
    if (proc->priority == 0){
        memory = available_res->realtime_mem_left - proc->mbytes;
    } 
    else {
        memory = available_res->user_mem_left - proc->mbytes;
    }
    if (available_res->cds >= proc->cds && available_res->modems >= proc->modems && available_res->printers >= proc->printers && available_res->scanners >= proc->scanners && memory >= 0){
        is_available = true;
    }
    return is_available;
}

// Allocates resources for a process
void alloc_resources(process_t *proc, resources_t *available_res){
    available_res->cds -= proc->cds;
    available_res->scanners -= proc->scanners;
    available_res->printers -= proc->printers;
    available_res->modems -= proc->modems;
    if (proc->priority == 0){
        available_res->realtime_mem_left -= proc->mbytes;
    }
    else {
        available_res->user_mem_left -= proc->mbytes;
    }
    proc->memory_index = alloc_mem(proc->mbytes, proc->priority);
}

//Allocates memory for a process
int alloc_mem(int size, int priority){
    int offset, start_index, end_index;
    bool found = false;
    if (priority == 0){
        start_index = 0; //real time memory starts at index 0
        end_index = 64 / MEM_BLOCKS;
    }
    else {
        start_index = 64 / MEM_BLOCKS; //user memory starts after real time memory blocks
        end_index = MEM_BLOCKS;
    }
    while (start_index < end_index){
        if (memory_index[start_index] == 0){
            for (int i = start_index; i < start_index + size / BASE_BLOCK; i++){
                if (memory_index[i] != 0){
                    found = false;
                    break;
                }
                found = true;
            }
        }
        if (found){
            break;
        }
        start_index++;
    }
    if (!found){
        return -1;
    } else {
        offset = start_index * BASE_BLOCK; // start of allocated memory in the memory array
        for (int i = offset; i < size; i++){
            memory[i] = 1;
        }
        for (int i = start_index; i < start_index + size / BASE_BLOCK; i++){
            memory_index[i] = 1;
        }
        return offset;
    }
}


// De-allocates resources once a process terminates
void dealloc_res(resources_t *res, process_t *proc){
    res->cds += proc->cds;
    res->scanners += proc->scanners;
    res->printers += proc->printers;
    res->modems += proc->modems;
    if (proc->priority == 0){
        res->realtime_mem_left += proc->mbytes;
    }
    else {
        res->user_mem_left += proc->mbytes;
    }
    dealloc_mem(memory, proc->memory_index, proc->mbytes);
}

// De-allocates memory 
void dealloc_mem(int *memory, int offset, int size){
    int start_index = offset / BASE_BLOCK, end_index = start_index + size / BASE_BLOCK;
    for (int i = offset; i < size; i++){
        memory[i] = 0;
    }
    for (int i = start_index; i < end_index; i++){
        memory_index[i] = 0;
    }
}