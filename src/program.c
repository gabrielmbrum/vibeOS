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
  program->header.semaphores = "";
  for (int i = 0; i < strlen(line); i++) {
    if (line[i] != ' ') {
      strcat(program->header.semaphores, &line[i]);
    }
  }

  // reading instructions
  program->instructions = malloc(sizeof(Instruction) * MAX_INSTRUCTIONS);
  int instruction_count = 0;
  while (fgets(line, BUFFER_READER_SIZE, file) != NULL) {
    line[strcspn(line, "\n")] = '\0';

    // getting the name
    char *name = malloc(sizeof(line));
    int i = 0, j = 0;
    for (; i < strlen(line); i++) {
      if (line[i] == ' ') {
        name[i] = '\0';
        break;
      }
      name[i] = line[i];
    }

    // getting the value
    char *runtime_str = malloc(sizeof(line) - i);
    for (; j < strlen(line) - i; j++) {
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

  return program;
}

