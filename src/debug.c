#include "../include/commons.h"

void print_bcp(Process **BCP) {
  if (*BCP == NULL) {
    update_dados(janela_SCHEDULER, 0, "BCP is empty");
    return;
  }

  //update_dados(janela_SCHEDULER, 0, "Process Control Block (BCP):");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      //update_dados(janela_SCHEDULER, 0, "PID %d, Name: %s, Page Counter: %d. ", 
             //(*BCP)[i].pid, (*BCP)[i].name, (*BCP)[i].page_table->page_count);
    }
  }
}

void print_SCHEDULER(Process **BCP) {
  if (*BCP == NULL) {
    update_dados(janela_SCHEDULER, 0, "BCP is empty");
    return;
  }

  //update_dados(janela_SCHEDULER, 0, "Process Control Block (BCP):");
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if ((*BCP)[i].pid != EMPTY_BCP_ENTRY) {
      update_dados(janela_SCHEDULER,0,"PID: %d Name: %s State: %s", (*BCP)[i].pid, (*BCP)[i].name, process_state_to_string((*BCP)[i].state));
      //update_dados(janela_SCHEDULER, 0,"PID: %d Priority: %d, Counter RW: %d, Time-Slice: %d", (*BCP)[i].pid, (*BCP)[i].priority, (*BCP)[i].counter_rw, (*BCP)[i].slice_time);
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

const char* process_state_to_string(ProcessState state) {
    switch (state) {
        case RUNNING:    return "RUNNING";
        case READY:     return "READY";
        case WAITING:   return "WAITING";
        case TERMINATED: return "TERMINATED";
        default:        return "UNKNOWN";
    }
}