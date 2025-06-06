#pragma once

#include <vector>
#include <unordered_map>
#include <type_traits>
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
    virtual void init() {}
    virtual ~game_unit() {}
};

struct Component : public game_unit
{
    Component() : game_unit() {}
    virtual ~Component() {}
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

    template <typename M, typename = std::enable_if_t<std::is_base_of<Component, M>::value>>
    void push(string alias = "")
    {
        components.push_back(new M());
        int index = (int)components.size() - 1;

        if(alias == "") alias = components[index]->default_name();

        components[index]->id = index;
        components[index]->name = alias;
        components[index]->owner = this;
        components[index]->init();

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

    template <typename M, typename = std::enable_if_t<std::is_base_of<Entity, M>::value>>
    void push(string alias = "")
    {
        entities.push_back(new M());
        int index = (int)entities.size() - 1;

        if(alias == "") alias = entities[index]->default_name();

        entities[index]->id = index;
        entities[index]->name = alias;
        entities[index]->owner = this;
        entities[index]->init();

        entitie_aliases[alias] = index;

        size = entities.size();
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

    void init() override { __on__init__(); }
    void update() { __on__update__(); }
    void draw() { __on__draw__(); }
    void exit() { __on__exit__(); FREE_ASSETS; }

    virtual void __on__init__() {}
    virtual void __on__update__() {}
    virtual void __on__draw__() {}
    virtual void __on__exit__() {}
};
