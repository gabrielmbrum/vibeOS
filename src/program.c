// ler o arquivo e montar o programa
// enviar header para process
// ler as intruçoes e jogar pra instruction_builder()
// preciso associar essas instructions ao process
#include "../include/program.h"
#include "../include/commons.h"
#include "../include/semaphore.h"

Program* read_program(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    update_dados(janela_OUTPUT, "Error opening file: %s.", filename);
    return NULL;
  }

  Program *program = malloc(sizeof(Program));

  char line[BUFFER_READER_SIZE];

  fgets(line, BUFFER_READER_SIZE, file);
  line[strcspn(line, "\n")] = '\0'; // remove newline character
  
  program->header.name = strdup(line); // strdup allocates memory and copies the string
  
  fgets(line, BUFFER_READER_SIZE, file);
  line[strcspn(line, "\n")] = '\0'; 
  program->header.segment_id = atoi(line);
  
  fgets(line, BUFFER_READER_SIZE, file);
  line[strcspn(line, "\n")] = '\0';
  program->header.original_priority = atoi(line);
  
  fgets(line, BUFFER_READER_SIZE, file);
  line[strcspn(line, "\n")] = '\0';
  program->header.segment_size = atoi(line);
  
  fgets(line, BUFFER_READER_SIZE, file);
  line[strcspn(line, "\n")] = '\0';
  program->header.semaphores = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(program->header.semaphores, "");
  for (int i = 0; i < (int) strlen(line); i++) {
    if (line[i] != ' ') {
      char *aux = malloc(sizeof(char) * 2); aux[0] = line[i]; aux[1] = '\0';
      strcat(program->header.semaphores, aux);
    }
  }
  //update_dados(janela_process,"semaphores: %s[FIM]", program->header.semaphores);

  fgets(line, BUFFER_READER_SIZE, file); //reading the empty line between the header and instructions

  program->instructions = malloc(sizeof(Instruction) * MAX_INSTRUCTIONS);
  int instruction_count = 0;
  while (fgets(line, BUFFER_READER_SIZE, file) != NULL) {
    line[strcspn(line, "\n")] = '\0';

    // getting the name
    char *name = malloc(sizeof(line));
    int i = 0, j = 0;
    for (; i < (int) strlen(line); i++) {
      if (line[i] == ' ') {
        break;
      }
      name[i] = line[i];
    }
    name[i] = '\0';

    // getting the value
    char *runtime_str = malloc(sizeof(line) - i);
    for (; j < (int) strlen(line) - i; j++) {
      runtime_str[j] = line[i + j + 1];
    }
    runtime_str[j] = '\0';    
    Instruction *instruction = instruction_builder(name, runtime_str);
    program->instructions[instruction_count] = *instruction;
    instruction_count++;

    free(name);
    free(runtime_str);
    free(instruction);
  }
  program->instructions_count = instruction_count;

  fclose(file);

  return program;
}

void free_program(Program *program) {
  if (program == NULL) {
      return;
  }

  // Free the name string in header
  if (program->header.name != NULL) {
      free(program->header.name);
      program->header.name = NULL;
  }

  // Free the semaphores string in header
  if (program->header.semaphores != NULL) {
      free(program->header.semaphores);
      program->header.semaphores = NULL;
  }

  // Free the instructions array
  if (program->instructions != NULL) {
      free(program->instructions);
      program->instructions = NULL;
  }

  // Finally free the program itself
  free(program);
}