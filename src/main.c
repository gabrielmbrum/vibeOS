#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/interface.h"

int main () {
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;
  int num = 0; // number of current inputs in the process display array

  char *input = malloc((MAX_INPUT_STR)*sizeof(char));
  char **displayprocessos = (char**)malloc(MAX_WD_HGH_PROCESS * sizeof(char*));
  for (int i = 0; i < MAX_WD_HGH_PROCESS; i++) {
      displayprocessos[i] = (char*)malloc((MAX_INPUT_STR) * sizeof(char));
  }

    // initialize introduction window
    initscr();	
    curs_set(0);		
    janela_intro();

  janela_menu = create_newwin(9, 59, 0, 1,"MENU");
  janela_menu = menu(janela_menu);
  janela_OUTPUT = create_newwin(5, 59, 9, 1," OUTPUT ");
  janela_SCHEDULER = create_newwin(16, 59, 14, 1," SCHEDULER ");
  janela_memory = create_newwin(10, 59, 0, 61," MEMORY ");
  janela_process = create_newwin(10, 59, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(10, 59, 20, 61," I/O ");

  do{
    move(6,34);
    getstr(input);
    add_input_list(displayprocessos,input,&num);
    print_input(janela_process, displayprocessos,&num);
    clear_input_space(20,6,34);
    move(6,34);
    refresh();
  }while(strcmp(input,"q"));

  // free memory
  free(input);

  // close lncurses window
  endwin();			
  
  Process *p1, *p2, *p3, *p4;
  p1 = create_process(1, "sexta-feira", 0);
  p2=create_process(2, "play-tv", 0);
  p3=create_process(3, "nóstacomo", 0);
  p4= create_process(4, "xambão", 0);

  p1->counter_rw = 10;
  p2->counter_rw = 5;
  p3->counter_rw = 11;
  p4->counter_rw = 11;

  init_Kernel();
  add_process_to_BCP(p1);
  add_process_to_BCP(p2);
  add_process_to_BCP(p3);
  add_process_to_BCP(p4);

  print_BCP(&kernel->BCP);

  choose_process();

  print_BCP(&kernel->BCP);

  print_BCP(&kernel->BCP);

  printf("FINISHED \n");
  return 0;
}