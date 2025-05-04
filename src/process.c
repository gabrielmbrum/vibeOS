#include "../include/commons.h"
#include "../include/process.h"

int global_pid = 0;

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

Process *create_process_from_program(Program *program) {
  Process *process = (Process *) malloc(sizeof(Process));
  if (process == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  process->pid = global_pid++; // Increment global PID for unique process ID
  process->state = READY;
  process->pc = 0;
  process->name = calloc((strlen(program->header.name) + 1), sizeof(char));
  strcpy(process->name, program->header.name); // Copy program name to process name

  process->priority = program->header.original_priority;

  process->counter_rw = 0; // Initialize counter_rw to 0

  process->segment_id = program->header.segment_id;
  process->segment_size = program->header.segment_size;

  process->runtime = 0;
  process->slice_time = 0;
  process->runtime_execution = 10;
  process->semaphores = calloc((strlen(program->header.semaphores) + 1), sizeof(char));
  strcpy(process->semaphores, program->header.semaphores);

  process->page_table = build_page_table(program->instructions, program->instructions_count); // Initialize page table to NULL
  
  return process;
}
