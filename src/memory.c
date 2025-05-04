#include "../include/commons.h"
#include "../include/memory.h" // Ensure this header defines 'Page'

void initialize_page(Page *page, int page_num, int instructions_count) {
  page->page_number = page_num;
  page->reference_bit = 0;
  page->used_bit = 0;
  page->instruction_count = 1;
  page->instructions = malloc(sizeof(Instruction) * instructions_count); // Initialize to NULL
}
        // // the position 'num of pages' will has this values
        // page_table->pages[page_table->page_count].page_number = page_table->page_count;
        // page_table->pages[page_table->page_count].reference_bit = 0;
        // page_table->pages[page_table->page_count].used_bit = 0;

        // // allocates memory for the instructions array, preparing it to the case of having all instructions in this page, this avoids segmentation fault
        // page_table->pages[page_table->page_count].instructions = malloc(sizeof(Instruction) * instructions_count);
        // if (page_table->pages[page_table->page_count].instructions == NULL) {
        //   fprintf(stderr, "Memory allocation for instructions failed!!!\n");
        //   exit(1);
        // }
        // page_table->pages[page_table->page_count].instruction_count = 1;

PageTable *build_page_table(Instruction *instructions, int instructions_count) {
  PageTable *page_table = malloc(sizeof(PageTable));
  
  page_table->pages = malloc(sizeof(Page) * RESIDENT_SET); // allocate 16 pages (resident set amount)
  if (page_table->pages == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  page_table->page_count = 0;
  int num_of_instructions_per_page, iteration_val, i;

  for (i = 0; i < instructions_count && page_table->page_count < RESIDENT_SET; i++) {
    // if the instruction is EXEC, it will create a new page
    if (instructions[i].opcode == EXEC) {
      for (int j = 0; (j < instructions[i].value / 1000  || j == 0) && page_table->page_count < RESIDENT_SET; j++) {        
        // every new page created will have zero instructions initially
        num_of_instructions_per_page = 0;

        initialize_page(&page_table->pages[page_table->page_count], page_table->page_count, instructions_count);

        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page] = instructions[i];

        // the value of the instruction is the original value minus 1000 * j, this way we can have the correct value for each instruction
        iteration_val = ((instructions[i].value - (1000 * j)) % 1000);
        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page++].value = iteration_val == 0 ? 1000 : iteration_val;
        page_table->page_count++;
      }
    } else {
      if (page_table->page_count == 0) {
        initialize_page(&page_table->pages[page_table->page_count ], page_table->page_count, instructions_count);

        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page++] = instructions[i];

        page_table->page_count++;
      } else {
        // there were already pages created, so we need to add the instruction to the last page created
        page_table->pages[page_table->page_count - 1].instruction_count++;
        page_table->pages[page_table->page_count - 1].instructions[num_of_instructions_per_page++] = instructions[i];
      }
    }
  }

  page_table->missing_instructions = (i < instructions_count && page_table->page_count == RESIDENT_SET) ? true : false;
  page_table->last_instruction_loaded = i-1;
  
  return page_table;
}