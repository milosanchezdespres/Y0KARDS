#include "scenes/DebugMap.h"

int main()
{
    window("YoKards", 640, 480);

    GOTO<DebugMap>();

    CURRENT_SCENE_INIT;

    while (RUNNING)
    {
        EVENTS;

        CURRENT_SCENE_UPDATE;

        CLEAR_BUFFER;

        CURRENT_SCENE_RENDER;

        FLIP_BUFFER;
    }

    CURRENT_SCENE_EXIT;

    EXIT;
}
