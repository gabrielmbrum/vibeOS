#include "../include/debug.h"
#include "../include/commons.h"

void print_BCP(Process **BCP, int processes) {
  if (*BCP == NULL) {
    printf("BCP is empty.\n");
    return;
  }

  printf("Process Control Block (BCP):\n");
  for (int i = 0; i < processes; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      printf("PID: %d, Name: %s, State: %d, PC: %d, Priority: %d, Counter RW: %d\n",
             (*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].state,
             (*BCP)[i].pc, (*BCP)[i].priority, (*BCP)[i].counter_rw);
    }
  }
}

void print_process(Process *process) {
  if (process == NULL) {
    printf("Process is NULL.\n");
    return;
  }

  printf("Process:\n");
  printf("PID: %d, Name: %s, State: %d, PC: %d, Priority: %d, Counter RW: %d\n",
         process->pid, process->name, process->state,
         process->pc, process->priority, process->counter_rw);
}