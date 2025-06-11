#include "../include/process.h" 
#include "../include/iohandler.h"
#include "../include/commons.h"


Disk *disk;

IOQueue *init_queue(IOQueue *queue){
    queue = malloc(sizeof(IOQueue));
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->iomutex,NULL);
    pthread_cond_init(&queue->iocond, NULL);
    queue->num_elements = 0;
    return queue;
}

IORequest *make_request(Process *process, Opcode opcode, int arg){
    IORequest *request = malloc(sizeof(IORequest));
    request->process = process;
    request->opcode = opcode;
    request->arg = arg;
    request->next = NULL;

    return request;
}



void enqueue(IOQueue *queue, IORequest *request){
    
    if(!queue || !request) return;
    LOCK_IO();
    if(!queue->head){
        queue->head = request;
        queue->tail = request;
    }
    else{
        queue->tail->next = request;
        queue->tail = request;
    }
    queue->num_elements++;
    pthread_cond_signal(&queue->iocond);
    UNLOCK_IO();
    return;
}
IORequest* dequeue(IOQueue *queue) {
    if (!queue) return NULL;

    pthread_mutex_lock(&queue->iomutex);  // FALTAVA TRAVAR AQUI!

    while (!queue->head) {
        pthread_cond_wait(&queue->iocond, &queue->iomutex);
    }


    IORequest *request = queue->head;
    if (!request || queue->num_elements == 0) {
        pthread_mutex_unlock(&queue->iomutex);
        pthread_mutex_unlock(&queue->iomutex);  // FALTAVA DESTRAVAR!
        return NULL;
    }

    queue->head = request->next;
    if (!queue->head) queue->tail = NULL;
    queue->num_elements--;

    pthread_mutex_unlock(&queue->iomutex);  // FALTAVA DESTRAVAR!
    return request;  // RETORNA O PONTEIRO ORIGINAL
}

Disk * init_disk(){
    disk = (Disk*) malloc(sizeof(Disk));
    disk->current_trail = 0;
    disk->buffer_occupation = 0;
    pthread_cond_init(&disk->disk_cond, NULL);
    pthread_mutex_init(&disk->disk_lock, NULL);
    return disk;
}

int sstf_policy(Disk *disk){
    print_win_args(janela_I_O,"Aplicando politica");

    int current_trail = disk->current_trail, closest_trail_idx = -1, min_distance = INT_MAX;
    if(disk->buffer_occupation == 0 ) return FAILURE;
    for(int i = 0; i < disk->buffer_occupation; i++){
        int distance = abs(current_trail - disk->requests[i].arg);
        if(distance < min_distance){
            min_distance = distance;
            closest_trail_idx = i;
        }
    }
    return closest_trail_idx;
}

void move_to_disk_buffer(IOQueue *queue){
    LOCK_DISK();
    if(!queue || queue->num_elements <= 0 ) {
        UNLOCK_DISK();
        return;
    }
    IORequest *request_to_be_moved = dequeue(queue);
    if (!request_to_be_moved) {
        UNLOCK_DISK();
        return;
    }
    print_win_args(janela_I_O,"passei e morri aqui: PEDIDO %d", request_to_be_moved->process->pid);
    disk->requests[disk->buffer_occupation] = *request_to_be_moved;
    disk->buffer_occupation++;
    pthread_cond_signal(&disk->disk_cond);
    pthread_cond_signal(&queue->iocond);
    UNLOCK_DISK();
}

IORequest *pick_request(int idx_request_to_be_executed){
    return &disk->requests[idx_request_to_be_executed];    
}