#pragma once

#include <iostream>
using namespace std;

#include "../ecs.h"

struct TestSystem : public System<Transform>
{
    TestSystem() : System<Transform>() { }

    void __on__start__() override
    {
        cout << "starting..." << endl;

        //...
    }

    void __on__execute__(int component_id) override
    {
        cout << "executing for [" + to_string(component_id) + "]..." << endl;

        //...
    }
};