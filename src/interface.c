#include "../include/interface.h"
#include "../include/kernel.h"
#include <ncurses.h>
#include <string.h>

WINDOW *janela_menu;
WINDOW *janela_OUTPUT;
WINDOW *janela_SCHEDULER;
WINDOW *janela_memory;
WINDOW *janela_PRINT;
WINDOW *janela_process;
WINDOW *janela_I_O;
WINDOW *selected_window;

char dados[MAX_OUTPUT_STR];

bool colorful_terminal = false;

// support functions
void clear_space(int y, int x, int size) {
  for (int i = 0; i < size; i++) {
    mvaddch(y, x + i, ' ');
  }
}

void print_win_args(WINDOW *local_window, char *message, ...) {
  char buffer[MAX_OUTPUT_STR] = " ";
  va_list args;

  va_start(args, message); // initialize args after 'message'
  vsnprintf(buffer, MAX_OUTPUT_STR, message, args);
  wmove(local_window, 1, 0);
  winsertln(local_window);
  mvwprintw(local_window, 1, 1, "%s", buffer);
  wrefresh(local_window);
}

int check_input(char *input) {
  for (int i = 1; i < SYNT; i++) {
    char base[50];
    sprintf(base, "sint%d", i);

    if (strcmp(input, base) == 0) {
      return 1;
    }
  }
  return 0;
}

char *get_input(char *input) {
  getstr(input);

  if (strlen(input) > MAX_INPUT_STR || check_input(input) == 0) {
    update_dados(janela_OUTPUT, "Invalid input. Try again.");
    refresh();
    return NULL;
  } else {
    update_dados(janela_OUTPUT, "Valid Input.");
    refresh();
    return input;
  }
}

// window operations
int lin, col;
void init_interface() {
  janela_intro();
  // Draw main window, sub-windows and components
  getmaxyx(stdscr, lin, col);
  int largura = col / 2;
  int altura = lin / 4.5;
  col = lin / 3.1;
  janela_menu = create_newwin(col, largura, 0, 0, " MENU ");
  janela_menu = init_menu_components(janela_menu);
  janela_OUTPUT = create_newwin(altura, largura, col, 0, " OUTPUT ");
  janela_process = create_newwin(lin - (col + altura), largura, (altura + col),
  0, " PROCESS ");

  janela_memory = create_newwin(altura, largura, 0, largura, " MEMORY ");
  janela_I_O = create_newwin(altura, largura, altura, largura, " I/O ");
  janela_PRINT =
  create_newwin(altura, largura, (altura * 2), largura, " PRINTER ");
  janela_SCHEDULER = create_newwin(lin - (altura * 3), largura, (altura * 3),
  largura, " SCHEDULER ");

  pthread_create(&interface_thread, NULL, (void *)update_main_window, NULL);

  // change window to get user input
  int men = col / 3.4;
  curs_set(1);
  if (men <= 17) {
  move(6, 33);
  } else {
  move((lin / 4) + 5, 33);
  }
  echo();
}

WINDOW *create_newwin(int height, int width, int starty, int startx,
                      char *title) {
  WINDOW *local_win = newwin(height - 1, width - 2, starty, startx + 1);
  WINDOW *border_win = newwin(height, width, starty, startx);
  refresh();

  box(border_win, 0, 0);
  mvwprintw(border_win, 0, 2, "%s", title);
  wrefresh(local_win);
  wrefresh(border_win);

  return local_win;
}

WINDOW *delete_window(WINDOW *local_win) {
  wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(local_win);
  delwin(local_win);

  return 0;
}

