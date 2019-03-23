## HOST-Dispatcher
HOST Dispatcher project to simulate a simple process dispatcher and scheduler.<br><br>

**Description:**<br><br>
The project involves implementing a HOST dispatcher process scheduler component for an OS (preferably UNIX based).<br><br>
The following sequence illustrates how it works:<br>
1. The process list is read from console. Each process contains its priority, runtime, and a list of resources including memory.
2. A job queue is created according to the time slice corresponding to the arrival time for each process.
3. The processes are processed into a real-time queue or user-queue given the priority for each process. (Note the processes are only processed into the queues if resources can be allocated for all the processes in the queues - not including the job queue).
4. If the real-time queue contains a process, it is run until it terminates. If the real-time queue is empty, we pick the highest priority non-empty queue and pop a process to run for the time slice.
5. If the process has not completed its run time for the time slice, it is pushed into the lower priority queue.
6. The steps are repeated till all queues are empty.

The processor time slice for this project is 1 second.<br><br>

**Usage (tested on Ubuntu linux):**<br><br>
* Clone the repository: https://github.com/Umar-Eh/HOST-Dispatcher.git
* Run the command **make** to create the hostd executible.
* Run the command **./hostd** to simulate the process dispatcher.
<br>Please note that there is a pre-compiled **process** binary used by the hostd dispatcher to simulate a process. You may<br>
create your own process binary and place it in the program directory.<br>

<br>**To-do:**<br><br>
1. Create windows compatibility.
2. Add a testing framework.
