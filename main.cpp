
#include "source/File.h"
#include "source/Colors.h"
#include "source/ECS.h"
#include "source/Json.h"

#include "ecs/components/Transform.h"

int main()
{
    Entity player = ecs.create();
    ecs.add(player, Transform{68, 34});

    cout << ecs.component<Transform>(player)->x << endl;

    return 0;
}
