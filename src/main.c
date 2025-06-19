#include "../include/commons.h"
#include "../include/debug.h"
#include "../include/instruction.h"
#include "../include/interface.h"
#include "../include/iohandler.h"
#include "../include/kernel.h"
#include "../include/process.h"
#include "../include/program.h"
#include "../include/semaphore.h"



int main (){
  srand(time(NULL));
  init_Kernel();


  janela_intro();
  char *input = malloc((MAX_OUTPUT_STR)*sizeof(char));
  strcpy(input,"\0");
  init_interface();
  
  
  while(strcmp(input,"q")){
    if(get_input(input,janela_OUTPUT)!= NULL){
        char aux[MAX_OUTPUT_STR] = "../programs/";
        strcat(aux,input);
        Program *prog1 = read_program(aux);
        if (prog1 == NULL) {
          print_win(janela_OUTPUT,"Failed to read program");
        return;
        }else{
          Process *process1 = create_process_from_program(prog1);
          add_process_to_BCP(process1);
        } 
    }; 

    clear_space(6,34,strlen(input));
    move(6,34);    
  }  
  //shutdown_Kernel();
  endwin();

 /*  Program *p1 = read_program("../programs/synt1");
  Process *process1 = create_process_from_program(p1);
  add_process_to_BCP(process1); */
}