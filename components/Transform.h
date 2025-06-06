#pragma once

#include "../ecs.h"

struct Transform : public Component
{
    float x, y;

    Transform() : Component() {}

    const char* default_name() override{ return "transform"; }
};