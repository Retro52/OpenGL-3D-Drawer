//
// Created by Anton on 05.09.2022.
//

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "JsonSceneSerializer.hpp"
#include "json.hpp"
#include "../Render/Renderer.h"
#include "../Logging/easylogging++.h"


Scene::Scene(const std::string &path) : path(path)
{
    LoadScene(path);
}

Scene::~Scene()
{
    try
    {
        SaveScene(path);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to save scene. Reason " << e.what();
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

void Scene::LoadScene(const std::string &loadPath)
{
    std::ifstream is(loadPath);
    GAME_ASSERT(is.is_open(), "Failed to open scene: " + loadPath);

    json data = json::parse(is);
    GAME_ASSERT(!data.empty(), "Failed to open scene: " + loadPath);

    if (!data["Name"].is_null())
    {
        LOG(INFO) << "Start loading scene " << data["Name"];
    }
    else
    {
        LOG(WARNING) << "Scene '" << path << "' does not have name";
    }
    for (const auto& model : data["Entities"].items())
    {
        try
        {
            Entity e = CreateEntity();

            const auto& components = model.value();

            const auto& nameComponent = components["Name"];
            const auto& cameraComponent = components["Camera"];
            const auto& model3Component = components["Model3D"];
            const auto& transformComponent = components["Transform"];
            const auto& pointLightComponent = components["Point light"];
            const auto& directionalLightComponent = components["Directional light"];

            if (components.contains("Transform"))
            {
                e.GetComponent<TransformComponent>().scale       = glm::vec3(glm::vec3(transformComponent["Scale"][0], transformComponent["Scale"][1], transformComponent["Scale"][2]));
                e.GetComponent<TransformComponent>().rotation    = glm::vec3(glm::vec3(transformComponent["Rotation"][0], transformComponent["Rotation"][1], transformComponent["Rotation"][2]));
                e.GetComponent<TransformComponent>().translation = glm::vec3(glm::vec3(transformComponent["Position"][0], transformComponent["Position"][1], transformComponent["Position"][2]));
                LOG(INFO) << "Transform component successfully loaded for " << model.key();
            }
            else
            {
                LOG(WARNING) << "Model " << model.key() << " missing transform component";
            }

            if (components.contains("Name"))
            {
                e.GetComponent<NameComponent>().name = nameComponent["Name"];
                LOG(INFO) << "Name component successfully loaded for " << model.key();
            }
            else
            {
                LOG(WARNING) << "Model " << model.key() << " missing name component";
            }


            if (components.contains("Camera"))
            {
                e.AddComponent<CameraComponent>(cameraComponent["FOV"]);
                e.GetComponent<CameraComponent>().isPrimary = cameraComponent["isPrimary"];
                LOG(INFO) << "Camera component successfully loaded for " << model.key();
            }

            if (components.contains("Model3D"))
            {
                e.AddComponent<Model3DComponent>(model3Component["Path"]);
                LOG(INFO) << "Model component successfully loaded for " << model.key();
            }

            if (components.contains("Point light"))
            {
                glm::vec3 amb(pointLightComponent["Ambient"][0], pointLightComponent["Ambient"][1], pointLightComponent["Ambient"][2]);
                glm::vec3 diff(pointLightComponent["Diffuse"][0], pointLightComponent["Diffuse"][1], pointLightComponent["Diffuse"][2]);
                glm::vec3 spec(pointLightComponent["Specular"][0], pointLightComponent["Specular"][1], pointLightComponent["Specular"][2]);
                float con  = pointLightComponent["Constant"];
                float lin  = pointLightComponent["Linear"];
                float quad = pointLightComponent["Quadratic"];

                e.AddComponent<PointLightComponent>(amb, diff, spec, con, lin, quad);

                LOG(INFO) << "Point light component successfully loaded for " << model.key();
            }
            if (components.contains("Directional light"))
            {
                glm::vec3 amb(directionalLightComponent["Ambient"][0], directionalLightComponent["Ambient"][1], directionalLightComponent["Ambient"][2]);
                glm::vec3 diff(directionalLightComponent["Diffuse"][0], directionalLightComponent["Diffuse"][1], directionalLightComponent["Diffuse"][2]);
                glm::vec3 dir(directionalLightComponent["Direction"][0], directionalLightComponent["Direction"][1], directionalLightComponent["Direction"][2]);
                glm::vec3 spec(directionalLightComponent["Specular"][0], directionalLightComponent["Specular"][1], directionalLightComponent["Specular"][2]);

                e.AddComponent<DirectionalLightComponent>(dir, diff, amb, spec);

                LOG(INFO) << "Directional light component successfully loaded for " << model.key();
            }

            LOG(INFO) << "Entity " << model.key() << " loaded";
        }
        catch (std::exception& e)
        {
            LOG(WARNING) << "Failed to load model " << model.key() << " . Reason: " << e.what();
        }
    }
}


void Scene::SaveScene(const std::string& savePath)
{
    JsonSceneSerializer::SaveScene(savePath, this);
}
