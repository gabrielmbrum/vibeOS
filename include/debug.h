#ifndef DEBUG_H
#define DEBUG_H

#include "process.h"
#include "semaphore.h"

void print_bcp(Process **BCP);
void print_SCHEDULER(Process **BCP); 
const char *state_to_string(ProcessState state);
const char *opcode_to_string(Opcode opcode);

#endif

