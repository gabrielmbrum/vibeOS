#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/semaphore.h"
#include "../include/iohandler.h"


void luigi_test() {
  Process *p1, *p2, *p3, *p4;
  p1 = processCreate(0, "sexta-feira", 0);
  //p2=processCreate(2, "play-tv", 0);
  //p3=processCreate(3, "nóstacomo", 0);
  //p4= processCreate(4, "xambão", 0);

  p1->counter_rw = 0;
  /*p2->counter_rw = 0;
  p3->counter_rw = 0;
  p4->counter_rw = 0;

  p1->runtime_execution = 10;
  p2->runtime_execution = 5;
  p3->runtime_execution = 11;
  p4->runtime_execution = 11;*/

  init_Kernel();
  add_process_to_BCP(p1);
  //add_process_to_BCP(p2);
  //add_process_to_BCP(p3);
  //add_process_to_BCP(p4);


  schedule();
  puts("");
  //print_BCP(&kernel->BCP);

  //print_BCP(&kernel->BCP);

  printf("FINISHED \n");
}

void luigi_testv2() {
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  // Carrega programas
  Program *prog1 = read_program("../programs/synt1");
  Program *prog2 = read_program("../programs/synt2");
  Program *prog3 = read_program("../programs/synt3");

  // Verifica erros na leitura
  if (!prog1 || !prog2 || !prog3) {
      fprintf(stderr, "Erro na leitura de programas\n");
      free_program(prog1);
      free_program(prog2);
      free_program(prog3);
      return;
  }

  // Cria processos
  Process *p1 = create_process_from_program(prog1);
  Process *p2 = create_process_from_program(prog2);
  Process *p3 = create_process_from_program(prog3);

  // Adiciona processos ao BCP (inicia automaticamente o escalonador)
  add_process_to_BCP(p1);
  //add_process_to_BCP(p3);
  add_process_to_BCP(p2);
  // Exibe estado inicial
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");

  // Loop principal: aguarda término dos processos
  while (kernel->process_amount > 0 && !kernel->shutdown_request) {
      sleep(1); // Reduz uso da CPU  
  }
  printf("Processos no escalonador %d\n",kernel->process_amount);
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");
  // Cleanup
  add_process_to_BCP(p3);

  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
}

void luigi_testv3(){
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }
  Process *p1 = create_process_from_program(prog1);
  //IORequest *r1 = make_request(p1, WRITE, 20);
  IORequest *r1 = make_request(p1, READ, 20);
  IOQueue *queue=init_queue(queue);
  enqueue(queue, r1);
  printf("PID: %d\n",queue->tail->process->pid);
  printf("ARG: %d\n",queue->tail->arg);
  printf("OPCODE: %d\n",queue->tail->opcode);
  exec_request(queue);
}

void luigi_testv4(){
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }
  Process *p1 = create_process_from_program(prog1);
  init_Kernel();
  add_process_to_BCP(p1);


}


void brum_test() {

  system("clear");
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  // Program *prog2 = read_program("../programs/synt2");
  // if (prog2 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog3 = read_program("../programs/synt3");
  // if (prog3 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog4 = read_program("../programs/synt4");
  // if (prog4 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog5 = read_program("../programs/synt5");
  // if (prog5 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // print_program(prog1);
  // print_program(prog2);
  // print_program(prog3);
  // print_program(prog4);
  // print_program(prog5);

  Process *proc1 = create_process_from_program(prog1);

  print_process(proc1);

  memory_status();

  refresh_page_table(&proc1->page_table, prog1->instructions, prog1->instructions_count, proc1->page_table->last_instruction_loaded);

  print_process(proc1);

  memory_status();

  free_program(prog1);
  // free_program(prog2);
  // free_program(prog3);
  // free_program(prog4);
  // free_program(prog5);
}

