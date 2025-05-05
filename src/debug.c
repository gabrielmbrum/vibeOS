#include "../include/commons.h"

void print_BCP(Process **BCP, int processes) {
  if (*BCP == NULL) {
    printf("BCP is empty.\n");
    return;
  }

  printf("Process Control Block (BCP):\n");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      printf("PID: %d, Name: %s, State: %d, PC: %d, Priority: %d, Counter RW: "
             "%d, Time-Slice: %d\n",
             (*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].state, (*BCP)[i].pc.last_instruction,
             (*BCP)[i].priority, (*BCP)[i].counter_rw, (*BCP)[i].slice_time);
    }
  }
}

const char *state_to_string(ProcessState state) {
  switch (state) {
    case RUNNING:
      return "RUNNING";
    case READY:
      return "READY";
    case WAITING:
      return "WAITING";
    case TERMINATED:
      return "TERMINATED";
    default:
      return "UNKNOWN";
  }
}

const char *opcode_to_string(Opcode opcode) {
  switch (opcode) {
    case EXEC:
      return "EXEC";
    case READ:
      return "READ";
    case WRITE:
      return "WRITE";
    case WAIT:
      return "WAIT";
    case P:
      return "P";
    case V:
      return "V";
    case PRINT:
      return "PRINT";
    default:
      return "UNKNOWN";
  }
}

const char *boolean_to_string(bool value) {
  return value ? "true" : "false";
}

void print_instruction(Instruction inst) {
  printf("\t%-6s", opcode_to_string(inst.opcode));

  switch (inst.opcode) {
    case EXEC:
    case READ:
    case WRITE:
    case PRINT:
      printf("\t%d", inst.value);
      break;
    case P:
    case V:
      printf("\t'%c'", inst.semaphore_name);
      break;
    case WAIT:
      printf(" (Wait)");
      break;
  }
  printf("\n");
}

void print_instructions(Instruction *instructions, int total_instructions) {
  for (int i = 0; i < total_instructions; i++) {
    printf("\t[instruction %d]  ", i);
    print_instruction(instructions[i]);
  }
}

void print_page_table(PageTable *page_table) {
  printf("\n---------*---------*---------*---------  Page Table ---------*---------*---------*---------\n\n");
  for (int i = 0; i < page_table->page_count; i++) {
    printf("Page %d: Reference Bit: %d, Used Bit: %d, Instructions Count: %d, Missing Instructions: %s, Last Instruction Loaded: %d\n",
          page_table->pages[i].page_number, 
          page_table->pages[i].reference_bit,
          page_table->pages[i].used_bit, 
          page_table->pages[i].instruction_count,
          boolean_to_string(page_table->missing_instructions),
          page_table->last_instruction_loaded);

    print_instructions(page_table->pages[i].instructions, page_table->pages[i].instruction_count);
  }
  printf("\n---------*---------*---------*--------- End of Page Table ---------*---------*---------*---------\n");
}

void print_process(Process *proc) {
  printf("\nProcess Details:\n");
  printf("PID: %d\n", proc->pid);
  printf("Name: %s\n", proc->name);
  printf("State: %s\n", state_to_string(proc->state));
  printf("PC Instruction: %d\n", proc->pc.last_instruction);
  printf("PC Page: %d\n", proc->pc.last_page);
  printf("Priority: %d\n", proc->priority);
  printf("RW Counter: %d\n", proc->counter_rw);
  printf("Segment ID: %d, Size: %d\n", proc->segment_id, proc->segment_size);
  printf("Runtime: %d\n", proc->runtime_execution);
  printf("Semaphores: %s\n", proc->semaphores);
  printf("Time-Slice: %d\n", proc->slice_time);
  printf("Last Instruction Loaded: %d\n", proc->page_table->last_instruction_loaded);
  printf("Last Instruction Counter: %d\n", proc->page_table->last_instruction_counter);
  if (proc->page_table->page_count == 0) {
    printf("Page Table is empty! No pages allocated\n");
  } else {
    //print_page_table(proc->page_table);
  }
  printf("\n");
}

void print_all_semaphores() {
  printf("\n=== Current Semaphores ===\n");
  for (int i = 0; i < total_semaphores; i++) {
      printf("Semaphore '%c': value=%d, blocked=%d\n",
             semaphores[i].name,
             semaphores[i].value,
             semaphores[i].blocked_count);
  }
}