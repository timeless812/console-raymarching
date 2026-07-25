#ifndef CAMERA_H
#define CAMERA_H

#include "terminal.h"
#include "vector.h"

typedef struct
{
	vec3 position;
	vec3 rotation;
}
camera;

void cam_control(camera* cam);

#endif