#pragma once

#include <string>
using namespace std;

#include "../ecs.h"
#include "../display/display.h"

#include "../entities/Entity2D.h"

struct MapScene : public SpatialScene
{
    Color fpstextcolor;
    string fpsstr;

    MapScene() : SpatialScene() {}

    void __on__init__() override
    {
        push<Entity2D>("test", "player/playerwalk", S(16, 0, 16, 32, 3, 3));

        entity<Entity2D>("test")->component<Transform>()->x = 100;
        entity<Entity2D>("test")->component<Transform>()->y = 100;

        fpstextcolor = to_color("#ffffff");
    }

    void __on__update__() override
    {
        //...

        fpsstr = "FPS :" + to_string(fps);
    }

    void __on__draw__() override
    {
        SpatialScene::__on__draw__();
        
        //...

        TEXT(fpsstr.c_str(), 20, 20, fpstextcolor, 2);
    }

    void __on__culling__(int id) override
    {
        Entity2D* entity = dynamic_cast<Entity2D*>(entities[id]);

        Texture2D* texture = entity->component<Sprite>()->texture;
        Surface surface = entity->component<Sprite>()->surface;

        float x = entity->component<Transform>()->x;
        float y = entity->component<Transform>()->y;

        auto it = std::find(culled.begin(), culled.end(), id);

        if(IS_IN_BOUNDS(texture, surface, x, y))
        {
            if(it == culled.end())
                culled.push_back(id);
        }
        else
        {
            if(it != culled.end())
                culled.erase(it);
        }
    }

};
