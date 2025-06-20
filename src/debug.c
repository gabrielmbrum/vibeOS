#include "../include/commons.h"

/* void print_BCP(Process **BCP, int processes) {
  if (*BCP == NULL) {
    //printf("BCP is empty.\n");
    print_win(janela_process,"BCP is empty.");
    return;
  }

  //printf("Process Control Block (BCP):\n");
  print_win(janela_process,"Process Control Block (BCP):");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      print_win_args(janela_SCHEDULER,"PID: %d, Name: %s, State: %d, PC: %d, Priority: %d, Counter RW: %d, Time-Slice: %d",
             (*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].state, (*BCP)[i].pc,
             (*BCP)[i].priority, (*BCP)[i].counter_rw, (*BCP)[i].slice_time);
    }
  }
} */

void print_bcp(Process **BCP) {
  if (*BCP == NULL) {
    //printf("BCP is empty.\n");
    print_win(janela_process,"BCP is empty.");
    return;
  }

  //printf("Process Control Block (BCP):\n");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      print_win_args(janela_process,"[BCP] PID: %d, Name: %s, Page Counter: %d",
             (*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].page_table->page_count);
    }
  }
  print_win(janela_process,"[BCP] Process Control Block:");
}

void print_SCHEDULER(Process **BCP) {
  if (*BCP == NULL) {
    //printf("BCP is empty.\n");
    printf("BCP is empty.\n");
    return;
  }

  //printf("Process Control Block (BCP):\n");
  //printf(janela_process,"Process Control Block (BCP):");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      //print_win_args(janela_SCHEDULER,"PID: %d, Name: %s, State: %d, Priority: %d, Counter RW: %d, Time-Slice: %d",
        //     (*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].state,
          //   (*BCP)[i].priority, (*BCP)[i].counter_rw, (*BCP)[i].slice_time);
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
  //printf("\t%-6s", opcode_to_string(inst.opcode));
  print_win_args(janela_process,"%-6s", opcode_to_string(inst.opcode));
  //printf("%-6s\n", opcode_to_string(inst.opcode));

  switch (inst.opcode) {
    case EXEC:
    case READ:
    case WRITE:
    case PRINT:
    //printf("%d", inst.value);
    print_win_args(janela_process,"%d", inst.value);
      break;
    case P:
    case V:
    print_win_args(janela_process,"'%c'", inst.semaphore_name);
      break;
    case WAIT:
      print_win_args(janela_process," (Wait)");
      break;
  }
}

/* void print_instructions(Instruction *instructions, int total_instructions) {
  for (int i = 0; i < total_instructions; i++) {
    //print_win_args(janela_SCHEDULER,"[instruction %d]  ", i);
    //print_instruction(instructions[i]);
  }
} */

void print_page_table(PageTable *page_table) {
  printf("---------Page Table ---------");
  for (int i = 0; i < page_table->page_count; i++) {
    //print_win_args(janela_SCHEDULER,"Page %d: Reference Bit: %d, Used Bit: %d, Instructions Count: %d, Missing Instructions: %s, Last Instruction Loaded: %d",
      //    page_table->pages[i].page_number, 
        //  page_table->pages[i].reference_bit,
          //page_table->pages[i].used_bit, 
        //  page_table->pages[i].instruction_count,
        //  boolean_to_string(page_table->missing_instructions),
        //  page_table->last_instruction_loaded);

    //print_instructions(page_table->pages[i].instructions, page_table->pages[i].instruction_count);
  }
  printf("--------- End of Page Table ---------");
}

void print_process(Process *proc) {
  //print_win(janela_SCHEDULER,"Process Details:");
  //print_win_args(janela_SCHEDULER,"PID: %d", proc->pid);
  //print_win_args(janela_SCHEDULER,"Name: %s", proc->name);
  //print_win_args(janela_SCHEDULER,"State: %s", state_to_string(proc->state));
  //print_win_args(janela_SCHEDULER,"PC Instruction: %d", proc->pc.last_instruction);
  //print_win_args(janela_SCHEDULER,"PC Page: %d", proc->pc.last_page);
  //print_win_args(janela_SCHEDULER,"Priority: %d", proc->priority);
  //print_win_args(janela_SCHEDULER,"RW Counter: %d", proc->counter_rw);
  //print_win_args(janela_SCHEDULER,"Segment ID: %d, Size: %d", proc->segment_id, proc->segment_size);
  //print_win_args(janela_SCHEDULER,"Runtime: %d", proc->runtime_execution);
  //print_win_args(janela_SCHEDULER,"Semaphores: %s", proc->semaphores);
  //print_win_args(janela_SCHEDULER,"Time-Slice: %d", proc->slice_time);
  if (proc->page_table->page_count == 0) {
    //print_win(janela_SCHEDULER,"Page Table is empty! No pages allocated");
  } else {
    //print_page_table(proc->page_table);
  };
}

void print_all_semaphores() {
  printf("=== Current Semaphores ===\n");
  for (int i = 0; i < total_semaphores; i++) {
    printf("Semaphore '%c': value=%d, blocked=%d",
             semaphores[i].name,
             semaphores[i].value,
             semaphores[i].blocked_count);
  }
}