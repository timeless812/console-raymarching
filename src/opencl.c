#include "opencl.h"
#include <stdio.h>

#define CHECK_ERR(err) \
    if (err != CL_SUCCESS) \
        { fprintf(stderr, "OpenCL error %d\n", err); exit(1); }

opencl_env init_opencl(const char* kernel_filename, terminal term)
{
    opencl_env cl = {0};
    cl_int err;
    
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
    cl.context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    CHECK_ERR(err);
    cl.queue = clCreateCommandQueue(cl.context, device, 0, &err);
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

    cl.program = clCreateProgramWithSource(cl.context, 1, (const char**)&kernel_source, NULL, &err);

    CHECK_ERR(err);
    free(kernel_source);
    
    err = clBuildProgram(cl.program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t  log_size;
        char*   build_log;

        clGetProgramBuildInfo(cl.program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        build_log = malloc(log_size);
        clGetProgramBuildInfo(cl.program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
        fprintf(stderr, "build error:\n%s\n", build_log);
        free(build_log);
        exit(1);
    }

    /* make kernel. */
    cl.render_kernel = clCreateKernel(cl.program, "render", &err);
    CHECK_ERR(err);

    /* make colors buffer. */
    cl.colors_buf = clCreateBuffer(cl.context, CL_MEM_WRITE_ONLY, term.size, NULL, &err);
    CHECK_ERR(err);

    /* set kernel args. */
    clSetKernelArg(cl.render_kernel, 0, sizeof(cl_mem),  &cl.colors_buf);
    clSetKernelArg(cl.render_kernel, 1, sizeof(int),     &term.width);
    clSetKernelArg(cl.render_kernel, 2, sizeof(int),     &term.height);
    clSetKernelArg(cl.render_kernel, 5, sizeof(int),     &term.total_colors);
    clSetKernelArg(cl.render_kernel, 6, sizeof(int),     &term.pixel_aspect);

    cl.global_size[0] = term.width;
    cl.global_size[1] = term.height;

    return cl;
}

void update_args(opencl_env cl, camera cam)
{
    clSetKernelArg(cl.render_kernel, 3, sizeof(cl_float3),  &cam.position);
    clSetKernelArg(cl.render_kernel, 4, sizeof(cl_float3),  &cam.rotation);
}

void run_kernel(opencl_env cl, terminal term)
{
    clEnqueueNDRangeKernel(cl.queue, cl.render_kernel, 2, NULL, cl.global_size, NULL, 0, NULL, NULL);
    clEnqueueReadBuffer(cl.queue, cl.colors_buf, CL_TRUE, 0, term.size, term.colors, 0, NULL, NULL);
}

void cleanup_cl(opencl_env cl)
{
    clReleaseMemObject(cl.colors_buf);
    clReleaseKernel(cl.render_kernel);
    clReleaseProgram(cl.program);
    clReleaseCommandQueue(cl.queue);
    clReleaseContext(cl.context);
}