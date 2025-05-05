#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#define MAX_INPUT_STR 25
#define MAX_OUTPUT_STR 120
#define NUMBER_OF_WINDOWS 5
#define POS_X 1
#define POS_Y 1
#define DEF_WIN_WDH 59
#define DEF_WIN_HGH_SMALL 5
#define DEF_WIN_HGH_MEDIUM 9
#define DEF_WIN_HGH_BIG 10
#define DEF_WIN_HGH_BIGGER 16
#define DEF_WIN_MAX_PRINTS_SMALL 3
#define DEF_WIN_MAX_PRINTS_BIG 8
#define DEF_WIN_MAX_PRINTS_BIGGER 14
#define SYNT 11

//! UPDATE THE functions 

// support functions
void clear_space(int y, int x,int size);

char *generate_blank_space(int size);

char **init_string_array(char**list,int bound, int stringSize);

int *init_int_array(int *list, int size);

//input operations
char*  get_input(char *input, WINDOW *out, char **output, int *sizes);

void add_element_list(char **list, char *input, int *num, int Vbound, int Hbound);

void print_message(WINDOW *local_window, char *message);

void print_multiple_messages(WINDOW *local_window, char **list, int *size);

int check_input(char *input);

// window operations
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title);

WINDOW *delete_window(WINDOW *local_win);

WINDOW *menu(WINDOW *local_win);

WINDOW *init_menu_components(WINDOW *menu);

WINDOW *janela_intro();

WINDOW *init_interface();

WINDOW *close_window();

