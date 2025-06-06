#pragma once

#include "../ecs.h"
#include "../components/State.h"

struct PlayerIdle : public StateData
{
    PlayerIdle() : StateData() {}

    void __on_enter__() override
    {
        //...
    }

    void __on_update__() override
    {
        //...
        cout << entity->name << endl;
    }

    void __on_exit__() override
    {
        //...
    }
};