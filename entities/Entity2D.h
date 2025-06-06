#pragma once
#include "../ecs.h"
#include "../display/display.h"

#include "../components/Sprite.h"
#include "../components/Transform.h"

struct Entity2D : public Entity
{
    void __on__init__(std::vector<std::any> args) override
    {
        push<Sprite>("");
        push<Transform>("");

        string texture_path = string(any_cast<const char*>(args[0]));
        component<Sprite>()->surface = any_cast<Surface>(args[1]);

        gload(component<Sprite>()->texture, (texture_path + ".bmp").c_str());
    }

    void draw() { __on__draw__(); }

    const char* default_name() override{ return "entity2D"; }

    virtual void __on__draw__()
    {
        blit
        (
            component<Sprite>()->texture, 
            component<Sprite>()->surface, 
            component<Transform>()->x, 
            component<Transform>()->y
        );
    }
};