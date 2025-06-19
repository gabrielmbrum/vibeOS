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
    //printf("Executando processo %d\n", kernel->scheduler->running_process->pid);
    print_win_args(janela_process,"Executando processo %d\n", kernel->scheduler->running_process->pid);
    if (result == IOException){
      //printf("Processo %d bloqueado para I/O \n", current->pid);
      print_win_args(janela_I_O,"Processo %d bloqueado para I/O \n", current->pid);
    
      kernel->scheduler->running_process = NULL;
    }
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
        kernel->scheduler->running_process = NULL;
      }
    }
  }

  usleep(1000);
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
  //printf("Adicionando processo %d\n", process->pid);
  print_win_args(janela_OUTPUT, "Adicionando processo %d\n", process->pid);
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
  kernel->scheduler->running_process = NULL;
  kernel->scheduler->QUANTUM_TIME = 200;
  kernel->scheduler_running = false;
  kernel->shutdown_request = false;
  kernel->queue_requests = init_queue(kernel->queue_requests);
  kernel->printer_queue = init_queue(kernel->printer_queue);
  init_disk();
  pthread_create(&kernel->input_thread, NULL, input_thread_func, NULL);
  pthread_create(&kernel->disk_thread, NULL, disk_thread_func, NULL);
  pthread_create(&kernel->printer_thread, NULL, printer_thread_func, NULL);
  pthread_create(&kernel->io_thread, NULL, io_thread_func, NULL);
}

void shutdown_Kernel() {
  kernel->shutdown_request = true;
  kernel->scheduler_running = false;

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

  if(kernel->BCP[removing_pid].waiting_operations>0){
    UNLOCK_BCP();
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
  //puts("politica aplicada encontrou");
  //printf("PID: %d", kernel->BCP[idx].pid);
  //puts("");
  print_win(janela_SCHEDULER,"politica aplicada encontrou");
  print_win_args(janela_SCHEDULER, "PID: %d", kernel->BCP[idx].pid);
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
    print_win_args(janela_process,"Process with PID: %d finished execution...", running_process->pid);
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
  switch(opcode){
    case READ...WRITE: {
      
      IORequest *request;
      request = make_request(process, opcode, arg);
      //printf("\ninserindo %s operation...\n",opcode_to_string(request->opcode));
      print_win_args(janela_process,"inserindo %s operation...",opcode_to_string(request->opcode));
      enqueue(kernel->queue_requests,request);
      pthread_cond_signal(&kernel->queue_requests->iocond);
      return IOException;
      break;
    }
    case PRINT: //MODIFY
      IORequest *print_request;
      print_request = make_request(process, opcode, arg);
      enqueue(kernel->printer_queue, print_request);
      pthread_cond_signal(&kernel->printer_queue->iocond);
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
  if (process->pc.global_index >= total_instructions && process->waiting_operations<=0) {
    change_process_state(&process, TERMINATED);
    return TERMINATED;
  }

  // Obtém a página e instrução atual
  int current_page = process->pc.last_page;
  int current_instruction = process->pc.last_instruction;
  Page *page = &current_pt->pages[current_page];

  //Pega instrução
  Instruction *inst = &page->instructions[current_instruction];


  //print_instruction(*inst);

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
  if (process->pc.global_index >= total_instructions&& process->waiting_operations<=0) {
      change_process_state(&process, TERMINATED);
      return TERMINATED;
  }

  return SUCCESS;
}



void exec_request(IOQueue *queue){
    pthread_mutex_lock(&queue->iomutex);
    LOCK_DISK();
     if (disk->buffer_occupation <= 0) {
        pthread_mutex_unlock(&queue->iomutex);
        UNLOCK_DISK();
        return;
    }
    
    int idx_request_to_be_executed = sstf_policy(disk);
    if(idx_request_to_be_executed == FAILURE){
      UNLOCK_DISK();
      pthread_mutex_lock(&queue->iomutex);
      return;
    }
    IORequest *request = pick_request(idx_request_to_be_executed);
    //printf("Request feito por %s with pid %d\n", request->process->name, request->process->pid);
    print_win_args(janela_process, "Request feito por %s with pid %d\n", request->process->name, request->process->pid);
    FILE *buffer =  NULL;
    disk->current_trail = request->arg; 
    for (int i = idx_request_to_be_executed; i < disk->buffer_occupation - 1; i++) {
        disk->requests[i] = disk->requests[i+1];
    }
    disk->buffer_occupation--;
    request->process->waiting_operations--;
    UNLOCK_DISK();
    pthread_mutex_unlock(&queue->iomutex);
    //printf("Executando %s\n", opcode_to_string(request->opcode));
    print_win_args(janela_SCHEDULER,"Executando %s", opcode_to_string(request->opcode));
    switch(request->opcode){
        case WRITE:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,  request->arg,SEEK_SET);
            int to_be_written = rand() % 2;
            fwrite(&to_be_written,sizeof(int),1,buffer);
            //printf("%s %d\n", "Escrita da trilha", request->arg);
            print_win_args(janela_SCHEDULER,"%s %d", "Escrita da trilha", request->arg);
            usleep(IO_QUANTUM);
            fclose(buffer);
            //("Arquivo fechado com sucesso!");
        break;
        case READ:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,request->arg,SEEK_SET);
            int data;
            fread(&data, sizeof(int), 1, buffer);
            //printf("Leitura da trilha %d: %c\n", request->arg, data);     
            print_win_args(janela_SCHEDULER,"Leitura da trilha %d: %c", request->arg, data);   
            usleep(IO_QUANTUM);
            fclose(buffer);
        break;
        default:
        break;
    }
    //printf("Processo esperando: %d\n", request->process->waiting_operations);
    print_win_args(janela_SCHEDULER,"Processo esperando: %d\n", request->process->waiting_operations);
    LOCK_BCP();
    if(request->process->state == WAITING) {
      change_process_state(&request->process, READY);
    }
    UNLOCK_BCP();
    pthread_cond_signal(&kernel->bcp_cond);
    request->process->counter_rw++;
    //printf("Pedidos em disco agora: %d", disk->buffer_occupation);
    print_win_args(janela_SCHEDULER,"Pedidos em disco agora: %d", disk->buffer_occupation);

    return;   
}

