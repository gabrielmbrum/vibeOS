#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <curses.h>
#include <stdarg.h>

//! SOME constatns were depriciated - fix later
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
#define PROGRAMS "../programs/"

// Including external windows for referencing 
extern WINDOW  *janela_menu;
extern WINDOW  *janela_OUTPUT;
extern WINDOW  *janela_SCHEDULER;
extern WINDOW  *janela_memory;
extern WINDOW  *janela_process;
extern WINDOW  *janela_I_O;
extern WINDOW  *janela_PRINT;

// input operations
void clear_space(int y, int x,int size);
char* get_input(char *input);
int check_input(char *input);
void print_win_args(WINDOW *local_window, char*message, ...);
void init_interface();
WINDOW *create_newwin(int height, int width, int starty, int startx, char *title);
WINDOW *delete_window(WINDOW *local_win);
WINDOW *init_menu_components(WINDOW *menu);
WINDOW *janela_intro();
WINDOW *close_window();
void *update_main_window() ;
char *update_dados(WINDOW *local, char *message, ...);
void check_responsivity(int men, int lin, char*input);
void shutdown_interface();
WINDOW *janela_exit();
void clear_main_windows();
void init_all_pair_colors();
//bool is_colorful_terminal();
void init_win_color(WINDOW *local);
void kill_win_color(WINDOW *local);

#endif