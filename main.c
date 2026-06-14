#include "src/opencl.h"
#include "src/control.h"
#include <ncurses.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    /* options. */
    bool truecolor = false;
    int ref_rate = 60;
    
    for (int opt; (opt = getopt(argc, argv, "htr:")) != -1;)
    {
        if (opt == 'h') {
            printf("use: sudo ./raymarching <flags>\n"
                "-h - help message\n"
                "-t - enable truecolor mod (default disable)\n"
                "-r <int> - set refresh rate (default 60)");
            return 0;
        }
        if (opt == 't') {
            truecolor = true;
        }
        if (opt == 'r') {
            truecolor = true;
            ref_rate = atoi(optarg);
        }
        if (opt == '?') {
            /* invalid option. */
            return 1;
        }
    }

    /* init ncurses. */
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
    MEVENT event;
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    /* init colors for color mode output. */
    start_color();

    /* truecolor. */
    if (truecolor)
    {
    	for (int i = 0; i < COLORS; i++) {
            int level = i * 1000 / (COLORS - 1);
            init_color(i, level, level, level);
            init_pair(i + 1, COLOR_BLACK, i);
    	}
    }

    /* 256 colors (default). */
    else
    {
    	for (int i = 232; i < COLORS; i++) {
    	    init_pair(i + 1, COLOR_BLACK, i);
    	}
        init_pair(232, COLOR_BLACK, 232);
    }

    OpenCLEnv      env;
    struct         winsize w;
    float          width, height;
    float          aspect;
    const float    pixel_aspect = 11.f / 23.f;
    const int      gradient_size = (truecolor?256:24);
    cl_float3      pos = {-2.f, 0.f, 0.f};
    cl_float3      rotation = {0.f, 0.f, 0.f};
    int            *colors;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col; height = w.ws_row;
    
    aspect = (float)width / height;

    colors = malloc(width * height * sizeof(int));

    env = init_opencl
    (
        "kernel.cl",
        width,
        height,
        aspect,
        pixel_aspect,
        gradient_size,
        pos,
        rotation,
        colors
    );

    size_t  global_size[2];
    WINDOW  *pad;
    int     mice_fd;

    global_size[0] = (size_t)width;
    global_size[1] = (size_t)height;

    pad = newpad(height, width);

    mice_fd = open("/dev/input/mice", O_RDONLY | O_NONBLOCK);

    /* type ctrl-c to exit. */
    for(;;)
    {
        /* update modified args. */
        clSetKernelArg(env.render_kernel, 5, sizeof(cl_float3), &pos);
        clSetKernelArg(env.render_kernel, 6, sizeof(cl_float3), &rotation);
        
        /* get the colors buffer. */
        clEnqueueNDRangeKernel(env.queue, env.render_kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);
        clEnqueueReadBuffer(env.queue, env.colors_buf, CL_TRUE, 0, width * height * sizeof(int), colors, 0, NULL, NULL);

        /* rendering main frame. */
        int color;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++)
            {
                color = colors[y * (int)width + x];
                mvwaddch(pad, y, x, ' ' | COLOR_PAIR(color + (truecolor?0:232)));
            }
        }

        /* aim. */
        mvwaddch(pad, height/2, width/2,   '+' | COLOR_PAIR(-1));
        mvwaddch(pad, height/2, width/2-1, ' ' | COLOR_PAIR(-1));
        mvwaddch(pad, height/2, width/2+1, ' ' | COLOR_PAIR(-1));

        prefresh(pad, 0, 0, 0, 0, height-1, width-1);
        usleep((int)(1000000/ref_rate));

        /* camera control. */
        kbd_control(&pos, &rotation);
        mouse_control(mice_fd, &rotation);
    }

    /* cleaning and exit. */
    cleanup(env);
    delwin(pad);
    endwin();
    close(mice_fd);
    free(colors);

    return 0;
}
