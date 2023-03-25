//
// Created by Anton on 05.09.2022.
//

#ifndef GRAPHICS_COMPONENTS_H
#define GRAPHICS_COMPONENTS_H

#include <utility>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../Core/Camera.h"
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
    explicit CameraComponent() : camera(Camera(glm::radians(60.0f))) {};
    explicit CameraComponent(float FOV) : camera(Camera(FOV)) {};
    CameraComponent(const CameraComponent&) = default;

    ~CameraComponent() = default;

    /**
     * Updates camera
     * @param rotation camera rotation
     */
    inline void UpdateCamera(const glm::vec3& rotation) { return camera.Update(rotation); }

    [[nodiscard]] inline glm::mat4 GetCameraProjection() const { return camera.GetProjection(); }
    [[nodiscard]] inline glm::mat4 GetCameraInfiniteProjection() const { return camera.GetInfiniteProjection(); }

    [[nodiscard]] inline glm::mat4 GetCameraView(const glm::vec3& position) { return camera.GetView(position); }

    Camera camera;
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

// TODO: remake
struct SkyBoxComponent
{
    SkyBoxComponent()
    {
        static const float skyboxVertices[] =
        {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    }
    ~SkyBoxComponent() = default;

    [[nodiscard]] unsigned int GetTexture() const
    {
        return cubeMapTexture->GetId();
    }

    void Draw() const
    {
        if(!cubeMapTexture)
        {
            return;
        }

        GLint prevDepthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        cubeMapTexture->Bind();

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(prevDepthFunc); // set depth function back to default
    }

    unsigned int GetCubeTextureId()
    {
        if(cubeMapTexture)
        {
            return cubeMapTexture->GetId();
        }
        return 0;
    }

    void Reload()
    {
        cubeMapTexture = std::make_shared<CubeMap>( std::array<std::string, 6> (
                {
                        rightTexture,
                        leftTexture,
                        topTexture,
                        bottomTexture,
                        frontTexture,
                        backTexture
                }
                ));
    }

    std::string topTexture;
    std::string bottomTexture;

    std::string leftTexture;
    std::string rightTexture;

    std::string backTexture;
    std::string frontTexture;

    std::shared_ptr<CubeMap> cubeMapTexture;
private:
    unsigned int skyboxVAO{}, skyboxVBO{};
};

#endif //GRAPHICS_COMPONENTS_H
