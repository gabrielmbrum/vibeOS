#include "../include/interface.h"
#include <string.h>
#include "../include/kernel.h"

WINDOW  *janela_menu;
WINDOW  *janela_OUTPUT;
WINDOW  *janela_SCHEDULER;
WINDOW  *janela_memory;
WINDOW  *janela_PRINT;
WINDOW  *janela_process;
WINDOW  *janela_I_O;
WINDOW *selected_window;

char dados[MAX_OUTPUT_STR];

int PAR = 1;
//init_color(COLOR_GREEN, 100, 700, 100);
//init_pair(PAR, COLOR_GREEN, COLOR_BLACK);

// support functions
void clear_space(int y, int x,int size){
  for (int i = 0; i < size; i++) {
    mvaddch(y, x + i, ' ');
  }
}

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
   update_dados(out,0,"Invalid input. Try again");
   refresh();
     return NULL;
  }else{
   update_dados(out, 0,"Valid Input");
   refresh();
     return input;
  }
}

// window operations
int lin, col;
void init_interface(){
    janela_intro();
    // Draw main window, sub-windows and components
    getmaxyx(stdscr, lin, col);
    int largura = col/2;
    int altura = lin/4.5;
    col = lin/3.1;
    janela_menu = create_newwin(col, largura, 0, 0," MENU ");
    janela_menu = init_menu_components(janela_menu);
    janela_OUTPUT = create_newwin(altura, largura, col, 0," OUTPUT ");
    janela_SCHEDULER = create_newwin(lin - (col + altura), largura, (altura + col), 0," SCHEDULER ");

    janela_memory = create_newwin(altura, largura, 0, largura," MEMORY ");
    janela_I_O = create_newwin(altura, largura, altura, largura," I/O ");
    janela_PRINT = create_newwin(altura, largura, (altura*2), largura, " PRINTER ");
    janela_process = create_newwin(lin - (altura*3), largura, (altura*3), largura, " PROCESS ");

    pthread_create(&interface_thread, NULL, (void *) update_main_window, NULL);

    // change window to get user input
    int men = col/3.4;
    curs_set(1);
      if(men <= 17){
      move(6, 33);
      }else{
      move((lin/4) + 5, 33);
      }
    echo();
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
  int lin, col, si;
  initscr();	
  if(has_colors() == FALSE){
    si = 1;
  }else{ 
    start_color();
    use_default_colors();
    //init_color(COLOR_GREEN, 69, 255, 133);
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
  }
  getmaxyx(stdscr, lin, col);
  curs_set(0);
  noecho();
  WINDOW *intro = newwin(10, 60, ((lin/2) - 5), ((col/2) - 30));
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
  if (si == 1) mvwprintw(intro, 9, 10, "%s", "Seu terminal não suporta cores T.T");
  wrefresh(intro);

  getch();
  werase(intro);
  wrefresh(intro);
  delete_window(intro);

  return intro;
}

WINDOW *init_menu_components(WINDOW *menu){
  int linha, coluna;
  getmaxyx(menu, linha, coluna);
  int men = coluna/3.4;

  if(men <= 17){
    mvwprintw(menu, 1, men, "%s", " ___   _____       ____  ____ ");
    mvwprintw(menu, 2, men, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
    mvwprintw(menu, 3, men, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
    mvwprintw(menu, 4, men, "%s", " |___/_/_.__/\\__/\\____/___/ ");
    mvwprintw(menu, 6, 2, "%s", "Insira a versão Syst desejada:");
    mvwprintw(menu, 7, 2, "%s", "Pressione q para sair");
    wrefresh(menu);
    move(6,33);
    curs_set(1);
  }else{
    men = coluna/3;
    int alt = linha/4;
    mvwprintw(menu, alt, men, "%s", " ___   _____       ____  ____ ");
    mvwprintw(menu, alt+1, men, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
    mvwprintw(menu, alt+2, men, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
    mvwprintw(menu, alt+3, men, "%s", " |___/_/_.__/\\__/\\____/___/ ");
    mvwprintw(menu, alt+5, 2, "%s", "Insira a versão Syst desejada:");
    mvwprintw(menu, alt+6, 2, "%s", "Pressione q para sair");
    wrefresh(menu);
    move(alt+5,33);
    curs_set(1);
  }
  return menu;
}

//testing
char *update_dados(WINDOW *local, int par, char *message, ...){
  va_list args;
  pthread_mutex_lock(&dados_mutex);  
    wattrset(local, COLOR_PAIR(par)); 
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

void check_responsivity(int men, int lin, char *input){
    if(men <= 17){
    clear_space(6, 33, strlen(input));
    move(6, 33);
    }else{
    clear_space((lin/4) + 5, 33, strlen(input));
    move((lin/4) + 5, 33);
    }
}
