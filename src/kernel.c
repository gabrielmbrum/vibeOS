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
    kernel->scheduler->QUANTUM_TIME = 2;
}

int rmv_process_of_BCP(int removing_pid) {
  if (kernel->BCP == NULL) { //BCP not allocated, return Failure.
    return FAILURE;
  }

  int idx = search_BCP(removing_pid);
  if(idx != FAILURE){
    //If PID present in BCP, get the index and remove;
    kernel->BCP[idx].pid = EMPTY_BCP_ENTRY;
    return SUCCESS;
  }
  kernel->process_amount--;
  return FAILURE;//If Search in BCP Failed, then PID not present in BCP, return FAILURE. 
}
  
  void processFinish(Process **process) {
    (*process)->pid=-1;//Pointing some things about this to be discussed later
  }
    int scheduler_POLICY(){
      int idx = -1, max_rw=0;
      
      for(int i=0;i<kernel->process_amount;i++){
          if(kernel->BCP[i].counter_rw > max_rw && kernel->BCP[i].pid>=0){
              max_rw = kernel->BCP[i].counter_rw;
              idx = i;
          }
          else if (kernel->BCP[i].counter_rw==max_rw && kernel->BCP[idx].pid > kernel->BCP[i].pid && kernel->BCP[i].pid>=0){
              idx = i;
          }
      }
      return idx;
  }

  void change_process_state(Process **process, ProcessState state){
    (*process)->state = state;
  }

void context_switch(Process *next, char *arg){
  Process *running_process = kernel->scheduler->running_process;

  if(strcmp(arg, "QUANTUM")==0){
    running_process->slice_time=0;
    change_process_state(&running_process, READY);
  }

  else if (strcmp(arg, "TERMINATED") == 0) {
    printf("Process with PID: %d finished execution...\n", running_process->pid);
    change_process_state(&running_process, TERMINATED);
    rmv_process_of_BCP(running_process->pid);
    }

  else if (strcmp(arg, "I/O") == 0) {
      change_process_state(&running_process, WAITING);
  }
  else {
      change_process_state(&running_process, READY);
  }
    kernel->scheduler->running_process = next;
    change_process_state(&next, RUNNING);
  }

  void processInterrupt(Process *next){ //Quantum atingido. 
    context_switch(next, "QUANTUM"); //\n
  }

int counter = 0;

void schedule() {
  while (counter < 200) {
      Process *current = kernel->scheduler->running_process;

      // Se não há processo rodando OU o processo atual é inválido
      if (!current || current->pid < 0) {
          int idx = scheduler_POLICY();
          
          // Se não há processos válidos, encerra o escalonador
          if (idx == FAILURE) {
              printf("Nenhum processo válido no BCP.\n");
              break;
          }
          
          kernel->scheduler->running_process = &kernel->BCP[idx];
          change_process_state(&kernel->scheduler->running_process, RUNNING);
          print_BCP(&kernel->BCP, kernel->process_amount);
          puts("");
          current = kernel->scheduler->running_process;
      }

      // Se chegou aqui, current é válido (pid >= 0)

      current->slice_time++;
      current->runtime_execution--;

      if (current->slice_time >= kernel->scheduler->QUANTUM_TIME) {
          int idx = scheduler_POLICY();
          if (idx != FAILURE) {
              Process *next = &kernel->BCP[idx];
              processInterrupt(next);
          }
      }

      if (current->runtime_execution <= 0) {
          int idx = scheduler_POLICY();
          if (idx != FAILURE) {
              Process *next = &kernel->BCP[idx];
              context_switch(next, "TERMINATED");
          }
      }

      if (counter % 5 == 0) {
          sleep(1);
      }
      counter++;
  }
}