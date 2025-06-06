#pragma once

#include "../ecs.h"
#include "../components/State.h"

struct PlayerIdle : public StateData
{
    Character* player;

    PlayerIdle() : StateData() { player = nullptr; }

    void __on_enter__() override
    {
        player = dynamic_cast<Character*>(entity);
        
        player->sprite->set(1);
    }

    void __on_update__() override
    {
        //...
    }

    void __on_exit__() override
    {
        //...
    }
};