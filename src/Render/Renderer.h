//
// Created by Anton on 06.09.2022.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#define GLEW_STATIC

#include "glew.h"
#include <glfw3.h>
#include <chrono>
#include "../Core/Window.h"
#include "../Entity/Entity.h"
#include "../Entity/Components.h"
#include "../Core/ResourcesManager.h"
#include "UBO.hpp"
#include "FBO.hpp"

/**
 * Not implemented so far
 */
namespace LightingType
{
    enum Type : unsigned int
    {
        Dynamic = 0,
        Baked   = 1
    };
}

class Renderer
{
public:

    Renderer() = delete;
    Renderer(Renderer&& other) = delete;
    Renderer(const Renderer& other) = delete;

    /**
     * Initializes renderer
     */
    static void Initialize();

    /**
     * Shuts down renderer
     */
    static void ShutDown();

    /**
     * Applies scene data to the shader
     * @param scene scene to render
     * @param drawMode global drawMode, see Global class for reference
     */
    static void Prepare(Scene& scene);

    /**
     * Renders scene to the color buffer
     * @param scene scene to render
     * @param shadowMap directional light rendered shadow map
     */
    static void Render(Scene& scene);

    static void RenderQuad()
    {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    /**
     * Renders scene to the depth buffer
     * @param scene scene to render
     */
    static void RenderToDepthBuffer(Scene& scene);

    /**
     * Enables depth testing
     */
    static void EnableDepthTesting()
    {
        glEnable(GL_DEPTH_TEST);
    }

    /**
     * Disables depth testing
     */
    static void DisableDepthTesting()
    {
        glDisable(GL_DEPTH_TEST);
    }

    /**
     * Sets background color
     * @param newClearColor
     */
    static void SetClearColor(const glm::vec3& newClearColor)
    {
        clearColor = newClearColor;
    }

    /**
     * Clears background color and buffers
     */
    static void Clear()
    {
        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    static void Clear(const glm::vec3& customColor)
    {
        glClearColor(customColor.r, customColor.g, customColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    /**
     * @return rendered frame FBO without post process effects applied
     */
    static std::unique_ptr<FBO>& GetViewportFBO() { return viewportFBO; };

    /**
     * @return rendered frame FBO with post process applied
     */
    static std::unique_ptr<FBO>& GetPostProcessFBO() { return postProcessFBO; };

    static unsigned int GetRenderedImage()
    {
        if(isPostProcessingActivated)
        {
            return postProcessFBO->GetColorTexture()->GetId();
        }
        else
        {
            return viewportFBO->GetColorTexture()->GetId();
        }
    }

    static void GeometryPass(Scene &scene);

    static void RenderShadowMaps(Scene &scene);

    static void LightingPass(Scene &scene);

    static void ApplyPostProcessing()
    {
        if(shouldDrawFinalToFBO)
        {
            postProcessFBO->Bind();
        }
        else
        {
            FBO::Reset();
            glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
        }

        DisableDepthTesting();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        Clear();

        ResourcesManager::GetShader("postProcessShader")->Use();
        viewportFBO->GetColorTexture()->Bind();
        RenderQuad();
        FBO::Reset();
        EnableDepthTesting();
    }

    static inline unsigned int GetFboWidth() { return fboWidth; }
    static inline unsigned int GetFboHeight() { return fboHeight; }

    static inline int GetCascadesCount() { return cascadesCount; }

    static std::string CurDrawModeToString(int dM = drawMode)
    {
        switch (dM)
        {
            case 0:
                return "Wireframe";
            case 1:
                return "Lit";
            case 2:
                return "Lighting only";
            case 3:
                return "Directional lighting only";
            case 4:
                return "Point lighting only";
            case 5:
                return "Diffuse map view";
            case 6:
                return "Normal map view";
            case 7:
                return "Specular map view";
            case 8:
                return "Shadows factor";
            case 9:
                return "Roughness map view";
            default:
                return std::to_string(drawMode);
        }
    }
private:
    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix, std::vector<float> &shadowCascadeLevels);

    static glm::mat4 getLightSpaceMatrix(float nearPlane, float farPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix);
public:
    inline static int drawMode = 1;
    inline static glm::vec3 clearColor;
    inline static bool isPostProcessingActivated = true, shouldDrawFinalToFBO = true;

    // way to calculate shadows
    inline static LightingType::Type lightingType = LightingType::Dynamic;

    // constants
    inline static float baseOffset = 4.48f;
    inline static float deltaOffset = 1.28f;
    inline static float factorMultiplier = 4.0f;
    inline static std::vector<float> cascadeLevels { 25.0f, 50.0f, 100.0f, 200.0f, 750.0f };

private:
    // main render flow
    inline static unsigned int quadVAO = 0, quadVBO = 0;
    inline static unsigned int fboWidth = 0, fboHeight = 0;
    inline static std::unique_ptr<FBO> viewportFBO = nullptr, postProcessFBO = nullptr, gBufferFBO = nullptr;

    // shadows
    inline static std::unique_ptr<FBO> shadowFBO = nullptr;
    inline static std::shared_ptr<Texture> shadowTexture = nullptr;

    inline static std::unique_ptr<UBO<glm::mat4x4, 16>> lightMatricesUBO;
    inline static std::unique_ptr<UBO<PointLight, 1024>> pointLightsUBO;

    inline static int shadowMapResolution = 2048, cascadesCount = 5;

    friend class RendererIniSerializer;
};

#endif //GRAPHICS_RENDERER_H
