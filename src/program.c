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
    //fprintf(stderr, "Error opening file: %s\n", filename);
    return NULL;
  }

  Program *program = malloc(sizeof(Program));

  char line[BUFFER_READER_SIZE];
  //puts("reading header....");
  // reading header
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
  //printf("\n\tsemaphores line: %s", line);
  line[strcspn(line, "\n")] = '\0';
  program->header.semaphores = malloc(sizeof(char) * (strlen(line) + 1));
  strcpy(program->header.semaphores, "");
  for (int i = 0; i < (int) strlen(line); i++) {
    if (line[i] != ' ') {
      char *aux = malloc(sizeof(char) * 2); aux[0] = line[i]; aux[1] = '\0';
      strcat(program->header.semaphores, aux);
      //printf("\n\t\tsemaphore list in char %d -> sem: %s | aux: %s\n", i, program->header.semaphores, aux);
    }
  }
  //printf("semaphores: %s[FIM]\n", program->header.semaphores);
  //puts("header readed :>");

  fgets(line, BUFFER_READER_SIZE, file); //reading the empty line between the header and instructions

  //puts("reading instructions....");
  //printf("reading instructions of process %s\n", program->header.name);
  // reading instructions
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
      //printf("name[%d]: %c ", i, name[i]);
    }
    name[i] = '\0';

    // getting the value
    char *runtime_str = malloc(sizeof(line) - i);
    for (; j < (int) strlen(line) - i; j++) {
      runtime_str[j] = line[i + j + 1];
    }
    runtime_str[j] = '\0';    
    // printf("\n\tsizeofs: line(%d) %s, name(%d) %s, runtime_str(%d) %s\n", (int) strlen(line), line, (int) strlen(name), name, (int) strlen(runtime_str), runtime_str);
    // printf("\n\n\t[instruction %d] name:%s & value: %s\n\n", instruction_count, name, runtime_str);

    Instruction *instruction = instruction_builder(name, runtime_str);
    program->instructions[instruction_count] = *instruction;
    // print_instruction(program->instructions[instruction_count]);
    instruction_count++;

    free(name);
    free(runtime_str);
    free(instruction);
  }
  //printf("instructions cont: %d\n", instruction_count);
  program->instructions_count = instruction_count;
  //puts("instructions readed :>");

  fclose(file);

  return program;
}

void print_program(Program *program) {
  if (program == NULL) {
      printf("NULL program\n");
      return;
  }

  // Print header information
  
  printf("--------------------------------------------------\n");
  printf("Printing program: %s\n", program->header.name);
  printf("Segment ID: %d\n", program->header.segment_id);
  printf("Original Priority: %d\n", program->header.original_priority);
  printf("Segment Size: %d\n", program->header.segment_size);
  printf("Semaphores: %s\n", program->header.semaphores);
  
  // Print instructions
  //printf("Instructions:\n");
  printf("Instructions:");
  //print_instructions(program->instructions, program->instructions_count);
  printf("--------------------------------------------------");
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