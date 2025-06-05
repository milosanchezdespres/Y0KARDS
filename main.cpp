#include "display/display.h"

int main()
{
    INIT2;

    window("YoKards", 640, 480);

    load(player_walk, "player/playerwalk.bmp");
    //.....

    float i = 0;
    int direction = 1; // 1 = moving right, -1 = moving left

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        blit(player_walk, S(16, 0, 16, 32, 3, 3), i, 200);

        write('A', 10, 10);
        write('B', 26, 10);
        write('C', 42, 10);

        i += direction * 250 * delta;

        if (i > SCREEN_WIDTH - player_walk->width) 
        {
            i = SCREEN_WIDTH - player_walk->width;
            direction = -1;
        }
        else if (i < 0)
        {
            i = 0;
            direction = 1;
        }

        FLIP_BUFFER;
    }

    EXIT;
}
