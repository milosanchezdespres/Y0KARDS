#pragma once

#include <string>
using namespace std;

#include "../ecs.h"
#include "../display/display.h"

#include "../entities/Character.h"
//...

#include "../states/PlayerIdle.h"
//...

#include "../systems/StateMachine.h"
//...

#define character_width 16
#define character_height 32
#define zoom 3

struct MapScene : public SpatialScene
{
    Color fpstextcolor;
    string fpsstr;

    bool enable_fps_display;
    Point fps_display;

    Character* player;

    Point spawn_point;

    MapScene() : SpatialScene()
    {
        enable_fps_display = true;
        player = nullptr;
    }

    void __on__init__() override
    {
        SpatialScene::__on__init__();

        player = spawn<Character, PlayerIdle>("player");

        attach<StateMachine>();
        //...

        //...

        fpstextcolor = to_color("#ffffff");
        fps_display = {20, 20};
    }

    void __on__update__() override
    {
        SpatialScene::__on__update__();
        
        //...

        if(enable_fps_display) fpsstr = "FPS :" + to_string(fps);
    }

    void __on__draw__() override
    {
        SpatialScene::__on__draw__();

        //...

        if(enable_fps_display) TEXT(fpsstr.c_str(), fps_display.x, fps_display.y, fpstextcolor, 2);
    }

    void __on__culling__(int id) override
    {
        SpatialScene::__on__culling__(id);

        if (id < 0 || id >= (int)entities.size())
            return; // Invalid ID

        Entity2D* entity = dynamic_cast<Entity2D*>(entities[id]);
        if (!entity)
            return; // Not an Entity2D, ignore

        Sprite* sprite = entity->component<Sprite>();
        Transform* transform = entity->component<Transform>();

        if (!sprite || !transform)
            return; // Missing components

        Texture2D* texture = sprite->texture;
        Surface surface = sprite->surface;

        float x = transform->x;
        float y = transform->y;

        auto it = std::find(culled.begin(), culled.end(), id);

        if (IS_IN_BOUNDS(texture, surface, x, y))
        {
            if (it == culled.end())
                culled.push_back(id);
        }
        else
        {
            if (it != culled.end())
                culled.erase(it);
        }
    }

    void __on__exit__() override {  FREE_ASSETS; }

    void respawn(Entity2D* entity) { entity->place(spawn_point.x, spawn_point.y); }

    template <typename C, typename M>
    C* spawn(string name, string path = "")
    {
        if (path == "") path = name;

        vector<std::any> args = { path, character_width, character_height, zoom };

        int id = push<C>(name, args);

        auto* ptr = dynamic_cast<C*>(entity(id));
        ptr->state->template go_to<M>();

        return ptr;
    }

};
