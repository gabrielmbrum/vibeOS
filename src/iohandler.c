#include "../include/process.h" 
#include "../include/iohandler.h"
#include "../include/commons.h"

#define RWTimeSlice 1


IOQueue *init_queue(IOQueue *queue){
    queue = malloc(sizeof(IOQueue));
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->iomutex,NULL);
    pthread_cond_init(&queue->iocond, NULL);

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
    pthread_cond_signal(&queue->iocond);
    UNLOCK_IO();
    return;
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
    pthread_cond_signal(&queue->iocond);
    UNLOCK_IO();

    return request;
}

void exec_request(IOQueue *queue){
    IORequest *request = dequeue(queue);
    if(!request || request->process->pid == EMPTY_BCP_ENTRY) return;
    LOCK_IO();
    FILE *buffer =  NULL;
    switch(request->opcode){
        case WRITE:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,  request->arg,SEEK_SET);
            //fprintf(buffer, "%c", 'L');
            //printf("%s %d\n", "Escrita da trilha", request->arg);
            print_win_args(janela_I_O,"Escrita da trilha %d", request->arg);
            sleep(RWTimeSlice);
            fclose(buffer);
            //puts("Arquivo fechado com sucesso!");
            print_win(janela_OUTPUT,"Arquivo fechado com sucesso!");
        break;
        case READ:
            buffer = fopen("../src/buffer.txt", "r+");
            fseek(buffer,request->arg,SEEK_SET);
            char data;
            fread(&data, 1, 1, buffer);
            //printf("Leitura da trilha %d: %c\n", request->arg, data);
            print_win_args(janela_I_O,"Leitura da trilha %d: %c", request->arg, data);            
            sleep(RWTimeSlice);
            pthread_cond_signal(&queue->iocond);
            fclose(buffer);
            puts("Arquivo fechado com sucesso!");
        break;
        case PRINT:
        int print_timing = 0;
            while(print_timing < request->arg){
                if(print_timing % 1000 == 0) print_win_args(janela_I_O,"Program PID Print Operation %d\n", request->process->pid);            

                //printf("Program PID Print Operation %d\n", request->process->pid);
                //Simulate screen print from program.
                //Adjust later.
                print_timing++;
                usleep(request->arg);
            }
        break;
    }
    request->process->counter_rw++;
    free(request);
    UNLOCK_IO();
    return;
}