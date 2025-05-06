#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum _opcode {
  EXEC = 1,
  READ = 2,
  WRITE = 3,
  WAIT = 4,
  P = 5,
  V = 6,
  PRINT = 7
} Opcode;

typedef struct _instruction {
  Opcode opcode; 
  int value; // it can be the disk track or the semaphore char id or time spend
  char semaphore_name;
} Instruction;

Instruction* instruction_builder(const char *name, const char *runtime_str);

// void print_instruction(const Instruction instruction);

// void print_instructions(const Instruction *instructions, int total_instructions);

#endif