void *io_thread_func() {
    while (!kernel->shutdown_request) {
        LOCK_DISK();
        pthread_mutex_lock(&kernel->queue_requests->iomutex);
        
        // Processa TODOS os requests pendentes
        while (disk->buffer_occupation > 0) {
            pthread_mutex_unlock(&kernel->queue_requests->iomutex);
            UNLOCK_DISK();
            
            exec_request(kernel->queue_requests);
            
            LOCK_DISK();
            pthread_mutex_lock(&kernel->queue_requests->iomutex);
        }
        
        pthread_mutex_unlock(&kernel->queue_requests->iomutex);
        UNLOCK_DISK();
        usleep(1000);
    }
    return NULL;
}

void *printer_thread_func(){
  while(!kernel->shutdown_request){
    if(kernel->printer_queue->num_elements >0){
      IORequest *print_request = dequeue(kernel->printer_queue);
      for(int i=0;i<print_request->arg;i++){
        if(i%20 ==0)
        print_win_args(janela_SCHEDULER,"Processo: %d solicitou impressora\n", print_request->process->pid);
        //printf("Processo: %d solicitou impressora\n", print_request->process->pid);
        //There will be a printer window, where a process will be printing for *arg amount of time (in ms). 
        //Be creative in what can be this action of printing, what will be printed, idk what it must be.
        
      }
      print_request->process->waiting_operations--;
      print_win_args(janela_SCHEDULER,"Processo esperando ainda %d operações\n", print_request->process->waiting_operations);
      //printf("Processo esperando ainda %d operações\n", print_request->process->waiting_operations);
    }
  }
  return NULL;
}
void *disk_thread_func() {
    while (!kernel->shutdown_request) {
        // Trava APENAS o mutex da fila para verificar condições
        pthread_mutex_lock(&kernel->queue_requests->iomutex);
        
        if (disk->buffer_occupation < buffer_size && 
            kernel->queue_requests->num_elements > 0) {
            // Libera o mutex da fila antes de chamar move_to_disk_buffer
            // (pois ela já vai travar os mutexes na ordem correta)
            pthread_mutex_unlock(&kernel->queue_requests->iomutex);
            
            move_to_disk_buffer(kernel->queue_requests);
        } else {
            pthread_mutex_unlock(&kernel->queue_requests->iomutex);
        }

        usleep(1000);  // Evita busy-waiting
    }
    return NULL;
}