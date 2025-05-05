#include "../include/semaphore.h"
#include "../include/kernel.h"
#include "../include/commons.h"
#include "../include/process.h"

Semaphore semaphores[MAX_SEMAPHORES];
int total_semaphores = 0;

void init_semaphores() {
    //printf("Initializing semaphores...\n");
    total_semaphores = 0;  // Reseta

    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphores[i].name = '\0';  // Só ta inicializando
        semaphores[i].value = 1;    // 1 é livre
        semaphores[i].blocked_count = 0;
        for (int j = 0; j < MAX_BLOCKED_PROCESSES; j++) {
            semaphores[i].blocked_pids[j] = -1;
        }
    }
}

Semaphore* get_semaphore(char name) {
    for (int i = 0; i < total_semaphores; i++) {
        if (semaphores[i].name == name)
            return &semaphores[i];
    }
    return NULL;
}

void register_process_semaphores(Process *process) {
    for (int i = 0; i < strlen(process->semaphores); i++) {
        char sem_name = process->semaphores[i];
        Semaphore *sem = find_or_create_semaphore(sem_name);
        sem->blocked_count = 0;  
    }
}

Semaphore* find_or_create_semaphore(char name) {
    for (int i = 0; i < total_semaphores; i++) {
        if (semaphores[i].name == name) {
            return &semaphores[i];
        }
    }
    
    if (total_semaphores < MAX_SEMAPHORES) {
        Semaphore *sem = &semaphores[total_semaphores];
        sem->name = name;
        sem->value = 1;  // Start unlocked
        sem->blocked_count = 0;
        total_semaphores++;
        //printf("Created new semaphore '%c' at slot %d\n", name, total_semaphores-1);
        return sem;
    }
    
    return NULL;
}

void sem_P(Process *process, char sem_name) {
    if (!process) {
        return;
    }

    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    sem->value--;
    if (sem->value <= 0) {
        if (sem->value < -MAX_BLOCKED_PROCESSES) {
            sem->value++;  
            return FAILURE; 
        }
        if (sem->blocked_count > MAX_BLOCKED_PROCESSES) {
            sem->value++;  
            return FAILURE; 
        }
        // Bloqueia o processo s context_switch
        // for (int i = 0; i < kernel->process_amount; i++) {
        //     if (kernel->BCP[i].pid == process->pid) {
        //         kernel->BCP[i].state = WAITING;
        //         break;
        //     }
        // }

        sem->blocked_pids[sem->blocked_count++] = process->pid;

        printf("[SEM_P] Blocking PID %d from semaphore '%c' (val=%d, blocked=%d/%d)\n", process->pid, sem_name, sem->value, sem->blocked_count, MAX_BLOCKED_PROCESSES);

        context_switch(process, "SEM_BLOCK");
    }
}

void sem_V(Process *process, char sem_name) {
    if (!process) {
        return;
    }

    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    sem->value++;
    if (sem->value <= 0 && sem->blocked_count > 0) {
        int unblocked_pid = sem->blocked_pids[0];

        // Remove da fila de processos bloqueados pelo semáforo
        for (int i = 0; i < sem->blocked_count - 1; i++)
            sem->blocked_pids[i] = sem->blocked_pids[i + 1];
        sem->blocked_count--;

        //tava fazendo a mudança de estado aqui pq nn tinha o context_switch

        // int idx = search_BCP(unblocked_pid);
        // if (idx != FAILURE) {
        //     kernel->BCP[idx].state = READY;

        printf("[SEM_V] Unblocking PID %d from semaphore '%c'\n", unblocked_pid, sem_name);

        context_switch(process, "SEM_UNBLOCK");
    }
}