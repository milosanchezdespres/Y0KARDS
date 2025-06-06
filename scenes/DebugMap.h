#pragma once

#include "MapScene.h"

#include "../entities/NPC.h"

#include "../states/PlayerIdle.h"

struct DebugMap : public MapScene
{
    DebugMap() : MapScene() {}

    void __on__init__() override
    {
        MapScene::__on__init__();

        spawn_point = {50, 50};

        respawn(player);

        //spawn<NPC, StateData>("npc_test", "charsets/player");

        //...
    }

    void __on__draw__() override
    {
        MapScene::__on__draw__();

        //...

        TEXT("this is just a test !", 20, 150, {"#05185f"}, 1);
    }
};