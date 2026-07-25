#include "cpu_engine.h"

vec3 rotate_x(vec3 a, vec3 b, float angle)
{    
    vec3 c = a;
    c.y = (a.y - b.y) * cos(angle) - (a.z - b.z) * sin(angle) + b.y;
    c.z = (a.y - b.y) * sin(angle) + (a.z - b.z) * cos(angle) + b.z;
    return c;
}

vec3 rotate_y(vec3 a, vec3 b, float angle)
{    
    vec3 c = a;
    c.x = (a.x - b.x) * cos(angle) - (a.z - b.z) * sin(angle) + b.x;
    c.z = (a.x - b.x) * sin(angle) + (a.z - b.z) * cos(angle) + b.z;
    return c;
}

vec3 rotate_z(vec3 a, vec3 b, float angle)
{
    vec3 c = a;
    c.x = (a.x - b.x) * cos(angle) - (a.y - b.y) * sin(angle) + b.x;
    c.y = (a.x - b.x) * sin(angle) + (a.y - b.y) * cos(angle) + b.y;
    return c;
}

float tor_sdf(vec3 p) {
    const float R = 0.2f;
    const float r = 0.1f;
    vec2 q = { len2((vec2){p.x, p.z}) - R, p.y };
    return len2(q) - r;
}

float scene(vec3 p)
{
    float dist = tor_sdf(p);
    return dist;
}

float get_dist(vec3 p) 
{ 
    return scene(p);
}

vec3 get_normal(vec3 p)
{
    const float eps = 0.0001f;
    return norm3((vec3)
    {
        get_dist((vec3){p.x + eps, p.y, p.z}) - get_dist((vec3){p.x - eps, p.y, p.z}),
        get_dist((vec3){p.x, p.y + eps, p.z}) - get_dist((vec3){p.x, p.y - eps, p.z}),
        get_dist((vec3){p.x, p.y, p.z + eps}) - get_dist((vec3){p.x, p.y, p.z - eps})
    });
}

float get_light(vec3 p, vec3 light_pos)
{ 
    vec3 n = get_normal(p);
    vec3 l = norm3(sub3(light_pos, p));
   
    float diff = fmax(0.f, dot3(n, l));
    float ambient = 0.2f;
    float intensity = ambient + diff;
    
    return clamp(intensity, 0.f, 1.f);
}

float raymarch(vec3 ro, vec3 rd)
{
    vec3  p;
    float   td;
    float   d;

    td = 0.f;

    for (int i = 0; i < 120; i++)
    {
        p = (vec3)
        {
            ro.x + rd.x * td,
            ro.y + rd.y * td,
            ro.z + rd.z * td
        };
        
        d = get_dist(p);

        if (d < 0.0005f) return td;
        if (td > 20.f) break;
        
        td += d;
    }

    return -1.f;
}

void render(terminal term, camera cam)
{
    int   j;
    int   i;
    float x;
    float y;
    vec2  uv;
    vec3  ro;
    vec3  rd;

    for (j = 0; j < term.width; j++) {
    	for (i = 0; i < term.height; i++)
    	{
		    x = (float)j / term.width * 2.f - 1.f;
		    y = (float)i / term.height * 2.f - 1.f;
		    uv = (vec2){x, y};
		    uv.x *= (float)term.width/term.height * term.pixel_aspect;

		    ro = cam.position;
		    rd = (vec3){1.f, uv.x, uv.y};
		    rd = norm3(rd);
		    rd = rotate_y(rd, (vec3){0.f, 0.f, 0.f}, cam.rotation.y);
		    rd = rotate_z(rd, (vec3){0.f, 0.f, 0.f}, cam.rotation.z);

		    int color = 0;
		    float  td = raymarch(ro, rd);

		    if (td >= 0)
		    {
		        vec3    p;
		        float   light;

                vec3 rd_td = (vec3){rd.x * td, rd.y * td, rd.z * td};
		        p = add3(ro, rd_td);

		        light = get_light(p, (vec3){0.f, 0.f, -2.f});
		        color = light * 20;
		        color = clamp(color, 0, term.total_colors - 1);
		    }

		    term.colors[i * term.width + j] = color;	
    	}
    }
}