#include "control.h"
#include <ncurses.h>
#include <math.h>
#include <unistd.h>

void kbd_control(cl_float3 *pos, cl_float3 *rotation)
{
    const float speed = 0.01f;

    int key;
    key = getch();

    if (key == 'w')
    { 
        pos -> x += cos(rotation -> z) * speed;
        pos -> y += sin(rotation -> z) * speed;
        pos -> z += sin(rotation -> y) * speed;
    };

    if (key == 's')
    {
        pos -> x -= cos(rotation -> z) * speed;
        pos -> y -= sin(rotation -> z) * speed;
        pos -> z -= sin(rotation -> y) * speed;
    };

    if (key == 'a')
    {
        pos -> x += sin(rotation -> z) * speed;
        pos -> y -= cos(rotation -> z) * speed;
    };
    
    if (key == 'd')
    {
        pos -> x -= sin(rotation -> z) * speed;
        pos -> y += cos(rotation -> z) * speed;
    };
}

void mouse_control(int fd, cl_float3 *rotation)
{
    signed char data[3];
    ssize_t n;

    for(;;)
    {
        n = read(fd, data, sizeof(data));

        if (n == -1) { break; }
        else if (n != sizeof(data)) { break; }

        rotation->z += data[1] * 0.0025f;
        rotation->y -= data[2] * 0.0025f;
    }
}
