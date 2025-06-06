#include "scenes/DebugMap.h"

int main()
{
    window("YoKards", 640, 480);

    DebugMap* scene = new DebugMap();
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
