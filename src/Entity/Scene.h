//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include "entt/entt.hpp"
#include "Components.h"
#include "../Render/Material.h"

class Entity;

class Scene
{
public:
    Scene() = default;
    explicit Scene(const std::string& path);
    ~Scene();

    /**
     * Creates new Entity in this scene
     * @param name entity name
     * @return created entity
     */
    Entity CreateEntity(const std::string& name = "");

    /**
     * Copies entity, which already exists
     * @param source entity to copy from
     * @return copied entity
     */
    Entity CopyEntity(const Entity& source);

    /**
     * Deletes entity and frees all the resources it holds
     * @param entity Entity reference to delete
     */
    void DeleteEntity(Entity& entity);

    /**
     * @return scene primary camera. Several primaries leads to UB
     */
    std::unique_ptr<Entity> GetPrimaryCamera();

    /**
     * @return scene directional light. Several directional lights leads to UB
     */
    std::unique_ptr<Entity> GetDirectionalLight();

    /**
     * Saves current scene
     * @param savePath path to save scene by
     */
    void SaveScene(const std::string& savePath);

    /**
     * Called every frame
     * @param deltaTime global delta time
     */
    void OnUpdate(double deltaTime);
private:
    /**
     * Loads scene
     * @param path scene path
     */
    void LoadScene(const std::string& path);

    std::string path;
    entt::registry registry;

    int idx = 0;

    friend class Entity;
    friend class Renderer;
    friend class EditorLayer;
    friend class JsonSceneSerializer;
};

#endif //GRAPHICS_SCENE_H
