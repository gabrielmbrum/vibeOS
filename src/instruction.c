#include "../include/instruction.h"
#include "../include/commons.h"

Instruction* instruction_builder(const char *name, const char *runtime_str) {
  Instruction *instruction = malloc(sizeof(Instruction));
  if (instruction == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }
  
  instruction->semaphore_name = '\0'; // Initialize semaphore_name to null
  
  switch (name[0]) {
    case 'e':
      instruction->opcode = EXEC;
      instruction->value = atoi(runtime_str);
      break;
    case 'r':
      instruction->opcode = READ;
      instruction->value = runtime_str[0];
      break;
    case 'w':
      instruction->opcode = WRITE;
      instruction->value = runtime_str[0];
      break;
    case 'P':
      instruction->opcode = P;
      instruction->value = atoi(runtime_str);
      instruction->semaphore_name = name[2];
      break;
    case 'V':
      instruction->opcode = V;
      instruction->value = atoi(runtime_str);
      instruction->semaphore_name = name[2];
      break;
    case 'p':
      instruction->opcode = PRINT;
      instruction->value = atoi(runtime_str);
      break;
  }
  
  return instruction;
}

// void print_instruction(const Instruction instruction) {
//   // Print instruction based on opcode
//   switch (instruction.opcode) {
//       case EXEC:
//           printf("EXEC %d\n", instruction.value);
//           break;
//       case READ:
//           printf("READ %c\n", instruction.value);
//           break;
//       case WRITE:
//           printf("WRITE %c\n", instruction.value);
//           break;
//       case WAIT:
//           printf("WAIT %d\n", instruction.value);
//           break;
//       case P:
//           printf("P %c\n", instruction.semaphore_name);
//           break;
//       case V:
//           printf("V %c\n", instruction.semaphore_name);
//           break;
//       case PRINT:
//           printf("PRINT %d\n", instruction.value);
//           break;
//       default:
//           printf("UNKNOWN INSTRUCTION\n");
//           break;
//   }
// }