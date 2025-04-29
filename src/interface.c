#include "../include/interface.h"

// *         _  _             ____    _____ *
// *        (_)| |           / __ \  / ____|*
// * __   __ _ | |__    ___ | |  | || (___  *
// * \ \ / /| || '_ \  / _ \| |  | | \___ \ *
// *  \ V / | || |_) ||  __/| |__| | ____) |*
// *   \_/  |_||_.__/  \___| \____/ |_____/ *
//
// para pegar o tamanho da tela atual -> getmaxyx(stdscr, linhas, colunas);

WINDOW *janela_intro(){
  int lin, col;
  getmaxyx(stdscr, lin, col);
  WINDOW *intro = newwin(8, 40, ((lin/2) - 5), ((col/2) - 20));
  refresh();

  wborder (intro, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  mvwprintw(intro, 0, 0, "%s", "         _  _             ____    _____ ");
  mvwprintw(intro, 1, 0, "%s", "        (_)| |           / __ \\  / ____|");
  mvwprintw(intro, 2, 0, "%s", " __   __ _ | |__    ___ | |  | || (___ ");
  mvwprintw(intro, 3, 0, "%s", " \\ \\ / /| || '_ \\  / _ \\| |  | | \\___ \\ ");
  mvwprintw(intro, 4, 0, "%s", "  \\ V / | || |_) ||  __/| |__| | ____) |");
  mvwprintw(intro, 5, 0, "%s", "   \\_/  |_||_.__/  \\___| \\____/ |_____/ ");
  mvwprintw(intro, 7, 4, "%s", "Aperte qualquer tecla para iniciar");
  wrefresh(intro);

  int c = getch();
  werase(intro);
  wrefresh(intro);
  delete_window(intro);

  return 0;
}

WINDOW *init_windows(WINDOW *janela_menu, WINDOW *janela_I_O, WINDOW *janela_memory, WINDOW *janela_process, WINDOW *janela_PID){
  janela_menu = create_newwin(9, 59, 0, 1);
  janela_process = create_newwin(10, 59, 9, 1);
  janela_memory = create_newwin(11, 59, 19, 1);
  janela_PID = create_newwin(15, 59, 0, 61);
  janela_I_O = create_newwin(15, 59, 15, 61);

  mvwprintw(janela_menu, 0, 2, "%s", " MENU ");
  menu(janela_menu);
  mvwprintw(janela_menu, 6, 2, "%s", "Insira a versão Syst desejada:");
  wrefresh(janela_menu);

  mvwprintw(janela_I_O, 0, 2, "%s", " I/O ");
  wrefresh(janela_I_O);

  mvwprintw(janela_PID, 0, 2, "%s", " PID ");
  wrefresh(janela_PID);

  mvwprintw(janela_memory, 0, 2, "%s", " MEMORY ");
  wrefresh(janela_memory);

  mvwprintw(janela_process, 0, 2, "%s", " PROCESS ");
  wrefresh(janela_process);
  
  return 0;
}

WINDOW *menu(WINDOW *local_win){
  mvwprintw(local_win, 1, 15, "%s", " _    ___ __       ____  ____ ");
  mvwprintw(local_win, 2, 15, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
  mvwprintw(local_win, 3, 15, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
  mvwprintw(local_win, 4, 15, "%s", " |___/_/_.__/\\__/\\____/___/ ");
  wrefresh(local_win);

  return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx){
  WINDOW *local_win = newwin(height, width, starty, startx);
  refresh();

  box(local_win, 0, 0);
  wrefresh(local_win);		

  return local_win;
}

WINDOW *delete_window(WINDOW  *local_win){
  wborder(local_win, ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');

  wrefresh(local_win);
  delwin(local_win);
}

int main(int argc, char *argv[]){
  WINDOW *janela_menu, *janela_I_O, *janela_memory, *janela_process, *janela_PID;

  initscr();	
  curs_set(0);		

  janela_intro();
  init_windows(janela_menu, janela_I_O, janela_memory, janela_process, janela_PID);
  int c = getch();

  endwin();			
  return 0;
}