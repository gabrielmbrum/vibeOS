#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "process.h"
#include <pthread.h>

#define MAX_SEMAPHORES 15
#define MAX_BLOCKED_PROCESSES 15 

// value = 1: Semáforo livre (processo pode entrar).
// value = 0: Semáforo ocupado (alguém acabou de pegar).
// value < 0: Há processos bloqueados, esperando esse semáforo liberar.

typedef struct {
    char name;
    int value;
    int blocked_pids[MAX_BLOCKED_PROCESSES];  // Fila de bloqueados
    int blocked_count;
    pthread_mutex_t mutex;
} Semaphore;

extern Semaphore semaphores[MAX_SEMAPHORES];
extern int total_semaphores;

void init_semaphores();
Semaphore* get_semaphore(char name);
void register_process_semaphores(Process *process);
Semaphore* find_or_create_semaphore(char name);
void sem_P(Process *process, char sem_name);
void sem_V(Process *process, char sem_name);
Semaphore* create_semaphore_define_value(char name, int value);



#endif