#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"
#include "../include/interface.h"

void luigi_test(){
  Process *p1, *p2, *p3, *p4;

  p1 = processCreate(1, "sexta-feira", 0);
  p2=processCreate(2, "play-tv", 0);
  p3=processCreate(3, "nóstacomo", 0);
  p4= processCreate(4, "xambão", 0);

  p1->counter_rw = 10;
  p2->counter_rw = 5;
  p3->counter_rw = 11;
  p4->counter_rw = 11;

  p1->runtime_execution = 10;
  p2->runtime_execution = 5;
  p3->runtime_execution = 11;
  p4->runtime_execution = 11;

  init_Kernel();
  add_process_to_BCP(p1);
  add_process_to_BCP(p2);
  add_process_to_BCP(p3);
  add_process_to_BCP(p4);

  print_BCP(&kernel->BCP, kernel->process_amount);

  schedule();
  puts("");
  //print_BCP(&kernel->BCP);

  //print_BCP(&kernel->BCP);

  printf("FINISHED \n");
}

void brum_test(){
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

  Program *prog3 = read_program("../programs/synt3");
  if (prog3 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  Program *prog4 = read_program("../programs/synt4");
  if (prog4 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  Program *prog5 = read_program("../programs/synt5");
  if (prog5 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  print_program(prog1);
  print_program(prog2);
  print_program(prog3);
  print_program(prog4);
  print_program(prog5);

  free_program(prog1);
  free_program(prog2);
  free_program(prog3);
  free_program(prog4);
  free_program(prog5);
}

int main (){
  //create Processes array
  Process** processlist= (Process**)malloc(MAX_PROCESSES* sizeof(Process*));
  int total =0; //todo change this later

  // create interface structures to handle the user input
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;
  char **displayP,**displayS,**displayI,**displayU,**displayM;
  displayP = init_string_array(displayP,DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  displayS = init_string_array(displayS, DEF_WIN_MAX_PRINTS_BIGGER,MAX_OUTPUT_STR);
  displayI = init_string_array(displayI, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  displayU = init_string_array(displayU, DEF_WIN_MAX_PRINTS_SMALL,MAX_OUTPUT_STR);
  displayM = init_string_array(displayM, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  int *sizes = init_int_array(sizes,NUMBER_OF_WINDOWS); 
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
  
  // Draw main window, sub-windows and components
  janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1," MENU ");
  janela_menu = init_menu_components(janela_menu);
  janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
  janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
  janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
  janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");

  // change window to get user input
  move(6,34);
  curs_set(1);
  //input loop
  while(strcmp(input,"q")){
    if(get_input(input,janela_OUTPUT,displayU,&sizes[0])!= NULL){
      Process *local_process = processCreate(1,input,10);
      processlist[total] = local_process; //todo change later
      total++; //todo change later   
      
      char processoutput[MAX_OUTPUT_STR] = "";
      StringifyProcess(local_process, processoutput, MAX_OUTPUT_STR);

      add_element_list(displayP,processoutput,&sizes[3],DEF_WIN_MAX_PRINTS_BIG,DEF_WIN_WDH);
      print_multiple_messages(janela_process, displayP,&sizes[3]);
      char t[MAX_OUTPUT_STR] = " ";
      sprintf(t, "%i", sizes[3]);
      print_message(janela_memory,t);
    };
    // change window to get user input
    clear_space(6,34,strlen(input));
    move(6,34);
    
  }
  //TODO freeMemory();
  close_window(); // closes window

}