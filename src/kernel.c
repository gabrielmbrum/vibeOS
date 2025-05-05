/* O kernel.c:
- controla o "núcleo" do sistema
- mantêm a lista de processos (BCP)
- permite que sem_wait e sem_post sejam usados pelas instruções

Funções principais:
- init_Kernel(): inicializa o kernel
- init_BCP(): inicializa a lista de processos
- add_process_to_BCP(): adiciona um processo ao kernel */

#include "../include/process.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/debug.h"
#include "../include/memory.h"
#include "../include/semaphore.h"
#include "../include/iohandler.h"

#define LOCK_BCP() pthread_mutex_lock(&kernel->bcp_mutex)
#define UNLOCK_BCP() pthread_mutex_unlock(&kernel->bcp_mutex)
#define IOException -101
#define RWTimeSlice 30 
Kernel *kernel=NULL;

void* input_thread_func(void* arg) {
  char command[10];
  while (!kernel->shutdown_request) {
      printf("\nDigite 'q' para encerrar o Kernel: \n");
      fflush(stdout);
      fgets(command, sizeof(command), stdin);

      if (command[0] == 'q' || command[0] == 'Q') {
          kernel->shutdown_request = true;
          break;
      }
  }
  return NULL;
}

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

void *scheduler_thread_func(void *arg){
  while(kernel->scheduler_running && !kernel->shutdown_request){
    LOCK_BCP();
    while(kernel->process_amount == 0 && kernel->scheduler_running){
      pthread_cond_wait(&kernel->bcp_cond, &kernel->bcp_mutex);
      puts("Inicializando thread...\n");
    }

    if(kernel->process_amount > 0){
      puts("Escalonando..\n");
      schedule();
    }
    printf("Quantidade de processos na BCP.. %d\n", kernel->process_amount);
    if(kernel->process_amount==0){
      puts("Encerrando escalonador...\n");
      scheduler_stop();
    }
    UNLOCK_BCP();
    usleep(10000);
  }
  return NULL;
}

void start_scheduler(){
  if(!kernel->scheduler_running){
    kernel->scheduler_running = true;
    pthread_create(&kernel->scheduler_thread, NULL, scheduler_thread_func, NULL);
  }
}

void scheduler_stop(){
  if(kernel->scheduler_running){
    kernel->scheduler_running = false;
    puts("Entrei aq");
    pthread_cond_signal(&kernel->bcp_cond);
    pthread_join(kernel->scheduler_thread, NULL);
  }
}
  
int search_BCP(int process_pid){
  if (kernel->BCP == NULL) {
    printf("[ERROR] BCP not initialized!\n");
    return FAILURE;
  }
  for (int i = 0; i < MAX_PROCESSES; i++) {
      if (kernel->BCP[i].pid == process_pid) {
          return i;
      }
  }
  return FAILURE;
} 
  
int add_process_to_BCP(Process *process) {
  LOCK_BCP();
  if (kernel->BCP == NULL) {
    init_BCP();
  }
  
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (kernel->BCP[i].pid == EMPTY_BCP_ENTRY) { // Assuming pid 0 means empty slot
      kernel->BCP[i] = *process;
      free(process);
      kernel->process_amount ++;   
      pthread_cond_signal(&kernel->bcp_cond);
      UNLOCK_BCP();
      if(kernel->process_amount == 1 && !kernel->scheduler_running) start_scheduler();
      return SUCCESS;
    }
  }
  pthread_cond_signal(&kernel->bcp_cond);
  UNLOCK_BCP();

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

    pthread_mutex_init(&kernel->bcp_mutex, NULL);
    pthread_cond_init(&kernel->bcp_cond, NULL);
    pthread_mutex_init(&kernel->io_mutex, NULL);
    pthread_cond_init(&kernel->io_cond, NULL);

    LOCK_BCP();

    init_BCP();  // Inicializa o BCP
    // Configura o scheduler
    pthread_cond_signal(&kernel->bcp_cond);
    UNLOCK_BCP();
    kernel->scheduler->running_process = NULL;
    kernel->scheduler->QUANTUM_TIME = 2;
    kernel->scheduler_running = false;
    kernel->shutdown_request = false;
    pthread_create(&kernel->input_thread, NULL, input_thread_func, NULL);
}

void shutdown_Kernel() {
  kernel->shutdown_request = true;
  kernel->scheduler_running = false;

  pthread_join(kernel->input_thread, NULL);
  pthread_join(kernel->scheduler_thread, NULL);

  free(kernel->BCP);
  free(kernel->scheduler);
  free(kernel);
}

void processFinish(Process *process) {
  process->pid=EMPTY_BCP_ENTRY;//Pointing some things about this to be discussed later
  process->counter_rw= -1;

}

int rmv_process_of_BCP(int removing_pid) {
  if (kernel->BCP == NULL) { //BCP not allocated, return Failure.
    return FAILURE;
  }

  int idx = search_BCP(removing_pid);
  if(idx != FAILURE){
    //If PID present in BCP, get the index and remove;
    processFinish(&kernel->BCP[idx]);
    kernel->process_amount--;
    return SUCCESS;
  }
  return FAILURE;//If Search in BCP Failed, then PID not present in BCP, return FAILURE. 
}

