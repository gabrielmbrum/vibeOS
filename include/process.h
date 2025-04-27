#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 5
#define EMPTY_BCP_ENTRY -1
#define SUCCESS 1
#define FAILURE -1

#include "memory.h"

typedef enum _process__state {
  RUNNING = 1,
  READY = 2,
  WAITING = 3,
  TERMINATED = 4
} ProcessState;

typedef struct {
  int pid;
  ProcessState state;
  int pc;
  char *name;
  int priority;
  int counter_rw;
  int segment_id;
  int segment_size;
  int runtime;
  char *semaphores;
  Page *page_table;
} Process;

/*

  * Create a new process with the given PID, name, and priority.
  * The process is initialized with state READY and PC set to 0.
  * Returns a pointer to the newly created process.

*/

Process *create_process(int pid, const char *name, int priority);



#endif