#include "../include/commons.h"
#include "../include/memory.h"

int page_counter = 0;

void memory_status() {
  printf("\n--------*-------- Memory status --------*--------\n");
  printf("\tTotal memory: %d KB\n", MEM_LENGTH);
  printf("\tOS memory: %d KB\n", OS_MEMORY_SIZE);
  printf("\tPages allocated: %d page(s)\n", page_counter);
  printf("\tPages available: %d KB\n", MEM_LENGTH - OS_MEMORY_SIZE - page_counter*PAGE_SIZE);
  printf("\tMEMORY OCUPATION: %.2f%% [%d/%d]\n", ((float)page_counter*PAGE_SIZE / (MEM_LENGTH - OS_MEMORY_SIZE)) * 100, page_counter*PAGE_SIZE, MEM_LENGTH - OS_MEMORY_SIZE);
  printf("\n------------------------------------------------\n");
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
  return sum;
}

PageTable *build_page_table(Instruction *instructions, int instructions_count) {
  PageTable *page_table = malloc(sizeof(PageTable));

  page_table->pages = malloc(sizeof(Page) * RESIDENT_SET); // allocate 16 pages (resident set amount)
  if (page_table->pages == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  page_table->page_count = 0;
  int num_of_instructions_per_page = 0, iteration_val, i, j;

  for (i = 0; i < instructions_count; i++) {
    // if the instruction is EXEC, it will create a new page
    if (instructions[i].opcode == EXEC && (page_table->page_count > 0 ? sum_of_exec_time(page_table->pages[page_table->page_count - 1].instructions, page_table->pages[page_table->page_count - 1].instruction_count) + instructions[i].value > 1000 : 1)) {
      // if it gets here, it means that other page will ne needed to be created, but we cant because the limit was reached
      
      for (j = 0; (j < instructions[i].value / 1000  || j == 0) && page_table->page_count < RESIDENT_SET; j++) {        
        // every new page created will have zero instructions initially
        printf("j: %d\n", j);
        num_of_instructions_per_page = 0;

        initialize_page(&page_table->pages[page_table->page_count], page_table->page_count, instructions_count);
        page_counter++;

        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page] = instructions[i];

        // the value of the instruction is the original value minus 1000 * j, this way we can have the correct value for each instruction
        iteration_val = ((instructions[i].value - (1000 * j)) % 1000);
        page_table->pages[page_table->page_count].instructions[num_of_instructions_per_page++].value = iteration_val == 0 ? 1000 : iteration_val;
        page_table->page_count++;
      }
      
      //printf("\nsaiu do if com j = %d sendo que instructions[i].value = %d e page_count = %d\n", j, instructions[i].value, page_table->page_count);
      //printf("boolean: %d\n", page_table->page_count == RESIDENT_SET && !(j < instructions[i].value / 1000));
      // it reached the maximum of pages and it still has instructions to load
      if (page_table->page_count == RESIDENT_SET && (j < instructions[i].value / 1000)) break;

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

  printf("\nj: %d | i: %d\n", j, i);
  page_table->missing_instructions = (i < instructions_count && page_table->page_count == RESIDENT_SET) ? true : false;
  page_table->last_instruction_loaded = i;
  page_table->last_instruction_counter = j;
  
  return page_table;
}

void refresh_page_table(PageTable **page_table, Instruction *instructions, int instructions_count) {
  
  int old_page_table_size = (*page_table)->page_count;
  (*page_table)->page_count = 0;
  int num_of_instructions_per_page = 0, iteration_val, i, j, remaining_pages = 0;
  
  /*
  
    if the last instruction loadaed was an EXEC and we didnt have the necessary space to load it completely,
    we need to create the reamining pages for it, if it reamins to no complete the instruction, 
    we will keep it signed to check in the next refresh

  */
  print_instruction(instructions[(*page_table)->last_instruction_loaded]);
  printf("last instruction loaded: %d\n", (*page_table)->last_instruction_loaded);
  printf("last instruction counter: %d\n", (*page_table)->last_instruction_counter);
  if (instructions[(*page_table)->last_instruction_loaded].opcode == EXEC && instructions[(*page_table)->last_instruction_loaded].value > 1000*((*page_table)->last_instruction_counter)) {
    puts("im doing rehab");
    // i will need to make the first pages have the last remaining space of the last instruction loaded
    remaining_pages = (instructions[(*page_table)->last_instruction_loaded].value - 1000*((*page_table)->last_instruction_counter)) / 1000 > 0 ? (instructions[(*page_table)->last_instruction_loaded].value - 1000*((*page_table)->last_instruction_counter)) / 1000 : 1;
    
    for (i = 0; i < (remaining_pages > RESIDENT_SET ? RESIDENT_SET : remaining_pages); i++) {
      // every new page created will have zero instructions initially
      num_of_instructions_per_page = 0;

      initialize_page(&(*page_table)->pages[(*page_table)->page_count], (*page_table)->page_count, instructions_count);

      (*page_table)->pages[(*page_table)->page_count].instructions[num_of_instructions_per_page] = instructions[(*page_table)->last_instruction_loaded];

      // the value of the instruction is the original value minus 1000 * (i + already counted times), this way we can have the correct value for each instruction
      iteration_val = ((instructions[(*page_table)->last_instruction_loaded].value - (1000 * (i + (*page_table)->last_instruction_counter))) % 1000);

      (*page_table)->pages[(*page_table)->page_count].instructions[num_of_instructions_per_page++].value = iteration_val == 0 ? 1000 : iteration_val;
      (*page_table)->page_count++;
    }
    (*page_table)->last_instruction_counter += i;

    // if it was added all the necessary pages
    if (i == remaining_pages) {
      (*page_table)->missing_instructions = false;
    }
  }

  /*
  
    * when get here i could already has done the rehab for the last instruction loaded or not
    * in both cases, if i check if the page count is less than the resident set, i will be able to add the instructions

  */

  if ((*page_table)->page_count < RESIDENT_SET) {
    printf("im getting here with this infos -> i: %d, page_count: %d\n", i, (*page_table)->page_count);
    for (i = (*page_table)->last_instruction_loaded + 1; i < instructions_count; i++) {
      if (instructions[i].opcode == EXEC && ((*page_table)->page_count > 0 ? sum_of_exec_time((*page_table)->pages[(*page_table)->page_count - 1].instructions, (*page_table)->pages[(*page_table)->page_count - 1].instruction_count) + instructions[i].value > 1000 : 1)) {
        if ((*page_table)->page_count == RESIDENT_SET) 
          break;
        for (j = 0; (j < instructions[i].value / 1000  || j == 0) && (*page_table)->page_count < RESIDENT_SET; j++) {        
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

      (*page_table)->missing_instructions = (i < instructions_count && (*page_table)->page_count == RESIDENT_SET) ? true : false;
      (*page_table)->last_instruction_loaded = i-1;
      (*page_table)->last_instruction_counter = j;
    }
  
  }

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