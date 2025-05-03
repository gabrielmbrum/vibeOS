#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/semaphore.h"

void luigi_test() {
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

void brum_test() {
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

  free_program(prog1);
  // free_program(prog2);
  // free_program(prog3);
  // free_program(prog4);
  // free_program(prog5);
}

void midori_test(){
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  print_program(prog1);

  free_program(prog1);
}
int main () {
/*   WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;
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
  endwin();			 */ 
  
  brum_test();
  return 0;
}