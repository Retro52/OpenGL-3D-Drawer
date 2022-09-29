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
     * @param id Entity id
     */
    void DeleteEntity(const UniqueID& id)
    {
        const auto& view = registry.view<NameComponent>();
        for(const auto& entity : view)
        {
            /* temp */
            const auto& n = view.get<NameComponent>(entity);
            if (n.id == id)
            {
                bool t = registry.any_of<Model3DComponent>(entity);
                registry.destroy(entity);
                if (t)
                {
                    Material::UnloadUnusedTextures();
                }
                return;
            }
        }
        throw InGameException("Entity #id" + std::to_string(id.operator unsigned long()) + " not found");
    }

    /**
     * @return scene primary camera. Several primaries leads to UB
     */
    Entity GetPrimaryCamera();

    /**
     * @return scene directional light. Several directional lights leads to UB
     */
    Entity GetDirectionalLight();

    /**
     * Saves current scene
     * @param savePath path to save scene by
     */
    void SaveScene(const std::string& savePath);

    /**
     * @return scene selected entity
     */
    [[nodiscard]] Entity GetSelectedEntity();

    /*
     * @param index entity index
     * @return entity by index
     */
    [[nodiscard]] Entity GetSelectedEntity(int& index);

    /**
     * @param type entity type
     * @return engine default entity
     */
    [[nodiscard]] Entity GetEngineDefault(EngineDefaultTypes type);

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
    friend class JsonSceneSerializer;
};

#endif //GRAPHICS_SCENE_H
