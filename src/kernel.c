#include "../include/process.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/debug.h"
#include "../include/memory.h"
#include "../include/semaphore.h"
#include "../include/iohandler.h"
#include "../include/interface.h"
#include <limits.h>
#include <string.h>

#define LOCK_BCP() pthread_mutex_lock(&kernel->bcp_mutex)
#define UNLOCK_BCP() pthread_mutex_unlock(&kernel->bcp_mutex)

#define IOException -101
#define RWTimeSlice 30 
Kernel *kernel = NULL;

//interface
pthread_mutex_t interface_mutex;
pthread_mutex_t dados_mutex;
pthread_t interface_thread;  


void *printer_thread_func(){
  while(!kernel->shutdown_request){
    if(kernel->printer_queue->num_elements >0){
      IORequest *print_request = dequeue(kernel->printer_queue);
      for(int i=0;i<print_request->arg;i++){
        if(i%10 == 0){
          update_dados(janela_PRINT, "PID %d using printer...", print_request->process->pid);
        }        
      }
    }
  }
  return NULL;
}

void init_BCP() {
    kernel->BCP = malloc(sizeof(Process) * MAX_PROCESSES);
    if (kernel->BCP == NULL) {
      update_dados(janela_memory, "Memory allocation failed.");
      exit(EXIT_FAILURE);
    }
  
    for (int i = 0; i < MAX_PROCESSES; i++) {
        kernel->BCP[i].pid = EMPTY_BCP_ENTRY; // Define o slot como vazio
    }
    kernel->process_amount = 0;
}

void schedule() {
  Process *current = kernel->scheduler->running_process;
  int result;
  // Se não há processo rodando OU o processo atual é inválido
  if (!current || current->pid < 0) {
    int idx = scheduler_POLICY();
    // Se não há processos válidos, encerra o escalonador
    if (idx == FAILURE) {
      return;
    }

    kernel->scheduler->running_process = &kernel->BCP[idx];
    change_process_state(&kernel->scheduler->running_process, RUNNING);
    print_SCHEDULER(&kernel->BCP);
    current = kernel->scheduler->running_process;
    result = processExecute(current);

    if (result == IOException){
      update_dados(janela_process, "PID: %d blocked for I/O.", current->pid);
    }else if (result == TERMINATED) {
      int idx = scheduler_POLICY();
      if (idx != FAILURE) {
        Process *next = &kernel->BCP[idx];
        context_switch(next, "TERMINATED");
      }
    }
  }
  // Se chegou aqui, current é válido (pid >= 0)
  if(current->pid!=EMPTY_BCP_ENTRY) current->slice_time++;
  if(current->runtime_execution>0)current->runtime_execution--;
  if (current->slice_time >= kernel->scheduler->QUANTUM_TIME) {
    int idx = scheduler_POLICY();
    if (idx != FAILURE) {
      Process *next = &kernel->BCP[idx];
      processInterrupt(next);
      result = processExecute(next);
      if(result == TERMINATED){
        context_switch(current, "TERMINATED");
      }
    }
  }
  usleep(1);
}

void *scheduler_thread_func() {
  while (kernel->scheduler_running && !kernel->shutdown_request) {
    LOCK_BCP();
    while (kernel->process_amount == 0 && kernel->scheduler_running) {
      pthread_cond_wait(&kernel->bcp_cond, &kernel->bcp_mutex);
    }

    if (kernel->process_amount > 0) {
      schedule();
    }
    UNLOCK_BCP();
    usleep(1000);
  }
  return NULL;
}

void start_scheduler() {
  if (!kernel->scheduler_running) {
    kernel->scheduler_running = true;
    pthread_create(&kernel->scheduler_thread,0, scheduler_thread_func,
                   NULL);
  }
}

void scheduler_stop() {
  if (kernel->scheduler_running) {
    kernel->scheduler_running = false;
    pthread_join(kernel->scheduler_thread, NULL);
  }
}

