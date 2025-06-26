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
    queue->num_elements=0;
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
    UNLOCK_IO();
    pthread_cond_signal(&queue->iocond);
    return;
}

IORequest* dequeue_sstf(IOQueue *queue) {
    if (!queue) return NULL;

    LOCK_IO();

    while (!queue->head) {
        pthread_cond_wait(&queue->iocond, &queue->iomutex);
    }

    if (queue->num_elements == 1) {
        IORequest *request = queue->head;
        queue->head = NULL;
        queue->tail = NULL;
        queue->num_elements--;
        UNLOCK_IO();
        return request;
    }

    IORequest *current = queue->head;
    IORequest *prev = NULL;
    
    IORequest *sstf_request = queue->head; 
    IORequest *sstf_prev = NULL;           

    int min_dist = abs(disk->current_trail - sstf_request->arg);
    
    prev = current;
    current = current->next;
    while (current) {
        int dist = abs(disk->current_trail - current->arg);
        if (dist < min_dist) {
            min_dist = dist;
            sstf_request = current;
            sstf_prev = prev;
        }
        prev = current;
        current = current->next;
    }

    if (sstf_request == queue->head) { 
        queue->head = sstf_request->next;
    } else { 
        sstf_prev->next = sstf_request->next;
    }

    if (sstf_request == queue->tail) { // Se o removido era a causa
        queue->tail = sstf_prev;
    }
    
    sstf_request->next = NULL; // Isola o request removido
    queue->num_elements--;
    UNLOCK_IO();
    return sstf_request;
}


IORequest* dequeue(IOQueue *queue) {
    if (!queue) return NULL;

    // Trava o mutex para operação segura
    LOCK_IO();

    // Espera enquanto a fila estiver vazia (evita busy-waiting)
    while (!queue->head) {
        pthread_cond_wait(&queue->iocond, &queue->iomutex);
    }

    // Se foi acordado por shutdown, retorna NULL

    // Remove o elemento da fila
    IORequest *request = queue->head;
    queue->head = request->next;

    // Se a fila esvaziou, atualiza tail
    if (!queue->head) {
        queue->tail = NULL;
    }
    // Destrava o mutex
    queue->num_elements--;
    pthread_cond_signal(&queue->iocond);
    UNLOCK_IO();

    return request;
}

void exec_request(IOQueue *queue){
    IORequest *request = dequeue_sstf(queue);
    if(!request || request->process->pid == EMPTY_BCP_ENTRY) return;
    LOCK_IO();
    FILE *buffer =  NULL;
    disk->current_request = request;
    disk->current_trail = request->arg;
    switch(request->opcode){
        case WRITE:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,  request->arg,SEEK_SET);
            update_dados(janela_I_O, "Writing on trail %d.", request->arg);
            sleep(IO_Quantum);
            fclose(buffer);
            //print_win(janela_OUTPUT,"Arquivo fechado com sucesso!");
        break;
        case READ:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,request->arg,SEEK_SET);
            update_dados(janela_I_O, "Current disk on trail %d.", disk->current_trail);
            char data;
            fread(&data, 1, 1, buffer);
            update_dados(janela_I_O, "Reading on trail %d.", request->arg, data);            
            sleep(IO_Quantum);
            pthread_cond_signal(&queue->iocond);
            fclose(buffer);
            //update_dados(janela_OUTPUT,"file closed");
        break;
        default:
        
        break;
    }
    request->process->counter_rw++;
    UNLOCK_IO();
    return;
}

Disk * init_disk(){
    disk = (Disk*) malloc(sizeof(Disk));
    disk->current_trail = 0;
    disk->buffer_occupation = 0;
    disk->current_request = NULL;
    return disk;
}
