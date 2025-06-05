#include "utils/window.h"
#include "utils/texture2d.h"

int main()
{
    INIT2;

    window("YoKards", 640, 480);

    Texture2D* player_walk = new Texture2D("player/playerwalk.bmp");

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        blit(player_walk, 50, 50);

        blit(player_walk, {{16, 32, 16, 32}, {4, 4}}, 450, 200);

        blit(player_walk, 150, 200);

        FLIP_BUFFER;
    }

    EXIT;
}
