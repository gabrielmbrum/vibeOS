#ifndef DEBUG_H
#define DEBUG_H

#include "process.h"
#include "semaphore.h"

//void print_BCP(Process **BCP, int processes);
void print_bcp(Process **BCP);
const char *state_to_string(ProcessState state);
const char *opcode_to_string(Opcode opcode);
void print_page_table(PageTable *page);
void print_process(Process *proc);
void print_instruction(Instruction inst);
void print_instructions(Instruction *instructions, int total_instructions);
void print_all_semaphores();

#endif

