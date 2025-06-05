#include "utils/window.h"
#include "utils/bmp.h"

int main()
{
    INIT2;

    window("YoKards", 640, 480);

    BMP* test = new BMP("player/playerwalk.bmp");
    Texture* texture = new Texture(test);

    while (RUNNING)
    {
        EVENTS;

        CLEAR_BUFFER;

        blit(texture, 10, 10);

        FLIP_BUFFER;
    }

    EXIT;
}
