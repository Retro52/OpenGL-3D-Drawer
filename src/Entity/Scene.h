//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include "../include/entt/entt.hpp"
#include "Components.h"

class Entity;

class Scene
{
public:
    Scene(const std::string& path);
    ~Scene();

    Entity CreateEntity(const std::string& name = "");

    Entity GetPrimaryCamera();
    Entity GetDirectionalLight();

    void OverridePrimaryCamera(Entity entity);

    void OnUpdate(float deltaTime);

    void Render();
    void SaveScene(const std::string& savePath);

private:
    void LoadScene(const std::string& path);
private:
    std::string path;
    entt::registry registry;
    friend class Entity;
    friend class Renderer;
    friend class JsonSceneSerializer;
};


#endif //GRAPHICS_SCENE_H
