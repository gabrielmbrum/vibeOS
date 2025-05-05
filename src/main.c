#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/semaphore.h"
#include "../include/iohandler.h"

// void luigi_test() {
//   Process *p1, *p2, *p3, *p4;
//   p1 = processCreate(0, "sexta-feira", 0);
//   //p2=processCreate(2, "play-tv", 0);
//   //p3=processCreate(3, "nóstacomo", 0);
//   //p4= processCreate(4, "xambão", 0);

//   p1->counter_rw = 0;
//   /*p2->counter_rw = 0;
//   p3->counter_rw = 0;
//   p4->counter_rw = 0;

//   p1->runtime_execution = 10;
//   p2->runtime_execution = 5;
//   p3->runtime_execution = 11;
//   p4->runtime_execution = 11;*/

//   init_Kernel();
//   add_process_to_BCP(p1);
//   //add_process_to_BCP(p2);
//   //add_process_to_BCP(p3);
//   //add_process_to_BCP(p4);

//   print_BCP(&kernel->BCP, kernel->process_amount);

//   schedule();
//   puts("");
//   //print_BCP(&kernel->BCP);

//   //print_BCP(&kernel->BCP);

//   printf("FINISHED \n");
// }

void luigi_testv2() {
  init_semaphores();
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  // Carrega programas
  Program *prog1 = read_program("../programs/synt1");
  Program *prog2 = read_program("../programs/synt2");
  //Program *prog3 = read_program("../programs/synt3");

  // Verifica erros na leitura
  if (!prog1 || !prog2 ) {
      fprintf(stderr, "Erro na leitura de programas\n");
      free_program(prog1);
      free_program(prog2);
      //free_program(prog3);
      return;
  }

  // Cria processos
  Process *p1 = create_process_from_program(prog1);
  Process *p2 = create_process_from_program(prog2);
  //Process *p3 = create_process_from_program(prog3);

  // Adiciona processos ao BCP (inicia automaticamente o escalonador)
  add_process_to_BCP(p1);
  //add_process_to_BCP(p3);
  add_process_to_BCP(p2);
  // Exibe estado inicial
  print_BCP(&kernel->BCP, kernel->process_amount);
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");

  // Loop principal: aguarda término dos processos
  while (kernel->process_amount > 0 && !kernel->shutdown_request) {
      sleep(1); // Reduz uso da CPU
      printf("\nProcessos restantes: %d", kernel->process_amount);
  }
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");
  // Cleanup
  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
}

// void luigi_testv3(){
//   Program *prog1 = read_program("../programs/synt1");
//   if (prog1 == NULL) {
//     fprintf(stderr, "Failed to read program\n");
//     return;
//   }
//   Process *p1 = create_process_from_program(prog1);
//   //IORequest *r1 = make_request(p1, WRITE, 20);
//   IORequest *r1 = make_request(p1, READ, 20);
//   IOQueue *queue=init_queue(queue);
//   enqueue(queue, r1);
//   printf("PID: %d\n",queue->tail->process->pid);
//   printf("ARG: %d\n",queue->tail->arg);
//   printf("OPCODE: %d\n",queue->tail->opcode);
//   exec_request(queue);
// }
// void brum_test() {
//   Program *prog1 = read_program("../programs/synt1");
//   if (prog1 == NULL) {
//     fprintf(stderr, "Failed to read program\n");
//     return;
//   }

//   // Program *prog2 = read_program("../programs/synt2");
//   // if (prog2 == NULL) {
//   //   fprintf(stderr, "Failed to read program\n");
//   //   return;
//   // }

//   // Program *prog3 = read_program("../programs/synt3");
//   // if (prog3 == NULL) {
//   //   fprintf(stderr, "Failed to read program\n");
//   //   return;
//   // }

//   // Program *prog4 = read_program("../programs/synt4");
//   // if (prog4 == NULL) {
//   //   fprintf(stderr, "Failed to read program\n");
//   //   return;
//   // }

