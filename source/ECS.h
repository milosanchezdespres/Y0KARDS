#pragma once

#include <string>
#include <typeindex>

#include <memory>
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;

using std::unique_ptr;
using std::type_index;

using std::make_unique;

using Entity = size_t;
using ComponentID = size_t;

struct IComponent
{
    virtual ~IComponent() {}
    virtual void init(void* data) = 0;
};

struct IComponentArray
{
    virtual ~IComponentArray() {}
};

template <typename C, typename = std::enable_if_t<std::is_base_of<IComponent, C>::value>>
struct ComponentArray : public IComponentArray
{
    vector<C> components;
    unordered_map<Entity, ComponentID> entities;

    ComponentArray() : IComponentArray() {}

    void insert(Entity e, const C& component)
    {
        entities[e] = components.size();
        components.push_back(component);
    }

    C* get(Entity e)
    {
        auto it = entities.find(e);
        if (it != entities.end()) return &components[it->second];
        return nullptr;
    }
};

struct ECS
{
    Entity __next_entity_id__;
    unordered_map<type_index, unique_ptr<IComponentArray>> component_arrays;

    ECS() { __next_entity_id__ = -1; }
    ~ECS() {}

    Entity create() { return __next_entity_id__++; }

    template<typename C>
    void add(Entity e, const C& component) { __component_array__<C>()->insert(e, component); }

    template<typename C>
    C* component(Entity e) { return __component_array__<C>()->get(e); }

    template<typename C>
    ComponentArray<C>* __component_array__()
    {
        type_index typeId = typeid(C);

        if (component_arrays.find(typeId) == component_arrays.end()) component_arrays[typeId] = make_unique<ComponentArray<C>>();
        return static_cast<ComponentArray<C>*>(component_arrays[typeId].get());
    }
};