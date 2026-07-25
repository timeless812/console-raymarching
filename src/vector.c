#include "vector.h"


vec2 sub2(vec2 a, vec2 b) { return (vec2){a.x - b.x, a.y - b.y}; }
float len2(vec2 a) { return sqrt(a.x * a.x + a.y * a.y); }

vec3 add3(vec3 a, vec3 b) { return (vec3){a.x + b.x, a.y + b.y, a.z + b.z}; }
vec3 sub3(vec3 a, vec3 b) { return (vec3){a.x - b.x, a.y - b.y, a.z - b.z}; }
vec3 norm3(vec3 a) { float l = len3(a); return (vec3){a.x/l, a.y/l, a.z/l}; }
float len3(vec3 a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }
float dot3(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float clamp(float val, float min, float max) { return fmax(min, fmin(val, max)); }