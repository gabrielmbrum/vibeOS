#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "process.h"
#include <pthread.h>

#define LOCK_IO() pthread_mutex_lock(&queue->iomutex)
#define UNLOCK_IO() pthread_mutex_unlock(&queue->iomutex)

typedef struct IORequest{
    Process *process;
    Opcode opcode;
    int arg;
    struct IORequest *next;
}IORequest;

typedef struct IOQueue{
    IORequest *head, *tail;
    pthread_mutex_t iomutex;
    pthread_cond_t iocond;
}IOQueue;
IORequest *make_request(Process *process, Opcode opcode, int arg);
IOQueue *init_queue(IOQueue *queue);
void enqueue(IOQueue *queue,IORequest *request);
IORequest *dequeue(IOQueue *queue);
void exec_request(IOQueue *queue);

#endif