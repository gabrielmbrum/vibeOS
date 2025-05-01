#include "../include/commons.h"
#include "../include/process.h"
#include "../include/debug.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"

void luigi_test() {
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

  schedule();

  print_BCP(&kernel->BCP);

  print_BCP(&kernel->BCP);

  printf("FINISHED \n");
}

void brum_test() {
  Program *program = read_program("../programs/prog1");
  if (program == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }
  print_program(program);

  free_program(program);
}

int main () {

  brum_test();

  return 0;
}