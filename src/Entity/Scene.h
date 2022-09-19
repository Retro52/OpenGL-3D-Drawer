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

    void DeleteEntity(const std::string& name)
    {
        const auto& view = registry.view<NameComponent>();
        for(const auto& entity : view)
        {
            /* temp */
            const auto& n = view.get<NameComponent>(entity);
            if (n.name == name)
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
        std::cerr << "Entity not found\n";
    }

    Entity GetPrimaryCamera();
    Entity GetDirectionalLight();

    void OverridePrimaryCamera(const Entity& entity);

    void SaveScene(const std::string& savePath);

private:
    void LoadScene(const std::string& path);

    std::string path;
    entt::registry registry;

    friend class Entity;
    friend class Renderer;
    friend class JsonSceneSerializer;
};

#endif //GRAPHICS_SCENE_H
