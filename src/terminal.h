#ifndef TERMINAL_H
#define TERMINAL_H

#include <ncurses.h>

#define PIXEL_ASPECT 8.f / 12.f

typedef struct
{
    WINDOW*  pad;
    int      *colors;
    int      color_mode;
    int      total_colors;
    float    pixel_aspect;
    int      size;
    int      width;
    int      height;
}
terminal;

void set_term_size(terminal* term);
terminal init_term();
void refresh_term(terminal *term);

#endif