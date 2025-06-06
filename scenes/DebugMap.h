#pragma once

#include "MapScene.h"

struct DebugMap : public MapScene
{
    DebugMap() : MapScene() {}

    void __on__init__() override
    {
        MapScene::__on__init__();

        int id = push<Entity2D>(name, "player/playerwalk", S(16, 0, 16, 32, 3, 3));

        entity<Entity2D>(id)->place(50, 50);

        //...
    }
};