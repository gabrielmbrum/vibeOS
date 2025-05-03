#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/interface.h"

int main () {
  WINDOW *janela_menu, *janela_I_O, *janela_memory, *janela_process, *janela_PID;
  int num = 0; // number of current inputs in the process display array

  //malloc array of processos and input to be displayed in the screen 
  //! This does not represent the total processes in the Simulator, only those that are displayed
  char *input = malloc((TAM_MAX_STR)*sizeof(char));
  char **displayprocessos = (char**)malloc(MAX_WD_HGH_PROCESS * sizeof(char*));
  for (int i = 0; i < MAX_WD_HGH_PROCESS; i++) {
      displayprocessos[i] = (char*)malloc((TAM_MAX_STR) * sizeof(char));
  }

  // initialize introduction window
  initscr();	
  curs_set(0);		
  janela_intro();

  // initialize main window and components
  janela_menu = create_newwin(9, 59, 0, 1);
  janela_menu = menu(janela_menu);
  wrefresh(janela_menu);

  janela_process = create_newwin(10, 59, 9, 1);
  mvwprintw(janela_process, 0, 2, "%s", " PROCESS ");
  wrefresh(janela_process);

  janela_memory = create_newwin(11, 59, 19, 1);
  mvwprintw(janela_memory, 0, 2, "%s", " MEMORY ");
  wrefresh(janela_memory);

  janela_PID = create_newwin(15, 59, 0, 61);
  mvwprintw(janela_PID, 0, 2, "%s", " PID ");
  wrefresh(janela_PID);

  janela_I_O = create_newwin(15, 59, 15, 61);
  mvwprintw(janela_I_O, 0, 2, "%s", " I/O ");
  wrefresh(janela_I_O);

  // change window to get user input
  move(6,34);
  curs_set(1);

  // gets the user input, clears the input box and prints the input in another sub-window
  do{
    getstr(input);
    add_input_list(displayprocessos,input,&num);
    print_input(janela_process, displayprocessos,&num);
    clear_input_space(20,6,34);
    move(6,34);
    refresh();
  }while(strcmp(input,"q"));
  
  //TODO: get the lines to be displayed in other windows
  //? Define other constants for each window bound?
  //? Create other array od strings for each window? 

  //TODO: A Platyplus?
  //? ..
  //? ..

  //!__======__
  //TODO: PERRY
  //? THE
  //? PLATYPUS

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