#ifndef OPENCL_H
#define OPENCL_H

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

typedef struct
{
    cl_context          context;
    cl_command_queue    queue;
    cl_program          program;
    cl_kernel           render_kernel;
    cl_mem              colors_buf;
} OpenCLEnv;

OpenCLEnv init_opencl
(
    const char*         kernel_filename,
    int                 width,
    int                 height,
    float               aspect,
    float               pixel_aspect,
    int                 gradient_size,
    cl_float3           pos,
    cl_float3           rotation,
    int*                colors
);

void cleanup(OpenCLEnv env);

#endif