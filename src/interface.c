#include "../include/interface.h"
#include "../include/semaphore.h"
#include "../include/commons.h"
#include "../include/kernel.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>

// janelas
WINDOW *janela_menu;
WINDOW *janela_OUTPUT;
WINDOW *janela_SCHEDULER;
WINDOW *janela_memory;
WINDOW *janela_process;
WINDOW *janela_I_O;
WINDOW *selected_window;

//semaforos
Semaphore *sem_dados;
Semaphore *sem_update_interface;
Process *p_interface;


// vars
char dados[MAX_OUTPUT_STR];
char dados_scheduler[MAX_OUTPUT_STR];

//initialize menu components
WINDOW *init_menu_components(WINDOW *menu) {
    mvwprintw(menu, 1, 15, " ___   _____       ____  ____ ");
    mvwprintw(menu, 2, 15, " | | / (_) /  ___ / __ \\/ __/ ");
    mvwprintw(menu, 3, 15, " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
    mvwprintw(menu, 4, 15, " |___/_/_.__/\\__/\\____/___/ ");
    mvwprintw(menu, 6, 2, "Insira a versão Syst desejada:");
    mvwprintw(menu, 7, 2, "Pressione q para sair");
    wrefresh(menu);
    return menu;
}

// janela de introdução
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

// Criação de janelas
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title) {
    WINDOW *local_win = newwin(height - 1, width - 2, starty, startx + 1);
    WINDOW *border_win = newwin(height, width, starty, startx);
    refresh();
    box(border_win, 0, 0);
    mvwprintw(border_win, 0, 2, "%s", title);
    wrefresh(local_win);
    wrefresh(border_win);
    return local_win;
}

// Remoção de janela
WINDOW *delete_window(WINDOW *local_win) {
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
    return NULL;
}

void init_interface() {
  pthread_t interface_thread;  
  pthread_t interface_thread_scheduler; 
   p_interface= processCreate(0,name3,1);

  janela_intro();  
  
  // Desenho de janelas
  janela_menu = create_newwin(DEF_WIN_HGH_MEDIUM, DEF_WIN_WDH, 0, 1, " MENU ");
  janela_menu = init_menu_components(janela_menu);
  janela_OUTPUT = create_newwin(DEF_WIN_HGH_SMALL, DEF_WIN_WDH, 9, 1, " OUTPUT ");
  janela_SCHEDULER = create_newwin(DEF_WIN_HGH_BIGGER, DEF_WIN_WDH, 14, 1, " SCHEDULER ");
  janela_memory = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 0, 61, " MEMORY ");
  janela_process = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 10, 61, " PROCESS ");
  janela_I_O = create_newwin(DEF_WIN_HGH_BIG, DEF_WIN_WDH, 20, 61, " I/O ");
  
  pthread_create(&interface_thread, NULL, (void *) update_main_window, NULL);
  
  // initializing semaphores
  sem_dados = create_semaphore_define_value(name1,1);
  sem_update_interface = create_semaphore_define_value(name2,0);

  //initializing vars
  selected_window = janela_OUTPUT;

  if(sem_dados != NULL && sem_update_interface != NULL){
    init_semaphores();
    update_dados(janela_OUTPUT,"Semaforos initializados com sucesso");
  } 
 
  move(6, 34);
  curs_set(1);
}

// Limpa uma linha de texto na tela
void clear_space(int y, int x, int size) {
    for (int i = 0; i < size; i++) {
        mvaddch(y, x + i, ' ');
    }
}

// Valida entrada do usuário
int check_input(char *input) {
    for (int i = 1; i < SYNT; i++) {
        char base[50];
        sprintf(base, "synt%d", i);
        if (strcmp(input, base) == 0) {
            return 1;
        }
    }
    return 0;
}

// Coleta e valida input do usuário
char *get_input(char *input) {
    getstr(input);
    if (strlen(input) > MAX_INPUT_STR || check_input(input) == 0) {
        print_win_args(janela_OUTPUT, "Invalid input. Try again");
        return NULL;
    } else {
        print_win_args(janela_OUTPUT, "Valid Input");
        return input;
    }
}

// Printa com argumentos formatados
void print_win_args(WINDOW *local_window, char *message, ...) {
    /* char *buffer = malloc(MAX_OUTPUT_STR * sizeof(char));
    va_list args;
    va_start(args, message);
    vsnprintf(buffer, MAX_OUTPUT_STR, message, args);
    va_end(args); */

    wmove(local_window, 1, 0);
    winsertln(local_window);
    mvwprintw(local_window, 1, 1, "%s", message);
    wrefresh(local_window);
    move(6, 34);
    //free(buffer);
}

//testing
char *update_dados(WINDOW *local, char *message, ...){
  va_list args;
  pthread_mutex_lock(&sem_dados->mutex);
  //sem_P(p_interface,sem_dados->name);
  va_start(args, message);
  vsnprintf(dados, MAX_OUTPUT_STR, message, args);
  va_end(args);
  selected_window = local;
  //sem_V(p_interface,sem_update_interface->name);
  //pthread_mutex_unlock(&sem->mutex);
  pthread_mutex_unlock(&sem_update_interface->mutex);
    return dados;
}

void *update_main_window() {
    while (TRUE) {
      //pthread_mutex_lock(&sem_dados->mutex);
      pthread_mutex_lock(&sem_update_interface->mutex);
      //sem_P(p_interface,sem_update_interface->name);
      usleep(200); 

      print_win_args(selected_window,dados);

      //sem_V(p_interface,sem_dados->name);
      pthread_mutex_unlock(&sem_dados->mutex);
    }
}

void refresh_scheduler_window() {

    update_dados(janela_SCHEDULER,kernel->BCP[0].pid);
}




