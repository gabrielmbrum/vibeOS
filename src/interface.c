#include "../include/interface.h"
#define MIN_WD_HGH 1
#define MAX_WD_HGH_PROCESS 8
#define MAX_INPUT_STR 25
#define MAX_OUTPUT_STR 100
#define POS_X 1

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

void print_input(WINDOW *local_window, char **list, int *size){
    for(int i=0;i<*size;i++){
      //! its not pretty but it works :)
      mvwprintw(local_window, i+1, POS_X, "%s", "                          ");
      mvwprintw(local_window, i+1, POS_X, "%s", list[i]);
      wrefresh(local_window);
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

WINDOW *init_menu_components(WINDOW *menu){
  mvwprintw(menu, 0, 2, "%s", " MENU ");
  wrefresh(menu);
  mvwprintw(menu, 1, 15, "%s", " ___   _____       ____  ____ ");
  mvwprintw(menu, 2, 15, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
  mvwprintw(menu, 3, 15, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
  mvwprintw(menu, 4, 15, "%s", " |___/_/_.__/\\__/\\____/___/ ");
  mvwprintw(menu, 6, 2, "%s", "Insira a versão Syst desejada:");
  mvwprintw(menu, 7, 2, "%s", "Pressione q para sair");
  wrefresh(menu);

  return menu;
}

int main(int argc, char *argv[]){
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;
  int num = 0; // number of current inputs in the process display array

  //malloc array of processos and input to be displayed in the screen 
  //! This does not represent the total processes in the Simulator, only those that are displayed
  char *input = malloc((MAX_INPUT_STR)*sizeof(char));
  char **displayprocessos = (char**)malloc(MAX_WD_HGH_PROCESS * sizeof(char*));
  for (int i = 0; i < MAX_WD_HGH_PROCESS; i++) {
      displayprocessos[i] = (char*)malloc((MAX_INPUT_STR) * sizeof(char));
  }

  // initialize introduction window
  initscr();	
  curs_set(0);		
  janela_intro();

  // initialize main window and components
  janela_menu = create_newwin(9, 59, 0, 1,"MENU");
  janela_menu = init_menu_components(janela_menu);
  janela_OUTPUT = create_newwin(5, 59, 9, 1," OUTPUT ");
  janela_SCHEDULER = create_newwin(16, 59, 14, 1," SCHEDULER ");
  janela_memory = create_newwin(10, 59, 0, 61," MEMORY ");
  janela_process = create_newwin(10, 59, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(10, 59, 20, 61," I/O ");

  // change window to get user input
  move(6,34);
  curs_set(1);

  // gets the user input, clears the input box and prints the input in another sub-window
  do{
    getstr(input);
    add_input_list(displayprocessos,input,&num);
    print_input(janela_process, displayprocessos,&num);
    clear_input_space(20,6,34);
    move(6,34);
    refresh();
  }while(strcmp(input,"q"));

  // free memory
  free(input);

  // close lncurses window
  endwin();			
  return 0;
}