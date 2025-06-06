#pragma once

#include "MapScene.h"

#include "../states/PlayerIdle.h"

struct DebugMap : public MapScene
{
    DebugMap() : MapScene() {}

    void __on__init__() override
    {
        MapScene::__on__init__();

        int id = push<Entity2D>("test", "charsets/player", 16, 32, 3);

        entity<Entity2D>(id)->sprite->set(1);
        entity<Entity2D>(id)->place(50, 50);

        int id2 = push<Entity2D>("test2", "charsets/player", 16, 32, 3);

        entity<Entity2D>(id)->state->go_to<PlayerIdle>();
    }

    void __on__draw__() override
    {
        MapScene::__on__draw__();

        //...

        TEXT("this is just a test !", 20, 150, {"#05185f"}, 1);
    }
};