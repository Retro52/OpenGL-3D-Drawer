//
// Created by Anton on 06.09.2022.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#define GLEW_STATIC

#include "../include/OpenGL/include/GLEW/glew.h"
#include "../Core/ResourcesManager.h"
#include "../Core/Window.h"
#include "../Entity/Components.h"
#include "../Entity/Entity.h"
#include "UBO.h"


class Renderer
{
public:
    static void Initialize()
    {
        glGenBuffers(1, &shadowsUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, shadowsUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, shadowsUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    static void Prepare(Scene& scene, int drawMode)
    {
        // TODO: change this
        Shader mShader = * ResourcesManager::GetShader("mainShader");
        auto& pCamera = scene.GetPrimaryCamera().GetComponent<CameraComponent>().camera;
        auto& cameraTransform = scene.GetPrimaryCamera().GetComponent<TransformComponent>();

        pCamera.position = cameraTransform.translation;

        mShader.Use();
        mShader.setInt("drawMode", drawMode);
        mShader.setMat4("view", pCamera.GetView());
        mShader.setVec3("ProjPos", cameraTransform.translation);
        mShader.setMat4("projection", pCamera.GetProjection());
        mShader.setDirLight(scene.GetDirectionalLight().GetComponent<DirectionalLightComponent>().directionalLight);
//        mShader.setPointLights(scene.GetPointLights());


        std::vector<float> cascadeLevels({ pCamera.farPlane / 50.0f, pCamera.farPlane / 25.0f, pCamera.farPlane / 10.0f, pCamera.farPlane / 2.0f });
        std::vector<glm::mat4> lightMatrices = getLightSpaceMatrices(pCamera.nearPlane, pCamera.farPlane, pCamera.fov, scene.GetDirectionalLight().GetComponent<DirectionalLightComponent>().directionalLight.direction, pCamera.GetView(), cascadeLevels);

        glBindBuffer(GL_UNIFORM_BUFFER, shadowsUBO);
        for (size_t i = 0; i < lightMatrices.size(); ++i)
        {
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        mShader.setFloat("farPlane", pCamera.farPlane);
        mShader.setInt("cascadeCount", cascadeLevels.size());
        for (size_t i = 0; i < cascadeLevels.size(); ++i)
        {
            mShader.setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", cascadeLevels[i]);
        }

        glClearColor(0.203f, 0.76f, 0.938f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    }
    static void Render(Scene& scene, const unsigned int shadowMap)
    {
        ResourcesManager::GetShader("mainShader")->Use();
        const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
        for (const auto& entity : view)
        {
            auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);
            m.model.Draw(* ResourcesManager::GetShader("mainShader"), t.GetTransform(), shadowMap);
        }
    }

    static void RenderToDepthBuffer(Scene& scene)
    {
        ResourcesManager::GetShader("shadowShader")->Use();
        const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
        for (const auto& entity : view)
        {
            auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);
            m.model.DrawIntoDepth(* ResourcesManager::GetShader("shadowShader"), t.GetTransform());
        }
    }
private:
    static unsigned int shadowsUBO;

    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
    {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x)
        {
            for (unsigned int y = 0; y < 2; ++y)
            {
                for (unsigned int z = 0; z < 2; ++z)
                {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }


    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
    {
        return getFrustumCornersWorldSpace(proj * view);
    }

    static glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, const float zoom, const glm::vec3& lightDir, const glm::mat4& viewMatrix)
    {
        const auto proj = glm::perspective(zoom, Window::GetAspectRatio(), nearPlane, farPlane);
        const auto corners = getFrustumCornersWorldSpace(proj, viewMatrix);

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners)
        {
            center += glm::vec3(v);
        }
        center /= corners.size();

        const auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();
        for (const auto& v : corners)
        {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0)
        {
            minZ *= zMult;
        }
        else
        {
            minZ /= zMult;
        }
        if (maxZ < 0)
        {
            maxZ /= zMult;
        }
        else
        {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

        return lightProjection * lightView;
    }

    static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, const float zoom, const glm::vec3& lightDir, const glm::mat4& viewMatrix, std::vector<float>& shadowCascadeLevels)
    {
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
        {
            if (i == 0)
            {
                ret.push_back(getLightSpaceMatrix(cameraNearPlane, shadowCascadeLevels[i], zoom, lightDir, viewMatrix));
            }
            else if (i < shadowCascadeLevels.size())
            {
                ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i], zoom, lightDir, viewMatrix));
            }
            else
            {
                ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], cameraFarPlane, zoom, lightDir, viewMatrix));
            }
        }
        return ret;
    }
};


#endif //GRAPHICS_RENDERER_H
