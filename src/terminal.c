#include "terminal.h"
#include <ncurses.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__)
    #include <sys/ioctl.h>
#endif

#include <unistd.h>
#include <stdlib.h>

void set_term_size(terminal* term)
{
    int new_width, new_height;
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        new_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        new_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    #elif defined(__linux__)
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        new_width = w.ws_col;
        new_height = w.ws_row;
    #endif

    if (new_width == term->width && new_height == term->height) return;
    
    term->width = new_width;
    term->height = new_height;
    term->size = term->width * term->height * sizeof(int);
    
    resizeterm(term->height, term->width);
    
    free(term->colors);
    term->colors = malloc(term->size);
    
    delwin(term->pad);
    term->pad = newpad(term->height, term->width);
}

terminal init_term()
{
    terminal term = {0};
    term.total_colors = term.color_mode?24:17;
    term.pixel_aspect = PIXEL_ASPECT;

    /* init ncurses. */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);

    /* total 24 colors. */
    start_color();
    for (int i = 232; i < COLORS; i++) {
        init_pair(i + 1, COLOR_BLACK, i);
    }
    init_pair(232, COLOR_BLACK, 232);

    set_term_size(&term);

    return term;
}

const char gradient[] = " .:!/r(l1Z4H9W8$@";

void refresh_term(terminal *term)
{
    int color;

    for (int y = 0; y < term->height; y++)
    {
        for (int x = 0; x < term->width; x++)
        {
            color = term->colors[y * (int)term->width + x];
            
            if (term->color_mode) mvwaddch(term->pad, y, x, ' ' | COLOR_PAIR(232 + color));
            else mvwaddch(term->pad, y, x, gradient[color]);
        }
    }
    prefresh(term->pad, 0, 0, 0, 0, term->height-1, term->width-1);
}