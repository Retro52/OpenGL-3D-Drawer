//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_COMPONENTS_H
#define GRAPHICS_COMPONENTS_H

#include <utility>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../Core/PerspectiveCamera.h"
#include "../Core/UniqueID.hpp"
#include "../Render/Model.h"

struct TransformComponent
{
    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    explicit TransformComponent(const glm::vec3& translation) : translation(translation) {}

    [[nodiscard]] inline glm::mat4 GetTransform() const
    {
        constexpr float sizeMultiplier = 1.0f;
        return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale * sizeMultiplier);
    }

    glm::vec3 scale       = { 1.0f, 1.0f, 1.0f };
    glm::vec3 rotation    = { 0.0f, 0.0f, 0.0f };
    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
};

struct NameComponent
{
    NameComponent() = default;
    NameComponent(const NameComponent&) = default;
    explicit NameComponent(std::string name = "Entity") : name(std::move(name)) {}

    std::string name;
    UniqueID id;
};

struct CameraComponent
{
    explicit CameraComponent() : camera(PerspectiveCamera(glm::radians(60.0f))) {};
    explicit CameraComponent(float FOV) : camera(PerspectiveCamera(FOV)) {};
    CameraComponent(const CameraComponent&) = default;

    ~CameraComponent() = default;

    /**
     * Updates camera
     * @param rotation camera rotation
     */
    inline void UpdateCamera(const glm::vec3& rotation) { return camera.Update(rotation); }

    [[nodiscard]] inline glm::mat4 GetCameraProjection() const { return camera.GetProjection(); }

    [[nodiscard]] inline glm::mat4 GetCameraView(const glm::vec3& position) { return camera.GetView(position); }

    PerspectiveCamera camera;
    bool isPrimary = false;
};


struct Model3DComponent
{
    Model3DComponent() = default;
    Model3DComponent(const std::string& modelPath) : model(modelPath) {};

    int tilingFactor = 1;

    bool castsShadow = true;
    bool shouldBeLit = true;

    Model model;
};

/* TODO: remove direction from DirectionalLight, calculate using rotation from TransformComponent */
struct DirectionalLightComponent
{
    DirectionalLightComponent() : directionalLight(DirectionalLight(glm::vec3(0.5), glm::vec3(0.5), glm::vec3(0.5))) {};

    DirectionalLightComponent(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec)
        : directionalLight(amb, diff, spec) {};
    ~DirectionalLightComponent() = default;

    DirectionalLight directionalLight;
};

struct PointLightComponent
{
    PointLightComponent() : pointLight(PointLight(glm::vec3(0.5), glm::vec3(0.5), glm::vec3(0.5), 0.1, 0.1, 0.1)) {};
    PointLightComponent(const glm::vec3 &amb, const glm::vec3 &diff, const glm::vec3 &spec, float con, float lin, float quad)
        : pointLight(PointLight(amb, diff, spec, con, lin, quad)) {};

    PointLightComponent(const PointLightComponent& ) = default;
    ~PointLightComponent() = default;


    PointLight pointLight;
};

enum EngineDefaultTypes
{
    Axes
};

struct EngineDefaultComponent
{
    EngineDefaultComponent() = delete;
    explicit EngineDefaultComponent(EngineDefaultTypes type) : type(type) {};
    EngineDefaultTypes type;
};

#endif //GRAPHICS_COMPONENTS_H
