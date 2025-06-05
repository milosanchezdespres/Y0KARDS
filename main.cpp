#include "display/display.h"

int main()
{
    window("YoKards", 640, 480, 1);

    load(player_walk, "player/playerwalk.bmp");
    load(player_walk2, "player/playerwalk.bmp");
    //.....

    float i = 0;
    int direction = 1; // 1 = moving right, -1 = moving left

    const char* fpsstr = "";

    Color textcolor = to_color("#ffffff");
    Color textcolor2 = to_color("#632708");

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        blit(player_walk, S(16, 0, 16, 32, 3, 3), i, 200);

        blit(player_walk2, 40, 150, 2);

        fpsstr = ("FPS :" + to_string(fps)).c_str();

        TEXT(fpsstr, 20, 20, textcolor2, 2);

        RTBOX("This is some text displayed on screen..", T(0, 100, SCREEN_WIDTH, SCREEN_HEIGHT), 1.5);

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
