#include "../include/commons.h"
#include "../include/process.h"
#include "../include/debug.h"
#include "../include/kernel.h"
#include "../include/program.h"
#include "../include/instruction.h"
#include "../include/iohandler.h"

void luigi_test() {
  Process *p1, *p2, *p3, *p4;
  p1 = processCreate(0, "sexta-feira", 0);
  //p2=processCreate(2, "play-tv", 0);
  //p3=processCreate(3, "nóstacomo", 0);
  //p4= processCreate(4, "xambão", 0);

  p1->counter_rw = 0;
  /*p2->counter_rw = 0;
  p3->counter_rw = 0;
  p4->counter_rw = 0;

  p1->runtime_execution = 10;
  p2->runtime_execution = 5;
  p3->runtime_execution = 11;
  p4->runtime_execution = 11;*/

  init_Kernel();
  add_process_to_BCP(p1);
  //add_process_to_BCP(p2);
  //add_process_to_BCP(p3);
  //add_process_to_BCP(p4);

  print_BCP(&kernel->BCP, kernel->process_amount);

  schedule();
  puts("");
  //print_BCP(&kernel->BCP);

  //print_BCP(&kernel->BCP);

  printf("FINISHED \n");
}

void luigi_testv2() {
  init_Kernel(); // Inicializa o kernel (incluindo mutex/cond)

  // Carrega programas
  Program *prog1 = read_program("../programs/synt1");
  Program *prog2 = read_program("../programs/synt2");
  Program *prog3 = read_program("../programs/synt3");

  // Verifica erros na leitura
  if (!prog1 || !prog2 || !prog3) {
      fprintf(stderr, "Erro na leitura de programas\n");
      free_program(prog1);
      free_program(prog2);
      free_program(prog3);
      return;
  }

  // Cria processos
  Process *p1 = create_process_from_program(prog1);
  Process *p2 = create_process_from_program(prog2);
  Process *p3 = create_process_from_program(prog3);

  // Adiciona processos ao BCP (inicia automaticamente o escalonador)
  add_process_to_BCP(p1);
  add_process_to_BCP(p3);
  add_process_to_BCP(p2);
  // Exibe estado inicial
  print_BCP(&kernel->BCP, kernel->process_amount);
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");

  // Loop principal: aguarda término dos processos
  while (kernel->process_amount > 0 && !kernel->shutdown_request) {
      sleep(1); // Reduz uso da CPU
      printf("\nProcessos restantes: %d\n", kernel->process_amount);
  }
  printf("\n▶ Scheduler status: %s\n", kernel->scheduler_running ? "ATIVO" : "INATIVO");
  // Cleanup
  pthread_join(kernel->input_thread, NULL);
  shutdown_Kernel();
}

void luigi_testv3(){
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }
  Process *p1 = create_process_from_program(prog1);
  //IORequest *r1 = make_request(p1, WRITE, 20);
  IORequest *r1 = make_request(p1, READ, 20);
  IOQueue *queue=init_queue(queue);
  enqueue(queue, r1);
  printf("PID: %d\n",queue->tail->process->pid);
  printf("ARG: %d\n",queue->tail->arg);
  printf("OPCODE: %d\n",queue->tail->opcode);
  exec_request(queue);
}

void luigi_testv4(){
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }
  Process *p1 = create_process_from_program(prog1);
  init_Kernel();
  add_process_to_BCP(p1);
  print_BCP(&kernel->BCP, kernel->process_amount);


}


void brum_test() {

  system("clear");
  Program *prog1 = read_program("../programs/synt1");
  if (prog1 == NULL) {
    fprintf(stderr, "Failed to read program\n");
    return;
  }

  // Program *prog2 = read_program("../programs/synt2");
  // if (prog2 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog3 = read_program("../programs/synt3");
  // if (prog3 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog4 = read_program("../programs/synt4");
  // if (prog4 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // Program *prog5 = read_program("../programs/synt5");
  // if (prog5 == NULL) {
  //   fprintf(stderr, "Failed to read program\n");
  //   return;
  // }

  // print_program(prog1);
  // print_program(prog2);
  // print_program(prog3);
  // print_program(prog4);
  // print_program(prog5);

  Process *proc1 = create_process_from_program(prog1);

  print_program(prog1);

  print_process(proc1);

  memory_status();

  refresh_page_table(&proc1->page_table, prog1->instructions, prog1->instructions_count);

  print_process(proc1);

  memory_status();

  free_program(prog1);
  // free_program(prog2);
  // free_program(prog3);
  // free_program(prog4);
  // free_program(prog5);
}

int main () {

  brum_test();

  return 0;
}