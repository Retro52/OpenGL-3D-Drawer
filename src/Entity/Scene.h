//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include "../vendors/include/entt/entt.hpp"
#include "Components.h"
#include "../Render/Material.h"

class Entity;

class Scene
{
public:
    explicit Scene(const std::string& path);
    ~Scene();

    Entity CreateEntity(const std::string& name = "");

    Entity CopyEntity(const Entity& source);

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

    Entity GetPrimaryCamera();
    Entity GetDirectionalLight();

    void OverridePrimaryCamera(const Entity& entity);

    void SaveScene(const std::string& savePath);

    [[nodiscard]] Entity GetSelectedEntity();
    [[nodiscard]] Entity GetSelectedEntity(int& index);

    [[nodiscard]] Entity GetEngineDefault(EngineDefaultTypes type);

    void OnUpdate(double deltaTime);
private:
    void LoadScene(const std::string& path);

    std::string path;
    entt::registry registry;

    int idx = 0;

    friend class Entity;
    friend class Renderer;
    friend class JsonSceneSerializer;
};

#endif //GRAPHICS_SCENE_H
