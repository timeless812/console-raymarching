#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

typedef struct
{
	float x;
	float y;
}
vec2;

typedef struct
{
	float x;
	float y;
	float z;
}
vec3;

vec2 add2(vec2 a, vec2 b);
vec2 sub2(vec2 a, vec2 b);
vec2 norm2(vec2 a);
float len2(vec2 a);
float dot2(vec2 a, vec2 b);

vec3 add3(vec3 a, vec3 b);
vec3 sub3(vec3 a, vec3 b);
vec3 norm3(vec3 a);
float len3(vec3 a);
float dot3(vec3 a, vec3 b);

float clamp(float val, float min, float max);

#endif