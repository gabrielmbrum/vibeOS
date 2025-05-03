#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#define MAX_INPUT_STR 25
#define MAX_OUTPUT_STR 100
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
#define SYNT 15

//! UPDATE THE functions 

// support functions
void clear_space(int y, int x,int size);

char *generate_blank_space(int size);

// window operations
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title);

WINDOW *delete_window(WINDOW *local_win);

WINDOW *menu(WINDOW *local_win);

WINDOW *janela_intro();