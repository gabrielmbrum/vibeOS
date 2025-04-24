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
  int value; // it can be the disk track or the semaphore char id
  char semaphore_name;
} Instruction;

Instruction* instruction_builder(const char *name, const char *runtime_str);

#endif