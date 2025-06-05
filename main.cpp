#include "display/display.h"

int main()
{
    INIT2;

    window("YoKards", 640, 480);

    load(player_walk, "player/playerwalk.bmp");
    //.....

    float i = 0;

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        blit(player_walk, 50, 50);

        blit(player_walk, 150, 200);

        blit(player_walk, S(16, 32, 16, 32, 4, 4), i, 200);

        i += 250 * delta;

        FLIP_BUFFER;
    }

    EXIT;
}
