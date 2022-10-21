//
// Created by Anton on 05.09.2022.
//

#include "Scene.h"
#include "Entity.h"

#include "Components.h"
#include "JsonSceneSerializer.hpp"
#include "json.hpp"
#include "../Render/Renderer.h"
#include "../Input/EventsHandler.h"


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
            const auto& engineDefaultComponent = components["Engine default"];
            const auto& directionalLightComponent = components["Directional light"];
            if (components.contains("Transform"))
            {
                e.GetComponent<TransformComponent>().scale       = glm::vec3(glm::vec3(transformComponent["Scale"][0], transformComponent["Scale"][1], transformComponent["Scale"][2]));
                e.GetComponent<TransformComponent>().rotation    = glm::radians(glm::vec3(transformComponent["Rotation"][0], transformComponent["Rotation"][1], transformComponent["Rotation"][2]));
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
                e.GetComponent<CameraComponent>().camera.aspectRatio = glm::vec2(cameraComponent["Aspect"][0], cameraComponent["Aspect"][1]);
                LOG(INFO) << "Camera component successfully loaded for " << model.key();
            }

            if (components.contains("Model3D"))
            {
                auto& component = e.AddComponent<Model3DComponent>(model3Component["Path"]);
                try
                {
                    component.castsShadow  = model3Component["castsShadow"];
                    component.shouldBeLit  = model3Component["shouldBeLit"];
                    component.tilingFactor = model3Component["tilingFactor"];
                    LOG(INFO) << "Model component successfully loaded for " << model.key();
                }
                catch(std::exception& e)
                {
                    LOG(WARNING) << "Failed to load Model3D component" << e.what();
                }
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
                glm::vec3 spec(directionalLightComponent["Specular"][0], directionalLightComponent["Specular"][1], directionalLightComponent["Specular"][2]);

                e.AddComponent<DirectionalLightComponent>(amb, diff, spec);

                LOG(INFO) << "Directional light component successfully loaded for " << model.key();
            }

            if (components.contains("Engine default"))
            {
                e.AddComponent<EngineDefaultComponent>(engineDefaultComponent["Type"]);
                LOG(INFO) << "Engine default component successfully loaded for " << model.key();
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

Entity Scene::GetSelectedEntity(int& index)
{
    auto view = registry.view<TransformComponent>(entt::exclude<CameraComponent, EngineDefaultComponent>);
    int counter = 0;
    entt::entity last;
    for(const auto& entity : view)
    {
        if (index == counter)
        {
            return { entity, this };
        }
        counter++;
        last = entity;
    }

    if (index < 0)
    {
        index = counter - 1;
        return { last, this };
    }

    index = 0;
    return { * view.begin(), this };
}

Entity Scene::GetEngineDefault(EngineDefaultTypes target)
{
    auto view = registry.view<EngineDefaultComponent>();
    for (const auto &item: view)
    {
        if (view.get<EngineDefaultComponent>(item).type == target)
        {
            return { item, this };
        }
    }
    throw InGameException("Engine default type " + std::to_string(target) + " is not registered");
}

void Scene::OnUpdate(double deltaTime)
{
    if(!EventsHandler::_cursor_locked)
    {
        return;
    }

    float speed = 10.0f;

    const glm::vec3 up(0, 1, 0);
    const glm::vec3 right(1, 0, 0);
    const glm::vec3 front(0, 0, 1);

    const float mouseSensitivity = 150.0f;

    auto& t = GetPrimaryCamera().GetComponent<TransformComponent>();
    auto& c = GetPrimaryCamera().GetComponent<CameraComponent>();
    
    if (EventsHandler::IsPressed(Key::LeftShift))
    {
        speed *= 10;
    }
    else if (EventsHandler::IsPressed(Key::LeftControl))
    {
        speed /= 10;
    }

    if (EventsHandler::IsPressed(Key::W))
    {
        t.translation += static_cast<float>(deltaTime) * speed * c.camera.GetFrontVector();
    }
    if (EventsHandler::IsPressed(Key::S))
    {
        t.translation -= static_cast<float>(deltaTime) * speed * c.camera.GetFrontVector();
    }
    if (EventsHandler::IsPressed(Key::D) && !EventsHandler::IsPressed(Key::LeftAlt))
    {
        t.translation += static_cast<float>(deltaTime) * speed * c.camera.GetRightVector();
    }
    if (EventsHandler::IsPressed(Key::A))
    {
        t.translation -= static_cast<float>(deltaTime) * speed * c.camera.GetRightVector();
    }
    if (EventsHandler::IsPressed(Key::Q))
    {
        t.translation += static_cast<float>(deltaTime) * speed * c.camera.GetUpVector();
    }
    if (EventsHandler::IsPressed(Key::E))
    {
        t.translation -= static_cast<float>(deltaTime) * speed * c.camera.GetUpVector();
    }

    /* PerspectiveCamera world orientation */
    if (EventsHandler::_cursor_locked)
    {
        t.rotation.x = glm::clamp(static_cast<float>(t.rotation.x - EventsHandler::deltaY * deltaTime * mouseSensitivity / (float) Renderer::GetFboHeight() * 2),
                                  - glm::radians(89.0f),
                                  glm::radians(89.0f));
        t.rotation.y += static_cast<float>(- EventsHandler::deltaX * deltaTime * mouseSensitivity / (float) Renderer::GetFboWidth() * 2);
        c.camera.Update(t.rotation);
    }
}

Entity Scene::GetSelectedEntity()
{
    return GetSelectedEntity(idx);
}

Entity Scene::CopyEntity(const Entity &source)
{
    Entity newEntity = CreateEntity(source.GetComponent<NameComponent>().name + "_copy");
    if (source.HasComponent<TransformComponent>())
    {
        newEntity.GetComponent<TransformComponent>() = source.GetComponent<TransformComponent>();
    }
    if (source.HasComponent<Model3DComponent>())
    {
        newEntity.AddComponent<Model3DComponent>(source.GetComponent<Model3DComponent>());
    }

    if (source.HasComponent<PointLightComponent>())
    {
        newEntity.AddComponent<PointLightComponent>(source.GetComponent<PointLightComponent>());
    }

    if (source.HasComponent<DirectionalLightComponent>())
    {
        newEntity.AddComponent<DirectionalLightComponent>(source.GetComponent<DirectionalLightComponent>());
    }

    if (source.HasComponent<EngineDefaultComponent>())
    {
        newEntity.AddComponent<EngineDefaultComponent>(source.GetComponent<EngineDefaultComponent>());
    }

    return newEntity;
}

void Scene::DeleteEntity(Entity &entity)
{
    registry.destroy(entity);
}
