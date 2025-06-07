#pragma once

#include <string>
#include <typeindex>

#include <memory>
#include <vector>
#include <unordered_map>

#include <algorithm>
#include <functional>

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

struct ISystem
{
    virtual ~ISystem(){}
    virtual void update(class ECS& ecs) = 0;
};

struct ECS
{
    Entity __next_entity_id__;
    unordered_map<type_index, unique_ptr<IComponentArray>> component_arrays;
    unordered_map<Entity, vector<type_index>> entity_components;
    unordered_map<type_index, std::function<void(Entity)>> remove_functions;

    vector<unique_ptr<ISystem>> systems;
    
    ECS() { __next_entity_id__ = 0; }
    ~ECS() {}

    void update()
    {
        for (auto& system : systems) { system->update(*this); }
    }

    Entity create() { return __next_entity_id__++; }

    template <typename C>
    void remove_component(Entity e)
    {
        auto* compArray = __component_array__<C>();

        auto it = compArray->entities.find(e);
        if (it == compArray->entities.end()) return;

        type_index typeId = typeid(C);

        auto& types = entity_components[e];
        types.erase(std::remove(types.begin(), types.end(), typeId), types.end());
        if (types.empty()) entity_components.erase(e);

        ComponentID index_to_remove = it->second;
        ComponentID last_index = compArray->components.size() - 1;

        compArray->components[index_to_remove] = compArray->components[last_index];

        Entity moved_entity = 0;
        for (const auto& pair : compArray->entities)
        {
            if (pair.second == last_index)
            {
                moved_entity = pair.first;
                break;
            }
        }

        compArray->entities[moved_entity] = index_to_remove;

        compArray->components.pop_back();
        compArray->entities.erase(it);
    }

    void remove(Entity e)
    {
        auto it = entity_components.find(e);
        if (it == entity_components.end()) return;

        for (auto& typeId : it->second)
        {
            auto func_it = remove_functions.find(typeId);
            if (func_it != remove_functions.end())
            {
                func_it->second(e);  
            }
        }

        entity_components.erase(it);
    }

    template<typename C>
    void add(Entity e, const C& component)
    {
        __component_array__<C>()->insert(e, component);

        auto& types = entity_components[e];
        type_index typeId = typeid(C);

        remove_functions[typeId] = [this](Entity e) { remove_component<C>(e); };

        if (std::find(types.begin(), types.end(), typeId) == types.end()) types.push_back(typeId);
    }

    template<typename C>
    C* component(Entity e) { return __component_array__<C>()->get(e); }

    template<typename C>
    ComponentArray<C>* __component_array__()
    {
        type_index typeId = typeid(C);

        if (component_arrays.find(typeId) == component_arrays.end())
        {
            component_arrays[typeId] = make_unique<ComponentArray<C>>();
            remove_functions[typeId] = [this](Entity e) { remove_component<C>(e); };
        }

        return static_cast<ComponentArray<C>*>(component_arrays[typeId].get());
    }

    template <typename S, typename... Args>
    void attach(Args&&... args) { systems.emplace_back(make_unique<S>(std::forward<Args>(args)...)); }

    template<typename C, typename Func>
    void foreach(Func&& func)
    {
        auto* component_array = __component_array__<C>();

        for (auto& [entity, index] : component_array->entities)
        { func(entity, component_array->components[index]); }
    }
};

inline ECS ecs;
