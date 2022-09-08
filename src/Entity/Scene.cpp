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
        catch (std::exception& e)
        {
            LOG(WARNING) << "Failed to load model " << model.key() << " . Reason: " << e.what();
        }
    }

    for (const auto& model : data["Point lights"].items())
    {
        try
        {
            Entity e = CreateEntity(model.key());

            glm::vec3 pos(model.value()["Position"][0], model.value()["Position"][1], model.value()["Position"][2]);
            glm::vec3 amb(model.value()["Ambient"][0], model.value()["Ambient"][1], model.value()["Ambient"][2]);
            glm::vec3 diff(model.value()["Diffuse"][0], model.value()["Diffuse"][1], model.value()["Diffuse"][2]);
            glm::vec3 spec(model.value()["Specular"][0], model.value()["Specular"][1], model.value()["Specular"][2]);
            float con = model.value()["Constant"];
            float lin = model.value()["Linear"];
            float quad = model.value()["Quadratic"];

            e.GetComponent<TransformComponent>().translation = pos;
            e.AddComponent<PointLightComponent>(pos, amb, diff, spec, con, lin, quad);

            LOG(INFO) << "Model " << model.key() << " loaded";
        }
        catch (std::exception& e)
        {
            LOG(WARNING) << "Failed to load model " << model.key() << " . Reason: " << e.what();
        }
    }

    try
    {
        const auto & dLightData = data["Directional light"];

        Entity e = CreateEntity("Directional light");

        glm::vec3 amb(dLightData["Ambient"][0], dLightData["Ambient"][1], dLightData["Ambient"][2]);
        glm::vec3 diff(dLightData["Diffuse"][0], dLightData["Diffuse"][1], dLightData["Diffuse"][2]);
        glm::vec3 dir(dLightData["Direction"][0], dLightData["Direction"][1], dLightData["Direction"][2]);
        glm::vec3 spec(dLightData["Specular"][0], dLightData["Specular"][1], dLightData["Specular"][2]);

        e.AddComponent<DirectionalLightComponent>(dir, diff, amb, spec);

        LOG(INFO) << "Directional light loaded";
    }
    catch(std::exception& e)
    {
        LOG(WARNING) << "Failed to load directional light. Reason: " << e.what();
    }

    try
    {
        const auto & pCameraData = data["Camera"];
        Entity e = CreateEntity("Directional light");

        e.AddComponent<CameraComponent>(glm::vec3(pCameraData["Position"][0], pCameraData["Position"][1], pCameraData["Position"][2]), pCameraData["FOV"]);
        e.GetComponent<CameraComponent>().isPrimary = pCameraData["isPrimary"];

        LOG(INFO) << "Camera loaded";
    }
    catch(std::exception& e)
    {
        LOG(WARNING) << "Failed to load camera. Reason: " << e.what();
    }
}

Entity Scene::CreateEntity(const std::string& name)
{
    Entity e = { registry.create(), this };
    e.AddComponent<TransformComponent>();
    e.AddComponent<NameComponent>(name);
    return e;
}


Entity Scene::GetPrimaryCamera()
{
    auto view = registry.view<CameraComponent>();
    for (auto entity : view)
    {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.isPrimary)
        {
            return { entity, this };
        }
    }
    throw InGameException("There are no primary cameras");
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

Entity Scene::GetDirectionalLight()
{
    auto view = registry.view<DirectionalLightComponent>();
    for (auto entity : view)
    {
        return { entity, this };
    }
    throw InGameException("There is no directional light");
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

void Scene::SaveScene(const std::string& path) const
{

}
