#include "../include/process.h"
#include "../include/commons.h"
 // Global variable to hold the BCP

 Process *processCreate(int pid, const char *name, int priority) {
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
  process->slice_time = 0;
  return process;
}


char* StringifyProcess(Process *p, char *buffer, int buffer_size) {
  if (p == NULL || buffer == NULL || buffer_size <= 0) return NULL;

  snprintf(buffer, buffer_size,
           "PID: %d, Name: %s, Priority: %d, Segment_id: %d, PC: %d, STATE: %d",
           p->pid, p->name, p->priority, p->segment_id, p->pc, p->state);
  
  return buffer;
}