#include "../include/interface.h"

WINDOW *create_newwin(int height, int width, int starty, int startx){
    WINDOW *local_win = newwin(height, width, starty, startx);
    refresh();

    // wborder (local_win, '|', '|', '-', '-', '-', '-', '-', '-');
    // /* Os parâmetros usados são
    // * 1. win: a janela na qual operar
    // * 2. ls: caractere a ser usado para o lado esquerdo da janela
    // * 3. rs: caractere a ser usado para o lado direito da janela
    // * 4. ts: caractere a ser usado na parte superior da janela
    // * 5. bs: caractere a ser usado na parte inferior da janela
    // * 6. tl: caractere a ser usado para o canto superior esquerdo da janela
    // * 7. tr: caractere a ser usado no canto superior direito da janela
    // * 8. bl: caractere a ser usado no canto inferior esquerdo da janela
    // * 9. br: caractere a ser usado no canto inferior direito da janela
    // */	
    box(local_win, 0, 0);
    wrefresh(local_win);		
  
    return local_win;
  }

int main(int argc, char *argv[]){
  WINDOW *janela_menu, *janela_I_O, *janela_memory, *janela_process;

  initscr();	
  curs_set(0);		
  janela_menu = create_newwin(20, 59, 0, 1);
  janela_I_O = create_newwin(10, 59, 20, 1);
  janela_memory = create_newwin(15, 59, 0, 61);
  janela_process = create_newwin(15, 59, 15, 61);

  int c = getch();

  endwin();			
  return 0;
}