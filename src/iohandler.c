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
    if(!queue || !request) {
        return;
    }

    pthread_mutex_lock(&queue->iomutex);  // FALTAVA TRAVAR AQUI!
    if(!queue->head){
        queue->head = request;
        queue->tail = request;
    }
    else{
        queue->tail->next = request;
        queue->tail = request;
    }
    queue->num_elements++;
    request->process->waiting_operations++;
    printf("Processo esperando: %d\n", request->process->waiting_operations);
    pthread_mutex_unlock(&queue->iomutex);
    pthread_cond_signal(&queue->iocond);
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
        pthread_cond_signal(&queue->iocond);  // FALTAVA DESTRAVAR!
        return NULL;
    }

    queue->head = request->next;
    if (!queue->head) queue->tail = NULL;
    queue->num_elements--;
    pthread_mutex_unlock(&queue->iomutex);
    pthread_cond_signal(&queue->iocond);
    request->next = NULL;  // FALTAVA DESTRAVAR!
    return request;  // RETORNA O PONTEIRO ORIGINAL
}

Disk * init_disk(){
    puts("Disk initialized");
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
        int distance = abs(current_trail - disk->requests[i].arg);
        if(distance < min_distance){
            min_distance = distance;
            closest_trail_idx = i;
        }
    }
    printf("Request em buffer com menor trilha: %d\n", closest_trail_idx);
    return closest_trail_idx;
}

void move_to_disk_buffer(IOQueue *queue) {
    if(!queue) return;
    
    // Primeiro trava o mutex da fila
    pthread_mutex_lock(&queue->iomutex);
    if (queue->num_elements <= 0) {
        pthread_mutex_unlock(&queue->iomutex);
        return;
    }
    // Depois trava o mutex do disco
    pthread_mutex_lock(&disk->disk_lock);
    /*    if(disk->buffer_occupation>=buffer_size){
        UNLOCK_DISK();
        pthread_mutex_unlock(&queue->iomutex);
        return;
    }
    */
    // Remove da fila
    IORequest *request_to_be_moved = queue->head;
    queue->head = request_to_be_moved->next;
    if(!queue->head) queue->tail = NULL;
    queue->num_elements--;
    
    // Adiciona ao buffer do disco
    disk->requests[disk->buffer_occupation] = *request_to_be_moved;
    disk->buffer_occupation++;
    printf("Requests em disco: %d\n", disk->buffer_occupation);
    // Destrava na ordem inversa
    pthread_mutex_unlock(&disk->disk_lock);
    pthread_mutex_unlock(&queue->iomutex);
    
    // Sinaliza as condições
    pthread_cond_signal(&queue->iocond);
    pthread_cond_signal(&disk->disk_cond);
}

IORequest *pick_request(int idx_request_to_be_executed){
    puts("picking..");
    return &disk->requests[idx_request_to_be_executed];    
}