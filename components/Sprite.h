#pragma once

#include "../ecs.h"
#include "../display/display.h"

struct Sprite : public Component
{
    Surface surface;
    Texture2D* texture = nullptr;
    
    Sprite() : Component() {}

    const char* default_name() override{ return "sprite"; }
};