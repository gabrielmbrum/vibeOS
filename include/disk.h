#ifndef DISK_H
#define DISK_H

#define buffer_size 10
#define IO_QUANTUM 4000
#include "iohandler.h"
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>
typedef struct Disk{
    IORequest requests[buffer_size];
    int current_trail;
    int buffer_occupation;
    pthread_mutex_t disk_lock;
    pthread_cond_t disk_cond;
}Disk;

Disk * init_disk(Disk *disk){
    disk = (Disk*) malloc(sizeof(Disk));
    disk->current_trail = 0;
    disk->buffer_occupation = 0;
    pthread_cond_init(&disk->disk_cond, NULL);
    pthread_mutex_init(&disk->disk_lock, NULL);
    return disk;
}

int sstf_policy(Disk *disk){
    int current_trail = disk->current_trail, closest_trail_idx = -1, min_distance = INT_MAX;
    if(disk->buffer_occupation == 0 ) return FAILURE;
    for(int i = 0; i < disk->buffer_occupation; i++){
        int distance = abs(disk->current_trail - disk->requests[i].arg);
        if(distance < min_distance){
            min_distance = distance;
            closest_trail_idx = i;
        }
    }
    return closest_trail_idx;
}

#endif