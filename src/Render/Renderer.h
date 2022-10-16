//
// Created by Anton on 06.09.2022.
//

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#define GLEW_STATIC

#include "glew.h"
#include "../Core/ResourcesManager.h"
#include "../Entity/Components.h"
#include "../Entity/Entity.h"
#include "UBO.hpp"
#include "FBO.hpp"

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
    static void Prepare(Scene& scene, int drawMode);

    /**
     * Renders scene to the color buffer
     * @param scene scene to render
     * @param shadowMap directional light rendered shadow map
     */
    static void Render(Scene& scene, unsigned int shadowMap);

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

    static void ApplyPostProcessing()
    {
        postProcessFBO->Bind();
        DisableDepthTesting();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        Clear();

        ResourcesManager::GetShader("postProcessShader")->Use();
        glBindVertexArray(viewportVAO);
        glBindTexture(GL_TEXTURE_2D, viewportFBO->GetColorTexture()->GetId());
        glDrawArrays(GL_TRIANGLES, 0, 6);

        FBO::Reset();
    }

    static inline unsigned int GetFboWidth() { return fboWidth; }
    static inline unsigned int GetFboHeight() { return fboHeight; }
private:
    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix, std::vector<float> &shadowCascadeLevels);

    static glm::mat4 getLightSpaceMatrix(float nearPlane, float farPlane, float zoom, float aspectRatio, const glm::vec3 &lightDir, const glm::mat4 &viewMatrix);
public:
    static glm::vec3 clearColor;
    static bool isPostProcessingActivated;

private:
    static std::vector<float> cascadeLevels;
    static unsigned int viewportVAO, viewportVBO;
    static unsigned int fboWidth, fboHeight;
    static std::unique_ptr<FBO> viewportFBO, postProcessFBO;
    static std::unique_ptr<UBO<glm::mat4x4, 16>> lightMatricesUBO;

    friend class RendererIniSerializer;
};

inline glm::vec3 Renderer::clearColor;
inline bool Renderer::isPostProcessingActivated = true;

inline unsigned int Renderer::fboWidth = 0, Renderer::fboHeight = 0;
inline unsigned int Renderer::viewportVAO = 0, Renderer::viewportVBO = 0;
inline std::unique_ptr<FBO> Renderer::viewportFBO = nullptr, Renderer::postProcessFBO = nullptr;

#endif //GRAPHICS_RENDERER_H
