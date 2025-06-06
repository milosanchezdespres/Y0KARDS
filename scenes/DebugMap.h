#pragma once

#include <string>
using namespace std;

#include "../ecs.h"
#include "../display/display.h"

#include "../entities/Entity2D.h"

struct DebugMap : public Scene
{
    Color fpstextcolor;
    string fpsstr;

    DebugMap() : Scene() {}

    void __on__init__() override
    {
        push<Entity2D>("test", "player/playerwalk", S(16, 0, 16, 32, 3, 3));

        entity<Entity2D>("test")->component<Transform>()->x = 100;
        entity<Entity2D>("test")->component<Transform>()->y = 100;

        fpstextcolor = to_color("#ffffff");
    }

    void __on__update__() override
    {
        //...

        fpsstr = "FPS :" + to_string(fps);
    }

    void __on__draw__() override
    {
        entity<Entity2D>("test")->draw();

        //...

        TEXT(fpsstr.c_str(), 20, 20, fpstextcolor, 2);
    }
};
