/* 
 * Host Dispatcher Shell Project 
 * 
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "queue.h"
#include "utility.h"

#define BUFFER_LEN 256
#define MAX_PROCESS 1000

process_t *load_process(queue_t *queue);
bool all_qs_empty(queue_t *realtime_queue, queue_t *first_priority, queue_t *second_priority, queue_t *third_priority);

int main(int argc, char *argv[])
{
    resources_t *available_res = NULL; // total available resources
    process_t *proc = NULL; // the current process run by the dispatcher 
    char filename[256]; // filename from input argument

    // number of processes read from the file, dispatcher time, and process status
    int num_processes = 0, time = 0, status;
    // flags: real time processing, program termination, process termination
    bool real_time_running = false, terminate = false, kill_process = false;

    // array to read process list into
    int dispatch_list[MAX_PROCESS][NUM_ATTR] = {0};
    pid_t pid;  // stores process pid upon forking

    // the process queues
    queue_t *job_queue = (queue_t*)malloc(sizeof(queue_t));
    job_queue->head = (node_t*)malloc(sizeof(node_t));
    job_queue->head->next_node = NULL;
    strcpy(job_queue->name,"JOB QUEUE");
    queue_t *realtime_queue = (queue_t*)malloc(sizeof(queue_t));
    realtime_queue->head = (node_t*)malloc(sizeof(node_t));
    realtime_queue->head->next_node = NULL;
    strcpy(realtime_queue->name,"REALTIME QUEUE");
    queue_t *first_priority = (queue_t*)malloc(sizeof(queue_t));
    first_priority->head = (node_t*)malloc(sizeof(node_t));
    first_priority->head->next_node = NULL;
    strcpy(first_priority->name,"1st QUEUE");
    queue_t *second_priority = (queue_t*)malloc(sizeof(queue_t));
    second_priority->head = (node_t*)malloc(sizeof(node_t));
    second_priority->head->next_node = NULL;
    strcpy(second_priority->name,"2nd QUEUE");
    queue_t *third_priority = (queue_t*)malloc(sizeof(queue_t));
    third_priority->head = (node_t*)malloc(sizeof(node_t));
    third_priority->head->next_node = NULL;
    strcpy(third_priority->name,"3rd QUEUE");


    // initializing the resources struct which contains the max
    // available resources
    available_res = (resources_t*) malloc(sizeof(resources_t));
    available_res->cds = 2;
    available_res->scanners = 1;
    available_res->printers = 2;
    available_res->modems = 1;
    available_res->user_mem_left = MEMORY - 64;
    available_res->realtime_mem_left = 64;

    // Checking if input file was specified
    if (argc != 2){
        printf("\nUsage: ./hostd <process list filename>\n\n");
        return EXIT_FAILURE;
    }
    else {
        strcpy(filename, argv[1]);
    }

    // Load the dispatchlist into the job queue. The job queue simply contains
    // all the processes read from the input file in order of arrival time.
    // The processes are read into a array
    num_processes = load_dispatch(filename, dispatch_list);

    // if no processes then terminate, else set terminate to false and load all processes
    if (num_processes > 0){
        terminate = false;
    }
    else {
        terminate = true;
    }
    // Start dispatcher
    while(!terminate){
	// Iterate through each item in the job dispatch list, add each process
        // to the appropriate queues
        load_jobs(time, num_processes, dispatch_list, job_queue, realtime_queue, first_priority, second_priority, third_priority, available_res);
        // If a real time process is running, it blocks all other processing
        if (real_time_running){
            sleep(1);
            proc->processor_time--;
            if (proc->processor_time == 0){ // process has finished all processing
                kill_process = true;
                real_time_running = false;
            } else {
                time++;
                continue; // real time takes precedence
            }
        }
        // Kill process, deallocate resources, wait for it to finish properly
        if (kill_process == true){
            kill_process = false;
            dealloc_res(available_res, proc);
            kill(proc->process_id, SIGINT);
            waitpid(proc->process_id, &status, WUNTRACED);
            if (status == 0){
                printf("Process %d terminated.\n", proc->process_num);
                proc = NULL;
            } else {
                printf("\nProcess did not terminate properly..\n\n");
                return EXIT_FAILURE;
            }
        } 

        // process is not real time but was not suspended earlier because 
        // of empty priority queues, hence it is continued
        if (proc != NULL && !proc->suspended){
            sleep(1);
            proc->processor_time--;
            waitpid(proc->process_id, &status, WNOHANG); 
            if (proc->processor_time == 0){  // process finished its run time
                kill_process = true;
                time++;
                continue;
            }
            // process hasn't finished its run time, and is not a real-time process,
            // hence suspend the process
            else {
                // if all queues are empty, no need to suspend the process
                // however, if one of the user/realtime queues contains a
                // process waiting to be executed, then suspend the current process
                if (!all_qs_empty(realtime_queue, first_priority, second_priority, third_priority)){
                    kill(proc->process_id, SIGTSTP);
                    waitpid(proc->process_id, &status, WUNTRACED);
                    printf("Process %d suspended.\n", proc->process_num);
                    proc->suspended = true;
                }
                else {
                    proc->suspended = false;
                } 
            }
            time++;
            continue;
        }

        // No real time processes currently running, perform other operations
        
        // process was suspended, push it back for other processes to continue
        // do not push the process back into the queue if the queues are empty
        if (proc != NULL && !all_qs_empty(realtime_queue, first_priority, second_priority, third_priority)){
            switch (proc->priority){
                case 1:
                    proc->priority++;
                    push(second_priority, *proc);
                    break;
                case 2:
                    proc->priority++;
                    push(third_priority, *proc);
                    break;
                case 3:
                    push(third_priority, *proc);
                    break;
                default:
                    break;
            }
        } 

        // load the new process if a queue is non-empty
        if (realtime_queue->head->next_node != NULL){
            proc = load_process(realtime_queue);
            real_time_running = true;
        }
        else if (first_priority->head->next_node != NULL){
            proc = load_process(first_priority);
        }
        else if (second_priority->head->next_node != NULL){
            proc = load_process(second_priority);
        }
        else if (third_priority->head->next_node != NULL){
            proc = load_process(third_priority);
       
        }

        // if proc is NULL then no more processes are left, terminate dispatcher;
        // however, if a process exists in the queues, keep processing 
        if (proc != NULL){
        // check if processes needs to be re-started or started from scratch as a new process
            if (proc->already_running){ // process was previously started
                printf("Process %d restarting..\n", proc->process_num);
                kill(proc->process_id, SIGCONT);
                sleep(1);
                proc->processor_time--;
                waitpid(proc->process_id, &status, WNOHANG); 
                if (proc->processor_time == 0){  // process finished its run time
                    kill_process = true;
                    time++;
                    continue;
                }
                // process hasn't finished its run time, and is not a real-time process,
                // hence suspend the process
                else {
                    // if all queues are empty, no need to suspend the process
                    // however, if one of the user/realtime queues contains a
                    // process waiting to be executed, then suspend the current process
                    if (!all_qs_empty(realtime_queue, first_priority, second_priority, third_priority)){
                        kill(proc->process_id, SIGTSTP);
                        waitpid(proc->process_id, &status, WUNTRACED);
                        printf("Process %d suspended.\n", proc->process_num);
                        proc->suspended = true;
                    }
                    else {
                        proc->suspended = false;
                    } 
                }
            } else { // process is new, starting from scratch
                pid = fork();
                if (pid == 0){
                    execl("./process", "process", NULL);
                    exit(EXIT_SUCCESS);
                }
                else {
                    // runs for 1 second. if process is not real-time, either suspend or 
                    // terminate process. if its realtime, do nothing as the next iteration
                    // will handle it due to the real time running flag being set already
                    proc->process_id = pid;
                    proc->already_running = true;
                    proc->processor_time--;
                    sleep(1);
                    waitpid(proc->process_id, &status, WNOHANG);
                    if (proc->processor_time == 0){
                        kill_process = true;
                        if (proc->priority == 0){
                            real_time_running = false;
                        }
                        time++;
                        continue;
                    }
                    // if its not a real time process
                    else if (!real_time_running) {
                        // if all queues are empty, no need to suspend the process
                        // however, if one of the user/realtime queues contains a
                        // process waiting to be executed, then suspend the current process
                        if (!all_qs_empty(realtime_queue, first_priority, second_priority, third_priority)){
                            kill(proc->process_id, SIGTSTP);
                            waitpid(proc->process_id, &status, WUNTRACED);
                            printf("Process %d suspended.\n", proc->process_num);
                            proc->suspended = true;
                        }
                        else {
                            proc->suspended = false;
                        } 
                    }
                }
            }
            time++;
        } else if (job_queue->head->next_node == NULL){
            terminate = true;
        }
    }

    proc = NULL;
    job_queue = NULL; first_priority = NULL; second_priority = NULL;
    third_priority = NULL; realtime_queue = NULL;
    return EXIT_SUCCESS;
}

// loads a process from the queue to be executed
process_t *load_process(queue_t *queue){
    process_t *temp = pop(queue);
    if (temp == NULL){
        printf("WARNING: Could not pop the process..check the queue: %s.\n", queue->name);
        exit(EXIT_FAILURE);
    }
    printf("Running process %d with priority %d....\n", temp->process_num, temp->priority);
    printf("Processes is using the following resources:\n");
    printf("Memory: %d MBytes, Printers: %d, Scanners: %d, CDs: %d, Modems: %d.\n", temp->mbytes, temp->printers, temp->scanners, temp->cds, temp->modems);
    printf("Process run-tume left: %d seconds.\n", temp->processor_time);
    return temp;
}

// returns false if any of the queues is non-empty, true otherwise
bool all_qs_empty(queue_t *realtime_queue, queue_t *first_priority, queue_t *second_priority, queue_t *third_priority){
    if (realtime_queue->head->next_node != NULL || first_priority->head->next_node != NULL || second_priority->head->next_node != NULL || third_priority->head->next_node != NULL){
        return false;
    }
    return true;
}