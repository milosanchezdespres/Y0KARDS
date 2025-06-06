#include "display/display.h"
#include "ecs.h"
#include "scenes/MapScene.h"

int main()
{
    window("YoKards", 640, 480);

    MapScene* scene = new MapScene();
    scene->init();

    while (RUNNING)
    {
        EVENTS;

        scene->update();

        CLEAR_BUFFER;

        scene->draw();

        FLIP_BUFFER;
    }

    scene->exit();

    EXIT;
}
