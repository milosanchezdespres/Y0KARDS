#pragma once

#include "../ecs.h"
#include "../display/display.h"

struct State;

struct StateData
{
    Entity* entity;
    State* component;

    const char* alias;

    StateData()
    {
        alias = "default";

        entity = nullptr;
        component = nullptr;
    }

    void enter() { __on_enter__(); }
    void update() { __on_update__(); }
    void exit() { __on_exit__(); }

    template <typename U>
    void go_to() { component->template go_to<U>(); }

    virtual void __on_enter__(){}
    virtual void __on_update__() { }
    virtual void __on_exit__() {}
};

struct State : public Component
{
    bool __initialized;
    StateData* data;

    State() : Component()
    {
        __initialized = false;
        data = nullptr;
    }

    template <typename U>
    void go_to()
    {
        if(__initialized) data->exit(); else __initialized = true;

        data = new U();

        data->entity = dynamic_cast<Entity*>(owner);
        data->component = this;

        data->enter();
    }

    const char* alias() { return data->alias; }

    const char* default_name() override{ return "state"; }
};