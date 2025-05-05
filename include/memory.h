#ifndef MEMORY_H
#define MEMORY_H

#include "instruction.h"
#include <stdbool.h>

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
  int last_instruction_loaded;  // keeps track of the last instruction loaded
  int last_instruction_counter; // keeps track of how many pages were made for the last instruction
} PageTable;

/*

  * Initializes a page with the given page number and instruction count.
  * Sets the reference bit and used bit to 0, and allocates memory for the instructions array.

*/
void initialize_page(Page *page, int page_num, int instructions_count);

/*

  * Creates a page table from an array of instructions and the lenght of it, alocating all the pages needed with the limit of RESIDENT_SET.
  * It checks if there are missing instructions and it sets the last instruction loaded.

*/
PageTable *build_page_table(Instruction *instructions, int instructions_count);

/*

  * Refreshes the page table, reusing the pages already alocated and putting the instructions not loaded to the pages.
  * It updates the last instruction loaded and checks for missing instructions.

  INITIAL ASSUMPTIONS
    - this function will only be called when the page table is full
    - the last instruction loaded is considered as read by complete
    - if for example, only more 4 pages will be needed, the other 12 pages MUST be freed

*/
void refresh_page_table(PageTable **page_table, Instruction *instructions, int instructions_count);

/*

  * Frees all the pages in the page table and the page table itself.
  * It also frees the instructions array in each page.

*/
void free_page_table(PageTable **page_table);

/*

  * Show the memory status, showin the total of pages allocated comparing to all the space available.

*/
void memory_status();

#endif 