//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_ENTITY_H
#define GRAPHICS_ENTITY_H

#include "../Core/InGameException.h"
#include "../include/entt/entt.hpp"
#include "Scene.h"

class Entity
{
public:
    Entity() = default;
    Entity(const Entity& other) = default;
    Entity(entt::entity entity, Scene * scene);

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) const
    {
        return scene->registry.template emplace<T>(thisEntity, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent() const
    {
        GAME_ASSERT(HasComponent<T>(), "Trying to get unregistered component");
        return scene->registry.template get<T>(thisEntity);
    }

    template<typename T>
    bool HasComponent() const
    {
        return scene->registry.all_of<T>(thisEntity);
    }

    template<typename T>
    void RemoveComponent() const
    {
        GAME_ASSERT(HasComponent<T>(), "Trying to remove unregistered component");
        scene->registry.template remove<T>(thisEntity);
    }

    explicit operator bool() const { return (std::uint32_t) thisEntity != 0; }
    explicit operator entt::entity() const { return thisEntity; }
private:
     entt::entity thisEntity { 0 };
     Scene * scene { nullptr };
};


#endif //GRAPHICS_ENTITY_H
