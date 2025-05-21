#include "../include/interface.h"

// *         _  _             ____    _____ *
// *        (_)| |           / __ \  / ____|*
// * __   __ _ | |__    ___ | |  | || (___  *
// * \ \ / /| || '_ \  / _ \| |  | | \___ \ *
// *  \ V / | || |_) ||  __/| |__| | ____) |*
// *   \_/  |_||_.__/  \___| \____/ |_____/ *
// * feel the vibe
// *para pegar o tamanho da tela atual -> getmaxyx(stdscr, linhas, colunas);

WINDOW  *janela_menu;
WINDOW  *janela_OUTPUT;
WINDOW  *janela_SCHEDULER;
WINDOW  *janela_memory;
WINDOW  *janela_process;
WINDOW  *janela_I_O;

// support functions
void clear_space(int y, int x,int size){
  for (int i = 0; i < size; i++) {
    mvaddch(y, x + i, ' ');
  }
}

/*
! DEPRECIATED
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
*/

// input and output message operations
/*
! DEPRECIATED
void add_element_list(char **list, char *input, int *num, int Vbound, int Hbound){
  int width = strlen(input);

  if (width < Hbound) {
    if (*num < Vbound) {
      strcpy(list[*num], input);
      (*num)++;
    } else {
      for (int i = 0; i < Vbound - 1; i++) {
        strcpy(list[i], list[i + 1]);
      }
      strcpy(list[Vbound - 1], input);
    }
  } else {
    char str1[Hbound];
    char str2[Hbound];

    strncpy(str1, input, Hbound - 1);
    str1[Hbound - 1] = '\0';

    strncpy(str2, input + Hbound - 2, Hbound - 1);
    str2[Hbound - 1] = '\0';

    if (*num <= Vbound - 2) {
      strcpy(list[*num], str1);
      (*num)++;
      strcpy(list[*num], str2);
      (*num)++;
    } else {

      int shifts = (*num >= Vbound) ? 2 : 1;
      for (int i = 0; i < Vbound - shifts; i++) {
        strcpy(list[i], list[i + shifts]);
      }
      if (Vbound >= 2) {
        strcpy(list[Vbound - 2], str1);
        strcpy(list[Vbound - 1], str2);
      }
      *num = Vbound;
    }
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
*/

void print_win(WINDOW *local_window, char *input){
  if(strlen(input)>DEF_WIN_WDH){
    wmove(local_window,1,0);
    winsertln(local_window);
    wmove(local_window,2,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s",input);
    wrefresh(local_window);
  }else{
    wmove(local_window,1,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s", input);
    wrefresh(local_window);
  }
}

int check_input(char *input){
  for (int i = 1; i < SYNT; i++) {
    char base[50];
    sprintf(base, "synt%d", i);

    if (strcmp(input, base) == 0) {
        return 1; 
    }
}
return 0; 
}

char* get_input(char *input,WINDOW*out){
  getstr(input);
  if(strlen(input)>MAX_INPUT_STR || check_input(input) == 0){
   print_win(out,"Invalid input. Try again");
   refresh();
     return NULL;
  }else{
   print_win(out,"Valid Input");
   refresh();
     return input;
  }
}

void print_win_args(WINDOW *local_window, char*message, ...) {
  char buffer[MAX_OUTPUT_STR] = " ";
  va_list args;

  va_start(args, message); // initialize args after 'message'
  vsnprintf(buffer, MAX_OUTPUT_STR, message, args);
  
  if(strlen(buffer)>DEF_WIN_WDH){
    wmove(local_window,1,0);
    winsertln(local_window);
    wmove(local_window,2,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s",buffer);
    wrefresh(local_window);
  }else{
    wmove(local_window,1,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s",buffer);
    wrefresh(local_window);
  }
}

// window operations
void init_interface(){
    // Draw main window, sub-windows and components
    janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1," MENU ");
    janela_menu = init_menu_components(janela_menu);
    janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
    janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
    janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
    janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
    janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");
  
    // change window to get user input
    move(6,34);
    curs_set(1);
}

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
  initscr();	
  getmaxyx(stdscr, lin, col);
  curs_set(0);
  WINDOW *intro = newwin(8, 60, ((lin/2) - 5), ((col/2) - 30));
  wrefresh(intro);
  refresh();

  wborder (intro, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  mvwprintw(intro, 0, 3, "%s", "         _  _             ____    _____           _/)");
  mvwprintw(intro, 1, 3, "%s", "        (_)| |           / __ \\  / ____|       .-(_(=:");
  mvwprintw(intro, 2, 3, "%s", " __   __ _ | |__    ___ | |  | || (___     |\\ |    \\)");
  mvwprintw(intro, 3, 3, "%s", " \\ \\ / /| || '_ \\  / _ \\| |  | | \\___ \\    \\ ||");
  mvwprintw(intro, 4, 3, "%s", "  \\ V / | || |_) ||  __/| |__| | ____) |    \\||");
  mvwprintw(intro, 5, 3, "%s", "   \\_/  |_||_.__/  \\___| \\____/ |_____/      \\| ");
  mvwprintw(intro, 6, 7, "%s", "                                          |");
  mvwprintw(intro, 7, 7, "%s", "   Aperte qualquer tecla para iniciar     |");
  wrefresh(intro);

  int c = getch();
  werase(intro);
  wrefresh(intro);
  delete_window(intro);

  return intro;
}

WINDOW *init_menu_components(WINDOW *menu){
  mvwprintw(menu, 1, 15, "%s", " ___   _____       ____  ____ ");
  mvwprintw(menu, 2, 15, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
  mvwprintw(menu, 3, 15, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
  mvwprintw(menu, 4, 15, "%s", " |___/_/_.__/\\__/\\____/___/ ");
  mvwprintw(menu, 6, 2, "%s", "Insira a versão Syst desejada:");
  mvwprintw(menu, 7, 2, "%s", "Pressione q para sair");
  wrefresh(menu);

  return menu;
}
/*  
!DEPRECIATED
WINDOW *init_interface(){
  // malloc the screen windows, display arrays and sizes
  WINDOW *janela_menu, *janela_OUTPUT, *janela_I_O, *janela_memory, *local_window, *janela_SCHEDULER;

   // Draw main window, sub-windows and components
  janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1," MENU ");
  janela_menu = init_menu_components(janela_menu);
  janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
  janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
  janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
  local_window = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");

  // change window to get user input
  move(6,34);
  curs_set(1); 
  //! input loop: this code DOES NOT GO HERE 
   while(strcmp(input,"q")){
    getstr(input);
    if(strlen(input)>MAX_INPUT_STR || check_input(input) == 0){
        print_message(janela_OUTPUT,"Entrada inválida");
    }else{
      // handle the correct user input 
      add_element_list(displayProcessos,input,&sizes[3],(DEF_WIN_HGH_BIG-2));
      mvwprintw(janela_I_O, POS_Y, POS_X, "%d",sizes[3]);
      wrefresh(janela_I_O);
      print_multiple_messages(local_window, displayProcessos,&sizes[3]); 

    }
    clear_space(6,34,strlen(input));
    move(6,34);
    refresh();
  }

  // free memory
  free(input);

  // close lncurses window
  endwin();			 
}
*/

WINDOW *close_window(){
/*      // free memory
     free(input); */
     endwin();

     return 0;
}
