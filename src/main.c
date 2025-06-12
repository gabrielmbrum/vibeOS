#include "../include/commons.h"
#include "../include/debug.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/iohandler.h"
#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/program.h"
#include "../include/semaphore.h"



int main (){
  srand(time(NULL));
  init_Kernel();
  Program *p1 = read_program("../programs/synt1");
  Process *process1 = create_process_from_program(p1);
  add_process_to_BCP(process1);
}