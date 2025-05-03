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
      int idx = -1, max_rw=0;
      
      for(int i=0;i<kernel->process_amount;i++){
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
}

void context_switch (Process *next, char *arg){
    Process *running_process = kernel->scheduler->running_process;

    if(running_process->runtime == kernel->scheduler->QUANTUM_TIME){
      puts("QUANTUM REACHED");
      running_process ->runtime = 0; //if runtime process equals quantum, it means the process is coming back and initiating a fully new time-slice
    }

    if (strcmp(arg, "TERMINATED") == 0) {
      change_process_state(&running_process, TERMINATED);
  } 
  else if (strcmp(arg, "I/O") == 0) {
      change_process_state(&running_process, WAITING);
  }
  else {
      puts("Entrei no if que coloca de executando pra pronto");
      change_process_state(&running_process, READY);
      printf("  %d estado", running_process->state);
  }
    kernel->scheduler->running_process = next;
    change_process_state(&next, RUNNING);
}

int counter = 0;

void schedule() {
  while (counter < 5) {
        Process *current = kernel->scheduler->running_process;

        // Verifica se current é NULL ANTES de usá-lo
        if (!current) {
            int idx = get_max_rw_process();
            kernel->scheduler->running_process = &kernel->BCP[idx];
            change_process_state(&kernel->scheduler->running_process, RUNNING);
            print_BCP(&kernel->BCP);
            current = kernel->scheduler->running_process;  // Atualiza current!
        }

        // Agora current é garantidamente não-NULL aqui
        printf("current PID: %d  current time: %d", current->pid, current->runtime);
        current->runtime++;

        if (current->runtime >= kernel->scheduler->QUANTUM_TIME) {
            int idx = get_max_rw_process();
            Process *next = &kernel->BCP[idx];
            context_switch(next, "QUANTUM");
        }
        sleep(1);
        counter++;
    }
}