#include <ncurses.h>
WINDOW *create_newwin(int height, int width, int starty, int startx);

WINDOW *init_windows(WINDOW *janela_menu, WINDOW *janela_I_O, WINDOW *janela_memory, WINDOW *janela_process, WINDOW *janela_PID);

WINDOW *delete_window(WINDOW *local_win);

WINDOW *menu(WINDOW *local_win);

WINDOW *janela_intro();