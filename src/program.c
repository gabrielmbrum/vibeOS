// ler o arquivo e montar o programa
// enviar header para process
// ler as intruçoes e jogar pra instruction_builder()
// preciso associar essas instructions ao process
#include "../include/program.h"
#include "../include/commons.h"

Program* read_program(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return NULL;
  }

  Program *program = malloc(sizeof(Program));

  char line[BUFFER_READER_SIZE];
  puts("reading header....");
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
  line[strcspn(line, "\n")] = '\0';
  program->header.semaphores = malloc(sizeof(char) * (strlen(line) + 1));
  for (int i = 0; i < (int) strlen(line); i++) {
    if (line[i] != ' ') {
      char *aux = malloc(sizeof(char) * 2); aux[0] = line[i]; aux[1] = '\0';
      strcat(program->header.semaphores, aux);
    }
  }
  //printf("semaphores: %s[FIM]\n", program->header.semaphores);
  puts("header readed :>");

  fgets(line, BUFFER_READER_SIZE, file); //reading the empty line between the header and instructions

  puts("reading instructions....");
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
        name[i] = '\0';
        break;
      }
      name[i] = line[i];
    }

    // getting the value
    char *runtime_str = malloc(sizeof(line) - i);
    for (; j < (int) strlen(line) - i; j++) {
      runtime_str[j] = line[i + j + 1];
    }
    runtime_str[j] = '\0';    
    //printf("[instruction %d] name:%s & value: %s\n", instruction_count, name, runtime_str);

    Instruction *instruction = instruction_builder(name, runtime_str);
    program->instructions[instruction_count] = *instruction;
    //print_instruction(program->instructions[instruction_count]);
    instruction_count++;

    free(name);
    free(runtime_str);
    free(instruction);
  }
  //printf("instructions cont: %d\n", instruction_count);
  program->instructions_count = instruction_count;
  puts("instructions readed :>");

  fclose(file);

  return program;
}

void print_program(const Program *program) {
  if (program == NULL) {
      printf("NULL program\n");
      return;
  }

  // Print header information
  printf("--------------------------------------------------\n");
  printf("\nPrinting program: %s\n", program->header.name);
  printf("\tSegment ID: %d\n", program->header.segment_id);
  printf("\tOriginal Priority: %d\n", program->header.original_priority);
  printf("\tSegment Size: %d\n", program->header.segment_size);
  printf("\tSemaphores: %s\n", program->header.semaphores);
  
  // Print instructions
  printf("Instructions:\n");
  print_instructions(program->instructions, program->instructions_count);
  printf("\n--------------------------------------------------\n\n");
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