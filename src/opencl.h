#ifndef OPENCL_H
#define OPENCL_H

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

#include "terminal.h"
#include "camera.h"

typedef struct
{
    cl_context          context;
    cl_command_queue    queue;
    cl_program          program;
    cl_kernel           render_kernel;
    cl_mem              colors_buf;
    size_t              global_size[2];
}
opencl_env;

opencl_env init_opencl(const char* kernel_filename, terminal term);

void update_args(opencl_env cl, camera cam);
void run_kernel(opencl_env cl, terminal term);
void cleanup_cl(opencl_env cl);

#endif