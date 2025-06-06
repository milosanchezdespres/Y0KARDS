#pragma once
#include "../ecs.h"
#include "../display/display.h"

#include "../components/Sprite.h"
#include "../components/State.h"

struct Entity2D : public Entity
{
    SpatialScene* scene;

    Sprite* sprite;
    Transform* transform;
    State* state;

    void __on__init__(vector<any> args) override
    {
        scene = dynamic_cast<SpatialScene*>(owner);

        push<Sprite>("");
        sprite = component<Sprite>();

        push<State>("");
        state = component<State>();

        transform = component<Transform>();

        if(args.size() > 0)
        {
            __on_texture_load__(args);

            place(0, 0);
        }
    }

    virtual void __on_texture_load__(vector<any> args)
    {
        string texture_path = string(any_cast<const char*>(args[0]));

        gload(sprite->texture, (texture_path + ".bmp").c_str());

        sprite->set(0, any_cast<int>(args[1]), any_cast<int>(args[2]), any_cast<int>(args[3]));
    }

    void place(float x, float y)
    {
        transform->x = x; 
        transform->y = y;

        scene->update_spatial_hash(this);
    }

    void draw() { __on__draw__(); }

    const char* default_name() override{ return "entity2D"; }

    void __on__draw__() override
    {
        blit
        (
            sprite->texture, 
            sprite->surface, 
            transform->x, 
            transform->y
        );
    }
};