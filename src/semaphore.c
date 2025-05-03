#include "../include/semaphore.h"
#include "../include/kernel.h"
#include "../include/commons.h"

Semaphore semaphores[MAX_SEMAPHORES];
int total_semaphores = 0;

void init_semaphores(char *sem_list) {      
    for (int i = 0; sem_list[i] != '\0'; i++) {
        semaphores[total_semaphores].name = sem_list[i];
        semaphores[total_semaphores].value = 1;
        semaphores[total_semaphores].blocked_count = 0;
        total_semaphores++;
    }
}

Semaphore* get_semaphore(char name) {
    for (int i = 0; i < total_semaphores; i++) {
        if (semaphores[i].name == name)
            return &semaphores[i];
    }
    return NULL;
}

void sem_P(Process *process, char sem_name) {
    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    sem->value--;
    if (sem->value < 0) {
        // Bloqueia o processo
        process->state = WAITING;
        sem->blocked_pids[sem->blocked_count++] = process->pid;
        printf("[Semaphore] Processo %d bloqueado no semáforo %c\n", process->pid, sem_name);
        processInterrupt(NULL); // Força troca de contexto
    }
}

void sem_V(Process *process, char sem_name) {
    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    sem->value++;
    if (sem->value <= 0 && sem->blocked_count > 0) {
        int unblocked_pid = sem->blocked_pids[0];

        // Remove da fila
        for (int i = 0; i < sem->blocked_count - 1; i++)
            sem->blocked_pids[i] = sem->blocked_pids[i + 1];
        sem->blocked_count--;

        int index = search_BCP(unblocked_pid);
        if (index != FAILURE) {
            kernel->BCP[index].state = READY;
            printf("[Semaphore] Processo %d desbloqueado do semáforo %c\n", unblocked_pid, sem_name);
        }
    }
}