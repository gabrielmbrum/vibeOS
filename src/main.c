#include "../include/commons.h"
#include "../include/process.h"
#include "../include/debug.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"

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

int main () {

  luigi_test();

  return 0;
}