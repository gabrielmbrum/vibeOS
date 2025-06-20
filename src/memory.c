#include "../include/commons.h"
#include "../include/memory.h"

int page_counter = 0;

void memory_status() {
  printf("--------*-------- Memory status --------*--------\n");
  printf("Total memory: %d KB\n", MEM_LENGTH);
  printf("OS memory: %d KB\n", OS_MEMORY_SIZE);
  printf("Pages allocated: %d page(s)\n", page_counter);
  printf("Pages available: %d KB\n", MEM_LENGTH - OS_MEMORY_SIZE - page_counter*PAGE_SIZE);
  printf("MEMORY OCUPATION: %.2f%% [%d/%d]\n", ((float)page_counter*PAGE_SIZE / (MEM_LENGTH - OS_MEMORY_SIZE)) * 100, page_counter*PAGE_SIZE, MEM_LENGTH - OS_MEMORY_SIZE);
  printf("------------------------------------------------\n");
}

void initialize_page(Page *page, int page_num, int instructions_count) {
  page->page_number = page_num;
  page->reference_bit = 0;
  page->used_bit = 0;
  page->instruction_count = 1;
  page->instructions = malloc(sizeof(Instruction) * instructions_count);
}

int sum_of_exec_time(Instruction *instructions, int instructions_count) {
  int sum = 0;
  for (int i=0; i < instructions_count; i++) {
    if (instructions[i].opcode == EXEC) {
      sum += instructions[i].value;
    }
  }
  //print_instructions(instructions, instructions_count);
  //print_win_args(janela_memory, "sum: %d", sum);
  return sum;
}

PageTable *build_page_table(Instruction *instructions, int instructions_count) {
  PageTable *page_table = malloc(sizeof(PageTable));
  
  page_table->pages = malloc(sizeof(Page) * RESIDENT_SET); // allocate 16 pages (resident set amount)
  if (page_table->pages == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }

  page_table->page_count = 0;
  int num_of_instructions_per_page = 0, iteration_val, i;

  for (i = 0; i < instructions_count && page_table->page_count < RESIDENT_SET; i++) {
    // if the instruction is EXEC, it will create a new page
    if (instructions[i].opcode == EXEC && (page_table->page_count > 0 ? sum_of_exec_time(page_table->pages[page_table->page_count - 1].instructions, page_table->pages[page_table->page_count - 1].instruction_count) + instructions[i].value > 1000 : 1)) {
      for (int j = 0; (j < instructions[i].value / 1000  || j == 0) && page_table->page_count < RESIDENT_SET; j++) {        
        // every new page created will have zero instructions initially
        num_of_instructions_per_page = 0;

        initialize_page(&page_table->pages[page_table->page_count], page_table->page_count, instructions_count);
        page_counter++;

        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page] = instructions[i];

        // the value of the instruction is the original value minus 1000 * j, this way we can have the correct value for each instruction
        iteration_val = ((instructions[i].value - (1000 * j)) % 1000);
        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page++].value = iteration_val == 0 ? 1000 : iteration_val;
        page_table->page_count++;
      }
    } else {
      if (page_table->page_count == 0) {
        initialize_page(&page_table->pages[page_table->page_count ], page_table->page_count, instructions_count);
        page_counter++;

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

void refresh_page_table(PageTable **page_table, Instruction *instructions, int instructions_count, int last_instruction_loaded) {
  /*
    INITIAL ASSUMPTIONS
      - this function will only be called when the page table is full
      - the last instruction loaded is considered as read by complete
      - if for example, only more 4 pages will be needed, the other 12 pages MUST be freed
  */

  int old_page_table_size = (*page_table)->page_count;
  (*page_table)->page_count = 0;
  int num_of_instructions_per_page = 0, iteration_val, i;
  
  for (i = last_instruction_loaded + 1; i < instructions_count && (*page_table)->page_count < RESIDENT_SET; i++) {
    if (instructions[i].opcode == EXEC && ((*page_table)->page_count > 0 ? sum_of_exec_time((*page_table)->pages[(*page_table)->page_count - 1].instructions, (*page_table)->pages[(*page_table)->page_count - 1].instruction_count) + instructions[i].value > 1000 : 1)) {
      for (int j = 0; (j < instructions[i].value / 1000  || j == 0) && (*page_table)->page_count < RESIDENT_SET; j++) {        
        // every new page created will have zero instructions initially
        num_of_instructions_per_page = 0;

        initialize_page(&(*page_table)->pages[(*page_table)->page_count], (*page_table)->page_count, instructions_count);

        (*page_table)->pages[(*page_table)->page_count].instructions[num_of_instructions_per_page] = instructions[i];

        // the value of the instruction is the original value minus 1000 * j, this way we can have the correct value for each instruction
        iteration_val = ((instructions[i].value - (1000 * j)) % 1000);
        (*page_table)->pages[(*page_table)->page_count].instructions[num_of_instructions_per_page++].value = iteration_val == 0 ? 1000 : iteration_val;
        (*page_table)->page_count++;
      }
    } else {
      if ((*page_table)->page_count == 0) {
        initialize_page(&(*page_table)->pages[(*page_table)->page_count ], (*page_table)->page_count, instructions_count);
        
        (*page_table)->pages[(*page_table)->page_count].instructions[num_of_instructions_per_page++] = instructions[i];
    
        (*page_table)->page_count++;
      } else {
        // there were already pages created, so we need to add the instruction to the last page created
        (*page_table)->pages[(*page_table)->page_count - 1].instruction_count++;
        (*page_table)->pages[(*page_table)->page_count - 1].instructions[num_of_instructions_per_page++] = instructions[i];
      }
    }
  }

  (*page_table)->missing_instructions = (i < instructions_count && (*page_table)->page_count == RESIDENT_SET) ? true : false;
  (*page_table)->last_instruction_loaded = i-1;

  printf("Page table refreshed. Old size: %d, New size: %d, Instructions Loaded [%d/%d]\n", old_page_table_size, (*page_table)->page_count, i, instructions_count);
  while (i < old_page_table_size) {
    free((*page_table)->pages[i].instructions);
    i++;
  }

  page_counter -= (old_page_table_size - (*page_table)->page_count);
}

void free_page_table(PageTable **page_table) {
  for (int i = 0; i < (*page_table)->page_count; i++) {
    free((*page_table)->pages[i].instructions);
  }
  free((*page_table)->pages);
  free(*page_table);
}