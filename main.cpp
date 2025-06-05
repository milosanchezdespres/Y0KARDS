#include "utils/window.h"

int main()
{
    INIT2;

    window("YoKards", 640, 480);

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        //...

        FLIP_BUFFER;
    }

    EXIT;
}
