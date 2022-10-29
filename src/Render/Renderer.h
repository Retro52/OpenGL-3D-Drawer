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
    static int drawMode;
    static glm::vec3 clearColor;
    static bool isPostProcessingActivated, shouldDrawFinalToFBO;

    // way to calculate shadows
    static LightingType::Type lightingType;

    // constants
    static float baseOffset;
    static float deltaOffset;
    static float factorMultiplier;
    static std::vector<float> cascadeLevels;

private:
    // main render flow
    static unsigned int quadVAO, quadVBO;
    static unsigned int fboWidth, fboHeight;
    static std::unique_ptr<FBO> viewportFBO, postProcessFBO, gBufferFBO, lBufferFBO;

    // shadows
    static std::unique_ptr<FBO> shadowFBO;
    static std::shared_ptr<Texture> shadowTexture;
    static std::unique_ptr<UBO<glm::mat4x4, 16>> lightMatricesUBO;
    static int shadowMapResolution, cascadesCount;

    friend class RendererIniSerializer;
};

inline int Renderer::drawMode = 1;
inline glm::vec3 Renderer::clearColor;
inline bool Renderer::shouldDrawFinalToFBO = true;
inline bool Renderer::isPostProcessingActivated = true;
inline LightingType::Type Renderer::lightingType = LightingType::Dynamic;


inline unsigned int Renderer::fboWidth = 0, Renderer::fboHeight = 0;
inline unsigned int Renderer::quadVAO = 0, Renderer::quadVBO = 0;
inline std::unique_ptr<FBO> Renderer::viewportFBO = nullptr, Renderer::postProcessFBO = nullptr, Renderer::shadowFBO = nullptr, Renderer::gBufferFBO = nullptr, Renderer::lBufferFBO = nullptr;
inline std::shared_ptr<Texture> Renderer::shadowTexture = nullptr;
inline int Renderer::shadowMapResolution = 2048, Renderer::cascadesCount = 5;

inline float Renderer::baseOffset        = 4.48f;
inline float Renderer::deltaOffset       = 1.28f;
inline float Renderer::factorMultiplier  = 4.0f;

#endif //GRAPHICS_RENDERER_H
