#include "src/terminal.h"
#ifdef OPENCL
    #include "src/opencl.h"
#else
    #include "src/cpu_engine.h"
#endif
#include <getopt.h>

int main(int argc, char *argv[])
{
    int color_mode = false;

    terminal term = init_term();
    term.color_mode = color_mode;

    camera cam;
    cam.position = (vec3){0.f, 0.5f, 0.f};
    cam.rotation = (vec3){0.f, 0.f,  -3.1415926535f/2.f};

    #ifdef OPENCL
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
    #else
        for(;;)
        {
            set_term_size(&term);
            cam_control(&cam);
            render(term, cam);
            refresh_term(&term);
        }
    #endif

  return 0;
}