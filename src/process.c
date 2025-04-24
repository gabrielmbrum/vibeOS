#include "../include/process.h"
#include "../include/commons.h"

Process *BCP = NULL; // Global variable to hold the BCP

void init_BCP() {
  BCP = malloc(sizeof(Process) * MAX_PROCESSES);
  if (BCP == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAX_PROCESSES; i++) {
    BCP[i].pid = EMPTY_BCP_ENTRY; // Define o slot como vazio
  }
}

int search_BCP(int process_pid){
  if (BCP == NULL) {
    return FAILURE;
  }

  for(int i=0;i<MAX_PROCESSES;i++){
    printf("Searching PID %d...\n", BCP[i].pid);
    if(BCP[i].pid == process_pid){
      return i;
    }
  }
  return FAILURE;
} 

Process *create_process(int pid, const char *name, int priority) {
  //Ensuring Unique PID at Process Creation //In order to avoid effortless.
  if(search_BCP(pid) != FAILURE) return NULL; //PID already present in BCP

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
  return process;
}

int add_process_to_BCP(Process *process) {
  if (BCP == NULL) {
    init_BCP();
  }
  
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (BCP[i].pid == EMPTY_BCP_ENTRY) { // Assuming pid 0 means empty slot
      BCP[i] = *process;
      free(process);
      return SUCCESS;
    }
  }

  return FAILURE;
}

int rmv_process_of_BCP(int removing_pid) {
  if (BCP == NULL) { //BCP not allocated, return Failure.
    return FAILURE;
  }

  int idx = search_BCP(removing_pid);
  if(idx != FAILURE){
    printf("PID %d Presente na BCP\n", removing_pid);
    //If PID present in BCP, get the index and remove;
    BCP[idx].pid = EMPTY_BCP_ENTRY;
    return SUCCESS;
  }
  return FAILURE;//If Search in BCP Failed, then PID not present in BCP, return FAILURE. 
}

void destroy_process(Process *process) { //Pointing some things about this to be discussed later
  if (process != NULL) { // Remove from BCP  -> Kill process?
    free(process->name); // Kill_Process, grep pid and then update at BCP?
    free(process); 
  }
}

int change_process_state(int process_pid, ProcessState state){
  int idx = search_BCP(process_pid);
  if(idx == FAILURE) return FAILURE;
  BCP[idx].state = state;
  return SUCCESS;
}