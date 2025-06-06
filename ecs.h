#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <type_traits>
#include <any>
#include <algorithm>

using namespace std;

struct game_unit
{
    int id;
    string name;
    game_unit* owner;

    game_unit() {}

    template <typename M, typename = std::enable_if_t<std::is_base_of<game_unit, M>::value>>
    M* parent() { return dynamic_cast<M*>(owner); }

    virtual const char* default_name() { return "game_unit"; }

    virtual ~game_unit() {}
};

struct Component : public game_unit
{
    Component() : game_unit() {}
    virtual ~Component() {}
};

struct Transform : public Component
{
    float x, y;

    Transform() : Component() {}

    const char* default_name() override{ return "transform"; }
};

struct Entity : public Component
{
    int size;
    vector<Component*> components;
    unordered_map<string, int> component_aliases;

    Entity() : Component() { size = 0; }

    ~Entity()
    {
        for (Component* c : components)
            delete c;
        components.clear();
        component_aliases.clear();
        size = 0;
    }

    template<typename... Args>
    void init(Args&&... args)
    {
        std::vector<std::any> vec = { std::forward<Args>(args)... };

        push<Transform>("");

        __on__init__(vec);
    }

    template <typename M, typename... Args, typename = std::enable_if_t<std::is_base_of<Component, M>::value>>
    void push(string alias, Args&&... args)
    {
        components.push_back(new M());
        int index = (int)components.size() - 1;

        if(alias == "") alias = components[index]->default_name();

        components[index]->id = index;
        components[index]->name = alias;
        components[index]->owner = this;

        component_aliases[alias] = index;

        size = components.size();
    }

    template <typename M, typename = std::enable_if_t<std::is_base_of<Component, M>::value>>
    M* component(int id) { return dynamic_cast<M*>(components[id]); }

    template <typename M, typename = std::enable_if_t<std::is_base_of<Component, M>::value>>
    M* component(string alias = "")
    {
        if(alias == "")
        {
            M* dummy = new M();
            alias = dummy->default_name();
            delete dummy;
        }

        auto it = component_aliases.find(alias);

        if(it != component_aliases.end())
            return dynamic_cast<M*>(components[it->second]);

        return nullptr;
    }

    void draw() { __on__draw__(); }

    virtual void __on__init__(std::vector<std::any> args) {}
    virtual void __on__draw__() {}
};

struct Scene : public Component
{
    int size;
    vector<Entity*> entities;
    unordered_map<string, int> entitie_aliases;

    Scene() : Component() { size = 0; }

    ~Scene()
    {
        for (Entity* e : entities)
            delete e;
        entities.clear();
        entitie_aliases.clear();
        size = 0;
    }

    template <typename M, typename... Args, typename = std::enable_if_t<std::is_base_of<Entity, M>::value>>
    int push(string alias, Args&&... args)
    {
        entities.push_back(new M());
        int index = (int)entities.size() - 1;

        if(alias == "") alias = entities[index]->default_name();

        entities[index]->id = index;
        entities[index]->name = alias;
        entities[index]->owner = this;
        entities[index]->init(std::forward<Args>(args)...);

        entitie_aliases[alias] = index;

        __on__push__(entities[index]);

        size = entities.size();
        return size - 1;
    }

    template <typename M, typename = std::enable_if_t<std::is_base_of<Entity, M>::value>>
    M* entity(int id) { return dynamic_cast<M*>(entities[id]); }

    template <typename M, typename = std::enable_if_t<std::is_base_of<Entity, M>::value>>
    M* entity(string alias = "")
    {
        if(alias == "")
        {
            M* dummy = new M();
            alias = dummy->default_name();
            delete dummy;
        }

        auto it = entitie_aliases.find(alias);

        if(it != entitie_aliases.end())
            return dynamic_cast<M*>(entities[it->second]);

        return nullptr;
    }

    bool remove(string alias)
    {
        auto it = entitie_aliases.find(alias);
        if(it == entitie_aliases.end()) return false;

        int index = it->second;

        __on__remove__(index);

        delete entities[index];
        entities.erase(entities.begin() + index);
        entitie_aliases.erase(it);

        for(int i = index; i < (int)entities.size(); i++)
        {
            entities[i]->id = i;
            entitie_aliases[entities[i]->name] = i;
        }

        size = entities.size();

        return true;
    }

    bool remove(int id)
    {
        if(id < 0 || id >= size) return false;

        string alias = entities[id]->name;
        return remove(alias);
    }

    void init() { __on__init__(); }
    void update() { __on__update__(); }
    void draw() { __on__draw__(); }
    void exit() { __on__exit__(); }

    virtual void __on__init__() {}
    virtual void __on__update__() {}
    virtual void __on__draw__() {}
    virtual void __on__exit__() {}

    virtual void __on__remove__(int id) {}
    virtual void __on__push__(Entity* entity) {}
};

struct SpatialScene : Scene
{
    vector<int> culled;

    unordered_map<int, size_t> entity_positions;
    unordered_map<size_t, vector<int>> positions_entities;
    unordered_set<int> dirty_entities;

    SpatialScene() : Scene() {}
    ~SpatialScene()
    {
        entity_positions.clear();
        positions_entities.clear();
        culled.clear();
        dirty_entities.clear();
    }

    void update_spatial_hash(Entity* entity)
    {
        int id = entity->id;

        int x = 0, y = 0;

        for(auto* component : entity->components)
        {
            auto* transform = dynamic_cast<Transform*>(component);

            if(transform)
            {
                x = transform->x;
                y = transform->y;
                break;
            }
        }

        size_t new_hashid = _hashid(x, y);

        auto it = entity_positions.find(id);
        if(it != entity_positions.end())
        {
            size_t old_hashid = it->second;

            if(old_hashid != new_hashid)
            {
                auto& old_vec = positions_entities[old_hashid];
                old_vec.erase(std::remove(old_vec.begin(), old_vec.end(), id), old_vec.end());

                entity_positions[id] = new_hashid;
                positions_entities[new_hashid].push_back(id);

                dirty_entities.insert(id);
            }
        }
        else
        {
            entity_positions[id] = new_hashid;
            positions_entities[new_hashid].push_back(id);
            dirty_entities.insert(id);
        }
    }

    size_t _hashid(float x, float y)
    {
        int X = x;
        int Y = y;

        static const size_t prime1 = 73856093;
        static const size_t prime2 = 19349663;

        return (X * prime1) ^ (Y * prime2);
    }

    void __on__update__() override
    {
        for(auto id : dirty_entities)
        {
            __on__culling__(id);
        }
        dirty_entities.clear();
    }

    void __on__draw__() override
    {
        for(int id : culled)
        {
            if (id < 0 || id >= (int)entities.size())
            {
                printf("[WARN] draw() skipping invalid id: %d (entities.size=%lu)\n", id, entities.size());
                continue;
            }

            entities[id]->draw();
        }
    }

    void __on__remove__(int id) override
    {
        auto it_pos = entity_positions.find(id);
        if(it_pos != entity_positions.end())
        {
            size_t hashid = it_pos->second;
            auto& vec = positions_entities[hashid];
            vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
            entity_positions.erase(it_pos);
        }

        auto it_cull = find(culled.begin(), culled.end(), id);
        if(it_cull != culled.end()) { culled.erase(it_cull); }
    }

    virtual void __on__culling__(int id) {}
};
