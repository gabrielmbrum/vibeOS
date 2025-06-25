#include "../include/interface.h"
#include <string.h>
#include "../include/kernel.h"

WINDOW  *janela_menu;
WINDOW  *janela_OUTPUT;
WINDOW  *janela_SCHEDULER;
WINDOW  *janela_memory;
WINDOW  *janela_process;
WINDOW  *janela_I_O;
WINDOW *selected_window;

char dados[MAX_OUTPUT_STR];

// support functions
void clear_space(int y, int x,int size){
  for (int i = 0; i < size; i++) {
    mvaddch(y, x + i, ' ');
  }
}

void print_win(WINDOW *local_window, char *input){
    wmove(local_window,1,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s", input);
    wrefresh(local_window);
} //! DEPRICIATED

void print_win_args(WINDOW *local_window, char*message, ...) {
  char buffer[MAX_OUTPUT_STR] = " ";
  va_list args;

  va_start(args, message); // initialize args after 'message'
  vsnprintf(buffer, MAX_OUTPUT_STR, message, args);
    wmove(local_window,1,0);
    winsertln(local_window);
    mvwprintw(local_window,1,1,"%s",buffer);
    wrefresh(local_window);
}

int check_input(char *input){
  for (int i = 1; i < SYNT; i++) {
    char base[50];
    sprintf(base, "sint%d", i);

    if (strcmp(input, base) == 0) {
        return 1; 
    }
}
return 0; 
}

char* get_input(char *input,WINDOW*out){
  getstr(input);
  if(strlen(input)>MAX_INPUT_STR || check_input(input) == 0){
   update_dados(out,"Invalid input. Try again");
   refresh();
     return NULL;
  }else{
   update_dados(out,"Valid Input");
   refresh();
     return input;
  }
}

// window operations
void init_interface(){
    janela_intro();

    // Draw main window, sub-windows and components
    janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1," MENU ");
    janela_menu = init_menu_components(janela_menu);
    janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1," OUTPUT ");
    janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1," SCHEDULER ");
    janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61," MEMORY ");
    janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
    janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61," I/O ");
  
    pthread_create(&interface_thread, NULL, (void *) update_main_window, NULL);


    // change window to get user input
    curs_set(1);
    echo();
    move(6, 34);
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
  noecho();
  WINDOW *intro = newwin(8, 60, ((lin/2) - 5), ((col/2) - 30));
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

  getch();
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

//testing
char *update_dados(WINDOW *local, char *message, ...){
  va_list args;
  pthread_mutex_lock(&dados_mutex);   
    va_start(args, message);
    vsnprintf(dados, MAX_OUTPUT_STR, message, args);
    va_end(args);
    selected_window = local;
  pthread_mutex_unlock(&interface_mutex);
    return dados;
}

void *update_main_window() {
  while (TRUE) {
    pthread_mutex_lock(&interface_mutex);     
    usleep(200); 
    print_win_args(selected_window,dados);
    pthread_mutex_unlock(&dados_mutex);    
  }
}
