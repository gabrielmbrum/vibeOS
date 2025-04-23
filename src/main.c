#include "../include/process.h"
#include "../include/debug.h"

int main () {
  Process *BCP;

  init_BCP(&BCP);

  Process *p1 = create_process(&BCP,1, "Process 1", 5);
  Process *p2 = create_process(&BCP,2, "Process 2", 3);
  Process *p3 = create_process(&BCP,3, "Process 3", 1);
  Process *p4 = create_process(&BCP,4, "Process 4", 2);
  Process *p5 = create_process(&BCP,5, "Process 5", 4);
  Process *p6 = create_process(&BCP,6, "Process 6", 0);

  add_process_to_BCP(p1, &BCP);
  add_process_to_BCP(p2, &BCP);
  add_process_to_BCP(p3, &BCP);
  add_process_to_BCP(p4, &BCP);
  add_process_to_BCP(p5, &BCP);
  add_process_to_BCP(p6, &BCP); // This should fail since BCP is full
  print_BCP(&BCP);
  rmv_process_of_BCP(1, &BCP);
  add_process_to_BCP(p6, &BCP); // This should succeed now
  print_BCP(&BCP);

}