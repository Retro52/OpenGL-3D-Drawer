//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_ENTITY_H
#define GRAPHICS_ENTITY_H

#include <utility>

#include "../Core/InGameException.h"
#include "entt/entt.hpp"
#include "Scene.h"

class Entity
{
public:
    Entity() = delete;
    Entity(const Entity& other) = default;
    Entity(entt::entity entity, Scene * base) : thisEntity(entity), scene(base) {}

    /**
     * Adds new component to the entity
     * @tparam T Component class
     * @param args arguments for component constructor
     * @return added component
     */
    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) const
    {
        GAME_ASSERT(!HasComponent<T>(), "Trying to add already registered component");
        return scene->registry.template emplace<T>(thisEntity, std::forward<Args>(args)...);
    }

    /**
     * Returns requested component. If entity does not own requested component, throws exception
     * @tparam T component class
     * @return requested component
     */
    template<typename T>
    [[nodiscard]] T& GetComponent() const
    {
        GAME_ASSERT(HasComponent<T>(), "Trying to get unregistered component");
        return scene->registry.template get<T>(thisEntity);
    }


    /**
     * Checks if entity has the component
     * @tparam T component class
     * @return true if entity owns the component, false otherwise
     */
    template<typename T>
    [[nodiscard]] bool HasComponent() const
    {
        return scene->registry.all_of<T>(thisEntity);
    }

    /**
     * Deletes component from entity
     * @tparam T component class
     */
    template<typename T>
    void RemoveComponent() const
    {
        GAME_ASSERT(HasComponent<T>(), "Trying to remove unregistered component");
        scene->registry.template remove<T>(thisEntity);
    }

    Entity CopyEntity()
    {
        return scene->CopyEntity(* this);
    }

    /**
     * Compares two entities
     * @param other Entity to compare with
     * @return if Entity.thisEntity ids are equal
     */
    bool operator == (const Entity& other)
    {
        return other.thisEntity == thisEntity;
    }

    /**
     * Compares two entities
     * @param other Entity to compare with
     * @return if Entity.thisEntity ids are equal
     */
    bool operator == (const entt::entity& other)
    {
        return other == thisEntity;
    }

    /**
     * Conversion operator from Entity class to entt:entity type
     * @return Entity.thisEntity
     */
    operator const entt::entity() { return thisEntity; }

    [[nodiscard]] inline entt::entity Get() const { return thisEntity; }
private:
     entt::entity thisEntity { 0 };
     Scene * scene;
};


#endif //GRAPHICS_ENTITY_H