void midori_test(){
  init_semaphores();
  init_Kernel();

  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  Program *prog2 = read_program("../programs/synt2");
  if (prog2 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  // Program *prog3 = read_program("../programs/synt3");
  // if (prog1 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog4 = read_program("../programs/synt4");
  // if (prog2 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  Process *p1 = create_process_from_program(prog1);
  Process *p2 = create_process_from_program(prog2);
  // Process *p3 = create_process_from_program(prog3);
  // Process *p4 = create_process_from_program(prog4);

  add_process_to_BCP(p1);
  add_process_to_BCP(p2);
  // add_process_to_BCP(p3);
  // add_process_to_BCP(p4);



  schedule();
  puts("");

  // //print_BCP(&kernel->BCP);

  //printf("FINISHED \n");

  free_program(prog1);
  free_program(prog2);
/*   free_program(prog3);
  free_program(prog4);
  free_program(prog5); */
}

int maria (){
  //create Processes array
  Process** processlist= (Process**)malloc(MAX_PROCESSES* sizeof(Process*));
  int total =0; //todo change this later
/* 
  !DEPRICIATED
  // create interface structures to handle the user input
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;

   char **displayP,**displayS,**displayI,**displayU,**displayM;
  displayP = init_string_array(displayP,DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  displayS = init_string_array(displayS, DEF_WIN_MAX_PRINTS_BIGGER,MAX_OUTPUT_STR);
  displayI = init_string_array(displayI, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  displayU = init_string_array(displayU, DEF_WIN_MAX_PRINTS_SMALL,MAX_OUTPUT_STR);a
  displayM = init_string_array(displayM, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  int *sizes = init_int_array(sizes,NUMBER_OF_WINDOWS); 
 */
  char *input = malloc((MAX_OUTPUT_STR)*sizeof(char));
  strcpy(input,"\0");
        /*
        ?   INDEX
        * 0 -> OUTPUT
        * 1 -> SCHEDULER
        * 2 -> MEMORY
        * 3 -> PROCESS
        * 4 -> I/O
        */
  // start with introduction window
  janela_intro();
  init_interface();

  //input loop
  while(strcmp(input,"q")){
    if(get_input(input,janela_OUTPUT)!= NULL){
        //*Testing with process
        //* NOTE - commented the prints from the functions for testing with the interface
        char aux[MAX_OUTPUT_STR] = "../programs/";
        strcat(aux,input);
        Program *prog1 = read_program(aux);
        if (prog1 == NULL) {
        print_win(janela_OUTPUT,"Faled to read program");
        return;
      }else{
        Process *p1 = create_process_from_program(prog1); 
        print_win_args(janela_process,"PID: %d, Name: %s, Priority: %d, Segment_id: %d, PC: %d, STATE: %d",
           p1->pid, p1->name, p1->priority, p1->segment_id, p1->pc, p1->state);     
      } 
    }; 
    // change window to get user input
    clear_space(6,34,strlen(input));
    move(6,34);    
  }     
      /*    
      !DEPRECIATED
      processlist[total] = local_process; //todo change later
      total++; //todo change later    
      */
      /*
      !DEPRECIATED
       add_element_list(displayP,processoutput,&sizes[3],DEF_WIN_MAX_PRINTS_BIG,DEF_WIN_WDH);
       print_multiple_messages(janela_process, displayP,&sizes[3]);
       */

  //TODO freeMemory();
  close_window(); // closes window
}

  int main(){
   init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

   char *input = malloc((MAX_OUTPUT_STR)*sizeof(char));
   strcpy(input,"\0");

   janela_intro();
   init_interface();

   //input loop
  while(strcmp(input,"q")){
    clear_space(6,34,strlen(input));
    move(6,34);
    if(get_input(input,janela_OUTPUT)!= NULL){
      char aux[MAX_OUTPUT_STR] = "../programs/";
      strcat(aux,input);
      Program *prog1 = read_program(aux);
      if (prog1 == NULL) {
        print_win(janela_OUTPUT,"Failed to read program");
        return;
      }else{
        Process *p1 = create_process_from_program(prog1); 
        add_process_to_BCP(p1);
      /* print_win_args(janela_process,"PID: %d, Name: %s, Priority: %d, Segment_id: %d, PC: %d, STATE: %d",
          p1->pid, p1->name, p1->priority, p1->segment_id, p1->pc, p1->state);  */    
      } 
    }  
  }
  clear_space(6,34,strlen(input));
  move(6,34);  

  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
  endwin();

  return 0;
}