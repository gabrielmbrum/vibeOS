#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>

#define MIN_WD_HGH 1
#define MAX_WD_HGH_PROCESS 8
#define MAX_INPUT_STR 25
#define MAX_OUTPUT_STR 100
#define POS_X 1

WINDOW *create_newwin(int height, int width, int starty, int startx, char* title);

//WINDOW *init_windows(WINDOW *janela_menu, WINDOW *janela_I_O, WINDOW *janela_memory, WINDOW *janela_process, WINDOW *janela_PID);

WINDOW *delete_window(WINDOW *local_win);

WINDOW *menu(WINDOW *local_win);

WINDOW *janela_intro();

void clear_input_space(int size, int start, int end);

void print_input(WINDOW *local_win, char **list, int *size);

void add_input_list(char **list, char *input, int *num);