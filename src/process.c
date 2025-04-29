#include "../include/process.h"
#include "../include/commons.h"
 // Global variable to hold the BCP

 Process *create_process(int pid, const char *name, int priority) {
  //Ensuring Unique PID at Process Creation //In order to avoid effortless.
  Process *process = (Process *) malloc(sizeof(Process));
  if (process == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }
  
  process->pid = pid; // Creating Processes
  process->state = READY;
  process->pc = 0;
  process->name = strdup(name);
  process->priority = priority;
  process->counter_rw = 0; // As long as the process's been created, insert at BCP
  process->runtime = 0;
  return process;
}

