#include "camera.h"
#include <ncurses.h>

void cam_control(camera* cam)
{
    const float speed = 0.01f;
    const float sens  = 0.04f;

    int key;
    key = getch();

    if (key == 'w')
    { 
        cam->position.x += cos(cam->rotation.z) * speed;
        cam->position.y += sin(cam->rotation.z) * speed;
        cam->position.z += sin(cam->rotation.y) * speed;
    };

    if (key == 's')
    {
        cam->position.x -= cos(cam->rotation.z) * speed;
        cam->position.y -= sin(cam->rotation.z) * speed;
        cam->position.z -= sin(cam->rotation.y) * speed;
    };

    if (key == 'a')
    {
        cam->position.x += sin(cam->rotation.z) * speed;
        cam->position.y -= cos(cam->rotation.z) * speed;
    };
    
    if (key == 'd')
    {
        cam->position.x -= sin(cam->rotation.z) * speed;
        cam->position.y += cos(cam->rotation.z) * speed;
    };

    if (key == 'i') cam->rotation.y -= sens;
    if (key == 'k') cam->rotation.y += sens;
    if (key == 'j') cam->rotation.z -= sens;
    if (key == 'l') cam->rotation.z += sens;
}
