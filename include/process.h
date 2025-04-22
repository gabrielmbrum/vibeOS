#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 5
#define EMPTY_BCP_ENTRY -1
#define SUCCESS 1
#define FAILURE 0

typedef enum _process__state {
  EXEC = 1,
  READY = 2,
  WAITING = 3,
  TERMINATED = 4
} ProcessState;

typedef struct {
  int  pid;
  ProcessState state;
  int  pc;
  char *name;
  int priority;
  int counter_rw;
} Process;

/*
  
  * Initialize the BCP (Process Control Block) with a maximum number of processes.
  * Each process is initialized with an empty PID and state.
  * The BCP is allocated dynamically and should be freed when no longer needed.
  * The function takes a pointer to a pointer to the BCP as an argument.

*/
void init_BCP(Process **BCP);

/*

  * Create a new process with the given PID, name, and priority.
  * The process is initialized with state READY and PC set to 0.
  * Returns a pointer to the newly created process.

*/
Process *create_process(int pid, const char *name, int priority);


/*

  * Add a process to the BCP.
  * If the BCP is not initialized, it will be initialized first.
  * The function searches for an empty slot in the BCP and adds the process there.
  * Returns SUCCESS if the process was added successfully, or FAILURE if the BCP is full.

*/
int add_process_to_BCP(Process *process, Process **BCP);


/*

  * Remove a process from the BCP.
  * The function searches for the process by its PID and marks it as empty.
  * Returns SUCCESS if the process was removed successfully, or FAILURE if the process was not found.

*/
int rmv_process_of_BCP(Process *process, Process **BCP);

/*

  * Destroy a process and free its allocated memory.
  * The function frees the name string and the process structure itself.
  * The process pointer should not be used after this function is called.

*/
void destroy_process(Process *process);

#endif