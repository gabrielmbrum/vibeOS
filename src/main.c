#include "../include/commons.h"
#include "../include/debug.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/iohandler.h"
#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/program.h"
#include "../include/semaphore.h"

int main() {
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  char *input = malloc((MAX_OUTPUT_STR) * sizeof(char));
  strcpy(input, "\0");

  init_interface();

  int lin, col;
  getmaxyx(janela_menu, lin, col);
  int men = col/3.4;

  // input loop
  while (strcmp(input, "q")) {
    check_responsivity(men,lin,input);
    if (get_input(input) != NULL) {
      char aux[MAX_OUTPUT_STR] = PROGRAMS;
      strcat(aux, input);
      Program *prog1 = read_program(aux);
      if (prog1 == NULL) {
        update_dados(janela_OUTPUT, 3, "Failed to read program");
        break;
      } else {
        Process *p1 = create_process_from_program(prog1);
        add_process_to_BCP(p1);
      }
    }
  }
  check_responsivity(men,lin,input);
  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
  free(input);
  shutdown_interface();
  return 0;
}