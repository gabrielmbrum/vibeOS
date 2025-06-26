#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "process.h"
#include <pthread.h>

#define LOCK_IO() pthread_mutex_lock(&queue->iomutex)
#define UNLOCK_IO() pthread_mutex_unlock(&queue->iomutex)
#define IO_Quantum 4
#define Buffer_Size 16 //Buffer size is equal MAX PROCESSES. 

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
    int num_elements;
}IOQueue;

typedef struct Disk{
    int current_trail;
    int buffer_occupation;
    IORequest *current_request;
}Disk;

extern Disk *disk;
IORequest *make_request(Process *process, Opcode opcode, int arg);
IOQueue *init_queue(IOQueue *queue);
void enqueue(IOQueue *queue,IORequest *request); //Enqueues at Printer Requests Queue, following "FIFO" method.
IORequest* dequeue_sstf(IOQueue *queue);  //Dequeues at READ/WRITE on Disk operations, emulating SSTF.
IORequest *dequeue(IOQueue *queue);
void exec_request(IOQueue *queue);
Disk * init_disk();
#endif