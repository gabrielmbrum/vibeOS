#include "../include/interface.h"

// *         _  _             ____    _____ *
// *        (_)| |           / __ \  / ____|*
// * __   __ _ | |__    ___ | |  | || (___  *
// * \ \ / /| || '_ \  / _ \| |  | | \___ \ *
// *  \ V / | || |_) ||  __/| |__| | ____) |*
// *   \_/  |_||_.__/  \___| \____/ |_____/ *
// * feel the vibe
// *para pegar o tamanho da tela atual -> getmaxyx(stdscr, linhas, colunas);

// support functions
void clear_space(int y, int x,int size){
  for (int i = 0; i < size; i++) {
    mvaddch(y, x + i, ' ');
  }
}

char *generate_blank_space(int size){
  if (size <= 0) return NULL;
  char *str = malloc(size);
  for (int i = 0; i < size; i++) {
      str[i] = ' ';
  }
  return str;
}

char **init_string_array(char**list,int bound, int stringSize){
  list = (char**)malloc(bound * sizeof(char*));
  for (int i = 0; i < bound; i++) {
    list[i] = (char*)malloc((stringSize) * sizeof(char));
    strcpy(list[i],"/0");
  }
  return list;
}

int *init_int_array(int *list, int size){
  list = (int*)malloc(size * sizeof(int));
  for(int i=0;i<size;i++){
    list[i] =0;
  }

  return list;
}


// input and output message operations
void add_input_list(char **list, char *input, int *num, int bound){
  if(*num<bound){
    strcpy(list[*num], input);
    *num = *num+1;
  }else{
    for(int i=0;i<(bound-1);i++){
      strcpy(list[i],list[i+1]);
    }
    strcpy(list[bound-1], input);
  }
}

void print_message(WINDOW *local_window, char *message){
    mvwprintw(local_window, POS_Y, POS_X, "%s",message);
    wrefresh(local_window);
}

void print_multiple_messages(WINDOW *local_window, char **list, int *size) {
  int win_width = getmaxx(local_window);
  int win_heigth = getmaxy(local_window);

  for (int i = 0; i < *size; i++) {
    if (*size >= (win_heigth-1)){
      char *blank = generate_blank_space(win_width - POS_X - 1);
      mvwprintw(local_window, i + 1, POS_X, "%s", blank);
      free(blank);
    }
    mvwprintw(local_window, i + 1, POS_X, "%s", list[i]);
  }

  wrefresh(local_window);  // faz refresh apenas uma vez no final
}

int check_input(char *input){
  for (int i = 0; i < SYNT; i++) {
    char base[50];
    sprintf(base, "synt%d", i);

    if (strcmp(input, base) == 0) {
        return 1; 
    }
}
return 0; 
}

// window operations
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title){
  WINDOW *local_win = newwin(height-1, width-2, starty, startx+1);
  WINDOW *border_win = newwin(height, width, starty, startx);
  refresh();

  box(border_win, 0, 0);
  mvwprintw(border_win, 0, 2, "%s", title);
  wrefresh(local_win);	
  wrefresh(border_win);	

  return local_win;
}

WINDOW *delete_window(WINDOW  *local_win){
  wborder(local_win, ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');

  wrefresh(local_win);
  delwin(local_win);

  return 0;
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

WINDOW *menu(WINDOW *menu){
  mvwprintw(menu, 1, 15, "%s", " ___   _____       ____  ____ ");
  mvwprintw(menu, 2, 15, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
  mvwprintw(menu, 3, 15, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
  mvwprintw(menu, 4, 15, "%s", " |___/_/_.__/\\__/\\____/___/ ");
  mvwprintw(menu, 6, 2, "%s", "Insira a versão Syst desejada:");
  mvwprintw(menu, 7, 2, "%s", "Pressione q para sair");
  wrefresh(menu);

  return menu;
}

int main(){
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *janela_process, *janela_SCHEDULER;

  //! This does not represent the total processes in the Simulator. The arrays contain only the information to be displayed in the screen
  char **displayProcessos = init_string_array(displayProcessos,DEF_WIN_MAX_PRINTS_BIG,MAX_INPUT_STR);
  char **displayScheduler = init_string_array(displayScheduler, DEF_WIN_MAX_PRINTS_BIGGER,MAX_OUTPUT_STR);
  char **displayIO = init_string_array(displayIO, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  char **displayOUT = init_string_array(displayOUT, (DEF_WIN_MAX_PRINTS_SMALL,MAX_OUTPUT_STR),MAX_OUTPUT_STR);
  char **displayMEMO = init_string_array(displayMEMO, DEF_WIN_MAX_PRINTS_BIG,MAX_OUTPUT_STR);
  char *input = malloc((MAX_INPUT_STR)*sizeof(char));
  strcpy(input,"\0");

  // Malloc the array containing the sizes of the dinamic arrays
  int *sizes = init_int_array(sizes,NUMBER_OF_WINDOWS);

  /* 
  ! INDEX
  * 0 -> OUtPUT
  * 1 -> SCHEDULER
  * 2 -> MEMORY
  * 3 -> PROCESS
  * 4 -> I/O
  */

  // initialize introduction window
  initscr();	
  curs_set(0);		
  janela_intro();

  // initialize main window and components
  janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1,"MENU");
  janela_menu = menu(janela_menu);
  janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
  janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
  janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
  janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");

  // change window to get user input
  move(6,34);
  curs_set(1);

  // input loop 
  while(strcmp(input,"q")){
    getstr(input);
    if(strlen(input)>MAX_INPUT_STR || check_input(input) == 0){
      //? This implementation is not perfect for every case (example: strings bigger than the screen), but it mostly works
      print_message(janela_OUTPUT,"Entrada inválida");
    }else{
      // handle the correct user input 
      add_input_list(displayProcessos,input,&sizes[3],(DEF_WIN_HGH_BIG-2));
      mvwprintw(janela_I_O, POS_Y, POS_X, "%d",sizes[3]);
      wrefresh(janela_I_O);
      print_multiple_messages(janela_process, displayProcessos,&sizes[3]); 

    }
    clear_space(6,34,strlen(input));
    move(6,34);
    refresh();
  }

  // free memory
  free(input);

  // close lncurses window
  endwin();			
  return 0;
} 