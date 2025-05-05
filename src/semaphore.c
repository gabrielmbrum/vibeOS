#include "../include/semaphore.h"
#include "../include/kernel.h"
#include "../include/commons.h"
#include "../include/process.h"

Semaphore semaphores[MAX_SEMAPHORES];
int total_semaphores = 0;

void init_semaphores() {
    printf("Initializing semaphores...\n");
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
        printf("Created new semaphore '%c' at slot %d\n", name, total_semaphores-1);
        return sem;
    }
    
    printf("ERROR: No space for new semaphore '%c'\n", name);
    return NULL;
}

void sem_P(Process *process, char sem_name) {
    if (!process) {
        fprintf(stderr, "[CRITICAL] sem_P: NULL process!\n");
        return;
    }

    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    printf("[SEM_P] PID %d acquiring '%c' (val=%d, blocked=%d/%d)\n",
        process->pid, sem_name, sem->value, 
        sem->blocked_count, MAX_BLOCKED_PROCESSES);

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
        // Bloqueia o processo
        for (int i = 0; i < kernel->process_amount; i++) {
            if (kernel->BCP[i].pid == process->pid) {
                kernel->BCP[i].state = WAITING;
                break;
            }
        }
        //process->state = WAITING;
        sem->blocked_pids[sem->blocked_count++] = process->pid;
        printf("[Semaphore_P] Processo %d bloqueado no semáforo %c\n", process->pid, sem_name);
        // Trigger context switch
        Process *next = scheduler_get_next_process(kernel->scheduler);
        context_switch(next, "SEM_BLOCK");
    }
}

void sem_V(Process *process, char sem_name) {
    if (!process) {
        printf("[ERROR] NULL process in sem_V!\n");
        return;
    }

    Semaphore *sem = get_semaphore(sem_name);
    if (!sem) return;

    sem->value++;
    if (sem->value <= 0 && sem->blocked_count > 0) {
        int unblocked_pid = sem->blocked_pids[0];
        printf("[SEM_V] Unblocking PID %d from semaphore '%c'\n", 
            unblocked_pid, sem_name);
        // Remove da fila
        for (int i = 0; i < sem->blocked_count - 1; i++)
            sem->blocked_pids[i] = sem->blocked_pids[i + 1];
        sem->blocked_count--;

        // for (int i = 0; i < kernel->process_amount; i++) {
        //     if (kernel->BCP[i].pid == unblocked_pid) {
        //         kernel->BCP[i].state = READY;
        //         break;
        //     }
        // }
       // process->state = READY;
        // printf("[Semaphore p] Processo %d desbloqueado do semáforo %c\n", unblocked_pid, sem_name);
        int idx = search_BCP(unblocked_pid);
        if (idx != FAILURE) {
            kernel->BCP[idx].state = READY;
            printf("[SEM_V] Unblocked PID %d from %c\n", unblocked_pid, sem_name);
            
            // If nothing else is running, schedule the unblocked process
            if (!kernel->scheduler->running_process) {
                context_switch(&kernel->BCP[idx], "SEM_UNBLOCK");
            }
        }
        
    }
}