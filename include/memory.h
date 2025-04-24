#ifndef MEMORY_H
#define MEMORY_H

#include "instruction.h"

#define MEM_LENGTH 1024
#define OS_MEMORY_SIZE 64
#define TOTAL_PROCESS_MEMORY_SIZE 960
#define PAGE_SIZE 1

typedef struct __page {
  int page_number;
  int reference_bit;
  int used_bit;
  Instruction *instructions;
} Page;

#endif 