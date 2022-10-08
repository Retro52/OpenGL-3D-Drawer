//
// Created by Anton on 06.09.2022.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#define GLEW_STATIC

#include "glew.h"
#include "../Core/ResourcesManager.h"
#include "../Core/Window.h"
#include "../Entity/Components.h"
#include "../Entity/Entity.h"
#include "UBO.hpp"


class Renderer
{
public:

    Renderer() = delete;
    Renderer(Renderer&& other) = delete;
    Renderer(const Renderer& other) = delete;

    /**
     * Initializes renderer
     */
    static void Initialize()
    {
        lightMatricesUBO = std::make_unique<UBO<glm::mat4x4, 16>>();
    }

    /**
     * Applies scene data to the shader
     * @param scene scene to render
     * @param drawMode global drawMode, see Global class for reference
     */
    static void Prepare(Scene& scene, int drawMode)
    {
        std::shared_ptr<Shader>& mShader = ResourcesManager::GetShader("mainShader");
        auto& cameraComponent = scene.GetPrimaryCamera().GetComponent<CameraComponent>();
        auto& cameraTransform = scene.GetPrimaryCamera().GetComponent<TransformComponent>();

        cameraComponent.UpdateCamera(cameraTransform.rotation);
        glm::mat4 cameraView = cameraComponent.GetCameraView(cameraTransform.translation);

        mShader->Use();
        mShader->setInt("drawMode", drawMode);
        mShader->setMat4("view", cameraView);
        mShader->setMat4("projection", cameraComponent.GetCameraProjection());
        mShader->setVec3("ProjPos", cameraTransform.translation);

        try
        {
            auto sceneDirLight = scene.GetDirectionalLight();
            const auto& dirLight = sceneDirLight.GetComponent<DirectionalLightComponent>().directionalLight;
            const auto& dlRotation = scene.GetDirectionalLight().GetComponent<TransformComponent>().rotation;

            std::vector<glm::mat4> lightMatrices = getLightSpaceMatrices(cameraComponent.camera.GetNearPlane(), cameraComponent.camera.GetFarPlane(), cameraComponent.camera.GetFieldOfView(), DirectionalLight::GetDirection(dlRotation), cameraView, cascadeLevels);

            mShader->setDirLight(dirLight, dlRotation);
            mShader->setBool("dirLight.isPresent", true);

            lightMatricesUBO->Bind();
            lightMatricesUBO->FillData(lightMatrices);
            lightMatricesUBO->Reset();

            mShader->setInt("cascadeCount", (int) cascadeLevels.size());
            mShader->setFloat("farPlane", cameraComponent.camera.GetFarPlane());

            for (size_t i = 0; i < cascadeLevels.size(); ++i)
            {
                mShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", cascadeLevels[i]);
            }
        }
        catch(const InGameException& e)
        {
            mShader->setBool("dirLight.isPresent", false);
            LOG(WARNING) << "Failed to bind directional light. Reason: " << e.what();
        }

        int idx = -1;
        const auto& view = scene.registry.view<TransformComponent, PointLightComponent>();
        for (const auto& entity : view)
        {
            idx++;
            auto [t, p] = view.get<TransformComponent, PointLightComponent>(entity);
            mShader->setPointLight(idx, p.pointLight, t.translation);
        }
        mShader->setInt("pointLightsCount", idx + 1);

        glClearColor(0.203f, 0.76f, 0.938f,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /**
     * Renders scene to the color buffer
     * @param scene scene to render
     * @param shadowMap directional light rendered shadow map
     */
    static void Render(Scene& scene, const unsigned int shadowMap)
    {
        const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
        auto& shader = ResourcesManager::GetShader("mainShader");
        shader->Use();
        for (const auto& entity : view)
        {
            auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);
            shader->Use();
            shader->setBool("material.shouldBeLit", m.shouldBeLit);
            shader->setInt("material.tilingFactor", m.tilingFactor);
            m.model.Draw(* shader, t.GetTransform(), shadowMap);
        }
    }

    /**
     * Renders scene to the depth buffer
     * @param scene scene to render
     */
    static void RenderToDepthBuffer(Scene& scene)
    {
        auto& shader = ResourcesManager::GetShader("shadowShader");
        const auto& view = scene.registry.view<TransformComponent, Model3DComponent>();
        shader->Use();
        try
        {
            shader->setVec3("lightDir", DirectionalLight::GetDirection(scene.GetDirectionalLight().GetComponent<TransformComponent>().rotation));
        }
        catch(const InGameException& e)
        {
            LOG(WARNING) << "Failed to set shadow shader light direction. Reason: " << e.what();
        }
        for (const auto& entity : view)
        {
            auto [t, m] = view.get<TransformComponent, Model3DComponent>(entity);
            if(m.castsShadow)
            {
                m.model.DrawIntoDepth(* shader, t.GetTransform());
            }
        }
    }
private:
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
private:
    static std::unique_ptr<UBO<glm::mat4x4, 16>> lightMatricesUBO;
    static std::vector<float> cascadeLevels;

};


#endif //GRAPHICS_RENDERER_H
