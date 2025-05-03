#ifndef KERNEL_H
#define KERNEL_H

#include "process.h"
#include "commons.h"
#include "debug.h"
typedef struct Scheduler{
  Process *running_process;
  int QUANTUM_TIME;
}Scheduler;

typedef struct Kernel{
    Process *BCP;
    Scheduler *scheduler;
    int process_amount;
}Kernel;

extern Kernel *kernel;

void init_BCP();
void init_Kernel();
void processFinish(Process *process);
void processInterrupt(Process *next);
int add_process_to_BCP(Process *process);
int rmv_process_of_BCP(int removing_pid);
int search_BCP(int process_pid);
void change_process_state(Process **process, ProcessState state);
int get_max_rw_process();
void context_switch();
void schedule();
#endif
