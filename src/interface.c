#include "../include/interface.h"

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

// output message operations



// window operations
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title){
  WINDOW *local_win = newwin(height, width, starty, startx);
  refresh();

  box(local_win, 0, 0);
  mvwprintw(local_win, 0, 2, "%s", title);
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

// WINDOW *init_windows(WINDOW *janela_menu, WINDOW *janela_output, WINDOW *janela_I_O, WINDOW *janela_memory, WINDOW *janela_process, WINDOW *janela_scheduler){

//   janela_menu = create_newwin(9, 59, 0, 1,"MENU");
//   janela_menu = menu(janela_menu);
//   janela_output = create_newwin(5, 59, 9, 1," OUTPUT ");
//   janela_scheduler = create_newwin(16, 59, 14, 1," SCHEDULER ");
//   janela_memory = create_newwin(10, 59, 0, 61," MEMORY ");
//   janela_process = create_newwin(10, 59, 10, 61, " PROCESS ");
//   janela_I_O = create_newwin(10, 59, 20, 61," I/O ");
// }

// int main(int argc, char *argv[]){
//   WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;
//   int num = 0; // number of current inputs in the process display array

//   char *input = malloc((MAX_INPUT_STR)*sizeof(char));
//   char **displayprocessos = (char**)malloc(MAX_WD_HGH_PROCESS * sizeof(char*));
//   for (int i = 0; i < MAX_WD_HGH_PROCESS; i++) {
//       displayprocessos[i] = (char*)malloc((MAX_INPUT_STR) * sizeof(char));
//   }

//   janela_menu = create_newwin(9, 59, 0, 1,"MENU");
//   janela_menu = init_menu_components(janela_menu);
//   janela_OUTPUT = create_newwin(5, 59, 9, 1," OUTPUT ");
//   janela_SCHEDULER = create_newwin(16, 59, 14, 1," SCHEDULER ");
//   janela_memory = create_newwin(10, 59, 0, 61," MEMORY ");
//   janela_process = create_newwin(10, 59, 10, 61, " PROCESS ");
//   janela_I_O = create_newwin(10, 59, 20, 61," I/O ");

//   do{
//     getstr(input);
//     add_input_list(displayprocessos,input,&num);
//     print_input(janela_process, displayprocessos,&num);
//     clear_input_space(20,6,34);
//     move(6,34);
//     refresh();
//   }while(strcmp(input,"q"));

//   // free memory
//   free(input);

//   // close lncurses window
//   endwin();			
//   return 0;
// }