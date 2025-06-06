#pragma once

#include "MapScene.h"

struct DebugMap : public MapScene
{
    DebugMap() : MapScene() {}

    void __on__init__() override
    {
        MapScene::__on__init__();

        int id = push<Entity2D>(name, "player/playerwalk", 16, 32, 3);

        entity<Entity2D>(id)->sprite->set(1);
        entity<Entity2D>(id)->place(50, 50);
    }
};