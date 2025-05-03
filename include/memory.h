#ifndef MEMORY_H
#define MEMORY_H

#include "instruction.h"

#define MEM_LENGTH 1024
#define OS_MEMORY_SIZE 64
#define PAGE_SIZE 1

#define RESIDENT_SET 16

typedef struct __page {
  int page_number;
  int reference_bit;
  int used_bit;
  Instruction *instructions;
  int instruction_count;
} Page;
 
typedef struct __page_table {
  Page *pages;
  int page_count;
  bool missing_instructions;
} PageTable;

PageTable *build_page_table(Instruction *instructions, int instructions_count);

#endif 