//   // Program *prog5 = read_program("../programs/synt5");
//   // if (prog5 == NULL) {
//   //   fprintf(stderr, "Failed to read program\n");
//   //   return;
//   // }

//   // print_program(prog1);
//   // print_program(prog2);
//   // print_program(prog3);
//   // print_program(prog4);
//   // print_program(prog5);

//   Process *proc1 = create_process_from_program(prog1);

//   print_process(proc1);

//   free_program(prog1);
//   // free_program(prog2);
//   // free_program(prog3);
//   // free_program(prog4);
//   // free_program(prog5);
// }

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
  print_BCP(&kernel->BCP, kernel->process_amount);

  // //print_BCP(&kernel->BCP);

  //printf("FINISHED \n");

  free_program(prog1);
  free_program(prog2);
  // free_program(prog3);
  // free_program(prog4);


  // sem_P(p1, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);
  // //sem_V(p1, 't');
  // //printf("Process %d state: %d\n", p2->pid, p2->state);
  // sem_P(p2, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);

  // sem_P(p3, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);

  // sem_V(p2, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);

  // sem_P(p4, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);

  // sem_V(p1, 't');
  // print_all_semaphores();
  // print_BCP(&kernel->BCP, kernel->process_amount);
  //puts("");
  //print_BCP(&kernel->BCP);

  //print_BCP(&kernel->BCP);
  print_all_semaphores();
  printf("FINISHED \n");
}
int main () {
  // WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;

  // //! This does not represent the total processes in the Simulator. The arrays contain only the information to be displayed in the screen
  // char **displayProcessos = init_string_array(displayProcessos,DEF_WIN_MAX_PRINTS_BIG,MAX_INPUT_STR);
  // char **displayScheduler = init_string_array(displayScheduler, DEF_WIN_MAX_PRINTS_BIGGER,MAX_OUTPUT_STR);
  // char **displayIO = init_string_array(displayIO, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  // char **displayOUT = init_string_array(displayOUT, (DEF_WIN_MAX_PRINTS_SMALL,MAX_OUTPUT_STR),MAX_OUTPUT_STR);
  // char **displayMEMO = init_string_array(displayMEMO, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  // char *input = malloc((MAX_INPUT_STR)*sizeof(char));
  // strcpy(input,"\0");

  // // Malloc the array containing the sizes of the dinamic arrays
  // int *sizes = init_int_array(sizes,NUMBER_OF_WINDOWS);

  
  // ! INDEX
  // * 0 -> OUtPUT
  // * 1 -> SCHEDULER
  // * 2 -> MEMORY
  // * 3 -> PROCESS
  // * 4 -> I/O
  

  // // initialize introduction window
  // initscr();	
  // curs_set(0);		
  // janela_intro();

  // // initialize main window and components
  // janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1,"MENU");
  // janela_menu = menu(janela_menu);
  // janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
  // janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
  // janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
  // janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
  // janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");

  // // change window to get user input
  // move(6,34);
  // curs_set(1);

  // // input loop 
  // while(strcmp(input,"q")){
  //   getstr(input);
  //   if(strlen(input)>MAX_INPUT_STR || check_input(input) == 0){
  //     //? This implementation is not perfect for every case (example: strings bigger than the screen), but it mostly works
  //     print_message(janela_OUTPUT,"Entrada inválida");
  //   }else{
  //     // handle the correct user input 
  //     add_input_list(displayProcessos,input,&sizes[3],(DEF_WIN_HGH_BIG-2));
  //     mvwprintw(janela_I_O, POS_Y, POS_X, "%d",sizes[3]);
  //     wrefresh(janela_I_O);
  //     print_multiple_messages(janela_process, displayProcessos,&sizes[3]); 

  //   }
  //   clear_space(6,34,strlen(input));
  //   move(6,34);
  //   refresh();
  // }

  // // free memory
  // free(input);

  // // close lncurses window
  // endwin();		 

  luigi_testv2();
  return 0;
}