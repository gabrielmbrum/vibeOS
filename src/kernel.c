#include "../include/process.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/debug.h"
#include "../include/memory.h"
#include "../include/semaphore.h"
#include "../include/iohandler.h"
#include "../include/interface.h"

#define LOCK_BCP() pthread_mutex_lock(&kernel->bcp_mutex)
#define UNLOCK_BCP() pthread_mutex_unlock(&kernel->bcp_mutex)
#define IOException -101
#define RWTimeSlice 30 
Kernel *kernel=NULL;

void *input_thread_func() {
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
  pthread_join(kernel->input_thread, NULL);
  ("Encerraaaado");
  return NULL;
} 

void init_BCP() {
    kernel->BCP = malloc(sizeof(Process) * MAX_PROCESSES);
    if (kernel->BCP == NULL) {
      //fprintf(stderr, "Memory allocation failed\n");
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

    if (result == IOException);
      //printf("Processo %d bloqueado para I/O \n", current->pid);
    else if (result == TERMINATED) {
      int idx = scheduler_POLICY();
      if (idx != FAILURE) {
        Process *next = &kernel->BCP[idx];
        context_switch(next, "TERMINATED");
      }
    }
  }
  //print_BCP(&kernel->BCP, kernel->process_amount);
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
    pthread_create(&kernel->scheduler_thread, NULL, scheduler_thread_func,
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
      print_bcp(&kernel->BCP);
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
    //fprintf(stderr, "Kernel already initialized\n");

    return;
  }

  kernel = malloc(sizeof(Kernel));
  if (kernel == NULL) {
    //fprintf(stderr, "Failed to allocate memory for Kernel\n");
    exit(EXIT_FAILURE);
  }

  // Aloca e inicializa o scheduler
  kernel->scheduler = malloc(sizeof(Scheduler));
  if (kernel->scheduler == NULL) {
    //fprintf(stderr, "Failed to allocate memory for Scheduler\n");
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
  pthread_mutex_init(&kernel->printer_cond, NULL);
  pthread_cond_init(&kernel->printer_cond, NULL);
  kernel->scheduler->running_process = NULL;
  kernel->scheduler->QUANTUM_TIME = 200;
  kernel->scheduler_running = false;
  kernel->shutdown_request = false;
  kernel->queue_requests = init_queue(kernel->queue_requests);
  kernel->printer_queue = init_queue(kernel->printer_queue);
  //pthread_create(&kernel->input_thread, NULL, input_thread_func, NULL);
  pthread_create(&kernel->disk_thread, NULL, disk_thread_func, NULL);
  init_disk();
  pthread_create(&kernel->io_thread, NULL, io_thread_func, NULL);
}

void shutdown_Kernel() {
  kernel->shutdown_request = true;
  kernel->scheduler_running = false;
  LOCK_BCP();
  pthread_cond_signal(&kernel->bcp_cond); // Acorda o scheduler
  UNLOCK_BCP();
  pthread_join(kernel->input_thread, NULL);
  pthread_join(kernel->scheduler_thread, NULL);
  pthread_join(kernel->disk_thread, NULL);
  free(disk);
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
  int idx = -1, min_rw=INT_MAX;
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
  //("Trocando contexto por motivos");

  if(strcmp(arg, "QUANTUM")==0){
    running_process->slice_time=0;
    change_process_state(&running_process, READY);
  }
  else if (strcmp(arg, "TERMINATED") == 0) {
    //printf("Process with PID: %d finished execution...\n", running_process->pid);
    change_process_state(&running_process, TERMINATED);
    rmv_process_of_BCP(running_process->pid);
    //("Processo finalizado removido da BCP!");

  }
  else if (strcmp(arg, "I/O") == 0) {
      change_process_state(&running_process, WAITING);
  }
  else if (strcmp(arg, "SEM_BLOCK") == 0) {
      //printf("Process with PID: %d blocked by semaphore.\n", running_process->pid);
      change_process_state(&running_process, WAITING);
  }
  else if (strcmp(arg, "SEM_UNBLOCK") == 0) {
    //printf("Process with PID: %d unblocked by semaphore.\n", running_process->pid);
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
  //printf("\nExecutando %s...\n",opcode_to_string(opcode));
  switch(opcode){
    case READ...WRITE: {
      IORequest *request;
      request = make_request(process, opcode, arg);
      enqueue(kernel->queue_requests,request);
      pthread_cond_signal(&kernel->queue_requests->iocond);
      return IOException;
      break;
    }
    case PRINT: //MODIFY
      IORequest *print_request;
      print_request = make_request(process, opcode, arg);
      enqueue(kernel->queue_requests, print_request);
      pthread_cond_signal(&kernel->queue_requests->iocond);
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
        break;; 
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
    change_process_state(&process, TERMINATED);
    return TERMINATED;
  }

  // Obtém a página e instrução atual
  int current_page = process->pc.last_page;
  int current_instruction = process->pc.last_instruction;
  Page *page = &current_pt->pages[current_page];

  //Pega instrução
  Instruction *inst = &page->instructions[current_instruction];


  print_instruction(*inst);

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
      change_process_state(&process, TERMINATED);
      return TERMINATED;
  }

  return SUCCESS;
}



void exec_request(IOQueue *queue){
    if(kernel->queue_requests->num_elements <=0) return;
    int idx_request_to_be_executed = sstf_policy(disk);
    if(idx_request_to_be_executed == FAILURE) return;
    LOCK_DISK();
    IORequest *request = pick_request(idx_request_to_be_executed);
    if(!request || request->process->pid == EMPTY_BCP_ENTRY) {
      UNLOCK_DISK();
      return;
    }
    FILE *buffer =  NULL;
    disk->current_trail = request->arg;
    switch(request->opcode){
        case WRITE:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,  request->arg,SEEK_SET);
            int to_be_written = rand() % 2;
            fwrite(&to_be_written,sizeof(int),1,buffer);
            //printf("%s %d\n", "Escrita da trilha", request->arg);
            usleep(IO_QUANTUM*100);
            fclose(buffer);
            //("Arquivo fechado com sucesso!");
        break;
        case READ:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,request->arg,SEEK_SET);
            int data;
            fread(&data, sizeof(int), 1, buffer);
            //printf("Leitura da trilha %d: %c\n", request->arg, data);           
            usleep(IO_QUANTUM*100);
            pthread_cond_signal(&queue->iocond);
            fclose(buffer);
            ("Arquivo fechado com sucesso!");
        break;
        default:
        break;
    }
    LOCK_BCP();
    if(request->process->state == WAITING) {
      change_process_state(&request->process, READY);
    }
    pthread_cond_signal(&kernel->bcp_cond);
    UNLOCK_BCP();
    disk->buffer_occupation --;
    pthread_cond_signal(&disk->disk_cond);
    UNLOCK_DISK();
    request->process->counter_rw++;
    return;
}

void *io_thread_func() {
  while (!kernel->shutdown_request) {
    
    if(disk->buffer_occupation > 0){
      exec_request(kernel->queue_requests);
    }
  }
  return NULL;
}

void *disk_thread_func(){
   pthread_mutex_lock(&kernel->queue_requests->iomutex);
        while (!kernel->queue_requests->head) {
            pthread_cond_wait(&kernel->queue_requests->iocond, &kernel->queue_requests->iomutex);
       }
        pthread_mutex_unlock(&kernel->queue_requests->iomutex);

  while (!kernel->shutdown_request) {
        LOCK_DISK();
        while (disk->buffer_occupation < buffer_size) {

            UNLOCK_DISK();
            move_to_disk_buffer(&kernel->queue_requests);
            LOCK_DISK();
        }
        UNLOCK_DISK();
        usleep(100);  
    }
    return NULL;
}