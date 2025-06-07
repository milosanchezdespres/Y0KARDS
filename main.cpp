#include "source/ECS.h"

#include <iostream>
using std::iostream;

struct MyComponent : public IComponent
{
    int value;

    MyComponent(int v = 0) : value(v) {}

    void init(void* data) override
    {
        if (data) value = *static_cast<int*>(data);
    }
};

int main()
{
    ECS ecs;
    Entity e = ecs.create();
    ecs.add<MyComponent>(e, MyComponent{42});

    MyComponent* c = ecs.component<MyComponent>(e);
    if (c) std::cout << c->value << std::endl;
    
    //...

    return 0;
}
