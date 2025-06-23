#include "../include/commons.h"
#include "../include/debug.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/iohandler.h"
#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/program.h"
#include "../include/semaphore.h"

int main(){
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  char *input = malloc((MAX_OUTPUT_STR) * sizeof(char));
  strcpy(input, "\0");

  init_interface();

  // input loop
  while (strcmp(input, "q")) {
    curs_set(1);
    echo();
    clear_space(6, 34, strlen(input));
    move(6, 34);

    if (get_input(input) != NULL) {
      char aux[MAX_OUTPUT_STR] = "../programs/";
      strcat(aux, input);
      Program *prog1 = read_program(aux);
      if (prog1 == NULL) {
        update_dados(janela_OUTPUT, "Failed to read program");
        break;
      } else {
        noecho();
        curs_set(0);
        move(6, 34);
        Process *p1 = create_process_from_program(prog1);
        add_process_to_BCP(p1);
          refresh_scheduler_window();
      }
    }
  }

  //close
  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
  endwin();
}