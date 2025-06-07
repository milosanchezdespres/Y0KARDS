#pragma once

#include "../../source/ECS.h"

struct Transform : public IComponent
{
    float x = 0.0f;
    float y = 0.0f;
    
    Transform(float x_ = 0.0f, float y_ = 0.0f) : x(x_), y(y_) {}
};