#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "process.h"

#define MAX_SEMAPHORES 10

typedef struct {
    char name;
    int value;
    int blocked_pids[MAX_PROCESSES];  // Fila de bloqueados
    int blocked_count;
} Semaphore;

extern Semaphore semaphores[MAX_SEMAPHORES];
extern int total_semaphores;

void init_semaphores(char *sem_list);
Semaphore* get_semaphore(char name);
void sem_P(Process *process, char sem_name);
void sem_V(Process *process, char sem_name);

#endif
