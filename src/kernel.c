#include "../include/process.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/debug.h"
Kernel *kernel=NULL;

void init_BCP() {
    kernel->BCP = malloc(sizeof(Process) * MAX_PROCESSES);
    if (kernel->BCP == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      exit(EXIT_FAILURE);
    }
  
    for (int i = 0; i < MAX_PROCESSES; i++) {
        kernel->BCP[i].pid = EMPTY_BCP_ENTRY; // Define o slot como vazio
    }
    kernel->process_amount = 0;
  }
  
  int search_BCP(int process_pid){
    if (kernel->BCP == NULL) {
      return FAILURE;
    }
  
    for(int i=0;i<MAX_PROCESSES;i++){
      printf("Searching PID %d...\n", kernel->BCP[i].pid);
      if(kernel->BCP[i].pid == process_pid){
        return i;
      }
    }
    return FAILURE;
  } 
  
  int add_process_to_BCP(Process *process) {
    if (kernel->BCP == NULL) {
      init_BCP();
    }
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (kernel->BCP[i].pid == EMPTY_BCP_ENTRY) { // Assuming pid 0 means empty slot
        kernel->BCP[i] = *process;
        free(process);
        kernel->process_amount ++;
        return SUCCESS;
      }
    }
  
    return FAILURE;
  }
  // Inclua para ter a definição completa

void init_Kernel() {
    if (kernel != NULL) {
        fprintf(stderr, "Kernel already initialized\n");
        return;
    }

    kernel = malloc(sizeof(Kernel));
    if (kernel == NULL) {
        fprintf(stderr, "Failed to allocate memory for Kernel\n");
        exit(EXIT_FAILURE);
    }

    // Aloca e inicializa o scheduler
    kernel->scheduler = malloc(sizeof(Scheduler));
    if (kernel->scheduler == NULL) {
        fprintf(stderr, "Failed to allocate memory for Scheduler\n");
        free(kernel);
        exit(EXIT_FAILURE);
    }

    init_BCP();  // Inicializa o BCP

    // Configura o scheduler
    kernel->scheduler->running_process = NULL;
    kernel->scheduler->QUANTUM_TIME = 5;
}

  int rmv_process_of_BCP(int removing_pid) {
    if (kernel->BCP == NULL) { //BCP not allocated, return Failure.
      return FAILURE;
    }
  
    int idx = search_BCP(removing_pid);
    if(idx != FAILURE){
      printf("PID %d Presente na BCP\n", removing_pid);
      //If PID present in BCP, get the index and remove;
      kernel->BCP[idx].pid = EMPTY_BCP_ENTRY;
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
  
  
  
  
    int get_max_rw_process(){
      int idx = 0, max_rw=0;
      for(int i=0;i<kernel->process_amount;i++){
        printf("OLHANDO PROCESSO");
        print_process(&kernel->BCP[i]);
          if(kernel->BCP[i].counter_rw > max_rw){
              max_rw = kernel->BCP[i].counter_rw;
              idx = i;
          }
          else if (kernel->BCP[i].counter_rw==max_rw && kernel->BCP[idx].pid > kernel->BCP[i].pid){
              idx = i;
          }
      }
      return idx;
  }
  void change_process_state(Process **process, ProcessState state){
    (*process)->state = state;
    if((*process)->state == RUNNING)printf("Process state: RUNNING\n");
  }
  int choose_process(){
      int idx = get_max_rw_process();
      kernel->scheduler->running_process =&kernel->BCP[idx];
      change_process_state(&kernel->scheduler->running_process, RUNNING);
      printf("Running PID: %d\n", kernel->scheduler->running_process->pid);
      return SUCCESS;
  }
  
  
  