int search_BCP(int process_pid) {
  if (kernel->BCP == NULL) {
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
    if (kernel->BCP[i].pid ==
        EMPTY_BCP_ENTRY) { // Assuming pid 0 means empty slot
      kernel->BCP[i] = *process;
      free(process);
      kernel->process_amount++;
      pthread_cond_signal(&kernel->bcp_cond);
      UNLOCK_BCP();
      //print_bcp(&kernel->BCP);
      if (kernel->process_amount == 1 && !kernel->scheduler_running)
        start_scheduler();
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
    update_dados(janela_SCHEDULER, "Kernel already initialized.");

    return;
  }

  kernel = malloc(sizeof(Kernel));
  if (kernel == NULL) {
    update_dados(janela_memory, "Failed to allocate memory for Kernel.");
    exit(EXIT_FAILURE);
  }

  // Aloca e inicializa o scheduler
  kernel->scheduler = malloc(sizeof(Scheduler));
  if (kernel->scheduler == NULL) {
    update_dados(janela_memory, "Failed to allocate memory for Scheduler.");
    free(kernel);
    exit(EXIT_FAILURE);
  }

  pthread_mutex_init(&kernel->bcp_mutex, NULL);
  pthread_cond_init(&kernel->bcp_cond, NULL);
  LOCK_BCP();
  init_BCP(); // Inicializa o BCP
  // Configura o scheduler
  pthread_cond_signal(&kernel->bcp_cond);
  UNLOCK_BCP();
  kernel->scheduler->running_process = NULL;
  kernel->scheduler->QUANTUM_TIME = 200;
  kernel->scheduler_running = false;
  kernel->shutdown_request = false;
  kernel->queue_requests = init_queue(kernel->queue_requests);
  kernel->printer_queue = init_queue(kernel->printer_queue);
  disk = init_disk();
  pthread_create(&kernel->io_thread, NULL, io_thread_func, NULL);
  pthread_create(&kernel->printer_thread, NULL, printer_thread_func, NULL);
}

void shutdown_Kernel() {
  kernel->shutdown_request = true;
  kernel->scheduler_running = false;
  LOCK_BCP();
  pthread_cond_signal(&kernel->bcp_cond); // Acorda o scheduler
  UNLOCK_BCP();
  pthread_join(kernel->input_thread, NULL);
  pthread_join(kernel->scheduler_thread, NULL);

  free(kernel->BCP);
  free(kernel->scheduler);
  free(kernel->queue_requests);
  free(kernel);
}

void processFinish(Process *process) {
  process->pid =
      EMPTY_BCP_ENTRY; // Pointing some things about this to be discussed later
  process->counter_rw = -1;
}

int rmv_process_of_BCP(int removing_pid) {
  if (kernel->BCP == NULL) { // BCP not allocated, return Failure.
    return FAILURE;
  }

  int idx = search_BCP(removing_pid);
  if (idx != FAILURE) {
    // If PID present in BCP, get the index and remove;
    processFinish(&kernel->BCP[idx]);
    kernel->process_amount--;
    print_bcp(&kernel->BCP);
    return SUCCESS;
  }
  return FAILURE; // If Search in BCP Failed, then PID not present in BCP,
                  // return FAILURE.
}

int scheduler_POLICY(){
  int idx = -1, min_rw = INT_MAX;
  for(int i=0;i<MAX_PROCESSES;i++){
      if(kernel->BCP[i].counter_rw < min_rw &&
        kernel->BCP[i].pid>=0 &&
        kernel->BCP[i].state!=TERMINATED){
          min_rw = kernel->BCP[i].counter_rw;
           idx = i;
      }
      else if (kernel->BCP[i].counter_rw==min_rw && kernel->BCP[idx].pid > kernel->BCP[i].pid && kernel->BCP[i].pid>=0){
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
    update_dados(janela_SCHEDULER, "Process with PID: %d finished execution...", running_process->pid);
    change_process_state(&running_process, TERMINATED);
    rmv_process_of_BCP(running_process->pid);

  }
  else if (strcmp(arg, "I/O") == 0) {
      change_process_state(&running_process, WAITING);
  }
  else if (strcmp(arg, "SEM_BLOCK") == 0) {
      change_process_state(&running_process, WAITING);
  }
  else if (strcmp(arg, "SEM_UNBLOCK") == 0) {
    change_process_state(&running_process, READY);
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

int exec_Instruction(Process *process, Opcode opcode, int arg){
  switch(opcode){
    case READ...WRITE: {
      IORequest *request;
      request = make_request(process, opcode, arg);
      enqueue(kernel->queue_requests,request);
      pthread_cond_signal(&kernel->queue_requests->iocond);
      update_dados(janela_process, "PID: %d request %s operation.", process->pid,opcode_to_string(opcode));
      return IOException;
      break;
    }
    case PRINT:
      IORequest *print_request;
      print_request = make_request(process, opcode, arg);
      enqueue(kernel->printer_queue, print_request);
      pthread_cond_signal(&kernel->printer_queue->iocond);
      update_dados(janela_process, "PID: %d request %s operation.", process->pid,opcode_to_string(opcode));
      return IOException;
      break;
    case EXEC:
      process->runtime_execution+=arg;
      break;
    default:
      break;
  }
  return SUCCESS;
}

int exec_Instruction_semaphore(Process *process, Opcode opcode, char arg){
  switch(opcode){
    case P:
      if (process->state == WAITING) {
        break;
      }
      sem_P(process, arg);
      break;
    case V:
      sem_V(process, arg);
      break;
    default: break;
  }
  return SUCCESS;
}

int get_total_instructions(PageTable *pt) {
  int total = 0;
  for (int i = 0; i < pt->page_count; i++) {
      total += pt->pages[i].instruction_count;
  }
  return total;
}

int processExecute(Process *process){
  if(!process || process->state == TERMINATED){
    return FAILURE;
  }

  PageTable *current_pt = process->page_table;
  int total_instructions = get_total_instructions(current_pt);

  // Verifica se o processo já terminou (PC >= total_instructions)
  if (process->pc.global_index >= total_instructions) {
    if (current_pt->missing_instructions) {
      char program_name[MAX_OUTPUT_STR] = "../programs/";
      strcat(program_name, process->name);
      Program *program = read_program(program_name);
      if (program == NULL) {
        update_dados(janela_memory, "Failed to read program with the rest of instructions!");
        return FAILURE;
      }
      refresh_page_table(&process->page_table, program->instructions, program->instructions_count, current_pt->last_instruction_loaded);
      free_program(program);
      process->pc.global_index = 0; // Reseta o índice global do PC
      process->pc.last_page = 0; // Reseta a última página
      process->pc.last_instruction = 0; // Reseta a última instrução
      processExecute(process); // Re-executa o processo após atualizar a tabela de páginas
    } else {
      change_process_state(&process, TERMINATED);
      return TERMINATED;
    }
  }

  // Obtém a página e instrução atual
  int current_page = process->pc.last_page;
  int current_instruction = process->pc.last_instruction;
  Page *page = &current_pt->pages[current_page];

  //Pega instrução
  Instruction *inst = &page->instructions[current_instruction];

  Opcode op = P;
  Opcode op2 = V;

  int result;
  //Verifica se é instrução de semáforo  executa
  if((inst->opcode == op) || (inst->opcode == op2)){
    exec_Instruction_semaphore(process, inst->opcode, inst->semaphore_name);
  }else{
    result = exec_Instruction(process, inst->opcode, inst->value);
  }

  // Avança o PC
  process->pc.last_instruction++;
  process->pc.global_index++;

  // Se terminou a página atual, avança para a próxima
  if (process->pc.last_instruction >= page->instruction_count) {
      process->pc.last_page++;
      process->pc.last_instruction = 0;
  }

  // Trata resultados (I/O ou término)
  if (result == IOException) {
      change_process_state(&process, WAITING);
      return IOException;
  }

  // Verifica se terminou todas as instruções
  if (process->pc.global_index >= total_instructions) {
    if (current_pt->missing_instructions) {
      char program_name[MAX_OUTPUT_STR] = "../programs/";
      strcat(program_name, process->name);

      Program *program = read_program(program_name);
      if (program == NULL) {
        update_dados(janela_memory, "Failed to read program with the rest of instructions!");
        return FAILURE;
      }
      refresh_page_table(&process->page_table, program->instructions, program->instructions_count, current_pt->last_instruction_loaded);
      free_program(program);
      process->pc.global_index = 0; // Reseta o índice global do PC
      process->pc.last_page = 0; // Reseta a última página
      process->pc.last_instruction = 0; // Reseta a última instrução
      processExecute(process); // Re-executa o processo após atualizar a tabela de páginas
    } else {
      change_process_state(&process, TERMINATED);
      return TERMINATED;
    }
  }

  return SUCCESS;
}

void *io_thread_func() {
  while (!kernel->shutdown_request) {

    exec_request(kernel->queue_requests);
    LOCK_BCP();
    /*if (req->process->state == WAITING){
      //puts("Processo estava esperando I/O..");
      //print_win(janela_SCHEDULER,"Processo estava esperando I/O..");
      //printf("Operação: %s %d\n", opcode_to_string(req->opcode), req->arg);
      //print_win_args(janela_SCHEDULER,"Operação: %s %d", opcode_to_string(req->opcode), req->arg);
      change_process_state(&req->process, READY);
    }*/
    change_process_state(&disk->current_request->process, READY);

    free(disk->current_request);
    UNLOCK_BCP();
  }
  return NULL;
}