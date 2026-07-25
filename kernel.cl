float3 rotate_x(float3 a, float3 b, float angle)
{    
    float3 c = a;
    c.y = (a.y - b.y) * cos(angle) - (a.z - b.z) * sin(angle) + b.y;
    c.z = (a.y - b.y) * sin(angle) + (a.z - b.z) * cos(angle) + b.z;
    return c;
}

float3 rotate_y(float3 a, float3 b, float angle)
{    
    float3 c = a;
    c.x = (a.x - b.x) * cos(angle) - (a.z - b.z) * sin(angle) + b.x;
    c.z = (a.x - b.x) * sin(angle) + (a.z - b.z) * cos(angle) + b.z;
    return c;
}

float3 rotate_z(float3 a, float3 b, float angle)
{
    float3 c = a;
    c.x = (a.x - b.x) * cos(angle) - (a.y - b.y) * sin(angle) + b.x;
    c.y = (a.x - b.x) * sin(angle) + (a.y - b.y) * cos(angle) + b.y;
    return c;
}

float tor_sdf(float3 p)
{
    float2 t = (float2){0.2f, 0.1f};
    float2 q;
    
    q = (float2){length(p.xz) - t.x, p.y};
    return length(q) - t.y;
}

float scene(float3 p)
{
    float dist = tor_sdf(p);
    return dist;
}

float get_dist(float3 p) 
{ 
    return scene(p);
}

float3 get_normal(float3 p)
{
    const float eps = 0.0001f;
    return normalize((float3)
    {
        get_dist((float3){p.x + eps, p.y, p.z}) - get_dist((float3){p.x - eps, p.y, p.z}),
        get_dist((float3){p.x, p.y + eps, p.z}) - get_dist((float3){p.x, p.y - eps, p.z}),
        get_dist((float3){p.x, p.y, p.z + eps}) - get_dist((float3){p.x, p.y, p.z - eps})
    });
}

float get_light(float3 p, float3 light_pos)
{ 
    float3 n = get_normal(p);
    float3 l = normalize(light_pos - p);
   
    float diff = max(0.f, dot(n, l));
    float ambient = 0.2f;
    float intensity = ambient + diff;
    
    return clamp(intensity, 0.f, 1.f);
}

float raymarch(float3 ro, float3 rd)
{
    float3  p;
    float   td;
    float   d;

    td = 0.f;

    for (int i = 0; i < 120; i++)
    {
        p = (float3)
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

__kernel void render
(
    __global int    *colors,
    int             width,
    int             height,
    float3          pos,
    float3          rotation,
    int             total_colors,
    float           pixel_aspect
)
{
    int       j;
    int       i;
    float     x;
    float     y;
    float2    uv;
    float3    ro;
    float3    rd;

    j = get_global_id(0);
    i = get_global_id(1);

    x = (float)j / width * 2.f - 1.f;
    y = (float)i / height * 2.f - 1.f;
    uv = (float2){x, y};
    uv.x *= (float)width/height * pixel_aspect;

    ro = pos;
    rd = (float3){1.f, uv.x, uv.y};
    rd = normalize(rd);
    rd = rotate_y(rd, (float3){0.f, 0.f, 0.f}, rotation.y);
    rd = rotate_z(rd, (float3){0.f, 0.f, 0.f}, rotation.z);

    int color = 0;
    float  td = raymarch(ro, rd);

    if (td >= 0)
    {
        float3  p;
        float   light;

        p = ro + rd * td;

        light = get_light(p, (float3){0.f, 0.f, -2.f});
        color = light * 20;
        color = clamp(color, 0, total_colors - 1);
    }

    colors[i * width + j] = color;
}