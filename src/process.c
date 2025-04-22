#include "../include/process.h"
#include "../include/commons.h"

void init_BCP(Process **BCP) {
  *BCP = malloc(sizeof(Process) * MAX_PROCESSES);
  if (*BCP == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_PROCESSES; i++) {
    (*BCP)[i].pid = EMPTY_BCP_ENTRY; // Define o slot como vazio
  }
}

Process *create_process(int pid, const char *name, int priority) {
  Process *process = (Process *) malloc(sizeof(Process));
  if (process == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }
  process->pid = pid;
  process->state = READY;
  process->pc = 0;
  process->name = strdup(name);
  process->priority = priority;
  process->counter_rw = 0;
  
  return process;
}

int add_process_to_BCP(Process *process, Process **BCP) {
  if (*BCP == NULL) {
    init_BCP(BCP);
  }
  
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid == EMPTY_BCP_ENTRY) { // Assuming pid 0 means empty slot
      (*BCP)[i] = *process;
      free(process);
      return SUCCESS;
    }
  }

  return FAILURE;
}

int rmv_process_of_BCP(Process *process, Process **BCP) {
  if (*BCP == NULL) {
    return FAILURE;
  }

  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid == process->pid) {
      (*BCP)[i].pid = EMPTY_BCP_ENTRY; // Mark as empty
      return SUCCESS;
    }
  }

  return FAILURE;
}

void destroy_process(Process *process) {
  if (process != NULL) {
    free(process->name);
    free(process);
  }
}