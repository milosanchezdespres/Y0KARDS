#pragma once

#include <iostream>
using namespace std;

#include "../ecs.h"
#include "../components/State.h";

struct StateMachine : public System<State>
{
    StateMachine() : System<State>() { }

    void __on__start__() override
    {
        //...
    }

    void __on__execute__(int component_id) override
    {
        if(component != nullptr && component->data == nullptr) component->go_to<StateData>();

        component->data->update();
    }
};