#ifndef KERNEL_H
#define KERNEL_H

#include "commons.h"
#include "debug.h"
#include "iohandler.h"
#include "process.h"
#include <pthread.h>
#include <stdbool.h>
typedef struct Scheduler {
  Process *running_process;
  int QUANTUM_TIME;
} Scheduler;

typedef struct Kernel {
  Process *BCP;
  Scheduler *scheduler;
  int process_amount;
  bool scheduler_running;
  bool shutdown_request;
  pthread_t scheduler_thread;
  pthread_mutex_t bcp_mutex;
  pthread_cond_t bcp_cond;
  pthread_t io_thread;
  pthread_t input_thread;
  IOQueue *queue_requests;
} Kernel;

extern Kernel *kernel;

void init_BCP();
void init_Kernel();
void processFinish(Process *process);
void processInterrupt(Process *next); 
int add_process_to_BCP(Process *process);
int rmv_process_of_BCP(int removing_pid);
int search_BCP(int process_pid);
void change_process_state(Process **process, ProcessState state);
int scheduler_POLICY();
void context_switch();
void schedule();
void scheduler_stop();
void start_scheduler();
void *scheduler_thread_func(void *arg);
void shutdown_Kernel();
int processExecute(Process *process);
int exec_Instruction_semaphore(Process *process, Opcode opcode, char arg);
int exec_Instruction(Process *process, Opcode opcode, int arg);
void *io_thread_func(void *arg);
void context_switch(Process *next, char *arg);

#endif