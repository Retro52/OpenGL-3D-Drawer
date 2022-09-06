//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_COMPONENTS_H
#define GRAPHICS_COMPONENTS_H

#include <utility>

#include "../include/OpenGL/include/glm/glm.hpp"
#include "../include/OpenGL/include/glm/gtc/matrix_transform.hpp"
#include "../include/OpenGL/include/glm/gtx/quaternion.hpp"
#include "../Core/PerspectiveCamera.h"
#include "../Render/Model.h"

struct TransformComponent
{
    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    explicit TransformComponent(const glm::vec3& translation) : translation(translation) {}

    [[nodiscard]] glm::mat4 GetTransform() const
    {
        return glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
    }

    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation    = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale       = { 1.0f, 1.0f, 1.0f };
};

struct NameComponent
{
    NameComponent(const NameComponent&) = default;
    explicit NameComponent(std::string name = "Entity") : name(std::move(name)) {}
    std::string name;
};

struct CameraComponent
{
    CameraComponent() : camera(PerspectiveCamera(glm::vec3(0), 60)) {};
    CameraComponent(const CameraComponent&) = default;

    PerspectiveCamera camera;
    bool isPrimary = true;
};


struct Model3DComponent
{
    Model3DComponent() = delete;
    Model3DComponent(const std::string& modelPath) : model(Model(modelPath)) {};

    Model model;
};

/* Absolutely useless TODO: think to move Shader from separate component to the Model3DComponent */
struct ShaderComponent
{
    ShaderComponent()  = default;
    ~ShaderComponent() = default;

    Shader shader;
};


struct DirectionalLightComponent
{
    DirectionalLightComponent() = default;
    ~DirectionalLightComponent() = default;

    DirectionalLight directionalLight;
};


struct PointLightComponent
{
    PointLightComponent() = default;
    ~PointLightComponent() = default;

    PointLight pointLight;
};
/* Maybe I will figure out how to make it useful instead of ...  */
struct ControllerComponent
{

};
#endif //GRAPHICS_COMPONENTS_H
