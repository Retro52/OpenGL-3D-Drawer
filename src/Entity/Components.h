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

#define all_components {TransformComponent, NameComponent};

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
    bool isPrimary = true;
};


struct Model3DComponent
{
    Model3DComponent() = delete;
    Model3DComponent(const std::string& modelPath) : model(Model(modelPath)) {};

    Model model;
};

/* TODO: remove direction from DirectionalLight, calculate using rotation from TransformComponent */
struct DirectionalLightComponent
{
    DirectionalLightComponent(const glm::vec3& dir, const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec)
        : directionalLight(dir, amb, diff, spec) {};
    ~DirectionalLightComponent() = default;

    DirectionalLight directionalLight;
};

/* TODO: remove position, we have Transform Component for that */
struct PointLightComponent
{
    PointLightComponent(const glm::vec3 &amb, const glm::vec3 &diff, const glm::vec3 &spec, float con, float lin, float quad)
        : pointLight(PointLight(amb, diff, spec, con, lin, quad)) {};
    ~PointLightComponent() = default;

    PointLight pointLight;
};

/* Maybe I will figure out how to make it useful */
struct ControllerComponent
{

};
#endif //GRAPHICS_COMPONENTS_H
