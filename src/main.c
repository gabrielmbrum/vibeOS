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
        /* print_win_args(janela_process,"PID: %d, Name: %s, Priority: %d,
           Segment_id: %d, PC: %d, STATE: %d", p1->pid, p1->name, p1->priority,
           p1->segment_id, p1->pc, p1->state);  */
      }
    }
  }
  clear_space(6, 34, strlen(input));
  move(6, 34);

  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
  endwin();

  return 0;
}