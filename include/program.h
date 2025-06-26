#ifndef PROGRAM_H
#define PROGRAM_H

#include "instruction.h"

#define BUFFER_READER_SIZE 32
#define MAX_INSTRUCTIONS 20 // temporary solution 

typedef struct _header {
  char *name;
  int segment_id;
  int original_priority;
  int segment_size;
  char *semaphores;
} Header;

typedef struct _program {
  Header header;
  Instruction *instructions;
  int instructions_count;
} Program;

Program* read_program(const char *filename);

void free_program(Program *program);

#endif