WINDOW *janela_intro() {
  int lin, col;
  initscr();
  if (has_colors()) {
    colorful_terminal = true;
    init_all_pair_colors();
  }

  getmaxyx(stdscr, lin, col);
  curs_set(0);
  noecho();
  WINDOW *intro = newwin(10, 60, ((lin / 2) - 5), ((col / 2) - 30));
  refresh();

  wborder(intro, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

  // "vibeOS" em verde, se suportar cores
  if (colorful_terminal) wattron(intro, COLOR_PAIR(1));
  mvwprintw(intro, 0, 3, "%s", "         _  _             ____    _____           _/)");
  mvwprintw(intro, 1, 3, "%s", "        (_)| |           / __ \\  / ____|       .-(_(=:");
  mvwprintw(intro, 2, 3, "%s", " __   __ _ | |__    ___ | |  | || (___     |\\ |    \\)");
  mvwprintw(intro, 3, 3, "%s", " \\ \\ / /| || '_ \\  / _ \\| |  | | \\___ \\    \\ ||");
  mvwprintw(intro, 4, 3, "%s", "  \\ V / | || |_) ||  __/| |__| | ____) |    \\||");
  mvwprintw(intro, 5, 3, "%s", "   \\_/  |_||_.__/  \\___| \\____/ |_____/      \\| ");
  mvwprintw(intro, 6, 7, "%s", "                                          |");
  mvwprintw(intro, 7, 7, "%s", "   Aperte qualquer tecla para iniciar     |");

  if (colorful_terminal) wattroff(intro, COLOR_PAIR(1));
  else mvwprintw(intro, 9, 10, "%s", "Seu terminal não suporta cores T.T");
  
  wrefresh(intro);

  getch();
  werase(intro);
  wrefresh(intro);
  delete_window(intro);

  return intro;
}

WINDOW *init_menu_components(WINDOW *menu) {
  int linha, coluna;
  getmaxyx(menu, linha, coluna);
  int men = coluna / 3.4;

  if (men <= 17) {
    init_win_color(menu);
    mvwprintw(menu, 1, men, "%s", " ___   __ __       ____  ____ ");
    mvwprintw(menu, 2, men, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
    mvwprintw(menu, 3, men, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
    mvwprintw(menu, 4, men, "%s", " |___/_/_.__/\\__/\\____/___/ ");

    kill_win_color(menu);

    mvwprintw(menu, 6, 2, "%s", "Insira a versão sint desejada:");
    mvwprintw(menu, 7, 2, "%s", "Ex. de programa: sint1 | Pressione q para sair");

    wrefresh(menu);
    move(6, 33);
    curs_set(1);
  } else {
    men = coluna / 3;
    int alt = linha / 4;
    
    init_win_color(menu);

    mvwprintw(menu, alt, men, "%s",     " ___  __  __       ____   ____ ");
    mvwprintw(menu, alt + 1, men, "%s", " | | / (_) /  ___ / __ \\/ __/ ");
    mvwprintw(menu, alt + 2, men, "%s", " | |/ / / _ \\/ -_) /_/ /\\ \\   ");
    mvwprintw(menu, alt + 3, men, "%s", " |___/_/_.__/\\__/\\____/___/ ");

    kill_win_color(menu);

    mvwprintw(menu, alt + 5, 2, "%s", "Insira a versão sint desejada:");
    mvwprintw(menu, alt + 6, 2, "%s", "Ex. de programa: sint1 | Pressione q para sair");

    wrefresh(menu);
    move(alt + 5, 33);
    curs_set(1);
  }
  return menu;
}

// testing
char *update_dados(WINDOW *local, char *message, ...) {
  init_win_color(local);
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
    print_win_args(selected_window, dados);
    pthread_mutex_unlock(&dados_mutex);
  }
}

void check_responsivity(int men, int lin, char *input) {
  if (men <= 17) {
    clear_space(6, 33, strlen(input));
    move(6, 33);
  } else {
    clear_space((lin / 4) + 5, 33, strlen(input));
    move((lin / 4) + 5, 33);
  }
}

WINDOW *janela_exit() {
  initscr();
  int lin, col;
  if (colorful_terminal) {
    init_pair(2, COLOR_RED, -1); // Par de cor: vermelho, fundo padrão
  }
  getmaxyx(stdscr, lin, col);
  curs_set(0);
  noecho();
  WINDOW *exit = newwin(10, 100, ((lin / 2) - 5), ((col / 2) - 5) - 40);
  refresh();

  wborder(exit, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

  if (colorful_terminal) {
    wattron(exit, COLOR_PAIR(2));
  }
  mvwprintw(exit, 0, 3, "%s", " __   _____  _   _   _  _____ _    _    ___ ___    _____ _  _ ___  __   _____ ___ ___ ");
  mvwprintw(exit, 1, 3, "%s", " \\ \\ / / _ \\| | | | | |/ /_ _| |  | |  | __|   \\  |_   _| || | __| \\ \\ / /_ _| _ ) __|");
  mvwprintw(exit, 2, 3, "%s", "  \\ V / (_) | |_| | | ' < | || |__| |__| _|| |) |   | | | __ | _|   \\ V / | || _ \\ _| ");
  mvwprintw(exit, 3, 3, "%s", "   |_| \\___/ \\___/  |_|\\_\\___|____|____|___|___/    |_| |_||_|___|   \\_/ |___|___/___|");
  mvwprintw(exit, 4, 3, "%s", "                                                                                      ");
  mvwprintw(exit, 7, 7, "%s", "                         Aperte qualquer tecla para sair                              ");

  if (colorful_terminal) {
    wattroff(exit, COLOR_PAIR(2));
  }
  wrefresh(exit);

  getch();
  werase(exit);
  wrefresh(exit);
  delete_window(exit);

  return exit;
}

void shutdown_interface() {
  usleep(200);
  clear_main_windows();
  janela_exit();
  clear();
  refresh();
  endwin();
}

void clear_main_windows() {
  clear();
  refresh();
  wclear(janela_I_O);
  wclear(janela_memory);
  wclear(janela_menu);
  wclear(janela_OUTPUT);
  wclear(janela_PRINT);
  wclear(janela_process);
  wclear(janela_SCHEDULER);
  wrefresh(janela_I_O);
  wrefresh(janela_memory);
  wrefresh(janela_menu);
  wrefresh(janela_OUTPUT);
  wrefresh(janela_PRINT);
  wrefresh(janela_process);
  wrefresh(janela_SCHEDULER);
  endwin();
}

void init_all_pair_colors() {
  start_color();
  use_default_colors();
  init_pair(1, COLOR_GREEN, -1);
  init_pair(2, COLOR_RED, -1);
  init_pair(3, COLOR_BLACK, -1);
  init_pair(4, COLOR_BLUE, -1);
  init_pair(5, COLOR_CYAN, -1);
  init_pair(6, COLOR_MAGENTA, -1);
  init_pair(7, COLOR_YELLOW, -1);
}

// bool is_colorful_terminal() {
//   return has_colors();
// }

void init_win_color(WINDOW* local) {
  if (colorful_terminal) {
    if (local == janela_menu) wattron(janela_menu, COLOR_PAIR(1));
    if (local == janela_memory) wattron(janela_memory, COLOR_PAIR(5));
    if (local == janela_OUTPUT) wattron(janela_OUTPUT, COLOR_PAIR(4));
    if (local == janela_SCHEDULER) wattron(janela_SCHEDULER, COLOR_PAIR(4));
    if (local == janela_process) wattron(janela_process, COLOR_PAIR(5));
    if (local == janela_I_O) wattron(janela_I_O, COLOR_PAIR(7));
    if (local == janela_PRINT) wattron(janela_PRINT, COLOR_PAIR(6));
  }
}

void kill_win_color(WINDOW* local) {
  if (colorful_terminal) {
    if (local == janela_menu) wattroff(janela_menu, COLOR_PAIR(1));
    if (local == janela_memory) wattroff(janela_memory, COLOR_PAIR(5));
    if (local == janela_OUTPUT) wattroff(janela_OUTPUT, COLOR_PAIR(4));
    if (local == janela_SCHEDULER) wattroff(janela_SCHEDULER, COLOR_PAIR(5));
    if (local == janela_process) wattroff(janela_process, COLOR_PAIR(6));
    if (local == janela_I_O) wattroff(janela_I_O, COLOR_PAIR(7));
    if (local == janela_PRINT) wattroff(janela_PRINT, COLOR_PAIR(5));
  }
}