#pragma once
#include "Entity2D.h"

struct Character : public Entity2D
{
    Character() : Entity2D() { }

    void __on__init__(vector<any> args) override
    {
        Entity2D::__on__init__(args);

        //...
    }
};