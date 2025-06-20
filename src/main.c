#include "../include/commons.h"
#include "../include/debug.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/iohandler.h"
#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/program.h"
#include "../include/semaphore.h"

void brum_test() {

  system("clear");
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program");
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

  refresh_page_table(&proc1->page_table, prog1->instructions,
                     prog1->instructions_count,
                     proc1->page_table->last_instruction_loaded);

  print_process(proc1);

  memory_status();

  free_program(prog1);
  // free_program(prog2);
  // free_program(prog3);
  // free_program(prog4);
  // free_program(prog5);
}

int main() {
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  char *input = malloc((MAX_OUTPUT_STR) * sizeof(char));
  strcpy(input, "\0");

  janela_intro();
  init_interface();

  // input loop
  while (strcmp(input, "q")) {
    clear_space(6, 34, strlen(input));
    move(6, 34);
    if (get_input(input, janela_OUTPUT) != NULL) {
      char aux[MAX_OUTPUT_STR] = "../programs/";
      strcat(aux, input);
      Program *prog1 = read_program(aux);
      if (prog1 == NULL) {
        print_win(janela_OUTPUT, "Failed to read program");
        break;
      } else {
        Process *p1 = create_process_from_program(prog1);
        add_process_to_BCP(p1);
      }    
    }

  }
  clear_space(6, 34, strlen(input));
  move(6, 34);

  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
  endwin();
}