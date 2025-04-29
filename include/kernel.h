#ifndef KERNEL_H
#define KERNEL_H

#include "process.h"
#include "commons.h"
#include "debug.h"
typedef struct Scheduler{
  Process *running_process;
  int QUANTUM_TIME;
}Scheduler;


typedef struct Kernel{
    Process *BCP;
    Scheduler *scheduler;
    int process_amount;
}Kernel;

extern Kernel *kernel;

void init_BCP();

void init_Kernel();

/*
  
  * Initialize the BCP (Process Control Block) with a maximum number of processes.
  * Each process is initialized with an empty PID and state.
  * The BCP is allocated dynamically and should be freed when no longer needed.
  * The function takes a pointer to a pointer to the BCP as an argument.

*/

/*

  * Add a process to the BCP.
  * If the BCP is not initialized, it will be initialized first.
  * The function searches for an empty slot in the BCP and adds the process there.
  * Returns SUCCESS if the process was added successfully, or FAILURE if the BCP is full.

*/
int add_process_to_BCP(Process *process);


/*

  * Remove a process from the BCP.
  * The function searches for the process by its PID and marks it as empty.
  * Returns SUCCESS if the process was removed successfully, or FAILURE if the process was not found.

*/
int rmv_process_of_BCP(int removing_pid);

/*

  * Destroy a process and free its allocated memory.
  * The function frees the name string and the process structure itself.
  * The process pointer should not be used after this function is called.

*/
void destroy_process(Process *process);


/*

  * Search for a process in the BCP by its PID.
  * The function returns the index of the process in the BCP if found, or FAILURE if not found.
  * This function is used internally by other functions to locate processes in the BCP.

*/
int search_BCP(int process_pid);


void change_process_state(Process **process, ProcessState state);

//Brum documenta ai bb rsrs. 
int get_max_rw_process();

int choose_process();

#endif
