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

/* TODO: move this shit to .cpp file */
constexpr float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

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

        viewportFBO    = std::make_unique<FBO>();
        postProcessFBO = std::make_unique<FBO>();

        /* TODO: add configurable fbo resolution */
        unsigned int fboWidth = 2048;
        unsigned int fboHeight = 2048;

        viewportFBO->AddTexture(std::make_shared<Texture>(
                fboWidth,
                fboHeight,
                GL_RGB,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                false
        ), GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);


        viewportFBO->AddTexture(std::make_shared<Texture>(
                fboWidth,
                fboHeight,
                GL_DEPTH_STENCIL,
                GL_DEPTH24_STENCIL8,
                GL_UNSIGNED_INT_24_8,
                false
        ), GL_TEXTURE_2D, GL_DEPTH_STENCIL_ATTACHMENT);

        postProcessFBO->AddTexture(std::make_shared<Texture>(
                fboWidth,
                fboHeight,
                GL_RGB,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                false
        ), GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);

        viewportFBO->Check();
        postProcessFBO->Check();

        FBO::Reset();

        glGenVertexArrays(1, &viewportVAO);
        glGenBuffers(1, &viewportVBO);
        glBindVertexArray(viewportVAO);
        glBindBuffer(GL_ARRAY_BUFFER, viewportVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
        glBindVertexArray(0);
    }

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
private:
    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

    static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

    static glm::mat4 getLightSpaceMatrix(float nearPlane, float farPlane, float zoom, const glm::vec3& lightDir, const glm::mat4& viewMatrix);

    static std::vector<glm::mat4> getLightSpaceMatrices(float cameraNearPlane, float cameraFarPlane, float zoom, const glm::vec3& lightDir, const glm::mat4& viewMatrix, std::vector<float>& shadowCascadeLevels);
private:
    static glm::vec3 clearColor;
    static std::vector<float> cascadeLevels;
    static unsigned int viewportVAO, viewportVBO;
    static std::unique_ptr<FBO> viewportFBO, postProcessFBO;
    static std::unique_ptr<UBO<glm::mat4x4, 16>> lightMatricesUBO;
};

inline glm::vec3 Renderer::clearColor;
inline unsigned int Renderer::viewportVAO = 0, Renderer::viewportVBO = 0;
inline std::unique_ptr<FBO> Renderer::viewportFBO = nullptr, Renderer::postProcessFBO = nullptr;

#endif //GRAPHICS_RENDERER_H
