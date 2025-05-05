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
    printf("[ERROR] BCP not initialized!\n");
    return FAILURE;
  }
  for (int i = 0; i < MAX_PROCESSES; i++) {
      if (kernel->BCP[i].pid == process_pid) {
          printf("[DEBUG] Found PID %d at BCP index %d\n", process_pid, i);
          return i;
      }
  }
  printf("[ERROR] PID %d not found in BCP!\n", process_pid);
  return FAILURE;
} 
  
int add_process_to_BCP(Process *process) {
  if (!process) return FAILURE;
    
  if (kernel->BCP == NULL) {
      init_BCP();
  }

  for (int i = 0; i < MAX_PROCESSES; i++) {
      if (kernel->BCP[i].pid == EMPTY_BCP_ENTRY) {
          // COPY the process data into BCP slot
          kernel->BCP[i] = *process; 
          // DON'T free the original - let caller manage that
          kernel->process_amount++;
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
    kernel->BCP[idx].pid = EMPTY_BCP_ENTRY;
    kernel->process_amount--;  // CORRECT PLACEMENT
    return SUCCESS;
  }
  return FAILURE;
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

void context_switch(Process *next, const char *reason) {
  // Save current process state
  Process *current = kernel->scheduler->running_process;
  
  if (current) {
      printf("\n[CONTEXT SWITCH] PID %d -> ", current->pid);
      
      // Handle current process state transition
      if (strcmp(reason, "TERMINATED") == 0) {
          printf("Process with PID: %d finished execution...\n", current->pid);
          change_process_state(&current, TERMINATED);
          rmv_process_of_BCP(current->pid);
      } 
      else if (strcmp(reason, "QUANTUM") == 0) {
          printf("Quantum expired, moving to READY\n");
          current->slice_time=0;
          change_process_state(&current, READY);
      }
      else if (strcmp(reason, "I/O") == 0) {
          printf("I/O wait, moving to WAITING\n");
          change_process_state(&current, WAITING);
      }
      else if (strcmp(reason, "SEM_BLOCK") == 0) {
          printf("Semaphore block, moving to WAITING\n");
          current->slice_time=0;
          change_process_state(&current, WAITING);
      }else if(next->state == WAITING && strcmp(reason, "UNBLOCK") == 0) {
        change_process_state(&next, READY);
      }else{
          printf("Unknown reason, moving to READY\n");
          change_process_state(&current, READY);
      }
      kernel->scheduler->running_process = next;
    change_process_state(&next, RUNNING);
  }

  // Set up next process
  if (next) {
      printf("[CONTEXT SWITCH] -> PID %d (%s)\n", next->pid, reason);
      next->state = RUNNING;
      kernel->scheduler->running_process = next;
      
      // Restore process context would go here
      // restore_registers(next->registers);
  } else {
      printf("[CONTEXT SWITCH] -> IDLE\n");
      kernel->scheduler->running_process = NULL;
  }

  // Print updated state
  print_BCP(&kernel->BCP, kernel->process_amount);
}

void processInterrupt(Process *next){ //Quantum atingido. 
  context_switch(next, "QUANTUM"); //\n
}

int time = 0;

void schedule() {
  while (time < 200 && kernel->process_amount > 0) {
      Process *current = kernel->scheduler->running_process;

      if (!current || current->state != RUNNING) {
          int idx = scheduler_POLICY();
          if (idx == FAILURE) {
              printf("Sem processos\n");
              break;
          }
          
          kernel->scheduler->running_process = &kernel->BCP[idx];
          change_process_state(&kernel->scheduler->running_process, RUNNING);
          print_BCP(&kernel->BCP, kernel->process_amount);
          puts("");
          current = kernel->scheduler->running_process;
          printf("Selected PID %d to run\n", current->pid);
      }

      // Execute instruction
      Instruction *inst = fetch_next_instruction(current);
      if (!inst) {
          printf("Process %d completed all instructions\n", current->pid);
          context_switch(NULL, "TERMINATED");
          continue;
      }

      // Actually execute the instruction
      instruction_execution(current, inst);

      // Update timing
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

      if (time % 5 == 0) {
          printf("\n[Time %d]", time);
          print_BCP(&kernel->BCP, kernel->process_amount);
          usleep(100000);
      }
  }
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

void save_process_context(Process *p) {
  // In a real system, this would save register state
  printf("[DEBUG] Saved context for PID %d\n", p->pid);
}

void restore_process_context(Process *p) {
  // In a real system, this would restore register state
  printf("[DEBUG] Restored context for PID %d\n", p->pid);
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

      case READ:
          printf("[READ] PID %d: Reading %c\n", process->pid, (char)inst->value);
          process->state = WAITING;
          processInterrupt(NULL);
          break;

      case WRITE:
          printf("[WRITE] PID %d: Writing %c\n", process->pid, (char)inst->value);
          process->state = WAITING;
          processInterrupt(NULL);
          break;

      case PRINT:
          printf("[PRINT] PID %d: %d\n", process->pid, inst->value);
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