#pragma once
#include "Character.h"

struct NPC : public Character
{
    NPC() : Character() { }

    void __on__init__(vector<any> args) override
    {
        Character::__on__init__(args);

        //...
    }
};