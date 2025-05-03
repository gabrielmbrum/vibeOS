#include "../include/interface.h"

// defines the minimum line to print something in the generic window
// defines the maximum line to print something in the process window
// defines the maximum name to a input process
// defines the default position in x to start printing

// *         _  _             ____    _____ *
// *        (_)| |           / __ \  / ____|*
// * __   __ _ | |__    ___ | |  | || (___  *
// * \ \ / /| || '_ \  / _ \| |  | | \___ \ *
// *  \ V / | || |_) ||  __/| |__| | ____) |*
// *   \_/  |_||_.__/  \___| \____/ |_____/ *
// * feel the vibe
// *para pegar o tamanho da tela atual -> getmaxyx(stdscr, linhas, colunas);

// input operations
void clear_input_space(int size, int start, int end){
  for (int i = 0; i < size; i++) {
    mvaddch(start, end + i, ' ');
  }
}

void print_input(WINDOW *local_win, char **list, int *size){
    for(int i=0;i<*size;i++){
      //! its not pretty but it works :)
      mvwprintw(local_win, i+1, POS_X, "%s", "                          ");
      mvwprintw(local_win, i+1, POS_X, "%s", list[i]);
      wrefresh(local_win);
    }
}

void add_input_list(char **list, char *input, int *num){
  if(*num<MAX_WD_HGH_PROCESS){
    strcpy(list[*num], input);
    *num = *num+1;
  }else{
    for(int i=0;i<(MAX_WD_HGH_PROCESS-1);i++){
      strcpy(list[i],list[i+1]);
    }
    strcpy(list[MAX_WD_HGH_PROCESS-1], input);
  }
}

// window operations
WINDOW *create_newwin(int height, int width, int starty, int startx){
  WINDOW *local_win = newwin(height, width, starty, startx);
  refresh();

  box(local_win, 0, 0);
  wrefresh(local_win);		

  return local_win;
}

WINDOW *delete_window(WINDOW  *local_win){
  //removes window border before desalocar(favor nn julgar o comentário) memory to said window
  wborder(local_win, ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');

  wrefresh(local_win);
  delwin(local_win);
}

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

WINDOW *menu(WINDOW *local_win){
  mvwprintw(local_win, 0, 2, "%s", " MENU ");
  wrefresh(local_win);
  mvwprintw(local_win, 1, 15, "%s", " ___   _____       ____  ____ ");
  mvwprintw(local_win, 2, 15, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
  mvwprintw(local_win, 3, 15, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
  mvwprintw(local_win, 4, 15, "%s", " |___/_/_.__/\\__/\\____/___/ ");
  mvwprintw(local_win, 6, 2, "%s", "Insira a versão Syst desejada:");
  mvwprintw(local_win, 7, 2, "%s", "Pressione q para sair");
  wrefresh(local_win);

  return local_win;
}

// int main(int argc, char *argv[]){
//   WINDOW *janela_menu, *janela_I_O, *janela_memory, *janela_process, *janela_PID;
//   int num = 0; // number of current inputs in the process display array

//   //malloc array of processos and input to be displayed in the screen 
//   //! This does not represent the total processes in the Simulator, only those that are displayed
//   char *input = malloc((TAM_MAX_STR)*sizeof(char));
//   char **displayprocessos = (char**)malloc(MAX_WD_HGH_PROCESS * sizeof(char*));
//   for (int i = 0; i < MAX_WD_HGH_PROCESS; i++) {
//       displayprocessos[i] = (char*)malloc((TAM_MAX_STR) * sizeof(char));
//   }

//   // initialize introduction window
//   initscr();	
//   curs_set(0);		
//   janela_intro();

//   // initialize main window and components
//   janela_menu = create_newwin(9, 59, 0, 1);
//   janela_menu = menu(janela_menu);
//   wrefresh(janela_menu);

//   janela_process = create_newwin(10, 59, 9, 1);
//   mvwprintw(janela_process, 0, 2, "%s", " PROCESS ");
//   wrefresh(janela_process);

//   janela_memory = create_newwin(11, 59, 19, 1);
//   mvwprintw(janela_memory, 0, 2, "%s", " MEMORY ");
//   wrefresh(janela_memory);

//   janela_PID = create_newwin(15, 59, 0, 61);
//   mvwprintw(janela_PID, 0, 2, "%s", " PID ");
//   wrefresh(janela_PID);

//   janela_I_O = create_newwin(15, 59, 15, 61);
//   mvwprintw(janela_I_O, 0, 2, "%s", " I/O ");
//   wrefresh(janela_I_O);

//   // change window to get user input
//   move(6,34);
//   curs_set(1);

//   // gets the user input, clears the input box and prints the input in another sub-window
//   do{
//     getstr(input);
//     add_input_list(displayprocessos,input,&num);
//     print_input(janela_process, displayprocessos,&num);
//     clear_input_space(20,6,34);
//     move(6,34);
//     refresh();
//   }while(strcmp(input,"q"));
  
//   //TODO: get the lines to be displayed in other windows
//   //? Define other constants for each window bound?
//   //? Create other array od strings for each window? 

//   //TODO: A Platyplus?
//   //? ..
//   //? ..

//   //!__======__
//   //TODO: PERRY
//   //? THE
//   //? PLATYPUS

//   // free memory
//   free(input);

//   // close lncurses window
//   endwin();			
//   return 0;
// }