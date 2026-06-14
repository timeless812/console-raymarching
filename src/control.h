#ifndef CONTROL_H
#define CONTROL_H

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

void kbd_control(cl_float3 *pos, cl_float3 *rotation);
void mouse_control(int fd, cl_float3 *rotation);

#endif