int scheduler_POLICY(){
  int idx = -1, max_rw=-1;
  for(int i=0;i<MAX_PROCESSES;i++){
      if(kernel->BCP[i].counter_rw > max_rw &&
        kernel->BCP[i].pid>=0 &&
        kernel->BCP[i].state!=TERMINATED){
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

void context_switch(Process *next, const char *reason) {
  // Save current process state
  Process *current = kernel->scheduler->running_process;
  
  if (current) {
      printf("\n[CONTEXT SWITCH] PID %d -> ", current->pid);
      
      // Handle current process state transition
      if (strcmp(reason, "QUANTUM") == 0) {
        printf("Quantum expired, moving to READY\n");
        current->slice_time=0;
        change_process_state(&current, READY);
      }
      else if (strcmp(reason, "TERMINATED") == 0) {
          printf("Process with PID: %d finished execution...\n", current->pid);
          change_process_state(&current, TERMINATED);
          rmv_process_of_BCP(current->pid);
      } 
      else if (strcmp(reason, "I/O") == 0) {
          printf("I/O wait, moving to WAITING\n");
          change_process_state(&current, WAITING);
      }
      else if (strcmp(reason, "SEM_BLOCK") == 0) {
          printf("Semaphore block, moving to WAITING\n");
          current->slice_time=0;
          change_process_state(&current, WAITING);
      }else{
          printf("Unknown reason, moving to READY\n");
          change_process_state(&current, READY);
      }
      kernel->scheduler->running_process = next;
      change_process_state(&next, RUNNING);
  }

  // Set up next process
  // if (next) {
  //     printf("[CONTEXT SWITCH] -> PID %d (%s)\n", next->pid, reason);
  //     next->state = RUNNING;
  //     kernel->scheduler->running_process = next;
      
  //     // Restore process context would go here
  //     // restore_registers(next->registers);
  // }

  // Print updated state
  print_BCP(&kernel->BCP, kernel->process_amount);
}

Process* scheduler_get_next_process(Scheduler *sched) {
  // Implement your scheduling policy here
  for (int i = 0; i < kernel->process_amount; i++) {
      if (kernel->BCP[i].state == READY) {
          return &kernel->BCP[i];
      }
  }
  return NULL;
}

void processInterrupt(Process *next){ //Quantum atingido. 
  context_switch(next, "QUANTUM"); //\n
}

void init_Buffer(){
  kernel->Trail_Buffer = fopen("buffer.txt", "w+");
}

int counter = 0;

void schedule() {
  Process *current = kernel->scheduler->running_process;
      // Se não há processo rodando OU o processo atual é inválido
  if (!current || current->pid < 0) {
      int idx = scheduler_POLICY();
          // Se não há processos válidos, encerra o escalonador
      if (idx == FAILURE) {
        return;
      }
      
      kernel->scheduler->running_process = &kernel->BCP[idx];
      change_process_state(&kernel->scheduler->running_process, RUNNING);
      //print_BCP(&kernel->BCP, kernel->process_amount);
      puts("");
      current = kernel->scheduler->running_process;
      printf("Selected PID %d to run\n", current->pid);
    }

    // Execute instruction
    Instruction *inst = fetch_next_instruction(current);
    if (!inst) {
        printf("Process %d completed all instructions\n", current->pid);
        context_switch(NULL, "TERMINATED");
    }

    // Actually execute the instruction
    instruction_execution(current, inst);

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

void instruction_execution(Process *process, Instruction *inst) {
  if (!process || !inst) return;

  switch (inst->opcode) {
      case EXEC:
          printf("[EXEC] PID %d: %d ms\n", process->pid, inst->value);
          process->runtime_execution += inst->value;
          break;

      case P:
          printf("[P] PID %d: Waiting on %c\n", process->pid, inst->semaphore_name);
          sem_P(process, inst->semaphore_name);
          if (process->state == WAITING) {
            return;  // Sai da execução da instrução
        }
          break;

      case V:
          printf("[V] PID %d: Signaling %c\n", process->pid, inst->semaphore_name);
          sem_V(process, inst->semaphore_name);
          break;

      case READ...WRITE:
          IORequest *request = make_request(process, inst->opcode, inst->value);
          enqueue(kernel->queue_requests,request);
          break;

      case PRINT:
          IORequest *print_request = make_request(process, inst->opcode, inst->value);
          enqueue(kernel->queue_requests, print_request);
          break;
  }
}

Instruction* fetch_next_instruction(Process *process) {
  if (!process || !process->page_table) return NULL;

  // Find current page and instruction
  for (int i = 0; i < process->page_table->page_count; i++) {
      Page *page = &process->page_table->pages[i];
      if (process->pc < page->instruction_count) {
          Instruction *inst = &page->instructions[process->pc];
          process->pc++;  // Advance to next instruction
          return inst;
      }
      process->pc -= page->instruction_count;  // Move to next page
  }
  return NULL;  // No more instructions
}

int exec_Instruction(Process *process, Opcode opcode, int arg){
  switch(opcode){
    case READ...WRITE:
      IORequest *request = make_request(process, opcode, arg);
      enqueue(kernel->queue_requests,request);
      break;
    case PRINT:
      IORequest *print_request = make_request(process, opcode, arg);
      enqueue(kernel->queue_requests, print_request);
      break;
    case P:
      printf("[P] PID %d: Waiting on %c\n", process->pid, arg);
      //sem_P(process, arg);
      // if (process->state == WAITING) {
      //   return;  // Sai da execução da instrução
      // }
      break;
    case V:
      printf("[V] PID %d: Signaling %c\n", process->pid, arg);
      //sem_V(process, arg);
      break;
  }
}

int exec_Process(Process *process){
  
}