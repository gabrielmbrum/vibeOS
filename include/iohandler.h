#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "process.h"
#include <pthread.h>
#include <math.h>
#include <limits.h>

#define LOCK_IO(queue) pthread_mutex_lock(&(queue)->iomutex)
#define UNLOCK_IO(queue) pthread_mutex_unlock(&(queue)->iomutex)
#define LOCK_DISK() pthread_mutex_lock(&disk->disk_lock)
#define UNLOCK_DISK() pthread_mutex_unlock(&disk->disk_lock)

#define buffer_size 32
#define IO_QUANTUM 4000

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
    unsigned int num_elements;
}IOQueue;

typedef struct Disk{
    IORequest requests[buffer_size];
    int current_trail;
    int buffer_occupation;
    pthread_mutex_t disk_lock;
    pthread_cond_t disk_cond;
}Disk;


extern Disk *disk;
IORequest *make_request(Process *process, Opcode opcode, int arg);
IOQueue *init_queue(IOQueue *queue);
void enqueue(IOQueue *queue,IORequest *request);
IORequest *dequeue(IOQueue *queue);
void exec_request(IOQueue *queue);
Disk * init_disk();
int sstf_policy(Disk *disk);
void move_to_disk_buffer(IOQueue *queue);
IORequest *pick_request(int idx_request_to_be_executed);
#endif