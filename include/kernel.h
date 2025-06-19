#ifndef KERNEL_H
#define KERNEL_H

#include "process.h"
#include "commons.h"
#include "debug.h"
#include "iohandler.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct Scheduler{
  Process *running_process;
  int QUANTUM_TIME;
}Scheduler;

typedef struct Kernel{
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
  pthread_t disk_thread;
  pthread_t printer_thread;
  IOQueue *queue_requests;
  IOQueue *printer_queue;
}Kernel;

extern Kernel *kernel;

/*
 * Input thread function: Monitors user input for shutdown command.
 * 
 * Continuously checks for 'q' input to initiate kernel shutdown.
 * Joins itself when shutdown is requested.
 */
void *input_thread_func();

/*
 * Initializes the Process Control Block (BCP) table.
 * 
 * Allocates memory for MAX_PROCESSES and marks all slots as empty.
 * Sets initial process count to zero.
 */
void init_BCP();

/*
 * Scheduling function: Manages process execution.
 * 
 * Implements:
 * 1. Process selection using scheduler policy
 * 2. Time quantum enforcement
 * 3. State transitions (RUNNING, READY, TERMINATED)
 * 4. I/O operation handling
 * 5. Context switching
 */
void schedule();

/*
 * Scheduler thread function: Core scheduling loop.
 * 
 * Waits for processes to be available, then continuously calls schedule().
 * Uses condition variable to sleep when no processes are available.
 */
void *scheduler_thread_func();

/*
 * Starts the scheduler thread.
 * 
 * Initializes scheduler state and creates the scheduler thread.
 */
void start_scheduler();

/*
 * Stops the scheduler thread.
 * 
 * Signals scheduler to stop and joins the thread.
 */
void scheduler_stop();

/*
 * Searches BCP for a process by PID.
 * 
 * Returns index if found, FAILURE otherwise.
 */
int search_BCP(int process_pid);

/*
 * Adds a process to the BCP table.
 * 
 * Finds first empty slot, adds process, and updates process count.
 * Signals scheduler if first process added.
 */
int add_process_to_BCP(Process *process);

/*
 * Initializes the kernel structure.
 * 
 * Creates:
 * - Scheduler component
 * - BCP table
 * - I/O queues (disk, printer)
 * - Synchronization primitives
 * - Management threads (input, disk, printer, I/O)
 */
void init_Kernel();

/*
 * Initializes the kernel structure.
 * 
 * Creates:
 * - Scheduler component
 * - BCP table
 * - I/O queues (disk, printer)
 * - Synchronization primitives
 * - Management threads (input, disk, printer, I/O)
 */
void shutdown_Kernel();


/*
 * Marks a process as terminated.
 * 
 * Resets PID and I/O counter to indicate empty slot.
 */
void processFinish(Process *process);

/*
 * Removes a process from BCP by PID.
 * 
 * Verifies no pending I/O operations before removal.
 */
int rmv_process_of_BCP(int removing_pid);

/*
 * Scheduling policy: Selects next process to run.
 * 
 * Criteria:
 * 1. Process with fewest I/O operations (counter_rw)
 * 2. On tie: Lower PID wins
 */
int scheduler_POLICY();

/*
 * Changes a process's state.
 * 
 * Valid states: RUNNING, READY, WAITING, TERMINATED.
 */
void change_process_state(Process **process, ProcessState state);

/*
 * Performs context switching between processes.
 * 
 * Handles state transitions based on reason:
 * - "QUANTUM": Time slice expired
 * - "TERMINATED": Process finished
 * - "I/O": I/O operation started
 * - "SEM_BLOCK": Semaphore block
 * - "SEM_UNBLOCK": Semaphore unblock
 */
void context_switch();


/*
 * Handles process interruption (quantum expiration).
 * 
 * Triggers context switch to next process.
 */
void processInterrupt(Process *next);

/*
 * Handles process interruption (quantum expiration).
 * 
 * Triggers context switch to next process.
 */
int exec_Instruction(Process *process, Opcode opcode, int arg);

/*
 * Executes semaphore operations (P/V).
 */
int exec_Instruction_semaphore(Process *process, Opcode opcode, char arg);

/*
 * Calculates total instructions in a process.
 */
int get_total_instructions(PageTable *pt);

/*
 * Executes the next instruction in a process.
 * 
 * Handles:
 * - Instruction fetching
 * - Semaphore and I/O operations
 * - Program counter advancement
 * - State transitions (TERMINATED, WAITING)
 */
int processExecute(Process *process);


/*
 * Executes a disk I/O request.
 * 
 * Uses SSTF policy to select request.
 * Simulates READ/WRITE operations with file I/O.
 * Updates process state upon completion.
 */
void exec_request(IOQueue *queue);

/*
 * I/O thread function: Processes disk requests.
 * 
 * Continuously checks for and executes pending disk I/O operations.
 */
void *io_thread_func();

/*
 * Printer thread function: Simulates print jobs.
 * 
 * Processes print requests by simulating work with usleep.
 */
void *printer_thread_func();

/*
 * Disk thread function: Moves requests to disk buffer.
 * 
 * Transfers requests from main queue to disk buffer when space available.
 */
void *disk_thread_func();


#endif