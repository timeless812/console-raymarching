#include "opencl.h"
#include <stdio.h>

#define CHECK_ERR(err) \
    if (err != CL_SUCCESS) \
        { fprintf(stderr, "OpenCL error %d\n", err); exit(1); }

OpenCLEnv init_opencl
(
    const char*    kernel_filename,
    int            width,
    int            height,
    float          aspect,
    float          pixel_aspect,
    int            gradient_size,
    cl_float3      pos,
    cl_float3      rotation,
    int*           colors
)
{
    OpenCLEnv  env = {0};
    cl_int     err;
    
    /* get platform. */
    cl_uint         num_platforms;
    cl_platform_id  *platforms;
    cl_platform_id  platform;

    err = clGetPlatformIDs(0, NULL, &num_platforms);
    CHECK_ERR(err);
    
    if (num_platforms == 0) {
        fprintf(stderr, "no OpenCL platforms found.\n");
        exit(1);
    }
    
    platforms = malloc(num_platforms * sizeof(cl_platform_id));

    err = clGetPlatformIDs(num_platforms, platforms, NULL);
    CHECK_ERR(err);

    platform = platforms[0];
    free(platforms);

    /* get device. */
    cl_uint       num_devices;
    cl_device_id  *devices;
    cl_device_id  device;

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    CHECK_ERR(err);
    
    if (num_devices == 0) {
        fprintf(stderr, "no OpenCL devices found.\n");
        exit(1);
    }
    
    devices = malloc(num_devices * sizeof(cl_device_id));

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    CHECK_ERR(err);

    device = devices[0];
    free(devices);

    /* create context and queue. */
    const cl_queue_properties properties[] = {0};

    env.context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERR(err);

    env.queue = clCreateCommandQueueWithProperties(env.context, device, properties, &err);
    CHECK_ERR(err);

    /* load and compile kernel. */
    FILE*   kernel_file;
    size_t  kernel_size;
    char*   kernel_source;
    size_t  read;

    kernel_file = fopen(kernel_filename, "r");
    if (!kernel_file) {
        fprintf(stderr, "failed to open %s\n", kernel_filename);
        exit(1);
    }
    
    fseek(kernel_file, 0, SEEK_END);
    kernel_size = ftell(kernel_file);
    rewind(kernel_file);
    kernel_source = malloc(kernel_size + 1);
    
    read = fread(kernel_source, 1, kernel_size, kernel_file);
    
    fclose(kernel_file);
    kernel_source[kernel_size] = '\0';

    env.program = clCreateProgramWithSource(env.context, 1, (const char**)&kernel_source, NULL, &err);

    CHECK_ERR(err);
    free(kernel_source);
    
    err = clBuildProgram(env.program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t  log_size;
        char*   build_log;

        clGetProgramBuildInfo(env.program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        build_log = malloc(log_size);
        clGetProgramBuildInfo(env.program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        fprintf(stderr, "build error:\n%s\n", build_log);
        free(build_log);
        exit(1);
    }

    /* make kernel. */
    env.render_kernel = clCreateKernel(env.program, "render", &err);
    CHECK_ERR(err);

    /* create color buffer. */
    env.colors_buf = clCreateBuffer(env.context, CL_MEM_WRITE_ONLY, width * height * sizeof(int), NULL, &err);
    CHECK_ERR(err);

    /* set kernel args. */
    clSetKernelArg(env.render_kernel, 0, sizeof(int),        &width);
    clSetKernelArg(env.render_kernel, 1, sizeof(int),        &height);
    clSetKernelArg(env.render_kernel, 2, sizeof(float),      &aspect);
    clSetKernelArg(env.render_kernel, 3, sizeof(float),      &pixel_aspect);
    clSetKernelArg(env.render_kernel, 4, sizeof(int),        &gradient_size);
    clSetKernelArg(env.render_kernel, 5, sizeof(cl_float3),  &pos);
    clSetKernelArg(env.render_kernel, 6, sizeof(cl_float3),  &rotation);
    clSetKernelArg(env.render_kernel, 7, sizeof(cl_mem),     &env.colors_buf);

    return env;
}

void cleanup(OpenCLEnv env)
{
    clReleaseMemObject(env.colors_buf);
    clReleaseKernel(env.render_kernel);
    clReleaseProgram(env.program);
    clReleaseCommandQueue(env.queue);
    clReleaseContext(env.context);
}