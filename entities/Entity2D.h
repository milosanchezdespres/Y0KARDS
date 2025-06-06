#pragma once
#include "../ecs.h"
#include "../display/display.h"

#include "../components/Sprite.h"

struct Entity2D : public Entity
{
    void __on__init__(std::vector<std::any> args) override
    {
        push<Sprite>("");

        string texture_path = string(any_cast<const char*>(args[0]));
        component<Sprite>()->surface = any_cast<Surface>(args[1]);

        gload(component<Sprite>()->texture, (texture_path + ".bmp").c_str());

        place(0, 0);
    }

    void place(float x, float y)
    {
        component<Transform>()->x = x; 
        component<Transform>()->y = y;

        dynamic_cast<SpatialScene*>(owner)->update_spatial_hash(this);
    }

    void draw() { __on__draw__(); }

    const char* default_name() override{ return "entity2D"; }

    void __on__draw__() override
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