#include "../include/process.h"
#include "../include/debug.h"
#include "../include/commons.h"
#include "../include/kernel.h"

int main () {
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