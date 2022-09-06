//
// Created by Anton on 05.09.2022.
//

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "../Loaders/json.hpp"
#include "../Render/Renderer.h"
#include "../Logging/easylogging++.h"


Scene::Scene(const std::string &path) : path(path)
{
    LoadScene(path);
}

Scene::~Scene()
{
    SaveScene(path);
}

void Scene::LoadScene(const std::string &loadPath)
{
    std::ifstream is(loadPath);
    GAME_ASSERT(is.is_open(), "Failed to open scene: " + loadPath);

    json data = json::parse(is);
    GAME_ASSERT(!data.empty(), "Failed to open scene: " + loadPath);

    for (const auto& model : data["Models"].items())
    {
        try
        {
            Entity e = CreateEntity(model.key());
            e.GetComponent<TransformComponent>().scale = glm::vec3(glm::vec3(model.value()["Scale"][0], model.value()["Scale"][1], model.value()["Scale"][2]));
            e.GetComponent<TransformComponent>().rotation = glm::vec3(glm::vec3(model.value()["Rotation"][0], model.value()["Rotation"][1], model.value()["Rotation"][2]));
            e.GetComponent<TransformComponent>().translation = glm::vec3(glm::vec3(model.value()["Position"][0], model.value()["Position"][1], model.value()["Position"][2]));

            e.AddComponent<Model3DComponent>(model.value()["Path"]);
            LOG(INFO) << "Model " << model.key() << " loaded";
        }
        catch (InGameException& e)
        {
            LOG(WARNING) << "Failed to load model " << model.key() << " . Reason: " << e.what();
        }
    }
}

Entity Scene::CreateEntity(const std::string& name)
{
    Entity e = { registry.create(), this };
    e.AddComponent<TransformComponent>();
    e.AddComponent<NameComponent>(name);
    return e;
}


CameraComponent Scene::GetPrimaryCamera() const
{
    auto view = registry.view<CameraComponent>();
    for (auto entity : view)
    {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.isPrimary)
        {
            return camera;
        }
    }
    throw InGameException("There are no primary cameras");
}


void Scene::Render() const
{
    Renderer::Prepare(GetPrimaryCamera().camera);

    auto view = registry.view<TransformComponent, Model3DComponent>();

    for (const auto& entity : view)
    {
        auto [transform, model] = view.get<TransformComponent, Model3DComponent>(entity);
        Renderer::Render(transform, model);
    }
}

/** @not_implemented yet */
void Scene::OverridePrimaryCamera(Entity entity)
{
    GAME_ASSERT(entity.HasComponent<CameraComponent>(), "Camera was not overridden. Reason: entity does not have CameraComponent");
    auto view = registry.view<CameraComponent>();
    for (const auto& entt : view)
    {
        auto camera = view.get<CameraComponent>(entt);
        if (camera.isPrimary)
        {
            camera.isPrimary = false;
        }
    }
}

std::vector<PointLightComponent> Scene::GetPointLights()
{
    std::vector<PointLightComponent> pLights;
    pLights.reserve(16);

    auto view = registry.view<PointLightComponent>();

    for (const auto& entity : view)
    {
        const auto& pLight = view.get<PointLightComponent>(entity);
        pLights.emplace_back(pLight);
    }
    return pLights;
}

