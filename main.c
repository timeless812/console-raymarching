#include "src/terminal.h"
#include "src/cpu_engine.h"
#include "src/opencl.h"
#include <getopt.h>

int main(int argc, char *argv[])
{
    int color_mode = false;
    int gpu_mode = false;

    for (int opt; (opt = getopt(argc, argv, "cg")) != -1;)
    {
        if (opt == 'c') {
            color_mode = true;
        }
        if (opt == 'g') {
            gpu_mode = true;
        }
        if (opt == '?') {
            return 1;
        }
    }

	terminal term = init_term();
    term.color_mode = color_mode;

    camera cam;
    cam.position = (vec3){0.f, 0.5f, 0.f};
    cam.rotation = (vec3){0.f, 0.f,  -3.1415926535f/2.f};

    if (gpu_mode)
    {
        opencl_env cl = init_opencl("kernel.cl", term);

        for(;;)
        {
            set_term_size(&term);
            cam_control(&cam);
            update_args(cl, cam);
            run_kernel(cl, term);
            refresh_term(&term);
        }
        cleanup_cl(cl);
    }

    else
    {
        for(;;)
        {
            set_term_size(&term);
            cam_control(&cam);
            render(term, cam);
            refresh_term(&term);
        }
    }

	return 0;
}